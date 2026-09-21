/**
 * @file
 * @brief Iomico Zephyr Intermediate course, lecture 5 task 1.
 * @note Purpose: monitor queue use and apply policy when queue overflows.
 * @note Some code based on Zephyr 4.4.0 zbus "hello_world" sample.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(l5_task1, LOG_LEVEL_DBG);

#define STACK_SIZE       1024
#define SENSOR_COUNT       10 // Sensor count is 18 in l4-demo2, red
#define SENSOR_PERIOD_MS  150

// Forward declaration
static void l4_listener_cb(const struct zbus_channel *chan);

/* ================================================================== */
/*  Shared channel message                                            */
/* ================================================================== */

struct acc_msg {
        int32_t x;
        int32_t y;
        int32_t z;
	uint32_t seq;
	uint32_t timestamp_ms;
};

ZBUS_LISTENER_DEFINE(l4_listener, l4_listener_cb);

#if 0
ZBUS_MSG_SUBSCRIBER_DEFINE(l4_subscriber);  // reads messages directly from net_buf, via
					    // zbus_sub_wait_msg().
#endif
ZBUS_SUBSCRIBER_DEFINE(l4_subscriber, CONFIG_SUBSCRIBER_QUEUE_SIZE);  // subscriber thread
					// unblocks via zbus_sub_wait(),
					// then reads message using zbus_chan_read().

ZBUS_CHAN_DEFINE(acc_data_chan,                                  /* Name */
                 struct acc_msg,                                 /* Message type */
                 NULL,                                           /* Validator */
                 NULL,                                           /* User data */
                 ZBUS_OBSERVERS(l4_listener, l4_subscriber),     /* observers */
                 ZBUS_MSG_INIT(.x = 0, .y = 0, .z = 0, .seq = 0,
			 	.timestamp_ms=0)                 /* Initial values */
);

/**
 * @note Timeout handler for task watchdog feature.
 */

struct task_timeout_context {
	uint32_t timestamp;
	char unit_of_time[8];
	char api_name[24];
};

static struct task_timeout_context task_ctx = {
	.timestamp = 0,
	.unit_of_time = "ms\0",
	.api_name = "l5-logging-fn",
};

//----------------------------------------------------------------------
// - SECTION - routines
//----------------------------------------------------------------------

static void task_timeout_cb(int channel_id, void* task_ctx)
{
	struct task_timeout_context* ctx = (struct task_timeout_context*)task_ctx;
#if 1
	LOG_ERR("  task watchdog channel %d.for %s timed out", channel_id, ctx->api_name);
	LOG_ERR("  (Could reset here)");
#else
	sys_reboot(SYS_REBOOT_COLD);
#endif
}

//----------------------------------------------------------------------
// - Listener
//----------------------------------------------------------------------

static void l4_listener_cb(const struct zbus_channel *chan)
{
        const struct acc_msg *acc = zbus_chan_const_msg(chan);

        LOG_INF("From l4 listener -> Acc x=%d, y=%d, z=%d", acc->x, acc->y, acc->z);
}

//----------------------------------------------------------------------
// - Publisher
//----------------------------------------------------------------------

static void sensor_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	k_thread_name_set(k_current_get(), "sensor");

	for (int i = 0; i < SENSOR_COUNT; i++) {
		struct acc_msg data = {
			.x = 10 * i,
			.y = 10 * i,
			.z = 10 * i,
			.seq = i,
		};

		LOG_INF("[SENSOR] publish seq=%u accel x=%d, y=%d, z=%d",
			data.seq,
       	         data.x, data.y, data.z);

		int rc = zbus_chan_pub(&acc_data_chan, &data, K_MSEC(100));
		if (rc != 0) {
			LOG_WRN("[SENSOR] publish failed rc=%d", rc);
		}

		k_msleep(SENSOR_PERIOD_MS);
	}

	LOG_INF("[SENSOR] done");
}

// For l5-task1, we are going to treat this logger thread as the consumer.
// To this thread we associate a task watchdog timer.

static void logger_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
	int32_t rc = 0;

	k_thread_name_set(k_current_get(), "l4-logger");

	const struct zbus_channel *chan;
	int received = 0;

	// Register a channel per monitored thread
	int task_wdt_id = task_wdt_add(100U,              // timeout ms
					task_timeout_cb,  // called if thread misses feed
					(void *)&task_ctx);

	while (received < SENSOR_COUNT) {
		struct acc_msg msg;

		/*
		 * Message subscribers receive a copy of the published message.
		 * The slow logger will not reread the latest channel value.
		 */

#if 0
		// TODO [ ] Create a symbol for hard-coded 1500 timeout:
		int rc = zbus_sub_wait_msg(&l4_subscriber, &chan, &msg, K_MSEC(1500));
		if (rc != 0) {
			LOG_WRN("[LOGGER-MSG] timeout rc=%d", rc);
			break;
		}
#else
		rc = zbus_sub_wait(&l4_subscriber, &chan, K_MSEC(10000));
		if (rc < 0) {
			LOG_ERR("Failed or timed out waiting for zbus channel %d", (uint32_t)chan);
		}

		rc = zbus_chan_read(chan, &msg, K_NO_WAIT);
		if (rc < 0) {
			LOG_ERR("Failed M3 . . .");
		}
#endif

		received++;

		LOG_INF("[LOGGER-MSG] thread=%s seq=%u x=%d y=%d z=%d latency=%ums",
			k_thread_name_get(k_current_get()),
			msg.seq,
			msg.x, msg.y, msg.z,
			k_uptime_get_32() - msg.timestamp_ms);

		/*
		 * Slow logger.
		 * Message copies let it process old samples safely.
		 */

		k_msleep(CONFIG_SUBSCRIBER_SIMULATED_WORK_DELAY_MS);

		LOG_INF("Feeding task watchdog timer . . .");
		task_wdt_feed(task_wdt_id);

		// REFERENCE https://docs.zephyrproject.org/latest/services/zbus/index.html
		uint32_t count = k_msgq_num_used_get(l4_subscriber.queue);
		LOG_INF("- M1 - hw5 subscriber queue holds %d of %d messages",
			count, CONFIG_SUBSCRIBER_QUEUE_SIZE);
	}

	LOG_INF("[LOGGER-MSG] done received=%d", received);
}

//----------------------------------------------------------------------
// - Threads
//----------------------------------------------------------------------

K_THREAD_DEFINE(sensor_thread, STACK_SIZE, sensor_thread_fn,
                NULL, NULL, NULL, 5, 0, 0);

K_THREAD_DEFINE(logger_thread, STACK_SIZE, logger_thread_fn,
                NULL, NULL, NULL, 6, 0, 0);
#if 0
K_THREAD_DEFINE(health_check_thread, STACK_SIZE, health_thread_fn,
                NULL, NULL, NULL, 5, 0, 0);
#endif

int main(void)
{
	LOG_INF("=== Lecture 5 task 1: Memory Resource Constraints ===");

	LOG_INF("sensor publishes every %dms", SENSOR_PERIOD_MS);
	LOG_INF("display listener runs in publisher context");
	LOG_INF("logger uses message subscriber copies");

	task_wdt_init(NULL);

	return 0;
}
