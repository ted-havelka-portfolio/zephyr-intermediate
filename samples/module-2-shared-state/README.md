# Zephyr Intermediate Course - lecture 1 task 2

Expected outputs . . .

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> l1_task2_shared_state: === L1 Task 2: Shared State Safety ===
[00:00:00.000,000] <inf> l1_task2_shared_state: l1-task2: mutex disabled (not compiled) in app
[00:00:00.000,000] <inf> l1_task2_shared_state: thread A prio=5 thread B prio=5
[00:00:00.000,000] <inf> l1_task2_shared_state: Waiting for a competing thread to finish counting work . . .
[00:00:00.167,000] <inf> l1_task2_shared_state: thread thread_a done
[00:00:00.167,000] <inf> l1_task2_shared_state: One thread finished.  Waiting for the other . . .
[00:00:00.175,000] <inf> l1_task2_shared_state: thread thread_b done
[00:00:00.175,000] <inf> l1_task2_shared_state: Counting threads done.
[00:00:00.175,000] <inf> l1_task2_shared_state: Each thread counted from zero to 1000000,
[00:00:00.175,000] <inf> l1_task2_shared_state: Global counter holds 1543699,
[00:00:00.175,000] <inf> l1_task2_shared_state: 
```

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> l1_task2_shared_state: === L1 Task 2: Shared State Safety ===
[00:00:00.000,000] <inf> l1_task2_shared_state: l1-task2: mutex enabled
[00:00:00.000,000] <inf> l1_task2_shared_state: thread A prio=5 thread B prio=5
[00:00:00.000,000] <inf> l1_task2_shared_state: Waiting for a competing thread to finish counting work . . .
[00:00:03.681,000] <inf> l1_task2_shared_state: thread thread_a done
[00:00:03.681,000] <inf> l1_task2_shared_state: One thread finished.  Waiting for the other . . .
[00:00:03.682,000] <inf> l1_task2_shared_state: thread thread_b done
[00:00:03.682,000] <inf> l1_task2_shared_state: Counting threads done.
[00:00:03.682,000] <inf> l1_task2_shared_state: Each thread counted from zero to 1000000,
[00:00:03.682,000] <inf> l1_task2_shared_state: Global counter holds 2000000,
[00:00:03.682,000] <inf> l1_task2_shared_state:
```

Note, with count upper bound set to only 100,000, the file-scoped counter
ultimately held the expected count of 200,000.  So on the particular STMicro
processor, 100,000 increment operations performed by two threads were
insufficient to cause the scheduler to interleave load, add, and store
operations at the machine code level.
