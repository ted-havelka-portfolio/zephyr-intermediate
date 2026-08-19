#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(l1_task2_shared_state, LOG_LEVEL_DBG);

#define STACK_SIZE 1024

// Choose same priority for both threads
#define PRIO_A 5
#define PRIO_B 5

#define COUNT_UPPER_BOUND 1000000

// TODO [ ] Find out whether C standard assigns zero to file-scoped, static vars.
static volatile uint32_t test_count_fs;

K_SEM_DEFINE(l1_sem, 0, 2);

#if CONFIG_MUTEX_FOR_SHARED_COUNT
K_MUTEX_DEFINE(l1_mtx);
#define L1_MUTEX_TIMEOUT_MS 10
#endif

void work_fn(void *p1, void *p2, void *p3)
{

	const char *name = k_thread_name_get(k_current_get());

	for (uint32_t i = 0; i < COUNT_UPPER_BOUND; i++) {
#if CONFIG_MUTEX_FOR_SHARED_COUNT
		if (k_mutex_lock(&l1_mtx, K_MSEC(L1_MUTEX_TIMEOUT_MS)) == 0) {
			/* mutex successfully locked */
		} else {
			LOG_ERR("Failed to lock mutex, skipping an increment operation, iter %d",
				i);
			continue;
		}
#endif
		test_count_fs++;
#if CONFIG_MUTEX_FOR_SHARED_COUNT
		k_mutex_unlock(&l1_mtx);
#endif
	}

	LOG_INF("thread %s done", name);
	k_sem_give(&l1_sem);
}

K_THREAD_DEFINE(thread_a, STACK_SIZE, work_fn,
		NULL, NULL, NULL, PRIO_A, 0, 0);
K_THREAD_DEFINE(thread_b, STACK_SIZE, work_fn,
		NULL, NULL, NULL, PRIO_B, 0, 0);

int main(void)
{
	LOG_INF("=== L1 Task 2: Shared State Safety ===");
#if CONFIG_MUTEX_FOR_SHARED_COUNT
	LOG_INF("l1-task2: mutex enabled");
#else
	LOG_INF("l1-task2: mutex disabled (not compiled) in app");
#endif
	LOG_INF("thread A prio=%d thread B prio=%d", PRIO_A, PRIO_B);

	LOG_INF("Waiting for a competing thread to finish counting work . . .");
	k_sem_take(&l1_sem, K_FOREVER);

	LOG_INF("One thread finished.  Waiting for the other . . .");
	k_sem_take(&l1_sem, K_FOREVER);

	LOG_INF("Counting threads done.");
	LOG_INF("Each thread counted from zero to %u,", COUNT_UPPER_BOUND);
	LOG_INF("Global counter holds %u,", test_count_fs);
	LOG_INF("");

	return 0;
}
