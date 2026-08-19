# Iomico Zephyr Intermediate Course - 2026 Q3

Assignment 1:  Kernel and Scheduling

## The Work

The tiny application this readme goes with, is an exercise of Zephyr threads.
Three preemptive (preempt-able) threads are started at the beginning of the app
and a fourth cooperative thread is also started.  Output messages over UART,
using Zephyr's logging system, show the scheduling behavior for the threads.

A custom Kconfig symbol allows for turning the cooperative thread on and off
at compile time.

## Hardware Tested

So far this work is flashed to and tested on an STMicro Nucleo_l432 development
board.  This board provides 64Kb of static RAM, and 256Kb of flash.

## How To Compile

```shell
west build -b nucleo_l432kc -p always
```

## How To Flash

A custom flash script is used in this first assignment, to overcome a limitation
in the supporting flash runners available to use with ``west``.  For STMicro
dev boards the script is called this way:

```shell
./flash-manually.sh w --config-path ./scripts
```

At time of writing, Zephyr 4.4.1 flash runner scripts don't support the use of
pyocd with the STMicro dev boards mentioned in the "Hardware Tested" section.
The west tool seems to prefer an stm32CubeProgrammer utility which is not
installed with the Zephyr tools nor its SDK.

To assist with flashing STMicro dev boards, a helper shell script is added to
this project.  A configuration file is also added.  This file, named oocd.cfg
contains:

_Pyocd config helper file_

```
# for use with STLINK V2 SWD programmer

source [find interface/stlink.cfg]
transport select hla_swd

source [find target/stm32l4x.cfg]
```

Other than the transport line, this file simply sources two other configuration
files.  Those files are where the more telling details of pyocd configuration
support lie.  Equally important, those configuration files are downloaded as
part of the Zephyr 4.4.1 set up process.

## Questions and Answers

Question:  Which thread executes the most often?

Answer:  Among the preemptive threads, the thread which runs the most often is
generally the thread with the shortest sleep time.

When a cooperative thread is added, and its "while one" loop construct calls
k_yield() rather than k_msleep(), no other threads execute.  In fact, Zephyr's
banner message does not even appear.  

Question: Does priority affect interleaving of threads?

Answer:  Thread priorities affect thread interleaving.  The highest priority
thread is the first to run, when a lower priority thread sleeps or becomes
suspended.

## Typical Output

In the first part of the task, three pre-emptable threads are created.  Their
numeric priorities and sleep times between logging statements are:

| priority | sleep (ms) |
|     7    |     300    |
|     5    |     200    |
|     3    |     100    |

Typical output looks like:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> demo: main.c thread test starting
[00:00:00.000,000] <inf> demo: thread   c
[00:00:00.000,000] <inf> demo: thread  b
[00:00:00.000,000] <inf> demo: thread a
[00:00:00.100,000] <inf> demo: thread   c
[00:00:00.200,000] <inf> demo: thread   c
[00:00:00.200,000] <inf> demo: thread  b
[00:00:00.300,000] <inf> demo: thread   c
[00:00:00.300,000] <inf> demo: thread a
[00:00:00.400,000] <inf> demo: thread   c
[00:00:00.400,000] <inf> demo: thread  b
[00:00:00.500,000] <inf> demo: thread   c
[00:00:00.600,000] <inf> demo: thread a
[00:00:00.600,000] <inf> demo: thread  b
[00:00:00.600,000] <inf> demo: thread   c
[00:00:00.700,000] <inf> demo: thread   c
[00:00:00.800,000] <inf> demo: thread  b
[00:00:00.801,000] <inf> demo: thread   c
[00:00:00.900,000] <inf> demo: thread a
[00:00:00.901,000] <inf> demo: thread   c
[00:00:01.000,000] <inf> demo: thread  b
[00:00:01.001,000] <inf> demo: thread   c
[00:00:01.101,000] <inf> demo: thread   c
[00:00:01.200,000] <inf> demo: thread a
[00:00:01.201,000] <inf> demo: thread  b
[00:00:01.201,000] <inf> demo: thread   c
[00:00:01.301,000] <inf> demo: thread   c
[00:00:01.401,000] <inf> demo: thread  b
[00:00:01.401,000] <inf> demo: thread   c
```

With the cooperative thread in play, output looks like:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> demo: thread    d - cooperative
[00:00:00.000,000] <inf> demo: main.c thread test starting
[00:00:00.000,000] <inf> demo: thread   c
[00:00:00.000,000] <inf> demo: thread  b
[00:00:00.000,000] <inf> demo: thread a
[00:00:00.100,000] <inf> demo: thread   c
[00:00:00.200,000] <inf> demo: thread  b
[00:00:00.200,000] <inf> demo: thread   c
[00:00:00.300,000] <inf> demo: thread a
[00:00:00.300,000] <inf> demo: thread   c
[00:00:00.400,000] <inf> demo: thread  b
[00:00:00.400,000] <inf> demo: thread   c
[00:00:00.500,000] <inf> demo: thread   c
[00:00:00.600,000] <inf> demo: thread  b
[00:00:00.600,000] <inf> demo: thread a
[00:00:00.600,000] <inf> demo: thread   c
[00:00:00.700,000] <inf> demo: thread   c
[00:00:00.800,000] <inf> demo: thread  b
[00:00:00.801,000] <inf> demo: thread   c
[00:00:00.900,000] <inf> demo: thread a
[00:00:00.901,000] <inf> demo: thread   c
[00:00:01.000,000] <inf> demo: thread    d - cooperative
[00:00:01.000,000] <inf> demo: thread  b
[00:00:01.001,000] <inf> demo: thread   c
[00:00:01.101,000] <inf> demo: thread   c
[00:00:01.200,000] <inf> demo: thread  b
[00:00:01.200,000] <inf> demo: thread a
[00:00:01.201,000] <inf> demo: thread   c
[00:00:01.301,000] <inf> demo: thread   c
[00:00:01.400,000] <inf> demo: thread  b
[00:00:01.401,000] <inf> demo: thread   c
[00:00:01.501,000] <inf> demo: thread a
[00:00:01.501,000] <inf> demo: thread   c
```

The cooperative thread is performing busy work for five loop iternations.
This busy work becomes more visible with an added logging statement.  While the
assignment calls for the cooperative thread to yield between busy work
executions, it's not clear whether "to yield" is meant in the strict sense,
that is, the thread must called ``k_yield()``, or whether this requirement
means the thread must in one way or another yield to other threads.  Taking the
less strict interpretation, and calling a kernel sleep API after busy work,
gives an output like:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> demo: thread    d - cooperative
[00:00:00.000,000] <inf> demo: thread    d - busy
[00:00:00.000,000] <inf> demo: thread    d - busy
[00:00:00.000,000] <inf> demo: thread    d - busy
[00:00:00.000,000] <inf> demo: thread    d - busy
[00:00:00.001,000] <inf> demo: thread    d - busy
[00:00:00.001,000] <inf> demo: main.c thread test starting
[00:00:00.001,000] <inf> demo: thread   c
[00:00:00.001,000] <inf> demo: thread  b
[00:00:00.001,000] <inf> demo: thread a
[00:00:00.101,000] <inf> demo: thread   c
[00:00:00.201,000] <inf> demo: thread  b
[00:00:00.201,000] <inf> demo: thread   c
[00:00:00.301,000] <inf> demo: thread a
[00:00:00.301,000] <inf> demo: thread   c
[00:00:00.401,000] <inf> demo: thread  b
[00:00:00.401,000] <inf> demo: thread   c
[00:00:00.501,000] <inf> demo: thread   c
[00:00:00.601,000] <inf> demo: thread a
[00:00:00.601,000] <inf> demo: thread  b
[00:00:00.601,000] <inf> demo: thread   c
[00:00:00.702,000] <inf> demo: thread   c
[00:00:00.801,000] <inf> demo: thread  b
[00:00:00.802,000] <inf> demo: thread   c
[00:00:00.901,000] <inf> demo: thread a
[00:00:00.902,000] <inf> demo: thread   c
[00:00:01.001,000] <inf> demo: thread    d - cooperative
[00:00:01.001,000] <inf> demo: thread    d - busy
[00:00:01.001,000] <inf> demo: thread    d - busy
[00:00:01.001,000] <inf> demo: thread    d - busy
[00:00:01.002,000] <inf> demo: thread    d - busy
[00:00:01.002,000] <inf> demo: thread    d - busy
[00:00:01.002,000] <inf> demo: thread   c
[00:00:01.002,000] <inf> demo: thread  b
[00:00:01.102,000] <inf> demo: thread   c
[00:00:01.201,000] <inf> demo: thread a
[00:00:01.202,000] <inf> demo: thread   c
[00:00:01.202,000] <inf> demo: thread  b
[00:00:01.302,000] <inf> demo: thread   c
[00:00:01.402,000] <inf> demo: thread  b
[00:00:01.402,000] <inf> demo: thread   c
[00:00:01.501,000] <inf> demo: thread a
[00:00:01.503,000] <inf> demo: thread   c
[00:00:01.602,000] <inf> demo: thread  b
[00:00:01.603,000] <inf> demo: thread   c
```
