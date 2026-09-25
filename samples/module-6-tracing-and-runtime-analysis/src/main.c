#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/tracing/tracing.h>

LOG_MODULE_REGISTER(l6_task, LOG_LEVEL_INF);

#define STACK_SIZE            2048
#define CONTROL_PRIORITY         7
#define MAINTENANCE_PRIORITY     4
#define EVENT_PERIOD_MS        250
// #define MAINTENANCE_LOAD_US  45000
#define MAINTENANCE_LOAD_US 300000

struct control_event {
    uint32_t seq;
    uint32_t ready_ms;
};

K_MSGQ_DEFINE(control_queue, sizeof(struct control_event), 4, 4);
K_SEM_DEFINE(maintenance_start, 0, 1);

static uint32_t missed_count_fs = 0;

/* ================================================================== */
/*  Timer expiry: creates one control event                           */
/* ================================================================== */

static void event_timer_expiry(struct k_timer *timer)
{
	ARG_UNUSED(timer);

	static uint32_t seq = 1;
	struct control_event event = {
		.seq = seq++,
		.ready_ms = k_uptime_get_32(),
	};

	// Not sure whether it is safe to lock scheduler here, but trying:
	k_sched_lock();
	/* Timer expiry runs in interrupt context, so never wait here. */
	int ret = k_msgq_put(&control_queue, &event, K_NO_WAIT);

	if (ret != 0) {
		LOG_ERR("Failed message add to queue, sequence no %d",
			event.seq);
		missed_count_fs++;
		k_sched_unlock();
		return;
	}

	/* Both threads become ready when the timer interrupt returns. */
	k_sem_give(&maintenance_start);
#if 1
	sys_trace_named_event("msg_ready", event.seq,
				k_msgq_num_used_get(&control_queue));
#endif
	k_sched_unlock();

	/* TODO: Add an application trace event for this sequence. */
}

K_TIMER_DEFINE(event_timer, event_timer_expiry, NULL);

static void missed_report_helper(const char* caller)
{
	static uint32_t time_previous;
	uint32_t time_present = k_uptime_get_32();
	uint32_t time_since_prev_report = time_present - time_previous;

	if (time_since_prev_report >= CONFIG_MISSED_COUNT_INTERVAL_MS) {
		LOG_INF("[%s] Missed message count %u", caller, missed_count_fs);
		time_previous = time_present;
	}
}

/* ================================================================== */
/*  Control thread                                                   */
/* ================================================================== */

static void control_fn(void *p1, void *p2, void *p3) // Begins with priority 7
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	while (true) {
		struct control_event event;
		int ret = k_msgq_get(&control_queue, &event, K_FOREVER);

		if (ret != 0) {
			LOG_ERR("[CONTROL] receive failed: %d", ret);
			continue;
		}

		// LOG_INF("[CONTROL] processed seq=%u", event.seq);
		LOG_INF("[CONTROL] processed seq=%u, pub time %u", event.seq,
		event.ready_ms);

		uint32_t time_now = k_uptime_get_32();
		LOG_INF("[CONTROL] processed message in %d milliseconds",
		time_now - event.ready_ms);

		/* TODO: Define a response-time guarantee. */
		/* TODO: Measure latency and count every deadline miss. */
		/* TODO: Rate-limit repeated warning messages. */
		/* TODO: Add an application trace event for completion. */

	// Response-time guarantee, assuming this means the trio of periods
	// waiting, being scheduled, executing, for the mainenance thread looks
	// like:
	//
	//	(1) EVENT_PERIOD_MS + context_switch + message_copy_time
	//
	// If we were to design and assume an application in which the context
	// switch to our control task were sub-millisecond, we could estimate
	// a guaranteed response time with some margin, say twenty percent
	// greater than the significant response-time element of waiting for the
	// event:
	//
	//	(2) EVENT_PERIOD_MS * 1.2
	//
	// However in this app the maintenance function is contrived to create
	// an additinal millisecond delay, so the context switching term in
	// response sime becomes important too.  A revised guaranteed
	// response-time estimate now looks like:
	//
	//	(3) (EVENT_PERIOD_MS + MAINT_PERIOD_MS) * 1.2
	//
	// The scaling multiplier, greater than one, reflects the fact that
	// there is some non-zero time both to schedule and to execute the
	// control thread.  Also worth noting, the maintenance period is a time
	// when an alternate task (in this case an initially higher priority
	// thread) is running.  That is to say, from the perspective of the
	// control thread, the maintenance period is part of the pending time
	// of the maximum latency for the control thread to complete its work.

		if (missed_count_fs > 0) {
			missed_report_helper("CONTROL");
		}
	}
}

/* ================================================================== */
/*  Background maintenance thread                                    */
/* ================================================================== */

static void maintenance_fn(void *p1, void *p2, void *p3) // Begins with priority 4
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	while (true) {
		k_sem_take(&maintenance_start, K_FOREVER);

		/* This work is important, but it has no short deadline. */
		k_busy_wait(MAINTENANCE_LOAD_US);
	}
}

K_THREAD_DEFINE(control, STACK_SIZE, control_fn,
	NULL, NULL, NULL, CONTROL_PRIORITY, 0, 0);

K_THREAD_DEFINE(maintenance, STACK_SIZE, maintenance_fn,
	NULL, NULL, NULL, MAINTENANCE_PRIORITY, 0, 0);

int main(void)
{
	LOG_INF("=== L6 Homework: Runtime Investigation ===");
	LOG_INF("Control work must start within 10 ms");
	LOG_INF("Inspect, measure, trace, explain, and correct the delay");

	printk("M1 - l6\n");

	k_timer_start(&event_timer, K_MSEC(500), K_MSEC(EVENT_PERIOD_MS));

	return 0;
}
