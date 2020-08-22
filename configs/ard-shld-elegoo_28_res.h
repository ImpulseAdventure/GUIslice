#ifndef _GUISLICE_CONFIG_ARD_H_
#define _GUISLICE_CONFIG_ARD_H_

// =============================================================================
// GUIslice library (example user configuration) for:
//   - CPU:     Arduino UNO / MEGA / etc
//   - Display: ILI9341
//   - Touch:   Simple Analog (Resistive)
//   - Wiring:  Shield (pinout defined by mcufriend_kbv library)
//
//   - Example display:
//     - ELEGOO UNO R3 2.8" TFT Touch Screen Shield
//       https://www.elegoo.com/product/elegoo-uno-r3-2-8-inches-tft-touch-screen/
//
// DIRECTIONS:
// - To use this example configuration, include in "GUIslice_config.h"
//
// WIRING:
// - As this config file is designed for a shield, no additional
//   wiring is required to support the GUI operation
//
// =============================================================================
// - Calvin Hass
// - https://github.com/ImpulseAdventure/GUIslice
// =============================================================================
//
// The MIT License
//
// Copyright 2016-2020 Calvin Hass
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
  #define DRV_DISP_ADAGFX           // Adafruit-GFX library
  #define DRV_DISP_ADAGFX_MCUFRIEND // prenticedavid/MCUFRIEND_kbv
  #define DRV_TOUCH_ADA_SIMPLE      // Adafruit_TouchScreen touch driver


  // -----------------------------------------------------------------------------
  // SECTION 2: Pinout
  // -----------------------------------------------------------------------------


  // SD Card
  #define ADAGFX_PIN_SDCS     10     // SD card chip select (if GSLC_SD_EN=1)



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
  // - These values should come from the diag_ard_touch_calib sketch output
  // - Please update the values to the right of ADATOUCH_PIN_* accordingly
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  // Set the pinout for the 4-wire resistive touchscreen
  // - These settings describe the wiring between the MCU and the
  //   resistive touch overlay.
  //
  // - The diag_ard_touch_detect sketch can be used to detect the
  //   pin connections (on Arduino devices) for your specific shield.
  //

  // - Definition of the pinout configuration options:
  //     ADATOUCH_PIN_YP      // "Y+": Must be an analog pin
  //     ADATOUCH_PIN_XM      // "X-": Must be an analog pin
  //     ADATOUCH_PIN_YM      // "Y-": Can be a digital pin
  //     ADATOUCH_PIN_XP      // "X+": Can be a digital pin

  // Pin connections from diag_ard_touch_detect:
  // - These values based on shield definition
  #define ADATOUCH_PIN_YP     A3
  #define ADATOUCH_PIN_XM     A2
  #define ADATOUCH_PIN_YM     9
  #define ADATOUCH_PIN_XP     8


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  // SECTION 4B: Update your calibration settings here
  // - These values should come from the diag_ard_touch_calib sketch output
  // - Please update the values to the right of ADATOUCH_X/Y_MIN/MAX_* accordingly
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  // Calibration settings from diag_ard_touch_calib:
  //
  // - A number of example calibration settings for common MCUFRIEND
  //   shields have been provided in SECTION 4C, each marked with their
  //   corresponding MCUFRIEND ID. However, note that these example
  //   calibration values may not provide accurate touch tracking, therefore
  //   using the diag_ard_touch_calib utility is strongly recommended.
  #define ADATOUCH_X_MIN    879
  #define ADATOUCH_X_MAX    153
  #define ADATOUCH_Y_MIN    107
  #define ADATOUCH_Y_MAX    915
  #define ADATOUCH_REMAP_YX 0    // Some touch controllers may swap X & Y coords

  // Touch overlay resistance value
  // - In most cases, this value can be left as-is
  #define ADATOUCH_RX       300   // "rxplate"


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  // SECTION 4D: Additional touch configuration
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  // Define pressure threshold for detecting a touch
  // - Specifying this range helps eliminate some erroneous touch events
  //   resulting from noise in the touch overlay detection
  // - For config details, please see:
  //   https://github.com/ImpulseAdventure/GUIslice/wiki/Configuring-Touch-Pressure
  #define ADATOUCH_PRESS_MIN  10
  #define ADATOUCH_PRESS_MAX  4000

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
  #define DEBUG_ERR               1   // 1,2 to enable, 0 to disable

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

  // Enable support for SD card
  // - Set to 1 to enable, 0 to disable
  // - Note that the inclusion of the SD library consumes considerable
  //   RAM and flash memory which could be problematic for Arduino models
  //   with limited resources.
  #define GSLC_SD_EN    0


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
  #define GSLC_USE_PROGMEM      1

  #define GSLC_LOCAL_STR        0   // 1=Use local strings (in element array), 0=External
  #define GSLC_LOCAL_STR_LEN    30  // Max string length of text elements

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
