# GUIslice library #
*A lightweight GUI framework suitable for embedded displays*
- [Website (www.impulseadventure.com)](https://www.impulseadventure.com/elec/guislice-gui.html)
- [Documentation wiki (github)](https://github.com/ImpulseAdventure/GUIslice/wiki)
- [GUIslice API documentation (PDF)](https://github.com/ImpulseAdventure/GUIslice/raw/master/docs/GUIslice_ref.pdf)
- [Release notes](https://github.com/ImpulseAdventure/GUIslice/releases)
- Build status: [![Build Status](https://travis-ci.org/ImpulseAdventure/GUIslice.svg?branch=master)](https://travis-ci.org/ImpulseAdventure/GUIslice)

## Features ##
- Pure C library, no dynamic memory allocation
- *Widgets*: text, images, buttons, checkboxes, radio buttons, sliders,
  radial controls, scrolling textbox / terminal, graphs, etc. plus extensions and multiple pages.
- *Platform-independent GUI core currently supports*: SDL1.2, SDL2.0, Adafruit-GFX, TFT_eSPI
- *Devices*: Raspberry Pi, Arduino, ESP8266 / NodeMCU, ESP32, Feather M0 (Cortex-M0), nRF52 (Cortex-M4F), LINUX, Beaglebone Black, M5Stack, (soon STM32)
- *Typical displays*: PiTFT, Waveshare, Adafruit TFT 3.5" / 2.8" / 2.4" / 2.2" / 1.44", OLED 0.96", 4D Cape
- *Display drivers include*: ILI9341, ST7735, SSD1306, HX8357
- *Touchscreen control including*: STMPE610, FT6206, XPT2046, tslib
- No GUIslice installation -- just add include files and go!
- *LINUX Dependencies*: sdl, sdl-ttf, optional: tslib
- *Arduino Dependencies*: TFT_eSPI or Adafruit-GFX plus display / touch driver libraries

## Screenshots ##

![Example 1](http://www.impulseadventure.com/elec/images/sdl_menu1.png)
![Example 2](http://www.impulseadventure.com/elec/images/microsdl-ex07.png)
![Example 3](http://www.impulseadventure.com/elec/images/guislice-ex06.png)
![Example 4](http://www.impulseadventure.com/elec/images/guislice-ex08.png)

![Example 5](http://www.impulseadventure.com/elec/images/guislice-ctrl2.png)


## Device Configuration
- The following table lists a number of devices that have been tested with GUIslice
  and the recommended configuration modes and test examples.
- NOTE: **STM32** support (eg. *STM32F103*) is in testing and will soon be released!

![guislice-devices](https://user-images.githubusercontent.com/8510097/35789025-15e0918c-09ee-11e8-9122-676833f0c3ee.PNG)

## Important Note for Arduino Users ##
- The baseline Arduino (**ATmega328P**) devices have very limited SRAM memory (2KB SRAM, 32KB FLASH).
  Therefore, it is important that GUI elements are stored in FLASH whenever possible. A set of
  examples that demonstrate this method are located in [`\arduino_min`](https://github.com/ImpulseAdventure/GUIslice/tree/master/arduino_min).
  The examples in [`\arduino`](https://github.com/ImpulseAdventure/GUIslice/tree/master/arduino) don't use the FLASH optimizations and are less likely
  to run on these limited devices.
- Other Arduino variants and devices such as **ATmega2560** (8KB SRAM, 256KB FLASH), **ESP8266**, **Node-MCU**,
  **Feather M0**, etc. tend to work much better as there is far more SRAM and FLASH available.
- By default, `DEBUG_ERR` is enabled on many devices to provide error messages via the Serial Monitor
  interface. However, if further reduction of FLASH memory is necessary, disable DEBUG_ERR
  in the `GUIslice_config_*.h`.
