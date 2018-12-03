# GUIslice library #
[![Build Status](https://travis-ci.org/ImpulseAdventure/GUIslice.svg?branch=master)](https://travis-ci.org/ImpulseAdventure/GUIslice)
[![GitHub Issues](https://img.shields.io/github/issues/ImpulseAdventure/GUIslice.svg)](https://github.com/ImpulseAdventure/GUIslice/issues)
![Contributions welcome](https://img.shields.io/badge/contributions-welcome-orange.svg)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

*A lightweight GUI framework suitable for embedded displays*

![Example 1](http://www.impulseadventure.com/elec/images/sdl_menu1.png)
![Example 5](http://www.impulseadventure.com/elec/images/guislice-ctrl2.png)

- [Documentation wiki (github)](https://github.com/ImpulseAdventure/GUIslice/wiki)
- [GUIslice API documentation (online)](https://impulseadventure.github.io/GUIslice/modules.html) & [(PDF)](https://github.com/ImpulseAdventure/GUIslice/raw/master/docs/GUIslice_ref.pdf)
- [Release notes](https://github.com/ImpulseAdventure/GUIslice/releases)
- [Website (www.impulseadventure.com)](https://www.impulseadventure.com/elec/guislice-gui.html)
- **Support email**: guislice @ impulseadventure . com

## Features ##
- Pure C library, no dynamic memory allocation
- *Widgets*: text, images, buttons, checkboxes, radio buttons, sliders,
  radial controls, scrolling textbox / terminal, graphs, etc. plus extensions and multiple pages.
- Includes cross-platform **GUIslice Builder** (beta) desktop application to generate layouts
- *Platform-independent GUI core currently supports*: SDL1.2, SDL2.0, Adafruit-GFX, TFT_eSPI
- *Devices*: Raspberry Pi, Arduino, ESP8266 / NodeMCU, ESP32, Feather M0 (Cortex-M0), nRF52 (Cortex-M4F), LINUX, Beaglebone Black, M5Stack, STM32
- *Typical displays*: PiTFT, Waveshare, Adafruit TFT 3.5" / 2.8" / 2.4" / 2.2" / 1.44", OLED 0.96", 4D Cape
- *Display drivers include*: ILI9341, ST7735, SSD1306, HX8357
- *Touchscreen control including*: STMPE610, FT6206, XPT2046, tslib
- Foreign characters / UTF-8 encoding (in SDL mode), anti-aliased fonts (in TFT_eSPI mode)
- Dynamic display rotation
- GPIO / pin / keyboard control for non-touchscreen devices
- No GUIslice installation -- just add include files and go!
- *LINUX Dependencies*: sdl, sdl-ttf, optional: tslib
- *Arduino Dependencies*: TFT_eSPI or Adafruit-GFX plus display / touch driver libraries

## Screenshots ##
![Example 3](http://www.impulseadventure.com/elec/images/guislice-ex06.png)
![Example 4](http://www.impulseadventure.com/elec/images/guislice-ex08.png)
![Example 2](http://www.impulseadventure.com/elec/images/microsdl-ex07.png)

## GUIslice Builder ##
- Includes cross-platform (Windows & LINUX) desktop application (beta) to generate GUIslice layouts
- Please refer to [GUIslice Builder wiki](https://github.com/ImpulseAdventure/GUIslice/wiki/GUIslice-Builder) for documentation
![GUIslice Builder](https://user-images.githubusercontent.com/8510097/48395316-c739be80-e6cb-11e8-9dd6-33336f1fca06.PNG)


## Device Configuration
- The following table lists a number of devices that have been tested with GUIslice
  and the recommended configuration modes and test examples.

![guislice-devices](https://user-images.githubusercontent.com/8510097/39462702-230a68b6-4cc8-11e8-8c18-a45b55ba55c6.png)

## Important Note for Arduino Users ##
- The baseline Arduino (**ATmega328P**) devices have very limited SRAM memory (2KB SRAM, 32KB FLASH).
  Therefore, it is important that GUI elements are stored in FLASH whenever possible. A set of
  examples that demonstrate this method are located in [`/examples/arduino_min`](https://github.com/ImpulseAdventure/GUIslice/tree/master/examples/arduino_min).
  The examples in [`/examples/arduino`](https://github.com/ImpulseAdventure/GUIslice/tree/master/examples/arduino) don't use the FLASH optimizations and are less likely
  to run on these limited devices.
- Other Arduino variants and devices such as **ATmega2560** (8KB SRAM, 256KB FLASH), **ESP8266**, **Node-MCU**,
  **Feather M0**, etc. tend to work much better as there is far more SRAM and FLASH available.
- By default, `DEBUG_ERR` is enabled on many devices to provide error messages via the Serial Monitor
  interface. However, if further reduction of FLASH memory is necessary, disable DEBUG_ERR
  in the `GUIslice_config_*.h`.
