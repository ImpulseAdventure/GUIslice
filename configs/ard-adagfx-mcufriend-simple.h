#ifndef _GUISLICE_CONFIG_ARD_H_
#define _GUISLICE_CONFIG_ARD_H_

// =============================================================================
// GUIslice library (example user configuration #???) for:
//   - CPU:     Arduino UNO (ATmega328P)
//              Arduino Mega2560 (ATmega2560)
//   - Display: mcufriend (various)
//   - Touch:   Simple Analog (Resistive)
//   - Wiring:  Uno/MEGA shield
//              - Display pinout defined by mcufriend_kbv library
//              - Touch pinout:
//                  CPU     Touch
//                  ----    -------
//                  A1      YP / Y+
//                  A2      XM / X-
//                  7       YM / Y-
//                  6       XP / X+
//
//   - Example display:
//     - 
//
// DIRECTIONS:
// - To use this example configuration, include in "GUIslice_config.h"
//
// WIRING:
// - The pinout configuration may need to be modified to match your wiring
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
  // Device Mode Selection
  // - The following defines the display and touch drivers
  //   and should not require modifications for this example config
  // -----------------------------------------------------------------------------
  #define DRV_DISP_ADAGFX           // Adafruit-GFX library
  #define DRV_DISP_ADAGFX_MCUFRIEND // MCUFRIEND_kbv display driver
  #define DRV_TOUCH_ADA_SIMPLE      // Adafruit_TouchScreen touch driver


  // -----------------------------------------------------------------------------
  // Pinout
  // -----------------------------------------------------------------------------

  // SD Card
  #define ADAGFX_PIN_SDCS     4     // SD card chip select (if GSLC_SD_EN=1)

  // -----------------------------------------------------------------------------
  // Orientation
  // -----------------------------------------------------------------------------

  // Set Default rotation of the display
  // - Values 0,1,2,3. Rotation is clockwise
  #define GSLC_ROTATE     1

  // -----------------------------------------------------------------------------
  // Touch Handling
  // - Documentation for configuring touch support can be found at:
  //   https://github.com/ImpulseAdventure/GUIslice/wiki/Configure-Touch-Support
  // -----------------------------------------------------------------------------

  // -----------------------------------------------------------------------------
  // Include some default configurations for various MCUFRIEND displays
  // - Set the following to match the ID reported by the mcufriend_kbv
  //   library, or reported by the GUIslice diag_ard_touch_calib sketch.
  // - Add your own configuration if needed
  #define MCUFRIEND_ID 0x9341
  // -----------------------------------------------------------------------------

  // Pinout for DRV_TOUCH_SIMPLE 4-wire resistive touchscreen

  // - Included sample pinout wiring for a few MCUFRIEND variants
  //     ADATOUCH_PIN_YP      // "Y+": Must be an analog pin
  //     ADATOUCH_PIN_XM      // "X-": Must be an analog pin
  //     ADATOUCH_PIN_YM      // "Y-": Can be a digital pin
  //     ADATOUCH_PIN_XP      // "X+": Can be a digital pin

  #if (MCUFRIEND_ID == 0x1520)
    #define ADATOUCH_PIN_YP   A1
    #define ADATOUCH_PIN_XM   A2
    #define ADATOUCH_PIN_YM   7
    #define ADATOUCH_PIN_XP   6
  #elif (MCUFRIEND_ID == 0x2053)
    #define ADATOUCH_PIN_YP   A2
    #define ADATOUCH_PIN_XM   A1
    #define ADATOUCH_PIN_YM   6
    #define ADATOUCH_PIN_XP   7
  #elif (MCUFRIEND_ID == 0x7783) // TESTED
    #define ADATOUCH_PIN_YP   A2
    #define ADATOUCH_PIN_XM   A1
    #define ADATOUCH_PIN_YM   6
    #define ADATOUCH_PIN_XP   7
  #elif (MCUFRIEND_ID == 0x7789)
    #define ADATOUCH_PIN_YP   A2
    #define ADATOUCH_PIN_XM   A1
    #define ADATOUCH_PIN_YM   7
    #define ADATOUCH_PIN_XP   6
  #elif (MCUFRIEND_ID == 0x8031)
    #define ADATOUCH_PIN_YP   A1
    #define ADATOUCH_PIN_XM   A2
    #define ADATOUCH_PIN_YM   7
    #define ADATOUCH_PIN_XP   6
  #elif (MCUFRIEND_ID == 0x9320)
    #define ADATOUCH_PIN_YP   A3
    #define ADATOUCH_PIN_XM   A2
    #define ADATOUCH_PIN_YM   9
    #define ADATOUCH_PIN_XP   8
  #elif (MCUFRIEND_ID == 0x9341) // TESTED
    #define ADATOUCH_PIN_YP   A1
    #define ADATOUCH_PIN_XM   A2
    #define ADATOUCH_PIN_YM   7
    #define ADATOUCH_PIN_XP   6
  #elif (MCUFRIEND_ID == 0x9320)
    #define ADATOUCH_PIN_YP   A3
    #define ADATOUCH_PIN_XM   A2
    #define ADATOUCH_PIN_YM   9
    #define ADATOUCH_PIN_XP   8
  #elif (MCUFRIEND_ID == 0x9327)
    #define ADATOUCH_PIN_YP   A2
    #define ADATOUCH_PIN_XM   A1
    #define ADATOUCH_PIN_YM   6
    #define ADATOUCH_PIN_XP   7
  #endif // MCUFRIEND_ID

  #define ADATOUCH_RX       300   // "rxplate"

  // Calibration for resistive displays
  // - These values may need to be updated to match your display
  // - Run /examples/arduino/diag_ard_touch_calib to determine these values

  // - Included sample calibration values for a few MCUFRIEND variants
  #if (MCUFRIEND_ID == 0x1520)
    #define ADATOUCH_X_MIN    893
    #define ADATOUCH_Y_MIN    99
    #define ADATOUCH_X_MAX    104
    #define ADATOUCH_Y_MAX    892
  #elif (MCUFRIEND_ID == 0x2053)
    #define ADATOUCH_X_MIN    138
    #define ADATOUCH_Y_MIN    132
    #define ADATOUCH_X_MAX    891
    #define ADATOUCH_Y_MAX    909
  #elif (MCUFRIEND_ID == 0x7783) // TESTED
    // DRV_TOUCH_ADA_SIMPLE [240x320]: (MCUFRIEND ID=0x7783) (XP=7,XM=A1,YP=A2,YM=6) 
    #define ADATOUCH_X_MIN    181
    #define ADATOUCH_Y_MIN    934
    #define ADATOUCH_X_MAX    937
    #define ADATOUCH_Y_MAX    219
  #elif (MCUFRIEND_ID == 0x7789)
    #define ADATOUCH_X_MIN    885
    #define ADATOUCH_Y_MIN    111
    #define ADATOUCH_X_MAX    148
    #define ADATOUCH_Y_MAX    902
  #elif (MCUFRIEND_ID == 0x8031)
    #define ADATOUCH_X_MIN    889
    #define ADATOUCH_Y_MIN    121
    #define ADATOUCH_X_MAX    151
    #define ADATOUCH_Y_MAX    886
  #elif (MCUFRIEND_ID == 0x9320)
    #define ADATOUCH_X_MIN    897
    #define ADATOUCH_Y_MIN    944
    #define ADATOUCH_X_MAX    122
    #define ADATOUCH_Y_MAX    141
  #elif (MCUFRIEND_ID == 0x9327)
    #define ADATOUCH_X_MIN    126
    #define ADATOUCH_Y_MIN    106
    #define ADATOUCH_X_MAX    905
    #define ADATOUCH_Y_MAX    966
  #elif (MCUFRIEND_ID == 0x9341) // TESTED
    // DRV_TOUCH_ADA_SIMPLE [240x320]: (MCUFRIEND ID=0x9341) (XP=6,XM=A2,YP=A1,YM=7) 
    #define ADATOUCH_X_MIN    905
    #define ADATOUCH_Y_MIN    950
    #define ADATOUCH_X_MAX    187
    #define ADATOUCH_Y_MAX    202
  #endif // MCUFRIEND_ID


  // Define pressure threshold for detecting a touch
  #define ADATOUCH_PRESS_MIN  10
  #define ADATOUCH_PRESS_MAX  1000

  // -----------------------------------------------------------------------------
  // Diagnostics
  // -----------------------------------------------------------------------------

  // Error reporting
  // - Set DEBUG_ERR to 1 to enable error reporting via the Serial connection
  // - Enabling DEBUG_ERR increases FLASH memory consumption which may be
  //   limited on the baseline Arduino (ATmega328P) devices.
  // - For baseline Arduino UNO, recommended to disable this after one has
  //   confirmed basic operation of the library is successful.
  #define DEBUG_ERR               1   // 1 to enable, 0 to disable

  // Debug initialization message
  // - By default, GUIslice outputs a message in DEBUG_ERR mode
  //   to indicate the initialization status, even during success.
  // - To disable the messages during successful initialization,
  //   uncomment the following line.
  //#define INIT_MSG_DISABLE

  // -----------------------------------------------------------------------------
  // Optional Features
  // -----------------------------------------------------------------------------

  // Enable of optional features
  // - For memory constrained devices such as Arduino, it is best to
  //   set the following features to 0 (to disable) unless they are
  //   required.
  #define GSLC_FEATURE_COMPOUND       0   // Compound elements (eg. XSelNum)
  #define GSLC_FEATURE_XGAUGE_RADIAL  0   // XGauge control with radial support
  #define GSLC_FEATURE_XGAUGE_RAMP    0   // XGauge control with ramp support
  #define GSLC_FEATURE_XTEXTBOX_EMBED 0   // XTextbox control with embedded color
  #define GSLC_FEATURE_INPUT          0   // Keyboard / GPIO input control

  // Enable support for SD card
  // - Set to 1 to enable, 0 to disable
  // - Note that the inclusion of the SD library consumes considerable
  //   RAM and flash memory which could be problematic for Arduino models
  //   with limited resources.
  #define GSLC_SD_EN    0


  // =============================================================================
  // INTERNAL CONFIGURATION
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
  #define GSLC_BMP_TRANS_RGB    0xFF,0x00,0xFF  // RGB color (default:pink)

  #define GSLC_USE_FLOAT      0   // 1=Use floating pt library, 0=Fixed-point lookup tables

  #define GSLC_DEV_TOUCH ""
  #define GSLC_USE_PROGMEM 1

  #define GSLC_LOCAL_STR      0   // 1=Use local strings (in element array), 0=External
  #define GSLC_LOCAL_STR_LEN  30  // Max string length of text elements

  // -----------------------------------------------------------------------------
  // Debug diagnostic modes
  // -----------------------------------------------------------------------------
  // - Uncomment any of the following to enable specific debug modes
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
