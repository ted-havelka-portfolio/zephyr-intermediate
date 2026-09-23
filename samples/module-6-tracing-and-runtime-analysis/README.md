l6-task1

## How To Build

To compile for native_sim, invoke meta-tool west as follows:

```
west build -b native_sim -p always
```

To build for nucleo_l432kc, call west with options and args:

Part of Zephyr's tracing libraries depends on their being an element, or
special device node property in the Zephyr "chosen" device tree node.  That
property is of the form:

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
