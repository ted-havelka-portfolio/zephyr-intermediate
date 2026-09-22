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
``

When it comes to Zephyr 4.4.0 used at the time of writing this readme, the
meta-tool west (or one of its called flash scripts) defaults to looking for
the runner for stm32cubeprogrammer.  Passing the -r option with a more
general flash utility supports correct firmware flashing, without a need to
install stm32cubeprogrammer.

## Application Output

```
ted@zakia:.../ted/zephyr-intermediano/samples/module-5-memory-resource-constraints/notes$ grep -n holds mc-002-pub-15-sub-200-ms.cap 
14:[00:00:00.200,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 1 of 5 messages
21:[00:00:00.400,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 1 of 5 messages
28:[00:00:00.600,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 1 of 5 messages
37:[00:00:00.801,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 2 of 5 messages
44:[00:00:01.001,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 2 of 5 messages
51:[00:00:01.201,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 2 of 5 messages
60:[00:00:01.401,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 3 of 5 messages
66:[00:00:01.601,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 2 of 5 messages
71:[00:00:01.802,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 1 of 5 messages
76:[00:00:02.002,000] <inf> l5_task1: - M1 - hw5 subscriber queue holds 0 of 5 messages
```

## Reference

- https://docs.zephyrproject.org/latest/services/zbus/index.html
