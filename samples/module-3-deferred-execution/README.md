# Zephyr Intermediate Course - Lecture 3 Deferred Execution

Note, the homework tasks are described in detail in a near top-of-file comment
block in src/main.c.

## How To Compile For nucleo_l432kc

```shell
west build -b nucleo_l432kc -p always
```

To build for a different board, in general it is only necessary to change the
board name, which comes after the '-b' in the command to build.

## How To Flash

Pending a restructuring of this Zephyr Intermediate course work, to use a T3 or
"forest" topology, homework related apps beyond the first app are located
alongside the app directory, in ``samples/module-n-topic_name``.  From this type
of location, and with a shortcoming of Zephyr 4.4.0 runner support for pyocd
and an older STM target board, author currently flashes the assigned apps
using a shell convenience script:

```shell
../../app/flash-manually.sh w --config-path ../../app/scripts
```

## To Observe Output

When targeting an STM nucleo_ type dev board:

Be sure when using minicom or similar UART utility, to select /dev/ttyUSBn,
where n is usually some small positive integer.

Note: do not point the UART utility to /dev/ttyACM0,.

# Task Outputs And Notes

There are four tasks in total.  The final task is a bonus task.

## Task 1

The inefficient, thread based polling is shown by the output messages:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> homework: === L3 Homework: Polling to Workqueue ===
[00:00:00.000,000] <inf> homework: M1
[00:00:00.000,000] <inf> homework: Starter: polling every 10ms, sensor fires every 100ms
[00:00:00.000,000] <inf> homework: Expected wasted wakeups: ~9 per event
[00:00:00.000,000] <inf> homework: Run this, count wakeups, then convert to workqueue.
[00:00:00.100,000] <inf> homework: [SENSOR] event 0  tick=100
[00:00:00.101,000] <inf> homework: [CONSUMER] processed event 1  wakeups_so_far=10  tick=101
[00:00:00.200,000] <inf> homework: [SENSOR] event 1  tick=200
[00:00:00.202,000] <inf> homework: [CONSUMER] processed event 2  wakeups_so_far=20  tick=202
[00:00:00.300,000] <inf> homework: [SENSOR] event 2  tick=300
[00:00:00.303,000] <inf> homework: [CONSUMER] processed event 3  wakeups_so_far=30  tick=303
[00:00:00.400,000] <inf> homework: [SENSOR] event 3  tick=400
[00:00:00.404,000] <inf> homework: [CONSUMER] processed event 4  wakeups_so_far=40  tick=404
[00:00:00.500,000] <inf> homework: [SENSOR] event 4  tick=500
[00:00:00.505,000] <inf> homework: [CONSUMER] processed event 5  wakeups_so_far=50  tick=505
[00:00:00.600,000] <inf> homework: [SENSOR] event 5  tick=600
[00:00:00.606,000] <inf> homework: [CONSUMER] processed event 6  wakeups_so_far=60  tick=606
[00:00:00.700,000] <inf> homework: [SENSOR] event 6  tick=700
[00:00:00.707,000] <inf> homework: [CONSUMER] processed event 7  wakeups_so_far=70  tick=707
[00:00:00.801,000] <inf> homework: [SENSOR] event 7  tick=801
[00:00:00.808,000] <inf> homework: [CONSUMER] processed event 8  wakeups_so_far=80  tick=808
[00:00:00.901,000] <inf> homework: [SENSOR] event 8  tick=901
[00:00:00.909,000] <inf> homework: [CONSUMER] processed event 9  wakeups_so_far=90  tick=909
[00:00:01.001,000] <inf> homework: [SENSOR] event 9  tick=1001
[00:00:01.001,000] <inf> homework: [SENSOR] all events produced
[00:00:01.010,000] <inf> homework: [CONSUMER] processed event 10  wakeups_so_far=100  tick=1010
[00:00:01.010,000] <inf> homework: 

[00:00:01.010,000] <inf> homework: [SUMMARY] events=10  total_wakeups=100  wasted=90
[00:00:01.010,000] <inf> homework: [SUMMARY] wasted wakeups = 90% of all wakeups
```

## Task 2

To replace the polling thread with a kernel work handler . . .

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> homework: === L3 Homework: Polling to Workqueue ===
[00:00:00.000,000] <inf> homework: Starter: polling disabled, simulated sensor fires every 100ms
[00:00:00.000,000] <inf> homework: Expecting no wasted wakeups per event
[00:00:00.100,000] <inf> homework: [SENSOR] event 0  tick=100
[00:00:00.100,000] <inf> homework: [CONSUMER] processed event 1  wakeups_so_far=0  tick=100
[00:00:00.200,000] <inf> homework: [SENSOR] event 1  tick=200
[00:00:00.200,000] <inf> homework: [CONSUMER] processed event 2  wakeups_so_far=0  tick=200
[00:00:00.300,000] <inf> homework: [SENSOR] event 2  tick=300
[00:00:00.300,000] <inf> homework: [CONSUMER] processed event 3  wakeups_so_far=0  tick=300
[00:00:00.400,000] <inf> homework: [SENSOR] event 3  tick=400
[00:00:00.400,000] <inf> homework: [CONSUMER] processed event 4  wakeups_so_far=0  tick=400
[00:00:00.501,000] <inf> homework: [SENSOR] event 4  tick=501
[00:00:00.501,000] <inf> homework: [CONSUMER] processed event 5  wakeups_so_far=0  tick=501
[00:00:00.601,000] <inf> homework: [SENSOR] event 5  tick=601
[00:00:00.601,000] <inf> homework: [CONSUMER] processed event 6  wakeups_so_far=0  tick=601
[00:00:00.701,000] <inf> homework: [SENSOR] event 6  tick=701
[00:00:00.701,000] <inf> homework: [CONSUMER] processed event 7  wakeups_so_far=0  tick=701
[00:00:00.801,000] <inf> homework: [SENSOR] event 7  tick=801
[00:00:00.801,000] <inf> homework: [CONSUMER] processed event 8  wakeups_so_far=0  tick=801
[00:00:00.901,000] <inf> homework: [SENSOR] event 8  tick=901
[00:00:00.901,000] <inf> homework: [CONSUMER] processed event 9  wakeups_so_far=0  tick=901
[00:00:01.002,000] <inf> homework: [SENSOR] event 9  tick=1002
[00:00:01.002,000] <inf> homework: [CONSUMER] processed event 10  wakeups_so_far=0  tick=1002
[00:00:01.002,000] <inf> homework: [SENSOR] all events produced
```
