#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(demo, LOG_LEVEL_DBG);

#define STACK_SIZE 1024

#define PRIO_A 7
#define PRIO_B 5
#define PRIO_C 3

void t_low_fn(void *p1, void *p2, void *p3)
{
	while (1) {
		LOG_INF("thread a");
		k_msleep(100);
	}
}

void t_med_fn(void *p1, void *p2, void *p3)
{
	while (1) {
		LOG_INF("thread  b");
		k_msleep(500);
	}
}

void t_high_fn(void *p1, void *p2, void *p3)
{
	while (1) {
		LOG_INF("thread   c");
		k_msleep(500);
	}
}

K_THREAD_DEFINE(thread_a, STACK_SIZE, t_low_fn,
                NULL, NULL, NULL, PRIO_A, 0, 0);
K_THREAD_DEFINE(thread_b, STACK_SIZE, t_med_fn,
                NULL, NULL, NULL, PRIO_B, 0, 0);
K_THREAD_DEFINE(thread_c, STACK_SIZE, t_high_fn,
                NULL, NULL, NULL, PRIO_C, 0, 0);

int main(void)
{
	LOG_INF("main.c thread test starting");
	return 0;
}
