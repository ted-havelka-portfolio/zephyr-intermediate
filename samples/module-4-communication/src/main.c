/**
 * @file
 * @brief Iomico Zephyr Intermediate course, lecture 4 task 1, zbus sample app.
 * @note Some code based on Zephyr 4.4.0 zbus "hello_world" sample.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(l4_task1, LOG_LEVEL_DBG);

#define STACK_SIZE       1024
#define SENSOR_COUNT       10 // Sensor count is 18 in l4-demo2
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

// TODO [ ] Account for or at least create named symbol for queue size parameter
//          presently hard-coded as '4':
//
// ZBUS_SUBSCRIBER_DEFINE(l4_subscriber, 4);
//
// . . . wait, this macro differs from Iomico's l4-demo2 application.  Iomico
// uses:

// ZBUS_MSG_SUBSCRIBER_DEFINE(logger_sub);
ZBUS_MSG_SUBSCRIBER_DEFINE(l4_subscriber);

ZBUS_CHAN_DEFINE(acc_data_chan,  /* Name */
                 struct acc_msg, /* Message type */

                 NULL,                                           /* Validator */
                 NULL,                                           /* User data */
                 ZBUS_OBSERVERS(l4_listener, l4_subscriber),     /* observers */
                 ZBUS_MSG_INIT(.x = 0, .y = 0, .z = 0, .seq = 0,
			 	.timestamp_ms=0)                 /* Initial value */
);

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

static void logger_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	k_thread_name_set(k_current_get(), "l4-logger");

	const struct zbus_channel *chan;
	int received = 0;

	while (received < SENSOR_COUNT) {
		struct acc_msg msg;

		/*
		 * Message subscribers receive a copy of the published message.
		 * The slow logger will not reread the latest channel value.
		 */

		// TODO [ ] Create a symbol for hard-coded 1500 timeout:
		int rc = zbus_sub_wait_msg(&l4_subscriber, &chan, &msg, K_MSEC(1500));
		if (rc != 0) {
			LOG_WRN("[LOGGER-MSG] timeout rc=%d", rc);
			break;
		}

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

		// TODO [ ] Create a symbol for this hard-coded loop delay:
		k_msleep(350);
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

int main(void)
{
    LOG_INF("=== Lecture 4 task 1: Zbus Pub-Sub ===");

    LOG_INF("sensor publishes every %dms", SENSOR_PERIOD_MS);
    LOG_INF("display listener runs in publisher context");
    LOG_INF("logger uses message subscriber copies");
    // LOG_INF("alarm uses a regular subscriber");
    // LOG_INF("alarm threshold: %d mC", TEMP_ALARM_MC);

    return 0;
}
