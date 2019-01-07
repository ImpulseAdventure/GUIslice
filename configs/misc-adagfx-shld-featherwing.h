#ifndef _GUISLICE_CONFIG_ARD_H_
#define _GUISLICE_CONFIG_ARD_H_

// =============================================================================
// GUIslice library (example user configuration #???) for:
//   - CPU:     Various
//   - Display: ILI9341
//   - Touch:   STMPE610 (Resistive)
//   - Wiring:  2.4" FeatherWing TFT shield
//
//   - Display shield:
//     - 2.4" FeatherWing TFT shield
//   - Example CPUs:
//     - Adafruit HUZZAH      (ESP8266)
//     - Adafruit HUZZAH32    (ESP-32)
//     - Adafruit Feather M0  (Cortex-M0)
//
// DIRECTIONS:
// - To use this example configuration, rename the file as "GUIslice_config_ard.h"
//   and copy into the GUIslice/src directory.
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
  #define DRV_DISP_ADAGFX_ILI9341   // Adafruit ILI9341
  #define DRV_TOUCH_ADA_STMPE610    // Adafruit STMPE610 touch driver


  // -----------------------------------------------------------------------------
  // Pinout
  // -----------------------------------------------------------------------------

  // For UNO/MEGA shields, the following pinouts are typically hardcoded
#ifdef ESP8266
  #define ADAGFX_PIN_CS       0     // Display chip select
  #define ADAGFX_PIN_DC       15    // Display SPI data/command
#elif ESP32
  #define ADAGFX_PIN_CS       15    // Display chip select
  #define ADAGFX_PIN_DC       33    // Display SPI data/command
#elif ARDUINO_SAMD_FEATHER_M0
  #define ADAGFX_PIN_CS       9     // Display chip select
  #define ADAGFX_PIN_DC       10    // Display SPI data/command
#else
  #define ADAGFX_PIN_CS       10    // Display chip select
  #define ADAGFX_PIN_DC       9     // Display SPI data/command
#endif
  #define ADAGFX_PIN_RST      0     // Display Reset //xxx UNUSED?
  //#define ADAGFX_PIN_WR       A1    // Display write pin (for parallel displays)
  //#define ADAGFX_PIN_RD       A0    // Display read pin (for parallel displays)

  // SD Card
#ifdef ESP8266
  #define ADAGFX_PIN_SDCS     2     // SD card chip select (if GSLC_SD_EN=1)
#elif ESP32
  #define ADAGFX_PIN_SDCS     14    // SD card chip select (if GSLC_SD_EN=1)
#elif ARDUINO_SAMD_FEATHER_M0
  #define ADAGFX_PIN_SDCS     5     // SD card chip select (if GSLC_SD_EN=1)
#else
  #define ADAGFX_PIN_SDCS     4     // SD card chip select (if GSLC_SD_EN=1)
#endif

  // Display interface type
  #define ADAGFX_SPI_HW       1	    // Display uses the hardware SPI interface

  // Display interface software SPI
  // - Hardware SPI: the following definitions are unused
  // - Software SPI: the following pins need to be defined
  #define ADAGFX_PIN_MOSI     11
  #define ADAGFX_PIN_MISO     12
  #define ADAGFX_PIN_CLK      13

  // -----------------------------------------------------------------------------
  // Orientation
  // -----------------------------------------------------------------------------

  // Set Default rotation of the display
  // - Values 0,1,2,3. Rotation is clockwise
  // - Note that changing this value may require a change
  //   to GSLC_TOUCH_ROTATE as well to ensure the touch screen
  //   orientation matches the display.
  #define GSLC_ROTATE     1

  // Set Default rotation of the touch overlay
  // - Values 0,1,2,3. Rotation is clockwise
  #define GSLC_TOUCH_ROTATE 3

  // -----------------------------------------------------------------------------
  // Touch Handling
  // -----------------------------------------------------------------------------

  // Select touch device wiring method by setting one of the following to 1, others to 0
  #define ADATOUCH_I2C_HW 0  // Touch controller via hardware I2C (uses ADATOUCH_I2C_ADDR)
  #define ADATOUCH_SPI_HW 1  // Touch controller via hardware SPI (uses ADATOUCH_PIN_CS)
  #define ADATOUCH_SPI_SW 0  // Touch controller via software SPI [not yet supported]

  // Touch bus & pinout
  #define ADATOUCH_I2C_ADDR   0x41  // Touch device I2C address (for ADATOUCH_I2C_HW=1)
#ifdef ESP8266
  #define ADATOUCH_PIN_CS     16
#elif ESP32
  #define ADATOUCH_PIN_CS     32
#elif ARDUINO_SAMD_FEATHER_M0
  #define ADATOUCH_PIN_CS     6
#else
  #define ADATOUCH_PIN_CS     8     // Touch device chip select (for ADATOUCH_SPI_HW=1)
#endif

  // Calibration for DRV_TOUCH_STMPE610 & DRV_TOUCH_SIMPLE
  // - These values may need to be updated to match your display
  // - Typically used in resistive displays
  // - These values can be determined from the Adafruit touchtest example sketch
  //   (check for min and max values reported from program as you touch display
  //   corners)
  // - Note that X & Y directions reference the display's natural orientation
  #define ADATOUCH_X_MIN 3800
  #define ADATOUCH_Y_MIN 100
  #define ADATOUCH_X_MAX 100
  #define ADATOUCH_Y_MAX 3750

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

  // Define how touch orientation changes with display orientation
  #define TOUCH_ROTATION_DATA 0x6350
  #define TOUCH_ROTATION_SWAPXY(rotation) ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 2 ) & 0x01 )
  #define TOUCH_ROTATION_FLIPX(rotation)  ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 1 ) & 0x01 )
  #define TOUCH_ROTATION_FLIPY(rotation)  ((( TOUCH_ROTATION_DATA >> ((rotation&0x03)*4) ) >> 0 ) & 0x01 )

  // - Set any of the following to 1 to perform touch display
  //   remapping functions, 0 to disable. Use DBG_TOUCH to determine which
  //   remapping modes should be enabled for your display
  // - Please refer to "docs/GUIslice_config_guide.xlsx" for detailed examples
  // - NOTE: Both settings, GLSC_TOUCH_ROTATE and SWAP / FLIP are applied, 
  //         try to set _SWAP_XY and _FLIP_X/Y to 0 and only use GLSC_TOUCH_ROTATE
  #define ADATOUCH_SWAP_XY  0
  #define ADATOUCH_FLIP_X   0
  #define ADATOUCH_FLIP_Y   0

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

  // Define compatibility for non-AVR to call PROGMEM functions
  #if defined(__AVR__)
    #define GSLC_USE_PROGMEM 1
  #else
    #define GSLC_USE_PROGMEM 0
  #endif

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
