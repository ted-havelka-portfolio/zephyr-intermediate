# Zephyr Intermediate Course - Lecture 5 Memory Resource Constraints

The task and application documentation here is based on the Zephyr zbus
exploration work from the fourth module lecture.

A small but notable change in the app, from lecture 4 developed code to
lecture 5, is:

```
ZBUS_MSG_SUBSCRIBER_DEFINE(l4_subscriber);

// becomes:

ZBUS_SUBSCRIBER_DEFINE(l5_subscriber, CONFIG_SUBSCRIBER_QUEUE_SIZE);
```

The macros are similar, but the second one gives us a parameter to choose queue
size.  This is helpful (and necessary) for the part of the assignment which
requires a run-time report of moments when the message queue fills above
seventy five percent capacity.

See the references section of this readme for links to Zephyr topics used in
this app.

## How To Compile For nucleo_l432kc

The development board used initially for this work is the nucleo_l432kc by
STMicro.  To compile the app, in a Zephyr workspace issue the build call to
'west' as follows:

```shell
west build -b nucleo_l432kc -p always
```

To build for a different board, in general it is only necessary to change the 
board name, which comes after the '-b' in the command to build.

## How To Flash

For many target boards a simple call to west, to flash, should work:

```
west flash -r openocd
```

When it comes to Zephyr 4.4.0 used at the time of writing this readme, the
meta-tool west (or one of its called flash scripts) defaults to looking for
the runner for stm32cubeprogrammer.  Passing the -r option with a more
general flash utility supports correct firmware flashing, without a need to
install stm32cubeprogrammer.

## Application Output

This lecture 5 application has several parameters which all influence behavior.
Their ranges of possible values allow for many permutations of successful, and
to varying degrees, lagging (data lossy) outcomes at run time.  App ouput in
figure one shows threads at work, where a producer sends 20 simluated sensor
readings at an interval of 150 milliseconds.  A consumer thread is configured
to take 320 milliseconds to process each message.  This consumer falls behind,
and the zbus based message queue fills to capacity.  A health monitoring thread
catches this state.

Note:  a listener from lecture 4 application is present, and runs quickly enough
to keep up with the message producer (publisher).

_Figure 1: l5-task1 output with fast producer slow consumer_


```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> l5_task1: === Lecture 5 task 1: Memory Resource Constraints ===
[00:00:00.000,000] <inf> l5_task1: * Sensor publishes every 150 ms
[00:00:00.000,000] <inf> l5_task1: * Logging thread runs a little over 225 ms
[00:00:00.000,000] <inf> l5_task1: * Display listener runs in publisher context
[00:00:00.000,000] <inf> l5_task1: * Logging thread uses message subscriber copies
[00:00:00.000,000] <inf> l5_task1: About to publish 20 simulated sensor readings . . .
[00:00:00.000,000] <inf> l5_task1: [SENSOR] publish seq=1 accel x=0, y=0, z=0
[00:00:00.000,000] <inf> l5_task1: From l5 listener -> Acc x=0, y=0, z=0
[00:00:00.000,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=1 x=0 y=0 z=0 latency=0ms
[00:00:00.150,000] <inf> l5_task1: [SENSOR] publish seq=2 accel x=10, y=10, z=10
[00:00:00.150,000] <inf> l5_task1: From l5 listener -> Acc x=10, y=10, z=10
[00:00:00.225,000] <inf> l5_task1: [LOGGING] sequence numbers 0, 1 look good, feeding task watchdog . . .
[00:00:00.225,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=2 x=10 y=10 z=10 latency=225ms
[00:00:00.300,000] <inf> l5_task1: [SENSOR] publish seq=3 accel x=20, y=20, z=20
[00:00:00.300,000] <inf> l5_task1: From l5 listener -> Acc x=20, y=20, z=20
[00:00:00.450,000] <inf> l5_task1: [LOGGING] sequence numbers 1, 2 look good, feeding task watchdog . . .
[00:00:00.450,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=3 x=20 y=20 z=20 latency=450ms
[00:00:00.450,000] <inf> l5_task1: [SENSOR] publish seq=4 accel x=30, y=30, z=30
[00:00:00.450,000] <inf> l5_task1: From l5 listener -> Acc x=30, y=30, z=30
[00:00:00.601,000] <inf> l5_task1: [SENSOR] publish seq=5 accel x=40, y=40, z=40
[00:00:00.601,000] <inf> l5_task1: From l5 listener -> Acc x=40, y=40, z=40
[00:00:00.675,000] <inf> l5_task1: [LOGGING] sequence numbers 2, 3 look good, feeding task watchdog . . .
[00:00:00.675,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=5 x=40 y=40 z=40 latency=675ms
[00:00:00.751,000] <inf> l5_task1: [SENSOR] publish seq=6 accel x=50, y=50, z=50
[00:00:00.751,000] <inf> l5_task1: From l5 listener -> Acc x=50, y=50, z=50
[00:00:00.901,000] <err> l5_task1: [LOGGING] Sequence numbers 3, 5 not consecutive!
[00:00:00.901,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:00.901,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=6 x=50 y=50 z=50 latency=901ms
[00:00:00.901,000] <inf> l5_task1: [SENSOR] publish seq=7 accel x=60, y=60, z=60
[00:00:00.901,000] <inf> l5_task1: From l5 listener -> Acc x=60, y=60, z=60
[00:00:00.976,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:00.976,000] <err> l5_task1:   (Could reset here)
[00:00:01.051,000] <inf> l5_task1: [SENSOR] publish seq=8 accel x=70, y=70, z=70
[00:00:01.051,000] <inf> l5_task1: From l5 listener -> Acc x=70, y=70, z=70
[00:00:01.126,000] <inf> l5_task1: [LOGGING] sequence numbers 5, 6 look good, feeding task watchdog . . .
[00:00:01.126,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=8 x=70 y=70 z=70 latency=1126ms
[00:00:01.201,000] <inf> l5_task1: [SENSOR] publish seq=9 accel x=80, y=80, z=80
[00:00:01.201,000] <inf> l5_task1: From l5 listener -> Acc x=80, y=80, z=80
[00:00:01.351,000] <err> l5_task1: [LOGGING] Sequence numbers 6, 8 not consecutive!
[00:00:01.351,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:01.351,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=9 x=80 y=80 z=80 latency=1351ms
[00:00:01.352,000] <inf> l5_task1: [SENSOR] publish seq=10 accel x=90, y=90, z=90
[00:00:01.352,000] <inf> l5_task1: From l5 listener -> Acc x=90, y=90, z=90
[00:00:01.426,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:01.426,000] <err> l5_task1:   (Could reset here)
[00:00:01.502,000] <inf> l5_task1: [SENSOR] publish seq=11 accel x=100, y=100, z=100
[00:00:01.502,000] <inf> l5_task1: From l5 listener -> Acc x=100, y=100, z=100
[00:00:01.576,000] <inf> l5_task1: [LOGGING] sequence numbers 8, 9 look good, feeding task watchdog . . .
[00:00:01.576,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=11 x=100 y=100 z=100 latency=1576ms
[00:00:01.652,000] <inf> l5_task1: [SENSOR] publish seq=12 accel x=110, y=110, z=110
[00:00:01.652,000] <inf> l5_task1: From l5 listener -> Acc x=110, y=110, z=110
[00:00:01.801,000] <err> l5_task1: [LOGGING] Sequence numbers 9, 11 not consecutive!
[00:00:01.801,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:01.801,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=12 x=110 y=110 z=110 latency=1801ms
[00:00:01.802,000] <inf> l5_task1: [SENSOR] publish seq=13 accel x=120, y=120, z=120
[00:00:01.802,000] <inf> l5_task1: From l5 listener -> Acc x=120, y=120, z=120
[00:00:01.876,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:01.876,000] <err> l5_task1:   (Could reset here)
[00:00:01.952,000] <inf> l5_task1: [SENSOR] publish seq=14 accel x=130, y=130, z=130
[00:00:01.952,000] <inf> l5_task1: From l5 listener -> Acc x=130, y=130, z=130
[00:00:02.000,000] <wrn> l5_task1: Queue at or over capacity of 75%
[00:00:02.027,000] <inf> l5_task1: [LOGGING] sequence numbers 11, 12 look good, feeding task watchdog . . .
[00:00:02.027,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=14 x=130 y=130 z=130 latency=2027ms
[00:00:02.103,000] <inf> l5_task1: [SENSOR] publish seq=15 accel x=140, y=140, z=140
[00:00:02.103,000] <inf> l5_task1: From l5 listener -> Acc x=140, y=140, z=140
[00:00:02.252,000] <err> l5_task1: [LOGGING] Sequence numbers 12, 14 not consecutive!
[00:00:02.252,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:02.252,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=15 x=140 y=140 z=140 latency=2252ms
[00:00:02.253,000] <inf> l5_task1: [SENSOR] publish seq=16 accel x=150, y=150, z=150
[00:00:02.253,000] <inf> l5_task1: From l5 listener -> Acc x=150, y=150, z=150
[00:00:02.327,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:02.327,000] <err> l5_task1:   (Could reset here)
[00:00:02.403,000] <inf> l5_task1: [SENSOR] publish seq=17 accel x=160, y=160, z=160
[00:00:02.403,000] <inf> l5_task1: From l5 listener -> Acc x=160, y=160, z=160
[00:00:02.477,000] <inf> l5_task1: [LOGGING] sequence numbers 14, 15 look good, feeding task watchdog . . .
[00:00:02.477,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=17 x=160 y=160 z=160 latency=2477ms
[00:00:02.627,000] <inf> l5_task1: [SENSOR] publish seq=18 accel x=170, y=170, z=170
[00:00:02.627,000] <inf> l5_task1: From l5 listener -> Acc x=170, y=170, z=170
[00:00:02.702,000] <err> l5_task1: [LOGGING] Sequence numbers 15, 17 not consecutive!
[00:00:02.702,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:02.702,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=18 x=170 y=170 z=170 latency=2702ms
[00:00:02.777,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:02.777,000] <err> l5_task1:   (Could reset here)
[00:00:02.852,000] <inf> l5_task1: [SENSOR] publish seq=19 accel x=180, y=180, z=180
[00:00:02.852,000] <inf> l5_task1: From l5 listener -> Acc x=180, y=180, z=180
[00:00:02.927,000] <inf> l5_task1: [LOGGING] sequence numbers 17, 18 look good, feeding task watchdog . . .
[00:00:02.928,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=180 y=180 z=180 latency=2928ms
[00:00:03.000,000] <wrn> l5_task1: Queue at or over capacity of 75%
[00:00:03.078,000] <inf> l5_task1: [SENSOR] publish seq=20 accel x=190, y=190, z=190
[00:00:03.078,000] <inf> l5_task1: From l5 listener -> Acc x=190, y=190, z=190
[00:00:03.153,000] <inf> l5_task1: [LOGGING] sequence numbers 18, 19 look good, feeding task watchdog . . .
[00:00:03.153,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=3153ms
[00:00:03.303,000] <inf> l5_task1: * [SENSOR] done  *
[00:00:03.303,000] <inf> l5_task1: [MAIN] Producer thread done.
[00:00:03.378,000] <inf> l5_task1: [LOGGING] sequence numbers 19, 20 look good, feeding task watchdog . . .
[00:00:03.378,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=3378ms
[00:00:03.603,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:03.603,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:03.603,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=3603ms
[00:00:03.678,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:03.678,000] <err> l5_task1:   (Could reset here)
[00:00:03.828,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:03.828,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:03.828,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=3828ms
[00:00:04.053,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:04.053,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:04.053,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=4053ms
[00:00:04.279,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:04.279,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:04.279,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=4279ms
[00:00:04.504,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:04.504,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:04.504,000] <inf> l5_task1: [LOGGING] done, received=20 messages
[00:00:04.504,000] <inf> l5_task1: [MAIN] Consumer thread done.
[00:00:05.000,000] <inf> l5_task1: [HEALTH] Done.
[00:00:05.000,000] <inf> l5_task1: [MAIN] Health thread done.
[00:00:05.000,000] <inf> l5_task1: [MAIN] Producer, consumer and health monitor all completed their work.
[00:00:05.000,000] <inf> l5_task1: [MAIN] App l5-task1 done.

```

When the consumer thread work latency is set low enough to feed task watchdog:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> l5_task1: === Lecture 5 task 1: Memory Resource Constraints ===
[00:00:00.000,000] <inf> l5_task1: * Sensor publishes every 150 ms
[00:00:00.000,000] <inf> l5_task1: * Logging thread runs a little over 32 ms
[00:00:00.000,000] <inf> l5_task1: * Display listener runs in publisher context
[00:00:00.000,000] <inf> l5_task1: * Logging thread uses message subscriber copies
[00:00:00.000,000] <inf> l5_task1: About to publish 20 simulated sensor readings . . .
[00:00:00.000,000] <inf> l5_task1: [SENSOR] publish seq=1 accel x=0, y=0, z=0
[00:00:00.000,000] <inf> l5_task1: From l5 listener -> Acc x=0, y=0, z=0
[00:00:00.000,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=1 x=0 y=0 z=0 latency=0ms
[00:00:00.032,000] <inf> l5_task1: [LOGGING] sequence numbers 0, 1 look good, feeding task watchdog . . .
[00:00:00.150,000] <inf> l5_task1: [SENSOR] publish seq=2 accel x=10, y=10, z=10
[00:00:00.150,000] <inf> l5_task1: From l5 listener -> Acc x=10, y=10, z=10
[00:00:00.150,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=2 x=10 y=10 z=10 latency=150ms
[00:00:00.182,000] <inf> l5_task1: [LOGGING] sequence numbers 1, 2 look good, feeding task watchdog . . .
[00:00:00.300,000] <inf> l5_task1: [SENSOR] publish seq=3 accel x=20, y=20, z=20
[00:00:00.300,000] <inf> l5_task1: From l5 listener -> Acc x=20, y=20, z=20
[00:00:00.300,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=3 x=20 y=20 z=20 latency=300ms
[00:00:00.332,000] <inf> l5_task1: [LOGGING] sequence numbers 2, 3 look good, feeding task watchdog . . .
[00:00:00.450,000] <inf> l5_task1: [SENSOR] publish seq=4 accel x=30, y=30, z=30
[00:00:00.450,000] <inf> l5_task1: From l5 listener -> Acc x=30, y=30, z=30
[00:00:00.450,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=4 x=30 y=30 z=30 latency=450ms
[00:00:00.483,000] <inf> l5_task1: [LOGGING] sequence numbers 3, 4 look good, feeding task watchdog . . .
[00:00:00.601,000] <inf> l5_task1: [SENSOR] publish seq=5 accel x=40, y=40, z=40
[00:00:00.601,000] <inf> l5_task1: From l5 listener -> Acc x=40, y=40, z=40
[00:00:00.601,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=5 x=40 y=40 z=40 latency=601ms
[00:00:00.633,000] <inf> l5_task1: [LOGGING] sequence numbers 4, 5 look good, feeding task watchdog . . .
[00:00:00.751,000] <inf> l5_task1: [SENSOR] publish seq=6 accel x=50, y=50, z=50
[00:00:00.751,000] <inf> l5_task1: From l5 listener -> Acc x=50, y=50, z=50
[00:00:00.751,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=6 x=50 y=50 z=50 latency=751ms
[00:00:00.783,000] <inf> l5_task1: [LOGGING] sequence numbers 5, 6 look good, feeding task watchdog . . .
[00:00:00.901,000] <inf> l5_task1: [SENSOR] publish seq=7 accel x=60, y=60, z=60
[00:00:00.901,000] <inf> l5_task1: From l5 listener -> Acc x=60, y=60, z=60
[00:00:00.901,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=7 x=60 y=60 z=60 latency=901ms
[00:00:00.933,000] <inf> l5_task1: [LOGGING] sequence numbers 6, 7 look good, feeding task watchdog . . .
[00:00:01.051,000] <inf> l5_task1: [SENSOR] publish seq=8 accel x=70, y=70, z=70
[00:00:01.051,000] <inf> l5_task1: From l5 listener -> Acc x=70, y=70, z=70
[00:00:01.051,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=8 x=70 y=70 z=70 latency=1051ms
[00:00:01.083,000] <inf> l5_task1: [LOGGING] sequence numbers 7, 8 look good, feeding task watchdog . . .
[00:00:01.201,000] <inf> l5_task1: [SENSOR] publish seq=9 accel x=80, y=80, z=80
[00:00:01.201,000] <inf> l5_task1: From l5 listener -> Acc x=80, y=80, z=80
[00:00:01.201,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=9 x=80 y=80 z=80 latency=1201ms
[00:00:01.234,000] <inf> l5_task1: [LOGGING] sequence numbers 8, 9 look good, feeding task watchdog . . .
[00:00:01.352,000] <inf> l5_task1: [SENSOR] publish seq=10 accel x=90, y=90, z=90
[00:00:01.352,000] <inf> l5_task1: From l5 listener -> Acc x=90, y=90, z=90
[00:00:01.352,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=10 x=90 y=90 z=90 latency=1352ms
[00:00:01.384,000] <inf> l5_task1: [LOGGING] sequence numbers 9, 10 look good, feeding task watchdog . . .
[00:00:01.502,000] <inf> l5_task1: [SENSOR] publish seq=11 accel x=100, y=100, z=100
[00:00:01.502,000] <inf> l5_task1: From l5 listener -> Acc x=100, y=100, z=100
[00:00:01.502,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=11 x=100 y=100 z=100 latency=1502ms
[00:00:01.534,000] <inf> l5_task1: [LOGGING] sequence numbers 10, 11 look good, feeding task watchdog . . .
[00:00:01.652,000] <inf> l5_task1: [SENSOR] publish seq=12 accel x=110, y=110, z=110
[00:00:01.652,000] <inf> l5_task1: From l5 listener -> Acc x=110, y=110, z=110
[00:00:01.652,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=12 x=110 y=110 z=110 latency=1652ms
[00:00:01.684,000] <inf> l5_task1: [LOGGING] sequence numbers 11, 12 look good, feeding task watchdog . . .
[00:00:01.802,000] <inf> l5_task1: [SENSOR] publish seq=13 accel x=120, y=120, z=120
[00:00:01.802,000] <inf> l5_task1: From l5 listener -> Acc x=120, y=120, z=120
[00:00:01.802,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=13 x=120 y=120 z=120 latency=1802ms
[00:00:01.834,000] <inf> l5_task1: [LOGGING] sequence numbers 12, 13 look good, feeding task watchdog . . .
[00:00:01.952,000] <inf> l5_task1: [SENSOR] publish seq=14 accel x=130, y=130, z=130
[00:00:01.952,000] <inf> l5_task1: From l5 listener -> Acc x=130, y=130, z=130
[00:00:01.952,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=14 x=130 y=130 z=130 latency=1952ms
[00:00:01.985,000] <inf> l5_task1: [LOGGING] sequence numbers 13, 14 look good, feeding task watchdog . . .
[00:00:02.103,000] <inf> l5_task1: [SENSOR] publish seq=15 accel x=140, y=140, z=140
[00:00:02.103,000] <inf> l5_task1: From l5 listener -> Acc x=140, y=140, z=140
[00:00:02.103,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=15 x=140 y=140 z=140 latency=2103ms
[00:00:02.135,000] <inf> l5_task1: [LOGGING] sequence numbers 14, 15 look good, feeding task watchdog . . .
[00:00:02.253,000] <inf> l5_task1: [SENSOR] publish seq=16 accel x=150, y=150, z=150
[00:00:02.253,000] <inf> l5_task1: From l5 listener -> Acc x=150, y=150, z=150
[00:00:02.253,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=16 x=150 y=150 z=150 latency=2253ms
[00:00:02.285,000] <inf> l5_task1: [LOGGING] sequence numbers 15, 16 look good, feeding task watchdog . . .
[00:00:02.403,000] <inf> l5_task1: [SENSOR] publish seq=17 accel x=160, y=160, z=160
[00:00:02.403,000] <inf> l5_task1: From l5 listener -> Acc x=160, y=160, z=160
[00:00:02.403,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=17 x=160 y=160 z=160 latency=2403ms
[00:00:02.435,000] <inf> l5_task1: [LOGGING] sequence numbers 16, 17 look good, feeding task watchdog . . .
[00:00:02.553,000] <inf> l5_task1: [SENSOR] publish seq=18 accel x=170, y=170, z=170
[00:00:02.553,000] <inf> l5_task1: From l5 listener -> Acc x=170, y=170, z=170
[00:00:02.553,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=18 x=170 y=170 z=170 latency=2553ms
[00:00:02.585,000] <inf> l5_task1: [LOGGING] sequence numbers 17, 18 look good, feeding task watchdog . . .
[00:00:02.703,000] <inf> l5_task1: [SENSOR] publish seq=19 accel x=180, y=180, z=180
[00:00:02.703,000] <inf> l5_task1: From l5 listener -> Acc x=180, y=180, z=180
[00:00:02.703,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=180 y=180 z=180 latency=2703ms
[00:00:02.736,000] <inf> l5_task1: [LOGGING] sequence numbers 18, 19 look good, feeding task watchdog . . .
[00:00:02.854,000] <inf> l5_task1: [SENSOR] publish seq=20 accel x=190, y=190, z=190
[00:00:02.854,000] <inf> l5_task1: From l5 listener -> Acc x=190, y=190, z=190
[00:00:02.854,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=2854ms
[00:00:02.886,000] <inf> l5_task1: [LOGGING] sequence numbers 19, 20 look good, feeding task watchdog . . .
[00:00:02.886,000] <inf> l5_task1: [LOGGING] done, received=20 messages
[00:00:03.004,000] <inf> l5_task1: * [SENSOR] done  *
[00:00:03.004,000] <inf> l5_task1: [MAIN] Producer thread done.
[00:00:03.004,000] <inf> l5_task1: [MAIN] Consumer thread done.
[00:00:03.186,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:03.186,000] <err> l5_task1:   (Could reset here)
[00:00:04.000,000] <inf> l5_task1: [HEALTH] Done.
[00:00:04.000,000] <inf> l5_task1: [MAIN] Health thread done.
[00:00:04.000,000] <inf> l5_task1: [MAIN] Producer, consumer and health monitor all completed their work.
[00:00:04.000,000] <inf> l5_task1: [MAIN] App l5-task1 done.
```

When the consumer (logging) thread work latency is set high enough, the message
queue soon fills to 100% capacity.  The health thread still first warns of 75%
queue use reached.  The task watchdog times out on work lost, but once the queue
is full, messages cannot even be saved to the queue:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> l5_task1: === Lecture 5 task 1: Memory Resource Constraints ===
[00:00:00.000,000] <inf> l5_task1: * Sensor publishes every 150 ms
[00:00:00.000,000] <inf> l5_task1: * Logging thread runs a little over 600 ms
[00:00:00.000,000] <inf> l5_task1: * Display listener runs in publisher context
[00:00:00.000,000] <inf> l5_task1: * Logging thread uses message subscriber copies
[00:00:00.000,000] <inf> l5_task1: About to publish 20 simulated sensor readings . . .
[00:00:00.000,000] <inf> l5_task1: [SENSOR] publish seq=1 accel x=0, y=0, z=0
[00:00:00.000,000] <inf> l5_task1: From l5 listener -> Acc x=0, y=0, z=0
[00:00:00.000,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=1 x=0 y=0 z=0 latency=0ms
[00:00:00.150,000] <inf> l5_task1: [SENSOR] publish seq=2 accel x=10, y=10, z=10
[00:00:00.150,000] <inf> l5_task1: From l5 listener -> Acc x=10, y=10, z=10
[00:00:00.300,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:00.300,000] <err> l5_task1:   (Could reset here)
[00:00:00.300,000] <inf> l5_task1: [SENSOR] publish seq=3 accel x=20, y=20, z=20
[00:00:00.300,000] <inf> l5_task1: From l5 listener -> Acc x=20, y=20, z=20
[00:00:00.450,000] <inf> l5_task1: [SENSOR] publish seq=4 accel x=30, y=30, z=30
[00:00:00.450,000] <inf> l5_task1: From l5 listener -> Acc x=30, y=30, z=30
[00:00:00.600,000] <inf> l5_task1: [LOGGING] sequence numbers 0, 1 look good, feeding task watchdog . . .
[00:00:00.600,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=4 x=30 y=30 z=30 latency=600ms
[00:00:00.601,000] <inf> l5_task1: [SENSOR] publish seq=5 accel x=40, y=40, z=40
[00:00:00.601,000] <inf> l5_task1: From l5 listener -> Acc x=40, y=40, z=40
[00:00:00.751,000] <inf> l5_task1: [SENSOR] publish seq=6 accel x=50, y=50, z=50
[00:00:00.751,000] <inf> l5_task1: From l5 listener -> Acc x=50, y=50, z=50
[00:00:00.900,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:00.900,000] <err> l5_task1:   (Could reset here)
[00:00:00.901,000] <inf> l5_task1: [SENSOR] publish seq=7 accel x=60, y=60, z=60
[00:00:00.901,000] <inf> l5_task1: From l5 listener -> Acc x=60, y=60, z=60
[00:00:01.000,000] <wrn> l5_task1: Queue at or over capacity of 75%
[00:00:01.051,000] <inf> l5_task1: [SENSOR] publish seq=8 accel x=70, y=70, z=70
[00:00:01.051,000] <inf> l5_task1: From l5 listener -> Acc x=70, y=70, z=70
[00:00:01.151,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:01.151,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:01.200,000] <err> l5_task1: [LOGGING] Sequence numbers 1, 4 not consecutive!
[00:00:01.200,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:01.200,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=8 x=70 y=70 z=70 latency=1200ms
[00:00:01.302,000] <inf> l5_task1: [SENSOR] publish seq=9 accel x=80, y=80, z=80
[00:00:01.302,000] <inf> l5_task1: From l5 listener -> Acc x=80, y=80, z=80
[00:00:01.452,000] <inf> l5_task1: [SENSOR] publish seq=10 accel x=90, y=90, z=90
[00:00:01.452,000] <inf> l5_task1: From l5 listener -> Acc x=90, y=90, z=90
[00:00:01.552,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:01.552,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:01.702,000] <inf> l5_task1: [SENSOR] publish seq=11 accel x=100, y=100, z=100
[00:00:01.702,000] <inf> l5_task1: From l5 listener -> Acc x=100, y=100, z=100
[00:00:01.801,000] <err> l5_task1: [LOGGING] Sequence numbers 4, 8 not consecutive!
[00:00:01.801,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:01.801,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=11 x=100 y=100 z=100 latency=1801ms
[00:00:01.951,000] <inf> l5_task1: [SENSOR] publish seq=12 accel x=110, y=110, z=110
[00:00:01.951,000] <inf> l5_task1: From l5 listener -> Acc x=110, y=110, z=110
[00:00:02.000,000] <wrn> l5_task1: Queue at or over capacity of 75%
[00:00:02.051,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:02.051,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:02.201,000] <inf> l5_task1: [SENSOR] publish seq=13 accel x=120, y=120, z=120
[00:00:02.201,000] <inf> l5_task1: From l5 listener -> Acc x=120, y=120, z=120
[00:00:02.301,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:02.301,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:02.401,000] <err> l5_task1: [LOGGING] Sequence numbers 8, 11 not consecutive!
[00:00:02.401,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:02.401,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=13 x=120 y=120 z=120 latency=2401ms
[00:00:02.451,000] <inf> l5_task1: [SENSOR] publish seq=14 accel x=130, y=130, z=130
[00:00:02.451,000] <inf> l5_task1: From l5 listener -> Acc x=130, y=130, z=130
[00:00:02.602,000] <inf> l5_task1: [SENSOR] publish seq=15 accel x=140, y=140, z=140
[00:00:02.602,000] <inf> l5_task1: From l5 listener -> Acc x=140, y=140, z=140
[00:00:02.702,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:02.702,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:02.852,000] <inf> l5_task1: [SENSOR] publish seq=16 accel x=150, y=150, z=150
[00:00:02.852,000] <inf> l5_task1: From l5 listener -> Acc x=150, y=150, z=150
[00:00:02.952,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:02.952,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:03.000,000] <wrn> l5_task1: Queue at or over capacity of 75%
[00:00:03.001,000] <err> l5_task1: [LOGGING] Sequence numbers 11, 13 not consecutive!
[00:00:03.001,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:03.001,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=16 x=150 y=150 z=150 latency=3001ms
[00:00:03.102,000] <inf> l5_task1: [SENSOR] publish seq=17 accel x=160, y=160, z=160
[00:00:03.102,000] <inf> l5_task1: From l5 listener -> Acc x=160, y=160, z=160
[00:00:03.253,000] <inf> l5_task1: [SENSOR] publish seq=18 accel x=170, y=170, z=170
[00:00:03.253,000] <inf> l5_task1: From l5 listener -> Acc x=170, y=170, z=170
[00:00:03.353,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:03.353,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:03.503,000] <inf> l5_task1: [SENSOR] publish seq=19 accel x=180, y=180, z=180
[00:00:03.503,000] <inf> l5_task1: From l5 listener -> Acc x=180, y=180, z=180
[00:00:03.601,000] <err> l5_task1: [LOGGING] Sequence numbers 13, 16 not consecutive!
[00:00:03.601,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:03.601,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=180 y=180 z=180 latency=3601ms
[00:00:03.751,000] <inf> l5_task1: [SENSOR] publish seq=20 accel x=190, y=190, z=190
[00:00:03.751,000] <inf> l5_task1: From l5 listener -> Acc x=190, y=190, z=190
[00:00:03.852,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:03.852,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:04.000,000] <wrn> l5_task1: Queue at or over capacity of 75%
[00:00:04.002,000] <inf> l5_task1: * [SENSOR] done  *
[00:00:04.002,000] <inf> l5_task1: [MAIN] Producer thread done.
[00:00:04.201,000] <err> l5_task1: [LOGGING] Sequence numbers 16, 19 not consecutive!
[00:00:04.201,000] <wrn> l5_task1: [LOGGING] Detected work lost
[00:00:04.201,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=4201ms
[00:00:04.802,000] <inf> l5_task1: [LOGGING] sequence numbers 19, 20 look good, feeding task watchdog . . .
[00:00:04.802,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=4802ms
[00:00:05.102,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:05.102,000] <err> l5_task1:   (Could reset here)
[00:00:05.402,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:05.402,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:05.402,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=5402ms
[00:00:06.002,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:06.002,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:06.002,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=6002ms
[00:00:06.602,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:06.602,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:06.602,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=6602ms
[00:00:07.202,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:07.202,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:10.203,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 0x080072D4
[00:00:10.203,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=10203ms
[00:00:10.803,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:10.803,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:13.803,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 0x080072D4
[00:00:13.803,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=13803ms
[00:00:14.403,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:14.403,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:17.403,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 0x080072D4
[00:00:17.403,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=17403ms
[00:00:18.003,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:18.003,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:21.003,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 0x080072D4
[00:00:21.003,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=21003ms
[00:00:21.603,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:21.603,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:24.603,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 0x080072D4
[00:00:24.603,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=24603ms
[00:00:25.203,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:25.203,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:28.204,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 0x080072D4
[00:00:28.204,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=28204ms
[00:00:28.804,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:28.804,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:31.804,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 0x080072D4
[00:00:31.804,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=31804ms
[00:00:32.404,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:32.404,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:35.404,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 0x080072D4
[00:00:35.404,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=20 x=190 y=190 z=190 latency=35404ms
[00:00:36.004,000] <err> l5_task1: [LOGGING] Sequence numbers 20, 20 not consecutive!
[00:00:36.004,000] <wrn> l5_task1: [LOGGING] Queue looks empty
[00:00:36.004,000] <inf> l5_task1: [LOGGING] done, received=20 messages
[00:00:36.004,000] <inf> l5_task1: [MAIN] Consumer thread done.
[00:00:37.003,000] <inf> l5_task1: [HEALTH] Done.
[00:00:37.003,000] <inf> l5_task1: [MAIN] Health thread done.
[00:00:37.003,000] <inf> l5_task1: [MAIN] Producer, consumer and health monitor all completed their work.
[00:00:37.003,000] <inf> l5_task1: [MAIN] App l5-task1 done.
```

## Design Notes

The feeding of a task watchdog timer in the consumer thread did not originally
seem able to meet the requirement that the watchdog detect lost progress.  The
publishing thread can know when a message publish fails, as evidenced by 
error messages like:

```
[00:00:01.851,000] <err> zbus: could not deliver notification to observer . Error code -11
```

But the consumer thread doesn't have insight into when a message queue is full.
It can detect work lost by testing sequence numbers.  This test informs the
consumer thread when work has been lost, and this in turn is used to defer
feeding the task watchdog.

In this sense, the task watchdog does not detect work lost, but it still
provides the callback to act on a work lost event.

The producer thread which publishes sensor readings (pushes them onto a
message queue) could be fitted with a task watchdog whose feeding depends on
notification delivery.

## Recovery Policy

For this exerice and demonstration app, the recovery policy is to provide a
notice when a message is detected as dropped.  In a real world context this
notice would be available to a larger application, which then decides how to
act following a lost work event.

## Reference

- https://docs.zephyrproject.org/latest/services/zbus/index.html

