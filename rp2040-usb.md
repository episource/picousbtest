## `RP2040:` USB Controller

### Memory Layout

The USB controller uses two blocks of memory.

The first block of memory contains endpoint data buffers and consists of 4,096
bytes of DPSRAM (dual-port static random access memory), beginning at address
0x5010000. The second block of memory contains additional registers and consists
of 156 bytes of standard (non-DPSRAM) memory, beginning at address 0x50110000.

DPSRAM memory is different from most memory on the RP2040 because it:

* Can be accessed by the processor and USB controller at the same time
* Supports 8, 16, and 32 bit access (most registers only support 32-bit access)
* Does not support the usb_hw_set and usb_hw_clear aliases

In the two memory block tables below, **bold** indicates the minimal set of the
registers and buffers to be able to perform *USB device enumeration* in either
host or device mode.

### `HOST:` Memory Block 1: 0x50100000 - 0x50100FFF

| Offset | Bytes | Host             | Info  |
| ---    | ---   | ---              | ---   |
| 0x00   | 8     | **SETUP packet** | 8 bytes for setup packet     |
| 0x08   | 4     | ECR1             | ECR for 1st polled endpoint  |
| 0x10   | 4     | ECR2             | ECR for 2nd polled endpoint  |
| 0x18   | 4     | ECR3             | ECR for 3rd polled endpoint  |
| ...    | ...   | ...              | ... |
| 0x78   | 4     | ECR15            | ECR for 15th polled endpoint |
| 0x80   | 4     | **BCR**          | Buffer control register (BCR) for software endpoint |
| 0x88   | 4     | BCR1             | BCR for 1st polled endpoint  |
| 0x90   | 4     | BCR2             | BCR for 2nd polled endpoint  |
| 0x98   | 4     | BCR3             | BCR for 3rd polled endpoint  |
| ...    | ...   | ...              | ... |
| 0x100  | 4     | **ECR**          | Endpoint control register (ECR) for software endpoint |
| ...    | ...   | ...              | ... |
| 0x180  | 64    | **Buffer 1**     | Data for 1st data buffer     |
| 0x1c0  | 64    | Buffer 2         | Data for 2nd data buffer     |
| 0x200  | 64    | Buffer 3         | Data for 3rd data buffer     |
| ...    | ...   | ...              | ... |
| 0xFC0  | 64    | Buffer 58        | Data for 58th data buffer    |

### `DEVICE:` Memory Block 1: 0x50100000 - 0x50100FFF

| Offset | Bytes | Device           | Info  |
| ---    | ---   | ---              | ---   |
| 0x00   | 8     | **SETUP packet** | 8 bytes for setup packet |
| 0x08   | 4     | ECR1/IN          | ECR for EP1/IN   |
| 0x10   | 4     | ECR1/OUT         | ECR for EP1/OUT  |
| 0x18   | 4     | ECR2/IN          | ECR for EP2/IN   |
| 0x20   | 4     | ECR2/OUT         | ECR for EP2/OUT  |
| ...    | ...   | ...              | ... |
| 0x78   | 4     | ECR15/IN         | ECR for EP15/IN  |
| 0x7C   | 4     | ECR15/OUT        | ECR for EP15/OUT |
| 0x80   | 4     | **BCR0/IN**      | BCR for EP0/IN   |
| 0x84   | 4     | **BCR0/OUT**     | BCR for EP0/OUT  |
| 0x88   | 4     | BCR1/IN          | BCR for EP1/IN   |
| 0x8C   | 4     | BCR1/OUT         | BCR for EP1/OUT  |
| ...    | ...   | ...              | ... |
| 0x100  | 64    | **EP0B**         | EP0 buffer 0 (shared between IN and OUT) |
| 0x140  | 64    | EP0B1            | EP0 buffer 1 (if double buffering on EP0) |
| 0x180  | 64    | Buffer 3         |     |
| ...    | ...   | ...              | ... |
| 0xFC0  | 64    | Buffer 60        | Data for 60th data buffer |

### Memory Block 2: 0x50110000 - 0x5011009B

| Offset | Host    | Info  |
| ---    | ---     | ---   |
| 0x00 | ADDR_ENDP (**DAR**) | Device and endpoint address (DAR) register for software endpoint |
| 0x04 | ADDR_ENDP1 | DAR for 1st polled endpoint |
| 0x08 | ADDR_ENDP2 | DAR for 2nd polled endpoint |
| 0x0c | ADDR_ENDP3 | DAR for 3rd polled endpoint |
| ...  | ...  | ... |
| 0x3C | ADDR_ENDP15 | DAR for 15th polled endpoint |
| 0x40 | **MAIN_CTRL** | Main control register |
| 0x44 | SOF_WR | `HOST:` Set the frame number for the next SOF (Start of Frame) sent by the host controller. SOF packets are sent every 1ms and the host will increment the frame number by 1 each time. |
| 0x48 | SOF_RD | `DEVICE:` Read the frame number from the last SOF (Start of Frame) received by the device controller. |
| 0x4C | SIE_CTRL (**SCR**) | SIE control register |
| 0x50 | SIE_STATUS (**SSR**) | SIE status register |
| 0x54 | INT_EP_CTRL | Polled endpoint control register |
| 0x58 | **BUFF_STATUS** | Buffer status register. When an endpoint enables an interrupt for its buffer status, this register will be set each time that buffer completes. |
| 0x5C | BUFF_CPU_SHOULD_HANDLE | When an endpoint is set for double buffering and sends an interrupt per buffer completion, this register toggles between 0 for buf_0 and 1 for buf_1 to indicate the buffer to handle. |
| 0x60 | EP_ABORT | `DEVICE:` For each bit in this register, a NAK will be sent for every access to the corresponding endpoint, overriding the BCR for that endpoint. |
| 0x64 | EP_ABORT_DONE | `DEVICE:` Used in conjunction with EP_ABORT. Set once an endpoint is idle, so the programmer knows it is safe to modify the BCR. |
| 0x68 | EP_STALL_ARM | `DEVICE:` This bit must be set in conjunction with the STALL bit in the BCR to send a STALL on EP0. The controller clears these bits when a SETUP packet is received. |
| 0x6C | NAK_POLL | `HOST:` If a device replies with a NAK, this value indicates the number of microseconds (μs) before trying again. |
| 0x70 | EP_STATUS_STALL_NAK | `DEVICE:` Each bit indicates that the corresponding endpoint has a NAK or STALL condition. EP0 is enabled via SIE_CTRL and all other endpoints are enabled in their ECR. |
| 0x74 | **USB_MUXING** | Controls where the USB controller is connected (usually `to_phy`). |
| 0x78 | **USB_PWR** | Controls VBUS and overcurrent configuration. |
| 0x7C | USBPHY_DIRECT | In conjunction with USBPHY_DIRECT_OVERRIDE, this register allows direct control of the USB Phy. |
| 0x80 | USBPHY_DIRECT_OVERRIDE | Enables overrides for each control in USBPHY_DIRECT. |
| 0x84 | USBPHY_TRIM | Used to adjust trim values for USB Phy pull down resistors. |
| 0x88 | Unknown | Unknown |
| 0x8C | INTR | Interrupt status (show all) |
| 0x90 | **INTE** | Enable interrupts |
| 0x94 | INTF | Forced interrupts |
| 0x98 | **INTS** | Interrupt status (masked or forced) |

### Device Enumeration

The [USB 2.0 Spec](https://www.usb.org/document-library/usb-20-specification)
states that when a USB device is attached to a USB bus, the host should perform
a reset and then begin a very specific process known as "device enumeration."
This process uses only SETUP packets, CONTROL transfers, endpoint zero (EP0),
and device zero (dev0) until a new device address is set. The purpose of device
enumeration is to assign the device a unique address and determine its
attributes so that it can operate properly.

A simple enumeration process looks like:

* **Reset** - The host resets the USB line state.
* **GDD/8/IN** - The host sends a `Get Device Descriptor` request and asks the
  device to send 8 bytes IN (from the device to the host) from the first 8 bytes
  of its device descriptor, using `80 06 00 01 00 00 08 00`. The device responds
  with the first 8 bytes of its device descriptor, in this example `12 01 10 02
  00 00 00 40` is sent. Once this is received, the host sends back a zero length
  status packet (called a "ZLP") to indicate that it has received the
  information from the device and is done with this request. The last byte of
  the packet sent by the device (`0x40` in this example), indicates that the
  maximum packet size of the device's endpoint zero (EP0) is 64 bytes. Using
  this information, the host then assigns the device a unique address and then
  requests the full device descriptor.
* **SDA/0/OUT** - The host sends a `Set Device Address` request, which includes
  the newly assigned device address and asks for 0 bytes of data back from the
  device. An example packet is `00 05 01 00 00 00 00 00`, with the `0x01`
  meaning that the device is being assigned device address 1. This request is an
  OUT request and request 0 bytes of data in return. Thus, the device only
  responds with a ZLP to indicate that the request has been received and
  acknowledged.
* **GDD/18/IN** - Now that the device has been assigned address 1, the host will
  request the full device descriptor from device 1, on endpoint 0, using a
  maximum packet size of 64 bytes, and it asks for 18 bytes of data. The reason
  it asks for 18 bytes is because the first byte of the device's initial
  response (the `0x12`) indicated that its full device descriptor is 18 bytes.
  Thus, this request would look like: `80 06 00 01 00 00 12 00`. The device will
  respond with all 18 bytes and the host will send back a ZLP to indicate that
  it has received the device's response, which may look something like `12 01 10
  02 00 00 00 40 8a 2e 0c 00 03 01 01 02 03 01`.
* **GCD/9/I** - The host now issues the `Get Configuration Descriptor` to
  request the device send the first 9 bytes of its configuration data. The
  process is similar to the requests above. The request looks like `80 06 00 02
  00 00 09 00` and the response looks like `09 02 62 00 03 01 00 80 32`.
* **SDC/0/O** - The host now issues a `Set Device Configuration` request to tell
  the device to use one of its configurations. In practice, this always seems to
  be configuration 1. The request looks like `00 09 01 00 00 00 00 00` and there
  is no response, except for the ZLP packet indicating the device is now
  configured.

At this point, the device has been assigned a unique address and is now
considered to be enumerated and configured.

### Endpoint control register (ECR)

The endpoint control register (ECR) controls how the endpoint behaves.

| Bits  | Info  |
| ---   | ---   |
| 31    | Enable this endpoint |
| 30    | 0: single buffer (64 bytes), 1: double buffering (64 bytes x 2)|
| 29    | Raise an interrupt for every buffer transferred |
| 28    | Raise an interrupt for every 2 buffers transferred (only for double buffering) |
| 27:26 | Type of endpoint, 0: control, 1: isochronous, 2: bulk, 3: interrupt |
| 25:18 | Polling interval minus 1ms (14 means poll every 15ms) |
| 15:0  | Byte offset for DPSRAM data buffer (lowest 6 bits must be zero) |

### Buffer control register (BCR)

The buffer control register (BCR) controls how to deal with single or double
buffering for each endpoint. If the endpoint is configured for single buffering,
then only the low bits (15:0) are used and they refer to the first buffer
(buffer 0). If the endpoint is configured for double buffering, then the high
bits (31:16) refer to the second/double buffer (buffer 1). Notice that each 16
bit half of this register is not exactly the same.

This register is also special because the upper 16 bits and lower 16 bits can be
read or written to independently. If this endpoint is single buffered, this has
no impact. But, if the endpoint is double buffered, then care should be taken to
write each 16 bit half of the register independently. In practice, this means it
is better to consider this 32 bit register as two separate 16 bit registers when
writing to it.

| Bits   | Info  |
| ---    | ---   |
| *High* | *Only used if double buffering and refers only to the second/double buffer (buffer 1)* |
| 31     | FULL buffer. `HOST:` 0: buffer is empty, ready to receive an IN packet from the device; 1: buffer is full, ready to send an OUT packet to the device. `DEVICE:` 0: buffer is empty, ready to receive an OUT packet from the host; 1: buffer is full, ready to send an IN packet to the host.
| 30     | LAST buffer in the transfer. If INTE has TRANS_COMPLETE set (bit 3), then the TRANS_COMPLETE interrupt will be raised when this buffer completes. |
| 29     | DATA PID for this packet. 0: DATA0, 1: DATA1. |
| 28:27  | `Isochronous endpoints:` Byte offset of this second buffer relative to the first buffer, 0: 128, 1: 256, 3: 1024. |
| 26     | Buffer is AVAILABLE for, and now owned by, the controller. 0: controller clears this bit to indicate that the processer now owns this buffer, 1: processor sets this bit to indicate that the controller now owns this buffer. |
| 25:16  | Buffer length for this buffer. |

| Bits   | Info  |
| ---    | ---   |
| *Low*  | *Used for the first buffer (buffer 0) of this endpoint* |
| 15     | FULL buffer. `HOST:` 0: buffer is empty, ready to receive an IN packet from the device; 1: buffer is full, ready to send an OUT packet to the device. `DEVICE:` 0: buffer is empty, ready to receive an OUT packet from the host; 1: buffer is full, ready to send an IN packet to the host.
| 14     | LAST buffer in the transfer. If INTE has TRANS_COMPLETE set (bit 3), then the TRANS_COMPLETE interrupt will be raised when this buffer completes. |
| 13     | DATA PID for this packet. 0: DATA0, 1: DATA1. |
| 12     | `DEVICE:` Reset buffer select to buffer 0. |
| 11     | `HOST:` Received a STALL. `DEVICE:` Send a STALL. |
| 10     | Buffer is AVAILABLE for, and now owned by, the controller. 0: controller clears this bit to indicate that the processer now owns this buffer, 1: processor sets this bit to indicate that the controller now owns this buffer. |
| 9:0    | Buffer length for this buffer. |

### SETUP Requests

The first 8 bytes of DPSRAM (for both `host` mode and `device` mode) are
reserved for SETUP packets, which are exactly 8 bytes in length. In order for
the host controller to send a SETUP request, the following is needed:

1) The USB controller must have been reset and configured
2) The MAIN_CTRL must have a value such as 0x3
3) The DAR must have a value of 0x0
4) The software ECR must have a value such as 0xA0000180
5) The 8 byte SETUP packet must be written to address 0x50100000
6) The software BCR must have a value such as 0x0000047B
7) The SCR (SIE_CTRL) must have a value such as 0X20008E0B

Once conditions such as this are established, the host controller will send a
packet to the device to begin the enumeration process.