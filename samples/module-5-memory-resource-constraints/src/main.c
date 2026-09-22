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
#define SENSOR_PERIOD_MS CONFIG_PUBLISHER_PERIOD_MS

// Forward declaration
static void l5_listener_cb(const struct zbus_channel *chan);

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

ZBUS_LISTENER_DEFINE(l5_listener, l5_listener_cb);

ZBUS_SUBSCRIBER_DEFINE(l5_subscriber, CONFIG_SUBSCRIBER_QUEUE_SIZE);  // subscriber thread
					// unblocks via zbus_sub_wait(),
					// then reads message using zbus_chan_read().

ZBUS_CHAN_DEFINE(acc_data_chan,                                  /* Name */
                 struct acc_msg,                                 /* Message type */
                 NULL,                                           /* Validator */
                 NULL,                                           /* User data */
                 ZBUS_OBSERVERS(l5_listener, l5_subscriber),     /* observers */
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

K_SEM_DEFINE(l5t1_work_done, 0, 3);

// Provide a little more granularity for app to see when health thread done:
K_SEM_DEFINE(producer_done, 0, 1);
K_SEM_DEFINE(consumer_done, 0, 1);
K_SEM_DEFINE(p_and_c_work_done, 0, 1);
K_SEM_DEFINE(health_monitor_done, 0, 1);

//----------------------------------------------------------------------
// - SECTION - routines
//----------------------------------------------------------------------

static void task_timeout_cb(int channel_id, void* task_ctx)
{
	struct task_timeout_context* ctx = (struct task_timeout_context*)task_ctx;
#if 1
	LOG_ERR("  task watchdog channel %d for %s timed out", channel_id, ctx->api_name);
	LOG_ERR("  (Could reset here)");
#else
	sys_reboot(SYS_REBOOT_COLD);
#endif
}

//----------------------------------------------------------------------
// - Listener
//----------------------------------------------------------------------

static void l5_listener_cb(const struct zbus_channel *chan)
{
        const struct acc_msg *acc = zbus_chan_const_msg(chan);

        LOG_INF("From l5 listener -> Acc x=%d, y=%d, z=%d", acc->x, acc->y, acc->z);
}

//----------------------------------------------------------------------
// - Publisher
//----------------------------------------------------------------------

static void sensor_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	k_thread_name_set(k_current_get(), "sensor");

	for (int i = 0; i < CONFIG_SENSOR_READING_COUNT; i++) {
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

	LOG_INF("* [SENSOR] done  *");

	k_sem_give(&producer_done);
}

// For l5-task1, we are going to treat this logging thread as the consumer.
// To this thread we associate a task watchdog timer.

static void logging_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
	int32_t rc = 0;

	k_thread_name_set(k_current_get(), "logging-thread-l5");

	const struct zbus_channel *chan;
	int received = 0;

	// Register a channel per monitored thread
	int task_wdt_id = task_wdt_add(CONFIG_SUBSCRIBER_WDT_TIMEOUT_MS,
					task_timeout_cb,  // called if thread misses feed
					(void *)&task_ctx);

	while (received < CONFIG_SENSOR_READING_COUNT) {
		struct acc_msg msg;

		/*
		 * Message subscribers receive a copy of the published message.
		 * The logging thread will not reread the latest channel value.
		 */

		rc = zbus_sub_wait(&l5_subscriber, &chan, K_MSEC(10000));
		if (rc < 0) {
			LOG_ERR("[LOGGING] Failed or timed out waiting for zbus channel %d", (uint32_t)chan);
		}

		rc = zbus_chan_read(chan, &msg, K_NO_WAIT);
		if (rc < 0) {
			LOG_ERR("[LOGGING] Failed to read zbus channel for sensor reading");
		}

		received++;

		LOG_INF("[LOGGING] thread=%s seq=%u x=%d y=%d z=%d latency=%ums",
			k_thread_name_get(k_current_get()),
			msg.seq,
			msg.x, msg.y, msg.z,
			k_uptime_get_32() - msg.timestamp_ms);

		/*
		 * Slow logging thread.
		 * Message copies let it process old samples safely.
		 */

		k_msleep(CONFIG_SUBSCRIBER_SIMULATED_WORK_DELAY_MS);

		// LOG_INF("Feeding task watchdog timer . . .");
		task_wdt_feed(task_wdt_id);
	}

	LOG_INF("[LOGGING] done, received=%d messages", received);
	k_sem_give(&consumer_done);
}

static void health_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
	int32_t rc = 0;

	k_thread_name_set(k_current_get(), "health-thread");

	while (1) {
		// REFERENCE https://docs.zephyrproject.org/latest/services/zbus/index.html
		uint32_t count = k_msgq_num_used_get(l5_subscriber.queue);
#if 0
		LOG_INF("- M1 - hw5 subscriber queue holds %d of %d messages",
			count, CONFIG_SUBSCRIBER_QUEUE_SIZE);
#endif
		// Log warning at 75% queue capacity
		if ((((count * 1000) / CONFIG_SUBSCRIBER_QUEUE_SIZE) / 10) >=
			CONFIG_L5T1_QUEUE_WATERMARK) {
			LOG_WRN("Queue at or over capacity of %d%%",
					CONFIG_L5T1_QUEUE_WATERMARK);
		}

		rc = k_sem_take(&p_and_c_work_done, K_NO_WAIT);
		if (rc == 0) {
			break;
		}

		k_msleep(1000);
	}

	LOG_INF("[HEALTH] Done.");
	k_sem_give(&health_monitor_done);
}

//----------------------------------------------------------------------
// - Threads
//----------------------------------------------------------------------

K_THREAD_DEFINE(sensor_thread, STACK_SIZE, sensor_thread_fn,
                NULL, NULL, NULL, 5, 0, 0);

K_THREAD_DEFINE(logging_thread_l5, STACK_SIZE, logging_thread_fn,
                NULL, NULL, NULL, 6, 0, 0);

K_THREAD_DEFINE(health_check_thread, STACK_SIZE, health_thread_fn,
                NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
	LOG_INF("=== Lecture 5 task 1: Memory Resource Constraints ===");

	LOG_INF("* Sensor publishes every %d ms", SENSOR_PERIOD_MS);
	LOG_INF("* Logging thread runs a little over %d ms",
		CONFIG_SUBSCRIBER_SIMULATED_WORK_DELAY_MS);
	LOG_INF("* Display listener runs in publisher context");
	LOG_INF("* Logging thread uses message subscriber copies");
	LOG_INF("About to publish %d simulated sensor readings . . .",
		CONFIG_SENSOR_READING_COUNT);

	task_wdt_init(NULL);

	k_sem_take(&producer_done, K_FOREVER);
	LOG_INF("[MAIN] Producer thread done.");
	k_sem_take(&consumer_done, K_FOREVER);
	LOG_INF("[MAIN] Consumer thread done.");

	k_sem_give(&p_and_c_work_done);
	k_sem_take(&health_monitor_done, K_FOREVER);
	LOG_INF("[MAIN] Health thread done.");

	LOG_INF("[MAIN] Producer, consumer and health monitor all completed their work.");
	LOG_INF("[MAIN] App l5-task1 done.");
	return 0;
}
