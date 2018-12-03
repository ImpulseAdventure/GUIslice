# GUIslice library - Examples for Arduino #
This folder contains examples for Arduino devices. The examples use a simpler
API since they don't use the FLASH memory optimizations.

### Compiling ###
The examples in this folder are designed to be compiled from the within the **Arduino IDE**

### Device Support ###
Sample code in these examples are intended for the following device platforms:
- Arduino (ATmega328P) / UNO / ProMini / etc. **[*Note #1*]**
- ATmega2560, etc.
- Adafruit Feather M0 (Cortex-M0)
- Adafruit Feather nRF52 (Cortex-M4F)
- ESP8266 / NodeMCU
- ESP32, M5Stack

**[*Note #1*]**: The base Arduino device is extremely limited in memory. As a result,
a number of the examples may not run properly. Instead, it is highly recommended
to run the minimal-RAM examples (see the `\examples\arduino_min` folder) instead.

### Resource Files ###
Some sketches (eg. `ex03_ard_btn_img`) are intended to demonstrate image loading from SD cards.
In order to support these functions, one needs to ensure:
- Resource files from the `/res` folder are copied into the root of the SD card
- SD card support is enabled in `GUIslice_config_ard.h` (via `#define GSLC_SD_EN 1`)
- Some sketches (such as `ex18_ard_compound`) may include optional image support and
  therefore require uncommenting a section of code associated with the SD card images

Note that the SD card library adds considerable demands on memory.
