
<br>

<div align = center>

[![Badge Contributions]][#]   
[![Badge License]][License]   
[![Badge Issues]][Issues]   
[![Badge Build]][Travis]

<br>

# GUIslice

*A lightweight framework for embedded displays*

<br>
<br>

Design your GUI with a **drag & drop builder**, then apply <br>
the same code to a wide range of displays, libraries and <br>
controllers with the **cross-platform framework**.

Open source **MIT license** grants free commercial usage.

<br>
<br>

![Example 1]
![Example 5]
![Preview]

</div>

<br>
<br>

- Extensive [Documentation] guides available
- [GUIslice API documentation (online)][API] & [(PDF)][PDF]
- Active development: see [latest updates & work in progress][Latest]
- [Release history][Releases]
- [Website (www.impulseadventure.com)][Website]
- **Support email**: guislice@gmail.com
- GUIslice by Calvin Hass and [GitHub contributors][Contributors], Builder by Paul Conti

<br>
<br>

## Features

-   Pure C library, no dynamic memory allocation

-   Cross-platform **GUIslice Builder** <br>
    application to generate layouts
    
-   IDE Support

    GUIslice has been tested for use in the *Arduino IDE* <br>
    and *Platform IO* environments, in addition to LINUX make

-   Foreign characters / UTF-8 encoding (in SDL mode), <br>
    anti-aliased fonts (in TFT_eSPI mode)

-   Dynamic display rotation

-   GPIO / pin / keyboard / Adafruit Seesaw <br>
    navigation for non-touchscreen devices

<br>

### Widgets

<kbd>  Scrolling Textbox / Terminal  </kbd>  
<kbd>  Radial Controls  </kbd>  
<kbd>  Custom Keypads  </kbd>  

<kbd>  Multiple Pages  </kbd>  
<kbd>  Extensions  </kbd>  
<kbd>  Checkboxes  </kbd>  
<kbd>  Listbox  </kbd>  
<kbd>  Text  </kbd>  

<kbd>  Buttons  </kbd>  
<kbd>  Images  </kbd>  
<kbd>  Radio Buttons  </kbd>  
<kbd>  Graphs  </kbd>  

<br>

### Platform-independent GUI Cores

<kbd>  Adafruit-GFX  </kbd>  
<kbd>  TFT_eSPI  </kbd>  
<kbd>  mcufriend  </kbd>

<kbd>  LCDGFX  </kbd>  
<kbd>  UTFT  </kbd>  
<kbd>  SDL1.2  </kbd>  
<kbd>  SDL2.0  </kbd>  

<br>

### Devices

<kbd>  ESP8266 / NodeMCU  </kbd>     
<kbd>  Raspberry Pi  </kbd>     
<kbd>  ATmega2560  </kbd>   
<kbd>  M5stack  </kbd>   

<kbd>  Feather M0 (Cortex-M0)  </kbd>  
<kbd>  Beaglebone Black  </kbd>  
<kbd>  ESP32  </kbd>  
<kbd>  LINUX  </kbd>  

<kbd>  nRF52 (Cortex-M4F)  </kbd>  
<kbd>  Teensy 3 / T4  </kbd>  
<kbd>  WIO Terminal  </kbd>  
<kbd>  STM32  </kbd>  

<kbd>  Arduino  </kbd>  
<kbd>  Due  </kbd>
    
<br>

### Typical Displays

<kbd>  PiTFT  </kbd>  
<kbd>  Adafruit TFT 3.5" / 2.8" / 2.4" / 2.2" / 1.44"  </kbd>

<kbd>  Waveshare  </kbd>  
<kbd>  4D Cape  </kbd>  
<kbd>  FeatherWing TFT  </kbd>  
<kbd>  OLED 0.96"  </kbd>

<kbd>  mcufriend  </kbd>  
<kbd>  BuyDisplay / EastRising 4.3" 5" 7"  </kbd>  

<br>

### Display Drivers
    
<kbd>  ILI9341  </kbd>  
<kbd>  ST7735  </kbd>  
<kbd>  SSD1306  </kbd>  
<kbd>  HX8347D  </kbd>  
<kbd>  HX8357  </kbd>  
<kbd>  PCD8544  </kbd>

<kbd>  RA8875  </kbd>  
<kbd>  RA8876  </kbd>  
<kbd>  ILI9225  </kbd>  
<kbd>  ILI9341_t3  </kbd>  
<kbd>  ILI9341_due  </kbd>

<br>

### Touchscreen Controls

<kbd>  4-wire  </kbd>  
<kbd>  tslib  </kbd>  
<kbd>  URTouch  </kbd>  
<kbd>  Adafruit Seesaw  </kbd>

<kbd>  STMPE610  </kbd>  
<kbd>  FT6206  </kbd>  
<kbd>  FT5206  </kbd>  
<kbd>  XPT2046  </kbd>

<br>
<br>

<div align = center>

## Screenshots

<br>

![Example 3]
![Example 4]
![Example 2]

</div>

<br>
<br>

## GUIslice Builder

-   Includes cross-platform (Windows, LINUX and Mac) <br>
    desktop application to generate GUIslice layouts

-   Please refer to [GUIslice Builder wiki][Builder] for documentation

<br>
<br>

## Disclaimer

The Software is not designed for use in devices or situations <br>
where there may be physical injury if the Software has errors.

<br>




<!---------------------------------{ General }--------------------------------->

[Documentation]: https://github.com/ImpulseAdventure/GUIslice/wiki
[Contributors]:  https://github.com/ImpulseAdventure/GUIslice/graphs/contributors
[Releases]:       https://github.com/ImpulseAdventure/GUIslice/releases
[Builder]:       https://github.com/ImpulseAdventure/GUIslice/wiki/GUIslice-Builder
[Latest]:        https://github.com/ImpulseAdventure/GUIslice/issues/85
[Issues]:        https://github.com/ImpulseAdventure/GUIslice/issues
[PDF]:           https://github.com/ImpulseAdventure/GUIslice/raw/master/docs/GUIslice_ref.pdf

[Website]: https://www.impulseadventure.com/elec/guislice-gui.html
[Travis]:  https://travis-ci.org/ImpulseAdventure/GUIslice
[API]:     https://impulseadventure.github.io/GUIslice/modules.html

[License]: LICENSE
[#]: #

<!-------------------------------{ Screenshots }------------------------------->

[Example 5]: https://www.impulseadventure.com/elec/images/guislice-ctrl2.png
[Example 4]: https://www.impulseadventure.com/elec/images/guislice-ex08.png
[Example 3]: https://www.impulseadventure.com/elec/images/guislice-ex06.png
[Example 1]: https://www.impulseadventure.com/elec/images/sdl_menu1.png
[Example 2]: https://user-images.githubusercontent.com/8510097/48299251-733a9a00-e47f-11e8-87ac-e35be6ba41d1.png

[Preview]: https://user-images.githubusercontent.com/8510097/90728338-9a8be100-e279-11ea-969e-cbd8bb0ac6c6.png


<!----------------------------------{ Badges }--------------------------------->

[Badge Contributions]: https://img.shields.io/badge/Contributions-Welcome-067a35.svg?style=for-the-badge&labelColor=0ABF53
[Badge License]: https://img.shields.io/badge/License-MIT-ac8b11.svg?style=for-the-badge&labelColor=yellow
[Badge Issues]: https://img.shields.io/github/issues/ImpulseAdventure/GUIslice.svg?style=for-the-badge&labelColor=1B72BE&color=13538b
[Badge Build]: https://img.shields.io/travis/ImpulseAdventure/GUIslice?style=for-the-badge&labelColor=EF2D5E&color=b11d42

