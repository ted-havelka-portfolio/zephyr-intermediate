# Scheduling Delay Investigation

This work goes with Iomico Zephyr Intermediate lecture series, lecture 6. The
application primarily demonstrates and explores some common scheduling delay
scenarios, and ways to log and to trace them.

This work is tagged l6-task1 in the forked code repository.

Some things to note in this task:

| Run-time actor | Implemented by                                           |
| :------------- | :------------------------------------------------------- |
| producer       | A kernel timer and callback, to push messages to a queue |
| consumer       | A thread named "control"; reads a message queue          |
| other CPU load | A thread named "maintenance"; calls k_busy_wait()        |

The maintenance thread simulate heavy CPU work. The call to k_busy_wait() uses
CPU time, and when this thread's priority is higher than that of the control
thread, the control thread remains pending until that simulated work of the
maintenance thread completes.

Note, the timer runs in an interrupt context, so it preempts whatever thread may
be running.

## Configuration for Logging and Tracing

This scheduling app enables a single UART on the "real hardware" dev board. The
UART can transport logging messages, and trace data, but only sensibly if one is
enabled at a time. The symbol L6_APP_ZEPHYR_TRACING is provided to select one or
the other diagnostic feature.

When L6_APP_ZEPHYR_TRACING is set to 'n' in prj.conf, standard Zephyr logging is
enabled. When this symbol is set to 'y', then Zephyr application tracing is
enabled.

## How To Build

To build this app for the STMicro nucleo_l432kc, call 'west' with options:

```
west build -b nucleo_l432kc -p always
```

It's possible to build this app for most all of Zephyr's supported dev boards,
by changing the board name to the given board on hand.

Note: boards may need a device tree overlay addition, to support CTF or other
format trace information to use a UART as the backend transport.

Part of Zephyr's tracing libraries depends on a special device node property in
the Zephyr "chosen" device tree node. That property is expressed in the line
which contains "tracing-uart" in this device tree code snippet:

```
/ {
        chosen {
                zephyr,console = &usart1;
                zephyr,shell-uart = &usart1;
                zephyr,tracing-uart = &usart1;
        };
};
```

Without it, the build may fail with an ultimate macro expansion error line of
the form:

```
   24 |         DEVICE_DT_GET(DT_CHOSEN(zephyr_tracing_uart));
```

So, make sure that a file named \<board_name>.overlay adds this to the project's
"Zephyr chosen" device tree node.

Note that the native_sim board does not expect a UART device tree node. A DTS
overlay named app.overlay will erroneously add a Zephyr "chosen" node
property+value pair. An overlay file named differently will not.

## How To Flash

For the STMicro nucleo_l432kc, openocd is a sufficient runner for flashing
Zephyr apps: Invoke this way:

```
west flash -r openocd
```

## Output On Real Hardware

First output is captured with application symbols in the app (not factored as
Kconfig symbols) set to:

```
#define STACK_SIZE            2048
#define CONTROL_PRIORITY         7
#define MAINTENANCE_PRIORITY     4
#define EVENT_PERIOD_MS        250
// #define MAINTENANCE_LOAD_US  45000
#define MAINTENANCE_LOAD_US 300000
```

With these settings the maintenance thread runs for 300 milliseconds. This is 50
millliseconds longer than the "event period" or interval for timer-based message
production. Sample output looks like the following, and shows dropped messages
on the consumer side, and queue full error messages on the producer side:

```
[00:00:00.000,000] <inf> l6_task: === L6 Homework: Runtime Investigation ===
[00:00:00.000,000] <inf> l6_task: Control work must start within 10 ms
[00:00:00.000,000] <inf> l6_task: Inspect, measure, trace, explain, and correct the delay
M1 - l6
[00:00:01.750,000] <err> l6_task: Failed message add to queue, sequence no 6
[00:00:02.000,000] <err> l6_task: Failed message add to queue, sequence no 7
[00:00:02.000,000] <inf> l6_task: [CONTROL] processed seq=1, pub time 500
[00:00:02.000,000] <inf> l6_task: [CONTROL] processed message in 1500 milliseconds
[00:00:02.001,000] <inf> l6_task: [CONTROL] processed seq=2, pub time 750
[00:00:02.001,000] <inf> l6_task: [CONTROL] processed message in 1251 milliseconds
[00:00:02.001,000] <inf> l6_task: [CONTROL] processed seq=3, pub time 1000
[00:00:02.001,000] <inf> l6_task: [CONTROL] processed message in 1001 milliseconds
[00:00:02.001,000] <inf> l6_task: [CONTROL] processed seq=4, pub time 1250
[00:00:02.001,000] <inf> l6_task: [CONTROL] processed message in 751 milliseconds
[00:00:02.001,000] <inf> l6_task: [CONTROL] processed seq=5, pub time 1500
[00:00:02.001,000] <inf> l6_task: [CONTROL] processed message in 501 milliseconds
[00:00:03.500,000] <err> l6_task: Failed message add to queue, sequence no 13
[00:00:03.750,000] <err> l6_task: Failed message add to queue, sequence no 14
[00:00:03.750,000] <inf> l6_task: [CONTROL] processed seq=8, pub time 2250
[00:00:03.750,000] <inf> l6_task: [CONTROL] processed message in 1500 milliseconds
[00:00:03.751,000] <inf> l6_task: [CONTROL] Missed message count 4
[00:00:03.751,000] <inf> l6_task: [CONTROL] processed seq=9, pub time 2500
[00:00:03.751,000] <inf> l6_task: [CONTROL] processed message in 1251 milliseconds
[00:00:03.751,000] <inf> l6_task: [CONTROL] processed seq=10, pub time 2750
[00:00:03.751,000] <inf> l6_task: [CONTROL] processed message in 1001 milliseconds
[00:00:03.751,000] <inf> l6_task: [CONTROL] processed seq=11, pub time 3000
[00:00:03.751,000] <inf> l6_task: [CONTROL] processed message in 751 milliseconds
[00:00:03.751,000] <inf> l6_task: [CONTROL] processed seq=12, pub time 3250
[00:00:03.751,000] <inf> l6_task: [CONTROL] processed message in 501 milliseconds
[00:00:05.250,000] <err> l6_task: Failed message add to queue, sequence no 20
[00:00:05.500,000] <err> l6_task: Failed message add to queue, sequence no 21
[00:00:05.500,000] <inf> l6_task: [CONTROL] processed seq=15, pub time 4000
[00:00:05.500,000] <inf> l6_task: [CONTROL] processed message in 1500 milliseconds
[00:00:05.501,000] <inf> l6_task: [CONTROL] processed seq=16, pub time 4250
[00:00:05.501,000] <inf> l6_task: [CONTROL] processed message in 1251 milliseconds
[00:00:05.501,000] <inf> l6_task: [CONTROL] processed seq=17, pub time 4500
[00:00:05.501,000] <inf> l6_task: [CONTROL] processed message in 1001 milliseconds
[00:00:05.501,000] <inf> l6_task: [CONTROL] processed seq=18, pub time 4750
[00:00:05.501,000] <inf> l6_task: [CONTROL] processed message in 751 milliseconds
[00:00:05.501,000] <inf> l6_task: [CONTROL] processed seq=19, pub time 5000
[00:00:05.501,000] <inf> l6_task: [CONTROL] processed message in 501 milliseconds
[00:00:07.000,000] <err> l6_task: Failed message add to queue, sequence no 27
[00:00:07.250,000] <err> l6_task: Failed message add to queue, sequence no 28
[00:00:07.250,000] <inf> l6_task: [CONTROL] processed seq=22, pub time 5750
[00:00:07.250,000] <inf> l6_task: [CONTROL] processed message in 1500 milliseconds
[00:00:07.251,000] <inf> l6_task: [CONTROL] Missed message count 8
[00:00:07.251,000] <inf> l6_task: [CONTROL] processed seq=23, pub time 6000
[00:00:07.251,000] <inf> l6_task: [CONTROL] processed message in 1251 milliseconds
[00:00:07.251,000] <inf> l6_task: [CONTROL] processed seq=24, pub time 6250
[00:00:07.251,000] <inf> l6_task: [CONTROL] processed message in 1001 milliseconds
[00:00:07.251,000] <inf> l6_task: [CONTROL] processed seq=25, pub time 6500
[00:00:07.251,000] <inf> l6_task: [CONTROL] processed message in 751 milliseconds
[00:00:07.251,000] <inf> l6_task: [CONTROL] processed seq=26, pub time 6750
[00:00:07.251,000] <inf> l6_task: [CONTROL] processed message in 501 milliseconds
[00:00:08.750,000] <err> l6_task: Failed message add to queue, sequence no 34
[00:00:09.000,000] <err> l6_task: Failed message add to queue, sequence no 35
[00:00:09.000,000] <inf> l6_task: [CONTROL] processed seq=29, pub time 7500
[00:00:09.000,000] <inf> l6_task: [CONTROL] processed message in 1500 milliseconds
[00:00:09.001,000] <inf> l6_task: [CONTROL] processed seq=30, pub time 7750
[00:00:09.001,000] <inf> l6_task: [CONTROL] processed message in 1251 milliseconds
[00:00:09.001,000] <inf> l6_task: [CONTROL] processed seq=31, pub time 8000
[00:00:09.001,000] <inf> l6_task: [CONTROL] processed message in 1001 milliseconds
[00:00:09.001,000] <inf> l6_task: [CONTROL] processed seq=32, pub time 8250
[00:00:09.001,000] <inf> l6_task: [CONTROL] processed message in 751 milliseconds
[00:00:09.001,000] <inf> l6_task: [CONTROL] processed seq=33, pub time 8500
[00:00:09.001,000] <inf> l6_task: [CONTROL] processed message in 501 milliseconds
[00:00:10.500,000] <err> l6_task: Failed message add to queue, sequence no 41
[00:00:10.750,000] <err> l6_task: Failed message add to queue, sequence no 42
[00:00:10.750,000] <inf> l6_task: [CONTROL] processed seq=36, pub time 9250
[00:00:10.750,000] <inf> l6_task: [CONTROL] processed message in 1500 milliseconds
[00:00:10.751,000] <inf> l6_task: [CONTROL] Missed message count 12
[00:00:10.751,000] <inf> l6_task: [CONTROL] processed seq=37, pub time 9500
[00:00:10.751,000] <inf> l6_task: [CONTROL] processed message in 1251 milliseconds
[00:00:10.751,000] <inf> l6_task: [CONTROL] processed seq=38, pub time 9750
[00:00:10.751,000] <inf> l6_task: [CONTROL] processed message in 1001 milliseconds
[00:00:10.751,000] <inf> l6_task: [CONTROL] processed seq=39, pub time 10000
[00:00:10.751,000] <inf> l6_task: [CONTROL] processed message in 751 milliseconds
[00:00:10.751,000] <inf> l6_task: [CONTROL] processed seq=40, pub time 10250
[00:00:10.751,000] <inf> l6_task: [CONTROL] processed message in 501 milliseconds
[00:00:12.250,000] <err> l6_task: Failed message add to queue, sequence no 48
[00:00:12.500,000] <err> l6_task: Failed message add to queue, sequence no 49
[00:00:12.500,000] <inf> l6_task: [CONTROL] processed seq=43, pub time 11000
[00:00:12.500,000] <inf> l6_task: [CONTROL] processed message in 1500 milliseconds
[00:00:12.501,000] <inf> l6_task: [CONTROL] processed seq=44, pub time 11250
[00:00:12.501,000] <inf> l6_task: [CONTROL] processed message in 1251 milliseconds
[00:00:12.501,000] <inf> l6_task: [CONTROL] processed seq=45, pub time 11500
[00:00:12.501,000] <inf> l6_task: [CONTROL] processed message in 1001 milliseconds
[00:00:12.501,000] <inf> l6_task: [CONTROL] processed seq=46, pub time 11750
[00:00:12.501,000] <inf> l6_task: [CONTROL] processed message in 751 milliseconds
[00:00:12.501,000] <inf> l6_task: [CONTROL] processed seq=47, pub time 12000
[00:00:12.501,000] <inf> l6_task: [CONTROL] processed message in 501 milliseconds
[00:00:14.000,000] <err> l6_task: Failed message add to queue, sequence no 55
[00:00:14.250,000] <err> l6_task: Failed message add to queue, sequence no 56
[00:00:14.250,000] <inf> l6_task: [CONTROL] processed seq=50, pub time 12750
[00:00:14.250,000] <inf> l6_task: [CONTROL] processed message in 1500 milliseconds
[00:00:14.251,000] <inf> l6_task: [CONTROL] Missed message count 16
[00:00:14.251,000] <inf> l6_task: [CONTROL] processed seq=51, pub time 13000
[00:00:14.251,000] <inf> l6_task: [CONTROL] processed message in 1251 milliseconds
[00:00:14.251,000] <inf> l6_task: [CONTROL] processed seq=52, pub time 13250
[00:00:14.251,000] <inf> l6_task: [CONTROL] processed message in 1001 milliseconds
[00:00:14.251,000] <inf> l6_task: [CONTROL] processed seq=53, pub time 13500
[00:00:14.251,000] <inf> l6_task: [CONTROL] processed message in 751 milliseconds
[00:00:14.251,000] <inf> l6_task: [CONTROL] processed seq=54, pub time 13750
[00:00:14.251,000] <inf> l6_task: [CONTROL] processed message in 501 milliseconds
[00:00:15.750,000] <err> l6_task: Failed message add to queue, sequence no 62
[00:00:16.000,000] <err> l6_task: Failed message add to queue, sequence no 63
[00:00:16.000,000] <inf> l6_task: [CONTROL] processed seq=57, pub time 14500
[00:00:16.000,000] <inf> l6_task: [CONTROL] processed message in 1500 milliseconds
[00:00:16.001,000] <inf> l6_task: [CONTROL] processed seq=58, pub time 14750
[00:00:16.001,000] <inf> l6_task: [CONTROL] processed message in 1251 milliseconds
[00:00:16.001,000] <inf> l6_task: [CONTROL] processed seq=59, pub time 15000
[00:00:16.001,000] <inf> l6_task: [CONTROL] processed message in 1001 milliseconds
[00:00:16.001,000] <inf> l6_task: [CONTROL] processed seq=60, pub time 15250
```

With the maintenance thread configured to run and finish faster than the message
producer interval, dropped message error and queue push failures go away:

```
[00:00:00.000,000] <inf> l6_task: === L6 Homework: Runtime Investigation ===
[00:00:00.000,000] <inf> l6_task: Control work must start within 10 ms
[00:00:00.000,000] <inf> l6_task: Inspect, measure, trace, explain, and correct the delay
M1 - l6
[00:00:00.545,000] <inf> l6_task: [CONTROL] processed seq=1, pub time 500
[00:00:00.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:00.795,000] <inf> l6_task: [CONTROL] processed seq=2, pub time 750
[00:00:00.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:01.045,000] <inf> l6_task: [CONTROL] processed seq=3, pub time 1000
[00:00:01.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:01.295,000] <inf> l6_task: [CONTROL] processed seq=4, pub time 1250
[00:00:01.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:01.545,000] <inf> l6_task: [CONTROL] processed seq=5, pub time 1500
[00:00:01.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:01.795,000] <inf> l6_task: [CONTROL] processed seq=6, pub time 1750
[00:00:01.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:02.045,000] <inf> l6_task: [CONTROL] processed seq=7, pub time 2000
[00:00:02.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:02.295,000] <inf> l6_task: [CONTROL] processed seq=8, pub time 2250
[00:00:02.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:02.545,000] <inf> l6_task: [CONTROL] processed seq=9, pub time 2500
[00:00:02.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:02.795,000] <inf> l6_task: [CONTROL] processed seq=10, pub time 2750
[00:00:02.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:03.045,000] <inf> l6_task: [CONTROL] processed seq=11, pub time 3000
[00:00:03.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:03.295,000] <inf> l6_task: [CONTROL] processed seq=12, pub time 3250
[00:00:03.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:03.545,000] <inf> l6_task: [CONTROL] processed seq=13, pub time 3500
[00:00:03.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:03.795,000] <inf> l6_task: [CONTROL] processed seq=14, pub time 3750
[00:00:03.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:04.045,000] <inf> l6_task: [CONTROL] processed seq=15, pub time 4000
[00:00:04.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:04.295,000] <inf> l6_task: [CONTROL] processed seq=16, pub time 4250
[00:00:04.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:04.545,000] <inf> l6_task: [CONTROL] processed seq=17, pub time 4500
[00:00:04.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:04.795,000] <inf> l6_task: [CONTROL] processed seq=18, pub time 4750
[00:00:04.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:05.045,000] <inf> l6_task: [CONTROL] processed seq=19, pub time 5000
[00:00:05.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:05.295,000] <inf> l6_task: [CONTROL] processed seq=20, pub time 5250
[00:00:05.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:05.545,000] <inf> l6_task: [CONTROL] processed seq=21, pub time 5500
[00:00:05.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:05.795,000] <inf> l6_task: [CONTROL] processed seq=22, pub time 5750
[00:00:05.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:06.045,000] <inf> l6_task: [CONTROL] processed seq=23, pub time 6000
[00:00:06.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:06.295,000] <inf> l6_task: [CONTROL] processed seq=24, pub time 6250
[00:00:06.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:06.545,000] <inf> l6_task: [CONTROL] processed seq=25, pub time 6500
[00:00:06.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:06.795,000] <inf> l6_task: [CONTROL] processed seq=26, pub time 6750
[00:00:06.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:07.045,000] <inf> l6_task: [CONTROL] processed seq=27, pub time 7000
[00:00:07.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:07.295,000] <inf> l6_task: [CONTROL] processed seq=28, pub time 7250
[00:00:07.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:07.545,000] <inf> l6_task: [CONTROL] processed seq=29, pub time 7500
[00:00:07.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:07.795,000] <inf> l6_task: [CONTROL] processed seq=30, pub time 7750
[00:00:07.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:08.045,000] <inf> l6_task: [CONTROL] processed seq=31, pub time 8000
[00:00:08.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:08.295,000] <inf> l6_task: [CONTROL] processed seq=32, pub time 8250
[00:00:08.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:08.545,000] <inf> l6_task: [CONTROL] processed seq=33, pub time 8500
[00:00:08.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:08.795,000] <inf> l6_task: [CONTROL] processed seq=34, pub time 8750
[00:00:08.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:09.045,000] <inf> l6_task: [CONTROL] processed seq=35, pub time 9000
[00:00:09.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:09.295,000] <inf> l6_task: [CONTROL] processed seq=36, pub time 9250
[00:00:09.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:09.545,000] <inf> l6_task: [CONTROL] processed seq=37, pub time 9500
[00:00:09.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:09.795,000] <inf> l6_task: [CONTROL] processed seq=38, pub time 9750
[00:00:09.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:10.045,000] <inf> l6_task: [CONTROL] processed seq=39, pub time 10000
[00:00:10.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:10.295,000] <inf> l6_task: [CONTROL] processed seq=40, pub time 10250
[00:00:10.295,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:10.545,000] <inf> l6_task: [CONTROL] processed seq=41, pub time 10500
[00:00:10.545,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:10.795,000] <inf> l6_task: [CONTROL] processed seq=42, pub time 10750
[00:00:10.795,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
[00:00:11.045,000] <inf> l6_task: [CONTROL] processed seq=43, pub time 11000
[00:00:11.045,000] <inf> l6_task: [CONTROL] processed message in 45 milliseconds
```

## Thread Inspection on Real Hardware

With the following thread priorities and delay parameter settings:

```
LOG_MODULE_REGISTER(l6_task, LOG_LEVEL_INF);

#define STACK_SIZE            2048
#define CONTROL_PRIORITY         7
#define MAINTENANCE_PRIORITY     4
#define EVENT_PERIOD_MS        250
#define MAINTENANCE_LOAD_US 300000
```

. . . a shell based inspection of thread states gives:

_Exerpt 1: thread state from lecture 6 homework app_

```
uart:~$ kernel thread list

Scheduler: 3 since last call
Threads:
 0x200001c8 maintenance
        options: 0x0, priority: 4 timeout: 0
        state: pending, entry: 0x80005ed
        stack size 2048, unused 1880, usage 168 / 2048 (8 %)

 0x20000280 control
        options: 0x0, priority: 7 timeout: 0
        state: pending, entry: 0x800060d
        stack size 2048, unused 1848, usage 200 / 2048 (9 %)

*0x20000840 shell_uart
        options: 0x0, priority: 14 timeout: -9223372036854775808
        state: queued, entry: 0x800391d
        stack size 2048, unused 1040, usage 1008 / 2048 (49 %)

 0x20000348 logging
        options: 0x0, priority: 14 timeout: 321
        state: pending, entry: 0x8001841
        stack size 768, unused 456, usage 312 / 768 (40 %)

 0x20000bc8 idle
        options: 0x1, priority: 15 timeout: 0
        state: , entry: 0x800aead
        stack size 320, unused 256, usage 64 / 320 (20 %)
```

## How To Build For native_sim

To compile for native_sim, invoke meta-tool west as follows:

```
west build -b native_sim -p always
```

## Running The App On native_sim

native_sim, something of an emulated or virtualized hardware which runs on the
dev host, the applicatio may be run with:

```
./build/zephyr/zephyr.exe -trace-file=/path/to/trace_file/channel0_0
```

The trace file may be viewed at a Linux command line, with babeltrace2
installed. The way to invoke this command is:

```
babeltrace2 /path/to/trace_file
```
