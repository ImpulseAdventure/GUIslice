#ifndef _GUISLICE_CONFIG_ARD_H_
#define _GUISLICE_CONFIG_ARD_H_

// =============================================================================
// GUIslice library (example user configuration #032) for:
//     M5stack
//
// DIRECTIONS:
// - To use this example configuration, rename the file as "GUIslice_config_ard.h"
//   and copy into the GUIslice/src directory.
//
// WIRING:
// - As the M5stack is an integrated display device, no additional
//   wiring is required to support the GUI operation
//
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =============================================================================
//
// The MIT License
//
// Copyright 2018 Calvin Hass
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
// - Please refer to "docs/GUIslice_config_guide.xlsx" for detailed examples
//   specific to board and display combinations
// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


  // =============================================================================
  // USER DEFINED CONFIGURATION
  // =============================================================================

  // Set default display rotation
  #define GSLC_ROTATE     1  // Values: 0,1,2,3

  // NOTE: Long-press detection is only available in the latest
  //       M5stack library releases. Uncomment the following
  //       if the Btn wasReleasefor() API is available.  
  //
  // Define duration (in ms) for a long-press button event
  //#define M5STACK_TOUCH_PRESS_LONG  300

  // Error reporting
  // - Set DEBUG_ERR to 1 to enable error reporting via the Serial connection
  // - Enabling DEBUG_ERR increases FLASH memory consumption
  #define DEBUG_ERR               1   // Debugging enabled by default

  // Enable of optional features
  // - To reduce memory consumption, disable the following features by
  //   setting a value of 0, enable with 1
  #define GSLC_FEATURE_COMPOUND       0   // Compound elements (eg. XSelNum)
  #define GSLC_FEATURE_XGAUGE_RADIAL  0   // XGauge control with radial support
  #define GSLC_FEATURE_XGAUGE_RAMP    0   // XGauge control with ramp support
  #define GSLC_FEATURE_XTEXTBOX_EMBED 0   // XTextbox control with embedded color
  #define GSLC_FEATURE_INPUT          1   // M5stack built-in physical buttons


  // Enable support for SD card
  // - Set to 1 to enable, 0 to disable
  // - Note that the inclusion of the SD library consumes considerable
  //   RAM and flash memory which could be problematic for Arduino models
  //   with limited resources.
  #define GSLC_SD_EN    0

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

  // -----------------------------------------------------------------------------
  // Debug diagnostic modes
  // - Uncomment any of the following to enable specific debug modes
  // -----------------------------------------------------------------------------

  //#define DBG_LOG           // Enable debugging log output
  //#define DBG_TOUCH         // Enable debugging of touch-presses
  //#define DBG_FRAME_RATE    // Enable diagnostic frame rate reporting
  //#define DBG_DRAW_IMM      // Enable immediate rendering of drawing primitives
  //#define DBG_DRIVER        // Enable graphics driver debug reporting



  // =============================================================================
  // INTERNAL CONFIGURATION
  // - The following settings should not require modification
  // =============================================================================

  #define DRV_DISP_M5STACK          // m5stack/M5Stack library
  #define DRV_TOUCH_M5STACK         // M5stack integrated button driver

  #define ADAGFX_PIN_SDCS   4       // SD card chip select
  #define TFT_LIGHT_PIN    32       // display backlight

  #define DRV_TOUCH_IN_DISP
  #define GSLC_TOUCH_MAX_EVT 1
  #define GSLC_TOUCH_ROTATE 1
  #define ADATOUCH_SWAP_XY  0
  #define ADATOUCH_FLIP_X   0
  #define ADATOUCH_FLIP_Y   0
  #define TOUCH_ROTATION_DATA 0x6350
  #define TOUCH_ROTATION_SWAPXY(rotation) ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 2 ) & 0x01 )
  #define TOUCH_ROTATION_FLIPX(rotation)  ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 1 ) & 0x01 )
  #define TOUCH_ROTATION_FLIPY(rotation)  ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 0 ) & 0x01 )
  #define GSLC_DEV_TOUCH ""

  #define GSLC_USE_PROGMEM 0

  #define GSLC_LOCAL_STR      0
  #define GSLC_LOCAL_STR_LEN  30

  // =============================================================================

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_CONFIG_ARD_H_
