# GUIslice library - Examples for Arduino (Minimal RAM) #
This folder contains examples for Arduino devices. The examples
use a more complex API since they use the FLASH memory
optimizations that minimize RAM consumption on limited memory devices.

### Compiling ###
The examples in this folder are designed to be compiled from the within the **Arduino IDE**

### Device Support ###
Sample code in these examples are intended for the following device platforms:
- Arduino (ATmega328P) / UNO / ProMini / etc.
- ATmega2560, etc.


### Resource Files ###
Some sketches (eg. `ex03_ard_btn_img`) are intended to demonstrate image loading from SD cards.
In order to support these functions, one needs to ensure:
- Resource files from the `/res` folder are copied into the root of the SD card
- SD card support is enabled in `GUIslice_config_ard.h` (via `#define GSLC_SD_EN 1`)
- Some sketches (such as `ex05_ard_pages`) may include optional image support and
  therefore require uncommenting a section of code associated with the SD card images

Note that the SD card library adds considerable demands on memory (and is not
recommended for low-memory devices).
