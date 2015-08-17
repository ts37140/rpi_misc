# rpi_misc

## rpigpio

rpigpio is a kernel driver which prints info about GPIO settings and state in Raspberry Pi.

Driver reads directly BCM2835 peripherals registry and prints info per GPIO:
* GPIO function (input, output, alt0 etc)
* GPIO level: pin is high or low
* GPIO event: active event for level or edge. Trigger info for input mode interrupts.

Note: GPIO numbers are based on BCM2835 I/O line numbering, not accoring to pins in Raspberry P1 header. More info in "BCM2835 ARM Peripherals" document.

Driver is tested with Rapsberry Pi Model B with Raspbian Debian Wheezy (kernel version: 3.18).
