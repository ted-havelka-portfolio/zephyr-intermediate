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

#define T_A_SLEEP_MS 300
#define T_B_SLEEP_MS 200
#define T_C_SLEEP_MS 100
#define T_D_SLEEP_MS 1000

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
	while (1) {
		LOG_INF("thread    d - cooperative");
		for (int i = 0; i < CO_OP_BUSY_ITERS; i++) {
			k_busy_wait(200);
			LOG_INF("thread    d - busy");
		}
		k_msleep(T_D_SLEEP_MS);
		// k_yield();  /* <-- nothing, not even Zephyr banner message */
		               /*     appear with call to k_yield() active.   */
	}
}
#endif

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
