This is a library for the Adafruit 2.8" TFT display.
This library works with the Adafruit 2.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/335
as well as Adafruit TFT Touch Shield
  ----> http://www.adafruit.com/products/376
 
Check out the links above for our tutorials and wiring diagrams.
These displays use 8-bit parallel to communicate, 12 or 13 pins are required
to interface (RST is optional).
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.

HEAVILY modified by Brian Chen for use with non-GPIO pins on the Teensy 3.x. There was no original support for the Teensy 3.x let alone support for non GPIO ports on the board.
Delays have been removed and pin_magic.h has been completely rewritten (removing all irrelevant parts for the T3.1)
Macros that have been nested in functions with the same name have been directly used instead of calling them as functions to optimise performance.

MIT license, all text above must be included in any redistribution