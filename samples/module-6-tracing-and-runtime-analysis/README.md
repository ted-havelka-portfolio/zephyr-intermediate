# Scheduling Delay Investigation

This work goes with Iomico Zephyr Intermediate lecture series, lecture 6.
The application primarily demonstrates and explores some common scheduling
delay scenarios, and ways to log and to trace them.

This work is tagged l6-task1 in the forked code repository.

Some things to note in this task:

| Run-time actor | Implemented by                                       |
| :------------- | :--------------------------------------------------- |
| producer       | A kernel timer and callback, to fill a message queue |
| consumer       | A thread named "control"; reads a message queue      |
| other CPU load | A thread named "maintenance"; calls k_busy_wait()    |

The maintenance thread simulate "heavy" CPU work.  The call to k_busy_wait()
uses CPU time, and when this thread's priority is higher than that of the
control thread, the control thread remains pending until that simulated work
of the maintenance thread completes.

Note, the timer runs in interrupt context, so it preempts whatever thread may be
running.

## How To Build

To compile for native_sim, invoke meta-tool west as follows:

```
west build -b native_sim -p always
```

To build for nucleo_l432kc, call west with options and args:

Part of Zephyr's tracing libraries depends on their being an element, or special
device node property in the Zephyr "chosen" device tree node. That property is
expressed in the line which contains "tracing-uart" in this device tree code
snippet:

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

So, make sure that a file named app.overlay, or a specific DTS board overlay
file adds this to the project's "Zephyr chosen" device tree node.

Note that the native_sim board does not expect a UART device tree node.  The DTS
overlay file which defines such a node for the Nucleo board causes a build error
when present during a native_sim build.

## How To Flash

```
west flash -r openocd
```

# Running the Producer-Consumer Application

On a board that's separate from the host with the build and test environment,
the app normally begins to run immediately after the board is flashed.  For
native_sim, something of an emulated or virtualized hardware which runs on the
dev host, the applicatio may be run with:

```
./build/zephyr/zephyr.exe -trace-file=/path/to/trace_file/channel0_0
```

The trace file may be viewed at a Linux command line, with babeltrace2
installed.  The way to invoke this command is:

```
babeltrace2 /path/to/trace_file
```

## Selected Outputs from Real Hardware

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

