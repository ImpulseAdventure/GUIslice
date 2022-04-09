
<h1>

GUIslice 
[![Badge Contributions]](#) 
[![Badge License]][License] 
[![Badge Issues]][Issues] 
[![Badge Build]][Travis]

</h1>

*A lightweight framework for embedded displays*

Design your GUI with a **drag & drop builder**, then apply the same code to a wide range of displays, libraries and controllers with the **cross-platform framework**. Open source **MIT license** grants free commercial usage.

![Example 1]
![Example 5]
![Preview]

- Extensive [Documentation] guides available
- [GUIslice API documentation (online)][API] & [(PDF)][PDF]
- Active development: see [latest updates & work in progress][Latest]
- [Release history][Releases]
- [Website (www.impulseadventure.com)][Website]
- **Support email**: guislice@gmail.com
- GUIslice by Calvin Hass and [GitHub contributors][Contributors], Builder by Paul Conti

## Features ##
- Pure C library, no dynamic memory allocation
- *Widgets*:
  - text, images, buttons, checkboxes, radio buttons, sliders, custom keypads, listbox,
  radial controls, scrolling textbox / terminal, graphs, etc. plus extensions and multiple pages.
- Cross-platform **GUIslice Builder** application to generate layouts
- *Platform-independent* GUI core currently supports:
  - Adafruit-GFX, TFT_eSPI, mcufriend, UTFT, LCDGFX, SDL1.2, SDL2.0
- *Devices*:
  - Raspberry Pi, Arduino, ATmega2560, ESP8266 / NodeMCU, ESP32, M5stack, Teensy 3 / T4, WIO Terminal, Feather M0 (Cortex-M0), nRF52 (Cortex-M4F), LINUX, Beaglebone Black, STM32, Due, etc.
- *Typical displays*:
  - PiTFT, Adafruit TFT 3.5" / 2.8" / 2.4" / 2.2" / 1.44", FeatherWing TFT, OLED 0.96", mcufriend, BuyDisplay / EastRising 4.3" 5" 7", Waveshare, 4D Cape
- *Display drivers include*:
  - ILI9341, ST7735, SSD1306, HX8347D, HX8357, PCD8544, RA8875, RA8876, ILI9225, ILI9341_t3, ILI9341_due
- *Touchscreen control including*:
  - STMPE610, FT6206, FT5206, XPT2046, 4-wire, tslib, URTouch, Adafruit Seesaw
- *IDE Support*:
  - GUIslice has been tested for use in the *Arduino IDE* and *Platform IO* environments, in addition to LINUX make
- Foreign characters / UTF-8 encoding (in SDL mode), anti-aliased fonts (in TFT_eSPI mode)
- Dynamic display rotation
- GPIO / pin / keyboard / Adafruit Seesaw navigation for non-touchscreen devices

## Screenshots ##
![Example 3]
![Example 4]
![Example 2]

## GUIslice Builder ##
- Includes cross-platform (Windows, LINUX and Mac) desktop application to generate GUIslice layouts
- Please refer to [GUIslice Builder wiki][Builder] for documentation

## Disclaimer ##
The Software is not designed for use in devices or situations where there may be physical injury if the Software has errors.


<!----------------------------------{ Badges }--------------------------------->

[Badge Contributions]: https://img.shields.io/badge/contributions-welcome-orange.svg
[Badge License]:       https://img.shields.io/badge/license-MIT-blue.svg
[Badge Issues]:        https://img.shields.io/github/issues/ImpulseAdventure/GUIslice.svg
[Badge Build]:         https://travis-ci.org/ImpulseAdventure/GUIslice.svg?branch=master


<!---------------------------------{ General }--------------------------------->

[License]: LICENSE

[Documentation]: https://github.com/ImpulseAdventure/GUIslice/wiki
[Contributors]:  https://github.com/ImpulseAdventure/GUIslice/graphs/contributors
[Builder]:       https://github.com/ImpulseAdventure/GUIslice/wiki/GUIslice-Builder
[Release]:       https://github.com/ImpulseAdventure/GUIslice/releases
[Latest]:        https://github.com/ImpulseAdventure/GUIslice/issues/85
[Issues]:        https://github.com/ImpulseAdventure/GUIslice/issues
[PDF]:           https://github.com/ImpulseAdventure/GUIslice/raw/master/docs/GUIslice_ref.pdf

[Website]: https://www.impulseadventure.com/elec/guislice-gui.html
[Travis]:  https://travis-ci.org/ImpulseAdventure/GUIslice
[API]:     https://impulseadventure.github.io/GUIslice/modules.html


<!-------------------------------{ Screenshots }------------------------------->

[Example 5]: https://www.impulseadventure.com/elec/images/guislice-ctrl2.png
[Example 4]: https://www.impulseadventure.com/elec/images/guislice-ex08.png
[Example 3]: https://www.impulseadventure.com/elec/images/guislice-ex06.png
[Example 1]: https://www.impulseadventure.com/elec/images/sdl_menu1.png
[Example 2]: https://user-images.githubusercontent.com/8510097/48299251-733a9a00-e47f-11e8-87ac-e35be6ba41d1.png

[Preview]: https://user-images.githubusercontent.com/8510097/90728338-9a8be100-e279-11ea-969e-cbd8bb0ac6c6.png
