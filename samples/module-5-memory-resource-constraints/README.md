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
[00:00:00.000,000
] <inf> l5_task1: === Lecture 5 task 1: Memory Resource Constraints ===
[00:00:00.000,000] <inf> l5_task1: * Sensor publishes every 150 ms
[00:00:00.000,000] <inf> l5_task1: * Logging thread runs a little over 320 ms
[00:00:00.000,000] <inf> l5_task1: * Display listener runs in publisher context
[00:00:00.000,000] <inf> l5_task1: * Logging thread uses message subscriber copies
[00:00:00.000,000] <inf> l5_task1: About to publish 20 simulated se
nsor readings . . .
[00:00:00.000,000] <inf> l5_task1: [SENSOR] publish seq=0 accel x=0, y=0, z=0
[00:00:00.000,000] <inf> l5_task1: From l5 listener -> Acc x=0, y=0, z=0
[00:00:00.000,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=0 x=0 y=0 z=0 latency=0ms
[00:00:00.150,000] <inf> l5_task1: [SENSOR] publish seq=1 accel x=10, y=10, z=10
[00:00:00.150,000] <inf> l5_task1: From l5 listener -> Acc x=10, y=10, z=10
[00:00:00.300,000] <inf> l5_task1: [SENSOR] publish seq=2 accel x=20, y=20, z=20
[00:00:00.300,000] <inf> l5_task1: From l5 listener -> Acc x=20, y=20, z=20
[00:00:00.320,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=2 x=20 y=20 z=20 latency=320ms
[00:00:00.450,000] <inf> l5_task1: [SENSOR] publish seq=3 accel x=30, y=30, z=30
[00:00:00.450,000] <inf> l5_task1: From l5 listener -> Acc x=30, y=30, z=30
[00:00:00.601,000] <inf> l5_task1: [SENSOR] publish seq=4 accel x=40, y=40, z=40
[00:00:00.601,000] <inf> l5_task1: From l5 listener -> Acc x=40, y=40, z=40
[00:00:00.640,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=4 x=40 y=40 z=40 latency=640ms
[00:00:00.751,000] <inf> l5_task1: [SENSOR] publish seq=5 accel x=50, y=50, z=50
[00:00:00.751,000] <inf> l5_task1: From l5 listener -> Acc x=50, y=50, z=50
[00:00:00.901,000] <inf> l5_task1: [SENSOR] publish seq=6 accel x=60, y=60, z=60
[00:00:00.901,000] <inf> l5_task1: From l5 listener -> Acc x=60, y=60, z=60
[00:00:00.960,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=6 x=60 y=60 z=60 latency=960ms
[00:00:01.051,000] <inf> l5_task1: [SENSOR] publish seq=7 accel x=70, y=70, z=70
[00:00:01.051,000] <inf> l5_task1: From l5 listener -> Acc x=70, y=70, z=70
[00:00:01.201,000] <inf> l5_task1: [SENSOR] publish seq=8 accel x=80, y=80, z=80
[00:00:01.201,000] <inf> l5_task1: From l5 listener -> Acc x=80, y=80, z=80
[00:00:01.281,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=8 x=80 y=80 z=80 latency=1281ms
[00:00:01.352,000] <inf> l5_task1: [SENSOR] publish seq=9 accel x=90, y=90, z=90
[00:00:01.352,000] <inf> l5_task1: From l5 listener -> Acc x=90, y=90, z=90
[00:00:01.502,000] <inf> l5_task1: [SENSOR] publish seq=10 accel x=100, y=100, z=100
[00:00:01.502,000] <inf> l5_task1: From l5 listener -> Acc x=100, y=100, z=100
[00:00:01.601,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=10 x=100 y=100 z=100 latency=1601ms
[00:00:01.751,000] <inf> l5_task1: [SENSOR] publish seq=11 accel x=110, y=110, z=110
[00:00:01.751,000] <inf> l5_task1: From l5 listener -> Acc x=110, y=110, z=110
[00:00:01.851,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:01.851,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:01.921,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=11 x=110 y=110 z=110 latency=1921ms
[00:00:02.000,000] <wrn> l5_task1: Queue at or over capacity of 75%
[00:00:02.001,000] <inf> l5_task1: [SENSOR] publish seq=12 accel x=120, y=120, z=120
[00:00:02.001,000] <inf> l5_task1: From l5 listener -> Acc x=120, y=120, z=120
[00:00:02.151,000] <inf> l5_task1: [SENSOR] publish seq=13 accel x=130, y=130, z=130
[00:00:02.151,000] <inf> l5_task1: From l5 listener -> Acc x=130, y=130, z=130
[00:00:02.241,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=13 x=130 y=130 z=130 latency=2241ms
[00:00:02.391,000] <inf> l5_task1: [SENSOR] publish seq=14 accel x=140, y=140, z=140
[00:00:02.391,000] <inf> l5_task1: From l5 listener -> Acc x=140, y=140, z=140
[00:00:02.492,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:02.492,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:02.561,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=14 x=140 y=140 z=140 latency=2561ms
[00:00:02.642,000] <inf> l5_task1: [SENSOR] publish seq=15 accel x=150, y=150, z=150
[00:00:02.642,000] <inf> l5_task1: From l5 listener -> Acc x=150, y=150, z=150
[00:00:02.792,000] <inf> l5_task1: [SENSOR] publish seq=16 accel x=160, y=160, z=160
[00:00:02.792,000] <inf> l5_task1: From l5 listener -> Acc x=160, y=160, z=160
[00:00:02.882,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=16 x=160 y=160 z=160 latency=2882ms
[00:00:03.000,000] <wrn> l5_task1: Queue at or over capacity of 75%
[00:00:03.032,000] <inf> l5_task1: [SENSOR] publish seq=17 accel x=170, y=170, z=170
[00:00:03.032,000] <inf> l5_task1: From l5 listener -> Acc x=170, y=170, z=170
[00:00:03.132,000] <err> zbus: could not deliver notification to observer . Error code -11
[00:00:03.132,000] <wrn> l5_task1: [SENSOR] publish failed rc=-11
[00:00:03.202,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=17 x=170 y=170 z=170 latency=3202ms
[00:00:03.282,000] <inf> l5_task1: [SENSOR] publish seq=18 accel x=180, y=180, z=180
[00:00:03.282,000] <inf> l5_task1: From l5 listener -> Acc x=180, y=180, z=180
[00:00:03.432,000] <inf> l5_task1: [SENSOR] publish seq=19 accel x=190, y=190, z=190
[00:00:03.432,000] <inf> l5_task1: From l5 listener -> Acc x=190, y=190, z=190
[00:00:03.522,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=190 y=190 z=190 latency=3522ms
[00:00:03.672,000] <inf> l5_task1: * [SENSOR] done  *
[00:00:03.672,000] <inf> l5_task1: [MAIN] Producer thread done.
[00:00:03.842,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=190 y=190 z=190 latency=3842ms
[00:00:04.000,000] <wrn> l5_task1: Queue at or over capacity of 75%
[00:00:04.162,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=190 y=190 z=190 latency=4162ms
[00:00:04.483,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=190 y=190 z=190 latency=4483ms
[00:00:04.803,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=190 y=190 z=190 latency=4803ms
[00:00:05.123,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=190 y=190 z=190 latency=5123ms
[00:00:05.943,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:05.943,000] <err> l5_task1:   (Could reset here)
[00:00:15.443,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 134246996
[00:00:15.443,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=190 y=190 z=190 latency=15443ms
[00:00:16.264,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:16.264,000] <err> l5_task1:   (Could reset here)
[00:00:25.764,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 134246996
[00:00:25.764,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=190 y=190 z=190 latency=25764ms
[00:00:26.584,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:26.584,000] <err> l5_task1:   (Could reset here)
[00:00:36.084,000] <err> l5_task1: [LOGGING] Failed or timed out waiting for zbus channel 134246996
[00:00:36.084,000] <inf> l5_task1: [LOGGING] thread=logging-thread-l5 seq=19 x=190 y=190 z=190 latency=36084ms
[00:00:36.404,000] <inf> l5_task1: [LOGGING] done, received=20 messages
[00:00:36.404,000] <inf> l5_task1: [MAIN] Consumer thread done.
[00:00:36.904,000] <err> l5_task1:   task watchdog channel 0 for l5-logging-fn timed out
[00:00:36.904,000] <err> l5_task1:   (Could reset here)
[00:00:37.003,000] <inf> l5_task1: [HEALTH] Done.
[00:00:37.003,000] <inf> l5_task1: [MAIN] Health thread done.
[00:00:37.003,000] <inf> l5_task1: [MAIN] Producer, consumer and health monitor all completed their work.
[00:00:37.003,000] <inf> l5_task1: [MAIN] App l5-task1 done.
```

## Design Notes

The feeding of a task watchdog timer in the consumer thread does not seem able
to meet the requirement, that the watchdog detect lost progress.  The
publishing thread can know when a message publish fails, as evidenced by 
error messages like:

```
[00:00:01.851,000] <err> zbus: could not deliver notification to observer . Error code -11
```

Though the consumer thread knows when it has finished its own processing, it
doesn't have insight into a queue that's too full.  So it's not very helpful
to have this consumer thread feed a task watchdog, when it does not know how
quickly the producer is finished its work.

The consumer could track sequence numbers in the simulated sensor readings, in
order to detect lost work or lost data.  But this test would not be watchdog
based.

It seems that the best place in this app to detect truly lost work is in the
producer thread.  The producer thread can test the return value it receives
from message delivery attempts.  This is the first and fastest way it can
detect lost work.  A task watchdog timer can be implemented, associated with
message submission.  That watchdog can time out, waiting for a successful
submission.  The watchdog in this situation, however, seems redundant to the
test of the message submission return value.

## Recovery Policy

For this exerice and demonstration app, the recovery policy is to provide a
notice when a message cannot be submitted.  In a real world context this
message becomes available to a larger application, which then decides how to
act in lost work situations.

## Reference

- https://docs.zephyrproject.org/latest/services/zbus/index.html

