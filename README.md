# pico-usb

A very small USB library (without TinyUSB) for the rp2040.

## Summary

The rp2040 chip in the Raspberry Pi Pico/W boards supports USB Host and
USB Device modes. Most examples and implementations use the excellent
[TinyUSB](https://github.com/hathach/tinyusb) library to provide this support. However, TinyUSB has some
limitations. These include a somewhat hard coded initial configuration
that requires the code to be recompiled when changes are made.

## Goals

The goal of this repo is to offer a simple USB Device example and a
simple USB Host example, without using any code from TinyUSB. There
is currently an example in the [pico-examples](https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_lowlevel) repository that shows
how to implement a basic low level USB Device without TinyUSB. I would
like to streamline this USB Device example and also add a USB Host
example to show how to implement basic versions of each of these.

Ideally, this could lead to a barebones and simplified USB library for
rp2040 based projects. Since the initial target is for the Pico/W
boards and since the goal is to be tiny, even "tinier" than TinyUSB,
it seems the name pico-usb is fitting.

## Contents

There is a directory called `device` which contains the simplified
USB Device example and a directory called `host` which contains
the simplified USB Host example. Hopefully, the contents of these
two directories will be very similar and have as similar code as
possible.

## Status

Currently, the code in `device` is a streamlined version
of the low level USB device from [pico-examples](https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_lowlevel).
It's a simple example that will echo anything received on EP1 to EP2.
The CMakeLists.txt files in this repo are written to be very minimal.
They work in VScode when opening these two directories. There is probably
a way to create a top-level CMakeLists.txt file so that each directory
doesn't need one, if someone can submit a way to open the top-level
directory and build either directory that would be great. The code
in `host` is being put together now, based on trying to read the spec
from the datasheet. If anyone has some minimal code for USB Host, it
would be great to add it to build out the USB Host example.

## TODO

- [x] Initial version of [`device.c`](https://github.com/shreeve/pico-usb/blob/f6c648e3a4bfbfedd53296ae70b41596cf719e3e/device/device.c) (a simpler [`dev_lowlevel.c`](https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_lowlevel) from `pico-examples`).
- [ ] Improved `device.c` with larger transfer size (more than 1 packet of 64 bytes).
- [ ] Initial version of `host.c` (I can't find a good reference example).
- [ ] Add double-buffering support to `device.c`
- [ ] Add double-buffering support to `host.c`

## Disable TinyUSB

Since this repo doesn't use TinyUSB at all, we need to disable it from the
pico-sdk code. The only way I found to do this is by commenting out the
following lines in `${PICO_SDK_PATH}/src/rp2_common/CMakeLists.txt`.

```
# pico_add_subdirectory(tinyusb)
# pico_add_subdirectory(pico_stdio_usb)
```

## Compiling

Since we are taking over the physical USB connector on the board, the
easiest way to program the board and test things out is by using a
picodebug unit. These can be purchased from numerous sources. The .vscode
directory contains some configuration for this setup.

## Example

With a picodebug unit to flash the code to the device and also communicate
with the UART, I build the code and then deploy and test it like this:

```
$ openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program build/device.elf verify reset exit"

==[ USB device example]==

Initialized EP0_OUT (0x00) with buffer address 0x50100100
Initialized EP0_IN  (0x80) with buffer address 0x50100100
Initialized EP1_OUT (0x01) with buffer address 0x50100180
Initialized EP2_IN  (0x82) with buffer address 0x501001C0

USB device reset

< Reset
< Setup | 00000000 | 00 05 03 00 00 00 00 00                          | Set address to 3
> ZLP
< Setup | 00000000 | 80 06 00 01 00 00 08 00                          | Get device descriptor 0
> 0x80  | 00000000 | 12 01 00 02 00 00 00 40                          | .......@
< Setup | 00000000 | 80 06 00 01 00 00 12 00                          | Get device descriptor 0
> 0x80  | 00000000 | 12 01 00 02 00 00 00 40 00 00 01 00 01 00 01 02  | .......@........
        | 00000010 | 03 01                                            | ..
< Setup | 00000000 | 80 06 02 03 09 04 02 00                          | Get string descriptor 2
> 0x80  | 00000000 | 0a 03                                            | ..
< Setup | 00000000 | 80 06 02 03 09 04 0a 00                          | Get string descriptor 2
> 0x80  | 00000000 | 0a 03 44 00 65 00 6d 00 6f 00                    | ..D.e.m.o.
< Setup | 00000000 | 80 06 01 03 09 04 02 00                          | Get string descriptor 1
> 0x80  | 00000000 | 10 03                                            | ..
< Setup | 00000000 | 80 06 01 03 09 04 10 00                          | Get string descriptor 1
> 0x80  | 00000000 | 10 03 50 00 69 00 63 00 6f 00 55 00 53 00 42 00  | ..P.i.c.o.U.S.B.
< Setup | 00000000 | 80 06 03 03 09 04 02 00                          | Get string descriptor 3
> 0x80  | 00000000 | 0c 03                                            | ..
< Setup | 00000000 | 80 06 03 03 09 04 0c 00                          | Get string descriptor 3
> 0x80  | 00000000 | 0c 03 31 00 32 00 33 00 34 00 35 00              | ..1.2.3.4.5.
< Setup | 00000000 | 80 06 00 02 00 00 09 00                          | Get config descriptor 0
> 0x80  | 00000000 | 09 02 20 00 01 01 04 c0 32                       | .. .....2
< Setup | 00000000 | 80 06 00 02 00 00 20 00                          | Get config descriptor 0
> 0x80  | 00000000 | 09 02 20 00 01 01 04 c0 32 09 04 00 00 02 ff 00  | .. .....2.......
        | 00000010 | 00 05 07 05 01 02 40 00 00 07 05 82 02 40 00 00  | ......@......@..
< Setup | 00000000 | 00 09 01 00 00 00 00 00                          | Set configuration to 1
> ZLP
< Setup | 00000000 | 80 06 05 03 09 04 02 00                          | Get string descriptor 5
> 0x80  | 00000000 | 0e 03                                            | ..
< Setup | 00000000 | 80 06 05 03 09 04 0e 00                          | Get string descriptor 5
> 0x80  | 00000000 | 0e 03 53 00 69 00 6d 00 70 00 6c 00 65 00        | ..S.i.m.p.l.e.
< Setup | 00000000 | 80 06 00 03 00 00 ff 00                          | Get string descriptor 0
> 0x80  | 00000000 | 04 03 09 04                                      | ....

USB device configured

$ verify.py

< Setup | 00000000 | 80 06 00 03 00 00 fe 00                          | Get string descriptor 0
> 0x80  | 00000000 | 04 03 09 04                                      | ....
< Setup | 00000000 | 80 06 04 03 09 04 fe 00                          | Get string descriptor 4
> 0x80  | 00000000 | 0a 03 45 00 61 00 73 00 79 00                    | ..E.a.s.y.
< 0x01  | 00000000 | 48 65 6c 6c 6f 2c 20 77 6f 72 6c 64 21 0a        | Hello, world!.
> 0x82  | 00000000 | 48 65 6c 6c 6f 2c 20 77 6f 72 6c 64 21 0a        | Hello, world!.

$ usbdiagnose

Full Speed device @ 118 (0x02400000): .............................................   Composite device: "Demo"
    Port Information:   0x001a
           Not Captive
           Attached to Root Hub
           External Device
           Connected
           Enabled
    Number Of Endpoints (includes EP0):
        Total Endpoints for Configuration 1 (current):   3
    Device Descriptor
        Descriptor Version Number:   0x0200
        Device Class:   0   (Composite)
        Device Subclass:   0
        Device Protocol:   0
        Device MaxPacketSize:   64
        Device VendorID/ProductID:   0x0000/0x0001   (unknown vendor)
        Device Version Number:   0x0001
        Number of Configurations:   1
        Manufacturer String:   1 "PicoUSB"
        Product String:   2 "Demo"
        Serial Number String:   3 "12345"
    Configuration Descriptor (current config): ......................   "Easy"
        Length (and contents):   32
            Raw Descriptor (hex)    0000: 09 02 20 00 01 01 04 C0  32 09 04 00 00 02 FF 00
            Raw Descriptor (hex)    0010: 00 05 07 05 01 02 40 00  00 07 05 82 02 40 00 00
            Unknown Descriptor   0020:
        Number of Interfaces:   1
        Configuration Value:   1
        Attributes:   0xC0 (self-powered)
        MaxPower:   100 mA
        Interface #0 - Vendor-specific ..............................................   "Simple"
            Alternate Setting   0
            Number of Endpoints   2
            Interface Class:   255   (Vendor-specific)
            Interface Subclass;   0   (Vendor-specific)
            Interface Protocol:   0
            Endpoint 0x01 - Bulk Output
                Address:   0x01  (OUT)
                Attributes:   0x02  (Bulk)
                Max Packet Size:   64
                Polling Interval:   0 ms
            Endpoint 0x82 - Bulk Input
                Address:   0x82  (IN)
                Attributes:   0x02  (Bulk)
                Max Packet Size:   64
                Polling Interval:   0 ms
```

## License

BSD-3-Clause license, the same as code in [pico-examples](https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_lowlevel).
