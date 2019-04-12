#ifndef _GUISLICE_CONFIG_H_
#define _GUISLICE_CONFIG_H_

// =======================================================================
// GUIslice library (user configuration) selection by device
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2019 Calvin Hass
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =======================================================================
// \file GUIslice_config.h


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// =========================================================================================
// SELECT ONE OF THE FOLLOWING EXAMPLE CONFIGURATIONS OR ADD YOUR OWN
// - Uncomment one of the following lines
// - These example configurations are located in the /configs folder
// - To add your own, make a copy of an example config, rename it
//   and add it to the list here.
// - If no line is uncommented, then the default combined configuration
//   file will be used, ie. GUIslice_config_ard.h / GUIslice_config_linux.h
//   which is selected at the bottom of this file
// - Refer to https://github.com/ImpulseAdventure/GUIslice/wiki/Display-Config-Table
//   to help identify a suitable config for your MCU shield / display
// =========================================================================================

  // ---------------------------------------------------------------------------------------
  // Add your own configs here:
  // ---------------------------------------------------------------------------------------
  //#include "../configs/my-config.h"

  // ---------------------------------------------------------------------------------------
  // Example configs included in library /configs:
  // ---------------------------------------------------------------------------------------

  // Arduino, ARM SAMD, Cortex M0/M4, nRF52:
  // ------------------------------------------------------
  //#include "../configs/ard-shld-adafruit_18_joy.h"
  //#include "../configs/ard-shld-adafruit_28_cap.h"
  //#include "../configs/ard-shld-adafruit_28_res.h"
  //#include "../configs/ard-shld-generic1_35_touch.h"
  //#include "../configs/ard-shld-mcufriend.h"
  //#include "../configs/ard-shld-mcufriend_4wire.h"
  //#include "../configs/ard-shld-mcufriend_xpt2046.h"
  //#include "../configs/ard-shld-waveshare_28_touch.h"
  //#include "../configs/ard-shld-osmart_22_68130_touch.h"
  //#include "../configs/ard-adagfx-hx8347-xpt2046.h"
  //#include "../configs/ard-adagfx-hx8357-ft6206.h"
  //#include "../configs/ard-adagfx-hx8357-notouch.h"
  //#include "../configs/ard-adagfx-hx8357-simple.h"
  //#include "../configs/ard-adagfx-hx8357-stmpe610.h"
  //#include "../configs/ard-adagfx-ili9341-ft6206.h"
  //#include "../configs/ard-adagfx-ili9341-input.h"
  //#include "../configs/ard-adagfx-ili9341-notouch.h"
  //#include "../configs/ard-adagfx-ili9341-simple.h"
  //#include "../configs/ard-adagfx-ili9341-stmpe610.h"
  //#include "../configs/ard-adagfx-ili9341-xpt2046.h"
  //#include "../configs/ard-adagfx-pcd8544-notouch.h"
  //#include "../configs/ard-adagfx-ra8875-notouch.h"
  //#include "../configs/ard-adagfx-ssd1306-notouch.h"
  //#include "../configs/ard-adagfx-st7735-notouch.h"

  // ESP-8266, ESP32, M5stack, TTGO:
  // ------------------------------------------------------
  //#include "../configs/esp-shld-m5stack.h"
  //#include "../configs/esp-shld-ttgo_btc_ticker.h"
  //#include "../configs/esp-tftespi-default-ft6206.h"
  //#include "../configs/esp-tftespi-default-notouch.h"
  //#include "../configs/esp-tftespi-default-simple.h"
  //#include "../configs/esp-tftespi-default-stmpe610.h"
  //#include "../configs/esp-tftespi-default-xpt2046.h"
  //#include "../configs/esp-tftespi-default-xpt2046_int.h"

  // STM32:
  // ------------------------------------------------------
  //#include "../configs/stm32-adagfx-mcufriend-simple.h"

  // Multi-device shields:
  // ------------------------------------------------------
  //#include "../configs/mult-shld-adafruit_24_feather_touch.h"
  //#include "../configs/mult-shld-adafruit_35_feather_touch.h"

  // Raspberry Pi / LINUX:
  // ------------------------------------------------------
  //#include "../configs/rpi-sdl1-default-tslib.h"
  //#include "../configs/linux-sdl1-default-mouse.h"


// =========================================================================================
// DEFAULT COMBINED CONFIGURATION FILE
// - If no user configuration has been selected, a default config will be selected here
// - Note that the include guard _GUISLICE_CONFIG_ARD_H_ and _GUISLICE_CONFIG_LINUX_H_
//   will prevent these from loading if any of the user configs have been loaded
// =========================================================================================

// Import user configuration depending on device type
#if defined(__linux__)
  #include "GUIslice_config_linux.h"
#elif defined(__AVR__) || defined(ARDUINO_SAMD_ZERO)
  #include "GUIslice_config_ard.h"
#elif defined(ESP8266) || defined(ESP32)
  #include "GUIslice_config_ard.h"
#elif defined(NRF52)
  #include "GUIslice_config_ard.h"
#elif defined(ARDUINO_STM32_FEATHER) || defined(__STM32F1__)
  #include "GUIslice_config_ard.h"
#elif defined(ARDUINO_ARCH_STM32)  // ST Core from STMicroelectronics
  #include "GUIslice_config_ard.h"
#elif defined(ARDUINO_ARCH_SAM)   // DUE
  #include "GUIslice_config_ard.h"
#elif defined(ARDUINO_ARCH_SAMD)   // M0_PRO
  #include "GUIslice_config_ard.h"
#else
  #error "Unknown device platform"
#endif

// -----------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_CONFIG_H_