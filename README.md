** This fork/repo in it's current state is configured to work with the following touchscreen:
https://www.buydisplay.com/5-inch-tft-lcd-display-capacitive-touchscreen-ra8875-controller-800x480

GFX driver is RA8876, touchscreen driver is GSL1680.  This repo contains a new driver (GUIslice_th_GSL1680.h) to support it.

# GUIslice library #
[![Build Status](https://travis-ci.org/ImpulseAdventure/GUIslice.svg?branch=master)](https://travis-ci.org/ImpulseAdventure/GUIslice)
[![GitHub Issues](https://img.shields.io/github/issues/ImpulseAdventure/GUIslice.svg)](https://github.com/ImpulseAdventure/GUIslice/issues)
![Contributions welcome](https://img.shields.io/badge/contributions-welcome-orange.svg)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

*A lightweight GUI framework for embedded displays*

Design your GUI with a **drag & drop builder**, then apply the same code to a wide range of displays, libraries and controllers with the **cross-platform framework**. Open source **MIT license** grants free commercial usage.

![Example 1](http://www.impulseadventure.com/elec/images/sdl_menu1.png)
![Example 5](http://www.impulseadventure.com/elec/images/guislice-ctrl2.png)
![GUIslice Builder](https://user-images.githubusercontent.com/8510097/63221174-14126000-c14a-11e9-93aa-20806f5627e0.png)

- Extensive [Documentation](https://github.com/ImpulseAdventure/GUIslice/wiki) guides available
- [GUIslice API documentation (online)](https://impulseadventure.github.io/GUIslice/modules.html) & [(PDF)](https://github.com/ImpulseAdventure/GUIslice/raw/master/docs/GUIslice_ref.pdf)
- Active development: see [latest updates & work in progress](https://github.com/ImpulseAdventure/GUIslice/issues/85)
- [Release history](https://github.com/ImpulseAdventure/GUIslice/releases)
- [Website (www.impulseadventure.com)](https://www.impulseadventure.com/elec/guislice-gui.html)
- **Support email**: guislice@gmail.com
- GUIslice by Calvin Hass and [GitHub contributors](https://github.com/ImpulseAdventure/GUIslice/graphs/contributors), Builder by Paul Conti

## Features ##
- Pure C library, no dynamic memory allocation
- *Widgets*:
  - text, images, buttons, checkboxes, radio buttons, sliders, keypad, listbox,
  radial controls, scrolling textbox / terminal, graphs, etc. plus extensions and multiple pages.
- Cross-platform **GUIslice Builder** application to generate layouts
- *Platform-independent* GUI core currently supports:
  - Adafruit-GFX, TFT_eSPI, mcufriend, UTFT, SDL1.2, SDL2.0
- *Devices*:
  - Raspberry Pi, Arduino, ATmega2560, ESP8266 / NodeMCU, ESP32, M5stack, Teensy 3, Feather M0 (Cortex-M0), nRF52 (Cortex-M4F), LINUX, Beaglebone Black, STM32, Due, etc.
- *Typical displays*:
  - PiTFT, Adafruit TFT 3.5" / 2.8" / 2.4" / 2.2" / 1.44", FeatherWing TFT, OLED 0.96", mcufriend, BuyDisplay / EastRising 4.3" 5" 7", Waveshare, 4D Cape
- *Display drivers include*:
  - ILI9341, ST7735, SSD1306, HX8347D, HX8357, PCD8544, RA8875, ILI9341_t3, ILI9341_due
- *Touchscreen control including*:
  - STMPE610, FT6206, FT5206, XPT2046, 4-wire, tslib, URTouch, Adafruit Seesaw
- Foreign characters / UTF-8 encoding (in SDL mode), anti-aliased fonts (in TFT_eSPI mode)
- Dynamic display rotation
- GPIO / pin / keyboard / Adafruit Seesaw control for non-touchscreen devices

## Screenshots ##
![Example 3](http://www.impulseadventure.com/elec/images/guislice-ex06.png)
![Example 4](http://www.impulseadventure.com/elec/images/guislice-ex08.png)
![snap07_c](https://user-images.githubusercontent.com/8510097/48299251-733a9a00-e47f-11e8-87ac-e35be6ba41d1.png)

## GUIslice Builder ##
- Includes cross-platform (Windows, LINUX and Mac) desktop application to generate GUIslice layouts
- Please refer to [GUIslice Builder wiki](https://github.com/ImpulseAdventure/GUIslice/wiki/GUIslice-Builder) for documentation

