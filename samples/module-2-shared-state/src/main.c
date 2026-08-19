#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(l1_task2_shared_state, LOG_LEVEL_DBG);

#define STACK_SIZE 1024

// Choose same priority for both threads
#define PRIO_A 5
#define PRIO_B 5

#define COUNT_UPPER_BOUND 50000000

// TODO [ ] Find out whether C standard assigns zero to file-scoped, static vars.
static volatile uint32_t test_count_fs;

K_SEM_DEFINE(l1_sem, 0, 2);

// void thread_a_fn(void *p1, void *p2, void *p3)
void work_fn(void *p1, void *p2, void *p3)
{
    const char *name = k_thread_name_get(k_current_get());

    for (uint32_t i = 0; i < COUNT_UPPER_BOUND; i++) {
	test_count_fs++;
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
    LOG_INF("thread A prio=%d thread B prio=%d", PRIO_A, PRIO_B);

    LOG_INF("Waiting for a competing thread to finish counting work . . .");
    k_sem_take(&l1_sem, K_FOREVER);

    LOG_INF("One thread finished.  Waiting for the other . . .");
    k_sem_take(&l1_sem, K_FOREVER);
    
    LOG_INF("Counting threads done.");
    LOG_INF("Each thread counted from zero to %u,", COUNT_UPPER_BOUND);
    LOG_INF("Global counter holds %u,", test_count_fs);

    return 0;
}
