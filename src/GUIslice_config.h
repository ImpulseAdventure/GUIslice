#ifndef _GUISLICE_CONFIG_H_
#define _GUISLICE_CONFIG_H_

// =======================================================================
// GUIslice library (user-specified configuration)
// - Calvin Hass
// - http://www.impulseadventure.com/elec/guislice-gui.html
// =======================================================================
//
// The MIT License
//
// Copyright 2017 Calvin Hass
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
#define DRV_DISP_SDL1                // LINUX: SDL 1.2 library
//#define DRV_DISP_SDL2              // LINUX: SDL 2.0 library
//#define DRV_DISP_ADAGFX            // Arduino: Adafruit-GFX library

  
// Specify the touchscreen driver
// - Uncomment one of the following touchscreen drivers
//#define DRV_TOUCH_NONE          // No touchscreen support
//#define DRV_TOUCH_SDL           // LINUX: Use SDL touch driver
#define DRV_TOUCH_TSLIB           // LINUX: Use tslib touch driver
//#define DRV_TOUCH_ADA_STMPE610  // Arduino: Use Adafruit STMPE610 touch driver
//#define DRV_TOUCH_ADA_FT6206    // Arduino: Use Adafruit FT6206 touch driver



// -----------------------------------------------------------------------------------------

// Graphics display driver-specific additional configuration  
#if defined(DRV_DISP_SDL1)
  // Define default device paths for framebuffer & touchscreen  
  #define GSLC_DEV_FB       "/dev/fb1"
  #define GSLC_DEV_TOUCH    "/dev/input/touchscreen"
  #define GSLC_DEV_VID_DRV  "fbcon"

  // Enable SDL startup workaround? (1 to enable, 0 to disable)
  #define DRV_SDL_FIX_START 0

  // Show SDL mouse (1 to show, 0 to hide)
  #define DRV_SDL_MOUSE_SHOW 0
  
  #define GSLC_LOCAL_STR  1
    
  // Error reporting
  #define DEBUG_ERR   1       // Enable error message reporting (requires more memory)


#elif defined(DRV_DISP_SDL2)
  // Define default device paths for framebuffer & touchscreen
  // - The following assumes display driver (eg. fbtft) reads from fb1
  // - Raspberry Pi can support hardware acceleration onto fb0
  // - To use SDL2.0 with hardware acceleration with such displays,
  //   use fb0 as the target and then run fbcp to mirror fb0 to fb1
  #define GSLC_DEV_FB       "/dev/fb0"
  #define GSLC_DEV_TOUCH    ""  
  #define GSLC_DEV_VID_DRV  "x11"

  // Show SDL mouse (1 to show, 0 to hide)
  #define DRV_SDL_MOUSE_SHOW 0
  // Enable hardware acceleration
  #define DRV_SDL_RENDER_ACCEL 1
  
  #define GSLC_LOCAL_STR  1

  // Error reporting
  #define DEBUG_ERR   1       // Enable error message reporting (requires more memory)


#elif defined(DRV_DISP_ADAGFX)  

  #define GSLC_DEV_TOUCH ""   // No device path used

  #define GSLC_LOCAL_STR  0

  // Error reporting
  #define DEBUG_ERR   1       // Enable error message reporting (requires more memory)
  
  // The Adafruit-GFX library supports a number of displays
  // - Select a display sub-type by uncommenting one of the
  //   following DRV_DISP_ADAGFX_* lines
  #define DRV_DISP_ADAGFX_ILI9341
  //#define DRV_DISP_ADAGFX_ST7735
  //#define DRV_DISP_ADAGFX_SSD1306

  
  // For Adafruit-GFX drivers, define pin connections
  // - Define general pins (modify these example pin assignments to match your board)
  // - Please refer to "docs/GUIslice_config_guide.xlsx" for detailed examples
  #define ADAGFX_PIN_CS    10   // Display chip select
  #define ADAGFX_PIN_DC     9   // Display SPI data/command
  #define ADAGFX_PIN_RST   11   // Display Reset
  #define ADAGFX_PIN_SDCS   4   // SD card chip select

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
  #define ADAGFX_SD_EN    0
  
  // Define buffer size for loading images from SD
  // - A larger buffer will be faster but at the cost of RAM
  #define ADAGFX_SD_BUFFPIXEL   20

  // Enable support for clipping (DrvSetClipRect)
  // - Note that this will impact performance of drawing graphics primitives
  #define ADAGFX_CLIP 1

#endif // DRV_DISP_*


// -----------------------------------------------------------------------------------------

// Touch Driver-specific additional configuration  
#if defined(DRV_TOUCH_SDL)
  #define DRV_TOUCH_IN_DISP   // Use the display driver (SDL) for touch events
  
#elif defined(DRV_TOUCH_ADA_STMPE610)

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
  
#endif // DRV_TOUCH_*

// Define any Touch Axis Swapping and Flipping
// - Set any of the following to 1 to perform touch display
//   remapping functions, 0 to disable. Use DBG_TOUCH to determine which
//   remapping modes should be enabled for your display
// - Please refer to "docs/GUIslice_config_guide.xlsx" for detailed examples
#define ADATOUCH_SWAP_XY  1
#define ADATOUCH_FLIP_X   0
#define ADATOUCH_FLIP_Y   1
  
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
#endif // _GUISLICE_CONFIG_H_


