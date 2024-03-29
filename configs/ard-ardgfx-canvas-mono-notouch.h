#ifndef _GUISLICE_CONFIG_ARD_H_
#define _GUISLICE_CONFIG_ARD_H_

// =============================================================================
// GUIslice library (example user configuration) for:
//   - CPU:     Arduino UNO / MEGA / etc
//   - Display: Arduino Canvas Mono
//   - Touch:   None
//   - Wiring:  Custom breakout
//              - Pinout:
//
//   - Example display:
//     -
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
#define DRV_DISP_ARDGFX           // Adafruit-GFX library
#define DRV_DISP_ARDGFX_CANVAS_MONO // Adafruit ILI9341
#define DRV_TOUCH_NONE            // No touch enabled


// -----------------------------------------------------------------------------
// SECTION 2: Pinout
// -----------------------------------------------------------------------------

// For shields, the following pinouts are typically hardcoded
#define ARDGFX_PIN_CS       -1    // Display chip select
#define ARDGFX_PIN_DC       -1    // Display SPI data/command
#define ARDGFX_PIN_RST      -1    // Display Reset

// Display interface type
#define ARDGFX_SPI_HW       0      // Display uses SPI interface: 1=hardware 0=software
#define ARDGFX_SPI_SPEED    0

// Display interface software SPI
// - Hardware SPI: the following definitions are unused
// - Software SPI: the following pins need to be defined
#define ARDGFX_PIN_MOSI     -1
#define ARDGFX_PIN_MISO     -1
#define ARDGFX_PIN_CLK      -1

// SD Card
#define ARDGFX_PIN_SDCS     -1     // SD card chip select (if GSLC_SD_EN=1)



// -----------------------------------------------------------------------------
// SECTION 3: Orientation
// -----------------------------------------------------------------------------

// Set Default rotation of the display
// - Values 0,1,2,3. Rotation is clockwise
#define GSLC_ROTATE     1

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
// Misc
// -----------------------------------------------------------------------------

// Define buffer size for loading images from SD
// - A larger buffer will be faster but at the cost of RAM
#define GSLC_SD_BUFFPIXEL   50

// Enable support for graphics clipping (DrvSetClipRect)
// - Note that this will impact performance of drawing graphics primitives
#define GSLC_CLIP_EN 0

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
