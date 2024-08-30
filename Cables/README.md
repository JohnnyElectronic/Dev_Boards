# Arduino Development Board Programming Cables

These cables were made to support my development boards that are used for adding electronics to scale model builds or other areas where a compact SOC is needed. By eliminating the programming socket it keeps the board as small as possible for placement in confined areas. 


## ICSP 6-pin ST Programming Cable

<img src="img/ICSP_6-pin_ST_cbl.jpg" width="50%">

This is a ICSP programming cable for the older 6-pin Arduino ICSP. It uses a spring loaded soft-touch (ST) 6-pin header that only needs to touch the ICSP pad for programming. 


<img src="img/6-pin_ST_board connection.jpg" width="30%">


The other end connects to a 6-pin ICSP header from a ISP programmer such as an Arduino Uno or Nano. When using an Arduino as ISP you will either need to modify the reset line to connect to D10 at the ICSP header or separate the orange pins and connect it directly to the D10 output. A 10uf capacitor is also needed on the ISP prgrammer reset line to allow programming as an ISP.

__Technical Details__
* Cable length is a minimum of 12" (30.5cm) in length 
* The soft-touch end is a spring-loaded 6 position gold plated header
* The reset line can be disconnected for attaching to D10 for programming


[schematic](schematics/ICSP_6Wire.pdf)


## UPDI 6-pin ST Programming Cable

<img src="img/UPDI_6-pin_ST_cbl.jpg" width="50%">

This is a UPDI programming cable for the newer AVR parts. It uses a spring loaded soft-touch (ST) 6-pin header that only needs to touch the ICSP pad for programming. 


This only works with ISCP pads that can be configured for Pin 1 to be used for a UPDI connection.


The other end connects to 4 single pin dupont headers that are used to connect to a USB to serial converter such as CH340 types. The cable has a Schottky diode and resistor included. This cable has been tested with CH340E and CH9340C USB to Serial converters. Others may work as well or need further resistor modifications.


<img src="img/UPDI_Circuit.jpg" width="25%">


__Technical Details__
* Cable length is a minimum of 12" (30.5cm) in length 
* The soft-touch end is a spring-loaded 6 position gold plated header
* Flying single pin dupont leads for connecting to a power source and a USB to serial converter
* Schottky diode and resistor built in to the cable

[schematic](schematics/UPDI_6wire.pdf)


## UPDI 3-pin ST Programming Cable

<img src="img/UPDI_3-pin_ST_cbl.jpg" width="50%">

This is a UPDI programming cable for the newer AVR parts. It uses a spring loaded soft-touch (ST) 3-pin header that only needs to touch the UPDI pad for programming. 


This only uses pin 2-4 of the UPDI 4 pin pad. The pin 1 Reset line is not used. 


The other end connects to 4 single pin dupont headers that are used to connect to a USB to serial converter such as CH340 types. The cable has a Schottky diode and resistor included. This cable has been tested with CH340E and CH9340C USB to Serial converters. Others may work as well or need further resistor modifications.


<img src="img/UPDI_Circuit.jpg" width="25%">


__Technical Details__
* Cable length is a minimum of 12" (30.5cm) in length 
* The soft-touch end is a spring-loaded 3 position gold plated header
* Flying single pin dupont leads for connecting to a power source and a USB to serial converter
* Schottky diode and resistor built in to the cable


[schematic](schematics/UPDI_3wire.pdf)


## Purchasing
[Visit my Tindie Store](https://www.tindie.com/stores/johnnyelectronic/)


> [!NOTE]
> [See My YouTube Channel for examples on programming my development boards with these cables](https://www.youtube.com/@Johnny_Electronic)


## Additional Information
 
The UPDI cables have been tested with a CH340E and CH9340C USB to serial converter. Others should work as well.

Using the CH340E or the CH9340C the baud rates listed below are known to work with the Arduino IDE.

    * SerialUPDI - Slow: 57600 baud
    * SerialUPDI - 230400 baud
    * SerialUPDI - Fast: 4.5v+ 460800 baud (CH340 – and maybe some others)
    * SerialUPDI – TURBO: 4.5v+ 912600 baud (CH340 (and maybe some others) only)

The Arduino IDE has programming support for Serial UPDI. Additional information is available here [SerialUPDI](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md)


__Cable Flying Lead Connections__

    * Connect the Red(+) and Black(-) leads to a power source for programming the part. Most USB to Serial converters have a 5v source and some also include a 3.3volt.
    * Connect the labeled TX lead to the TX of the converter
    * Connect the labeled RX lead to the RX of the converter


## Directories
- [schematics](schematics/)
- [images](img/)


## Licensing

This work is licensed under Creative Commons Attribution-ShareAlike 4.0 International. 
To view a copy of this license, visit [https://creativecommons.org/licenses/by-sa/4.0/](https://creativecommons.org/licenses/by-sa/4.0/)

Distributed as-is; no warranty is given.

