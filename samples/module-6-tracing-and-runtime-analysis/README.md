l6-task1

# Scheduling Delay Investgation

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

## How To Flash

```
west flash -r openocd
```

## Running the Producer-Consumer Application
