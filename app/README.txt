Iomico Zephyr Intermediate Course - 2026 Q3

Assignment 1:  Kernel and Scheduling

## The Work


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
installed in the Zephyr tools and SDK set up processor, so that flash utility
is also not available, at least not through a direct invocation of west.

To overcome this, a helper shell script is added along with an Open OCD
configuration file, to support the use of the open source pyocd flashing
utility.   The config file is named oocd.cfg.  Its contents are:

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

## Typical Output

In the first part of the task, three pre-emptable threads are created.  Their
numeric priorities and sleep times between logging statements are:

| priority | sleep (ms) |
|     7    |     100    |
|     5    |     400    |
|     3    |     700    |

The lowest priority thread sleeps for the shortest time, and therefore wants to
execute the most often.

Typical output looks like:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> demo: main.c thread test starting
[00:00:00.000,000] <inf> demo: thread   c
[00:00:00.000,000] <inf> demo: thread  b
[00:00:00.000,000] <inf> demo: thread a
[00:00:00.100,000] <inf> demo: thread a
[00:00:00.200,000] <inf> demo: thread a
[00:00:00.300,000] <inf> demo: thread a
[00:00:00.400,000] <inf> demo: thread  b
[00:00:00.400,000] <inf> demo: thread a
[00:00:00.500,000] <inf> demo: thread a
[00:00:00.600,000] <inf> demo: thread a
[00:00:00.700,000] <inf> demo: thread   c
[00:00:00.700,000] <inf> demo: thread a
[00:00:00.800,000] <inf> demo: thread  b
[00:00:00.801,000] <inf> demo: thread a
[00:00:00.901,000] <inf> demo: thread a
[00:00:01.001,000] <inf> demo: thread a
[00:00:01.101,000] <inf> demo: thread a
[00:00:01.200,000] <inf> demo: thread  b
[00:00:01.201,000] <inf> demo: thread a
[00:00:01.301,000] <inf> demo: thread a
[00:00:01.400,000] <inf> demo: thread   c
[00:00:01.401,000] <inf> demo: thread a
[00:00:01.501,000] <inf> demo: thread a
[00:00:01.600,000] <inf> demo: thread  b
[00:00:01.601,000] <inf> demo: thread a
[00:00:01.701,000] <inf> demo: thread a
[00:00:01.802,000] <inf> demo: thread a
[00:00:01.902,000] <inf> demo: thread a
[00:00:02.000,000] <inf> demo: thread  b
[00:00:02.002,000] <inf> demo: thread a
```

When the two higher priority threads share the same sleep time and the same
priority, as described in this table,

| priority | sleep (ms) |
|     7    |     100    |
|     3    |     500    |
|     3    |     500    |

the output looks like this:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> demo: main.c thread test starting
[00:00:00.000,000] <inf> demo: thread  b
[00:00:00.000,000] <inf> demo: thread   c
[00:00:00.000,000] <inf> demo: thread a
[00:00:00.100,000] <inf> demo: thread a
[00:00:00.200,000] <inf> demo: thread a
[00:00:00.300,000] <inf> demo: thread a
[00:00:00.400,000] <inf> demo: thread a
[00:00:00.500,000] <inf> demo: thread  b
[00:00:00.500,000] <inf> demo: thread   c
[00:00:00.500,000] <inf> demo: thread a
[00:00:00.600,000] <inf> demo: thread a
[00:00:00.700,000] <inf> demo: thread a
[00:00:00.801,000] <inf> demo: thread a
[00:00:00.901,000] <inf> demo: thread a
[00:00:01.000,000] <inf> demo: thread  b
[00:00:01.000,000] <inf> demo: thread   c
[00:00:01.001,000] <inf> demo: thread a
[00:00:01.101,000] <inf> demo: thread a
[00:00:01.201,000] <inf> demo: thread a
[00:00:01.301,000] <inf> demo: thread a
[00:00:01.401,000] <inf> demo: thread a
[00:00:01.500,000] <inf> demo: thread  b
[00:00:01.500,000] <inf> demo: thread   c
[00:00:01.501,000] <inf> demo: thread a
[00:00:01.601,000] <inf> demo: thread a
[00:00:01.702,000] <inf> demo: thread a

```

Changing only the priority and holding sleep times of the two lowest priority
threads equal,

| priority | sleep (ms) |
|     7    |     100    |
|     5    |     500    |
|     3    |     500    |

the output looks like this:

```
*** Booting Zephyr OS build v4.4.0 ***
[00:00:00.000,000] <inf> demo: main.c thread test starting
[00:00:00.000,000] <inf> demo: thread   c
[00:00:00.000,000] <inf> demo: thread  b
[00:00:00.000,000] <inf> demo: thread a
[00:00:00.100,000] <inf> demo: thread a
[00:00:00.200,000] <inf> demo: thread a
[00:00:00.300,000] <inf> demo: thread a
[00:00:00.400,000] <inf> demo: thread a
[00:00:00.500,000] <inf> demo: thread   c
[00:00:00.500,000] <inf> demo: thread  b
[00:00:00.500,000] <inf> demo: thread a
[00:00:00.600,000] <inf> demo: thread a
[00:00:00.700,000] <inf> demo: thread a
[00:00:00.801,000] <inf> demo: thread a
[00:00:00.901,000] <inf> demo: thread a
[00:00:01.000,000] <inf> demo: thread   c
[00:00:01.000,000] <inf> demo: thread  b
[00:00:01.001,000] <inf> demo: thread a
[00:00:01.101,000] <inf> demo: thread a
[00:00:01.201,000] <inf> demo: thread a
```


With he addition of a cooperative thread . . .
