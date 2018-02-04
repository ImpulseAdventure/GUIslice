#ifndef _GUISLICE_CONFIG_ARD_H_
#define _GUISLICE_CONFIG_ARD_H_

// =======================================================================
// GUIslice library (user configuration) for Arduino / Cortex-M0
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
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
// =======================================================================

// =======================================================================
// User Configuration
// - This file can be modified by the user to match the
//   intended target configuration
// - Please refer to "docs/GUIslice_config_guide.xlsx" for detailed examples
//   specific to board and display combinations
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// -----------------------------------------------------------------------------------------

// Specify the graphics driver library
// - Uncomment one of the following graphics drivers
#define DRV_DISP_ADAGFX            // Arduino: Adafruit-GFX library


// Specify the touchscreen driver
// - Uncomment one of the following touchscreen drivers
//#define DRV_TOUCH_NONE          // No touchscreen support
#define DRV_TOUCH_ADA_STMPE610  // Arduino: Use Adafruit STMPE610 touch driver
//#define DRV_TOUCH_ADA_FT6206    // Arduino: Use Adafruit FT6206 touch driver
//#define DRV_TOUCH_ADA_SIMPLE    // Arduino: Use Adafruit Touchscreen


// -----------------------------------------------------------------------------------------
// Enable of optional features
// - For memory constrained devices such as Arduino, it is best to
//   set the following features to 0 (to disable) unless they are
//   required.

#define GSLC_FEATURE_COMPOUND       0   // Compound elements (eg. XSelNum)
#define GSLC_FEATURE_XGAUGE_RADIAL  0   // XGauge control with radial support
#define GSLC_FEATURE_XGAUGE_RAMP    0   // XGauge control with ramp support

// Error reporting
// - Set DEBUG_ERR to 1 to enable error reporting via the Serial connection
// - Enabling DEBUG_ERR increases FLASH memory consumption which may be
//   limited on the baseline Arduino (ATmega328P) devices.
#if defined(__AVR__)
    #define DEBUG_ERR               0   // Disable by default on low-mem Arduino
#else
    #define DEBUG_ERR               1   // Enable by default on all other devices
#endif

// -----------------------------------------------------------------------------------------

// Graphics display driver-specific additional configuration
#if defined(DRV_DISP_ADAGFX)

  #define GSLC_DEV_TOUCH ""   // No device path used

  #define GSLC_LOCAL_STR      0
  #define GSLC_USE_FLOAT      0 // Use fixed-point lookup tables instead


  // The Adafruit-GFX library supports a number of displays
  // - Select a display sub-type by uncommenting one of the
  //   following DRV_DISP_ADAGFX_* lines
  #define DRV_DISP_ADAGFX_ILI9341         // Adafruit ILI9341
  //#define DRV_DISP_ADAGFX_ILI9341_8BIT  // Adafruit ILI9341 (8-bit interface)
  //#define DRV_DISP_ADAGFX_ST7735        // Adafruit ST7735
  //#define DRV_DISP_ADAGFX_SSD1306       // Adafruit SSD1306
  //#define DRV_DISP_ADAGFX_HX8357        // Adafruit HX8357
  //#define DRV_DISP_ADAGFX_PCD8544       // Adafruit PCD8544


  // For Adafruit-GFX drivers, define pin connections
  // - Define general pins (modify these example pin assignments to match your board)
  // - Please refer to "docs/GUIslice_config_guide.xlsx" for detailed examples
  #define ADAGFX_PIN_CS    10   // Display chip select
  #define ADAGFX_PIN_DC     9   // Display SPI data/command
  #define ADAGFX_PIN_RST    0   // Display Reset (some displays could use pin 11)
  #define ADAGFX_PIN_SDCS   4   // SD card chip select
  #define ADAGFX_PIN_WR    A1   // Display write pin (for parallel displays)
  #define ADAGFX_PIN_RD    A0   // Display read pin (for parallel displays)

  // Use hardware SPI interface?
  // - Set to 1 to enable hardware SPI interface, 0 to use software SPI
  // - Software SPI may support the use of custom pin selection (via ADAGFX_PIN_MOSI,
  //   ADAGFX_PIN_MISO, ADAGFX_PIN_CLK). These pin definitions can be left blank in
  //   hardware SPI mode.
  #define ADAGFX_SPI_HW     1

  // Define custom SPI pin connections used in software SPI mode (ADAGFX_SPI_HW=0)
  // - These definitions can be left blank in hardware mode (ADAGFX_SPI_HW=1)
  #define ADAGFX_PIN_MOSI
  #define ADAGFX_PIN_MISO
  #define ADAGFX_PIN_CLK


  // Enable support for SD card
  // - Set to 1 to enable, 0 to disable
  // - Note that the inclusion of the SD library consumes considerable
  //   RAM and flash memory which could be problematic for Arduino models
  //   with limited resources.
  #define GSLC_SD_EN    0

  // Define buffer size for loading images from SD
  // - A larger buffer will be faster but at the cost of RAM
  #define GSLC_SD_BUFFPIXEL   50

  // Enable support for clipping (DrvSetClipRect)
  // - Note that this will impact performance of drawing graphics primitives
  #define GSLC_CLIP_EN 1

  // Set Default rotation
  // - Note that if you change this you will likely have to change ADATOUCH_FLIP_X & ADATOUCH_FLIP_Y
  //   as well to ensure that the touch screen orientation matches the display rotation
  #define GSLC_ROTATE     1


#endif // DRV_DISP_*


// -----------------------------------------------------------------------------------------

// Touch Driver-specific additional configuration
#if defined(DRV_TOUCH_ADA_STMPE610)

  // Select wiring method by setting one of the following to 1
  #define ADATOUCH_I2C_HW 0
  #define ADATOUCH_SPI_HW 1
  #define ADATOUCH_SPI_SW 0  // [TODO]

  // For ADATOUCH_I2C_HW=1
  #define ADATOUCH_I2C_ADDR   0x41  // I2C address of touch device

  // For ADATOUCH_SPI_HW=1

  #define ADATOUCH_PIN_CS     8 // From Adafruit 2.8" TFT touch shield

  // Calibration values for touch display
  // - These values may need to be updated to match your display
  // - Typically used in resistive displays
  // - These values can be determined from the Adafruit touchtest example sketch
  //   (check for min and max values reported from program as you touch display corners)
  // - Note that X & Y directions reference the display's natural orientation
  #define ADATOUCH_X_MIN 230
  #define ADATOUCH_Y_MIN 260
  #define ADATOUCH_X_MAX 3800
  #define ADATOUCH_Y_MAX 3700



#elif defined(DRV_TOUCH_ADA_FT6206)
  // Define sensitivity coefficient (capacitive touch)
  #define ADATOUCH_SENSITIVITY  40


#elif defined(DRV_TOUCH_ADA_SIMPLE)
  // Calibration values for touch display
  // - These values may need to be updated to match your display
  // - Typically used in resistive displays
  #define ADATOUCH_X_MIN 100
  #define ADATOUCH_Y_MIN 150
  #define ADATOUCH_X_MAX 900
  #define ADATOUCH_Y_MAX 900



#endif // DRV_TOUCH_*

// Define any Touch Axis Swapping and Flipping
// - Set any of the following to 1 to perform touch display
//   remapping functions, 0 to disable. Use DBG_TOUCH to determine which
//   remapping modes should be enabled for your display
// - Please refer to "docs/GUIslice_config_guide.xlsx" for detailed examples
#define ADATOUCH_SWAP_XY  1
#define ADATOUCH_FLIP_X   0
#define ADATOUCH_FLIP_Y   1

// Define the maximum number of touch events that are handled
// per gslc_Update() call. Normally this can be set to 1 but certain
// displays may require a greater value (eg. 30) in order to increase
// responsiveness of the touch functionality.
#define GSLC_TOUCH_MAX_EVT    1

// -----------------------------------------------------------------------------------------


// When using element local string storage (GSLC_LOCAL_STR=1),
// this defines the fixed length buffer used for every element
#define GSLC_LOCAL_STR_LEN  30  // Max string length of text elements


// Debug modes
// - Uncomment the following to enable specific debug modes
//#define DBG_LOG           // Enable debugging log output
//#define DBG_TOUCH         // Enable debugging of touch-presses
//#define DBG_FRAME_RATE    // Enable diagnostic frame rate reporting
//#define DBG_DRAW_IMM      // Enable immediate rendering of drawing primitives
//#define DBG_DRIVER        // Enable graphics driver debug reporting

// Enable for bitmap transparency and definition of color to use
#define GSLC_BMP_TRANS_EN     1               // 1 = enabled, 0 = disabled
#define GSLC_BMP_TRANS_RGB    0xFF,0x00,0xFF  // RGB color (default:pink)


// -----------------------------------------------------------------------------------------

// Define compatibility for non-AVR to call PROGMEM functions
#if defined(__AVR__)
    #define GSLC_USE_PROGMEM 1
#else
    #define GSLC_USE_PROGMEM 0
#endif


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_CONFIG_ARD_H_
