#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);

#define STACK_SIZE 1024

#define PRIO_A 7
#define PRIO_B 5
#define PRIO_C 3
#if CONFIG_COOPERATIVE_THREAD
#define PRIO_D -1
#endif

#define T_A_SLEEP_MS 3000
#define T_B_SLEEP_MS 2000
#define T_C_SLEEP_MS 1000
#define T_D_BUSY_WAIT_MS 15000

#define CO_OP_BUSY_ITERS 5

void t_low_fn(void *p1, void *p2, void *p3)
{
	while (1) {
		LOG_INF("thread a");
		k_msleep(T_A_SLEEP_MS);
	}
}

void t_med_fn(void *p1, void *p2, void *p3)
{
	while (1) {
		LOG_INF("thread  b");
		k_msleep(T_B_SLEEP_MS);
	}
}

void t_high_fn(void *p1, void *p2, void *p3)
{
	while (1) {
		LOG_INF("thread   c");
		k_msleep(T_C_SLEEP_MS);
	}
}

#if CONFIG_COOPERATIVE_THREAD
void t_coop_fn(void *p1, void *p2, void *p3)
{
	LOG_INF("thread    d - cooperative");
	for (int i = 0; i < CO_OP_BUSY_ITERS; i++) {
		k_busy_wait(T_D_BUSY_WAIT_MS);
		LOG_INF("thread    d - busy");
	}

	k_yield();  /* See Note 1 just past this function */
}
#endif

/**
 * @note Note 1 - if the cooperative thread's body is defined with a
 *  forever-loop, a 'while (1)' construct, in this demo app, the app produces
 *  no output.  This is because the coooperative thread has a uniquely high
 *  high priorty, in this case a priority of -1.  The remaining threads have
 *  lower priorities of 3, 5, and 7.  Further, Zephyr logging thread when
 *  configured for deferred mode logging has a low thread priority.  Its
 *  priority is certainly in the preemptive range of thread priorities.
 *  Deferred logging is designed to stay out of the way of the application's
 *  intended timing, as much as possilble.
 *
 *  Zephyr k_yield() kernel API yields the CPU from a given thread's execution
 *  to another ready thread with equal or higher priority.  See
 *  https://docs.zephyrproject.org/latest/doxygen/html/group__thread__apis.html#ga08a3484c33444ecedc2d71d78495a295.
 */

K_THREAD_DEFINE(thread_a, STACK_SIZE, t_low_fn,
                NULL, NULL, NULL, PRIO_A, 0, 0);
K_THREAD_DEFINE(thread_b, STACK_SIZE, t_med_fn,
                NULL, NULL, NULL, PRIO_B, 0, 0);
K_THREAD_DEFINE(thread_c, STACK_SIZE, t_high_fn,
                NULL, NULL, NULL, PRIO_C, 0, 0);
#if CONFIG_COOPERATIVE_THREAD
K_THREAD_DEFINE(thread_d, STACK_SIZE, t_coop_fn,
                NULL, NULL, NULL, PRIO_D, 0, 0);
#endif

int main(void)
{
	LOG_INF("main.c thread test starting");
	return 0;
}
