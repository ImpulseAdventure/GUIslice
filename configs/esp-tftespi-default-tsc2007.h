#ifndef _GUISLICE_CONFIG_ARD_H_
#define _GUISLICE_CONFIG_ARD_H_

// =============================================================================
// GUIslice library (example user configuration) for:
//   - CPU:     ESP8266 / ESP32
//   - Display: TFT_eSPI (defined by TFT_eSPI config)
//   - Touch:   TSC2007 (Resistive)
//   - Wiring:  Custom breakout
//              - Pinout defined by TFT_eSPI's User_Setup.h
//
//   - Example display:
//     -
//
// TFT_eSPI Notes:
//   - When using the TFT_eSPI library, there are additional
//     library-specific configuration files that may need
//     customization (including pin configuration), such as
//     "User_Setup_Select.h" (typically located in the
//     Arduino /libraries/TFT_eSPI folder). Please refer to
//     Bodmer's TFT_eSPI library for more details:
//     https://github.com/Bodmer/TFT_eSPI
//
//   - To avoid potential SPI conflicts, it is recommended
//     that SUPPORT_TRANSACTIONS is defined in TFT_eSPI's "User Setup"
//
//   - For touch support by external drivers, the TFT_eSPI's integrated
//     XPT2046 touch driver should be disabled. This can be done by
//     ensuring that TFT_eSPI's User_Setup.h does not define TOUCH_CS
//     (ie. it should be commented out).
//
// DIRECTIONS:
// - To use this example configuration, include in "GUIslice_config.h"
//
// WIRING:
// - As this config file is designed for a breakout board, customization
//   of the Pinout in SECTION 2 will be required to match your display.
//
// =============================================================================
// - Calvin Hass
// - https://github.com/ImpulseAdventure/GUIslice
// =============================================================================
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
// =============================================================================
// \file GUIslice_config_ard.h

// =============================================================================
// User Configuration
// - This file can be modified by the user to match the
//   intended target configuration
// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


  // =============================================================================
  // USER DEFINED CONFIGURATION
  // =============================================================================

  // -----------------------------------------------------------------------------
  // SECTION 1: Device Mode Selection
  // - The following defines the display and touch drivers
  //   and should not require modifications for this example config
  // -----------------------------------------------------------------------------
  #define DRV_DISP_TFT_ESPI         // bodmer/TFT_eSPI
  #define DRV_TOUCH_ADA_TSC2007     // Adafruit TSC2007 touch driver

  // -----------------------------------------------------------------------------
  // SECTION 2: Pinout
  // -----------------------------------------------------------------------------

  // For TFT_eSPI, the display wiring is defined by TFT_eSPI's User_Setup.h

  // SD Card
  //#define ADAGFX_PIN_SDCS    2 // ESP8266 + Adafruit FeatherWing 2.4"
  //#define ADAGFX_PIN_SDCS     14 // ESP32   + Adafruit FeatherWing 2.4"
  //#define ADAGFX_PIN_SDCS    5 // Others  + Adafruit FeatherWing 2.4"

 #ifdef ESP8266
  #define ADAGFX_PIN_SDCS     2     // SD card chip select (if GSLC_SD_EN=1)
#elif ESP32
  #define ADAGFX_PIN_SDCS     14    // SD card chip select (if GSLC_SD_EN=1)
#elif ARDUINO_SAMD_FEATHER_M0
  #define ADAGFX_PIN_SDCS     5     // SD card chip select (if GSLC_SD_EN=1)
#else
  #define ADAGFX_PIN_SDCS     4     // SD card chip select (if GSLC_SD_EN=1)
#endif

 // -----------------------------------------------------------------------------
  // SECTION 3: Orientation
  // -----------------------------------------------------------------------------

  // Set Default rotation of the display
  // - Values 0,1,2,3. Rotation is clockwise
  #define GSLC_ROTATE     1

  // -----------------------------------------------------------------------------
  // SECTION 4: Touch Handling
  // - Documentation for configuring touch support can be found at:
  //   https://github.com/ImpulseAdventure/GUIslice/wiki/Configure-Touch-Support
  // -----------------------------------------------------------------------------


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  // SECTION 4A: Update your pin connections here
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  // TFT_eSPI: Chip Select for Touch Device
  // - Since we are not using the TFT_eSPI's integrated XPT2046 touch driver,
  //   it is best to disable it. This can be done by ensuring that the TOUCH_CS line
  //   in TFT_eSPI's User_Setup.h is commented out.

  // Select touch device wiring method by setting one of the following to 1, others to 0
  #define ADATOUCH_I2C_HW 1  // Touch controller via hardware I2C (uses ADATOUCH_I2C_ADDR)

  // Touch bus & pinout
  #define ADATOUCH_I2C_ADDR   0x48  // Touch device I2C address (for ADATOUCH_I2C_HW=1)

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  // SECTION 4B: Update your calibration settings here
  // - These values should come from the diag_ard_touch_calib sketch output
  // - Please update the values to the right of ADATOUCH_X/Y_MIN/MAX_* accordingly
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  // Calibration settings from diag_ard_touch_calib:

  // For TSC2007
  #define ADATOUCH_X_MIN    232
  #define ADATOUCH_X_MAX    3820
  #define ADATOUCH_Y_MIN    129
  #define ADATOUCH_Y_MAX    3847
  #define ADATOUCH_REMAP_YX 0  
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  // SECTION 4D: Additional touch configuration
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  // -----------------------------------------------------------------------------
  // SECTION 5: Diagnostics
  // -----------------------------------------------------------------------------

  // Error reporting
  // - Set DEBUG_ERR to >0 to enable error reporting via the Serial connection
  // - Enabling DEBUG_ERR increases FLASH memory consumption which may be
  //   limited on the baseline Arduino (ATmega328P) devices.
  //   - DEBUG_ERR 0 = Disable all error messaging
  //   - DEBUG_ERR 1 = Enable critical error messaging (eg. init)
  //   - DEBUG_ERR 2 = Enable verbose error messaging (eg. bad parameters, etc.)
  // - For baseline Arduino UNO, recommended to disable this after one has
  //   confirmed basic operation of the library is successful.
  #define DEBUG_ERR                0  // 1,2 to enable, 0 to disable

  // Debug initialization message
  // - By default, GUIslice outputs a message in DEBUG_ERR mode
  //   to indicate the initialization status, even during success.
  // - To disable the messages during successful initialization,
  //   uncomment the following line.
  //#define INIT_MSG_DISABLE

  // -----------------------------------------------------------------------------
  // SECTION 6: Optional Features
  // -----------------------------------------------------------------------------

  // Enable of optional features
  // - For memory constrained devices such as Arduino, it is best to
  //   set the following features to 0 (to disable) unless they are
  //   required.
  #define GSLC_FEATURE_COMPOUND       0   // Compound elements (eg. XSelNum)
  #define GSLC_FEATURE_XTEXTBOX_EMBED 0   // XTextbox control with embedded color
  #define GSLC_FEATURE_INPUT          0   // Keyboard / GPIO input control
  #define DRV_HAS_DRAW_TXT_U8G2       1   // Support U8G2 Library

  // Enable support for SD card
  // - Set to 1 to enable, 0 to disable
  // - Note that the inclusion of the SD library consumes considerable
  //   RAM and flash memory which could be problematic for Arduino models
  //   with limited resources.
  #define GSLC_SD_EN    0

  // Enable support for SPIFFS File System access
  // - Set to 1 to enable, 0 to disable
  // - Note that this requires the inclusion of TFT_eFEX library
  #define GSLC_SPIFFS_EN               1 

  // =============================================================================
  // SECTION 10: INTERNAL CONFIGURATION
  // - The following settings should not require modification by users
  // =============================================================================

  // -----------------------------------------------------------------------------
  // Touch Handling
  // -----------------------------------------------------------------------------

  // Define the maximum number of touch events that are handled
  // per gslc_Update() call. Normally this can be set to 1 but certain
  // displays may require a greater value (eg. 30) in order to increase
  // responsiveness of the touch functionality.
  #define GSLC_TOUCH_MAX_EVT    1

  // -----------------------------------------------------------------------------
  // Misc
  // -----------------------------------------------------------------------------

  // Define buffer size for loading images from SD
  // - A larger buffer will be faster but at the cost of RAM
  #define GSLC_SD_BUFFPIXEL   50

  // Enable support for graphics clipping (DrvSetClipRect)
  // - Note that this will impact performance of drawing graphics primitives
  #define GSLC_CLIP_EN 1

  // Enable for bitmap transparency and definition of color to use
  #define GSLC_BMP_TRANS_EN     1               // 1 = enabled, 0 = disabled
  #define GSLC_BMP_TRANS_RGB    0xFF,0x00,0xFF  // RGB color (default: MAGENTA)

  #define GSLC_USE_FLOAT        0   // 1=Use floating pt library, 0=Fixed-point lookup tables

  #define GSLC_DEV_TOUCH ""
  #define GSLC_USE_PROGMEM      0

  #define GSLC_LOCAL_STR        0   // 1=Use local strings (in element array), 0=External
  #define GSLC_LOCAL_STR_LEN    30  // Max string length of text elements

  // -----------------------------------------------------------------------------
  // Debug diagnostic modes
  // -----------------------------------------------------------------------------
  // - Uncomment any of the following to enable specific debug modes
  //#define DBG_REDRAW       // Enable tracing redraw regions
  //#define DBG_LOG           // Enable debugging log output
  //#define DBG_TOUCH         // Enable debugging of touch-presses
  //#define DBG_FRAME_RATE    // Enable diagnostic frame rate reporting
  //#define DBG_DRAW_IMM      // Enable immediate rendering of drawing primitives
  //#define DBG_DRIVER        // Enable graphics driver debug reporting


  // =============================================================================

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_CONFIG_ARD_H_
