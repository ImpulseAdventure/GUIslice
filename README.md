# GUIslice library #
*A lightweight GUI framework suitable for embedded displays*
- [Website (www.impulseadventure.com)](https://www.impulseadventure.com/elec/guislice-gui.html)
- [Documentation wiki (github)](https://github.com/ImpulseAdventure/GUIslice/wiki)
- [Release notes](https://github.com/ImpulseAdventure/GUIslice/releases)

## Features ##
- Pure C library, no dynamic memory allocation
- *Widgets*: text, images, buttons, checkboxes, radio buttons, sliders,
  radial controls, scrolling textbox / terminal, graphs, etc. plus extensions and multiple pages.
- *Platform-independent GUI core currently supports*: SDL1.2, SDL2.0, Adafruit-GFX, TFT_eSPI
- *Typical target*: Raspberry Pi, Arduino, ESP8266 / NodeMCU, Cortex M0 (Feather M0), LINUX, Beaglebone Black
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

## Important Note for Arduino Users ##
- *Major changes are arriving soon in **GUIslice version 0.10**, which will reduce
  memory consumption for Arduino devices considerably. Stay tuned!*
- The baseline Arduino (**ATmega328P**) devices have very limited SRAM memory (2KB SRAM, 32KB FLASH).
  Therefore, it is important that GUI elements are stored in FLASH whenever possible. A set of
  examples that demonstrate this method are located in `arduino_min`.
  The examples in `arduino` don't use the FLASH optimizations and are less likely
  to run on these limited devices.
- Other Arduino variants and devices such as **ATmega2560** (8KB SRAM, 256KB FLASH), **ESP8266**, **Node-MCU**,
  **Feather M0**, etc. tend to work much better as there is far more SRAM and FLASH available.
- By default, `DEBUG_ERR` is enabled to provide error messages via the Serial Monitor
  interface. However, if further reduction of FLASH memory is necessary, disable DEBUG_ERR
  in the `GUIslice_config.h`.
