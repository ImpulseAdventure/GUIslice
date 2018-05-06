#ifndef _GUISLICE_CONFIG_LINUX_H_
#define _GUISLICE_CONFIG_LINUX_H_

// =======================================================================
// GUIslice library (user configuration) for LINUX / Raspberry Pi / Beaglebone
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
#define DRV_DISP_SDL1                // LINUX: SDL 1.2 library
//#define DRV_DISP_SDL2              // LINUX: SDL 2.0 library



// Specify the touchscreen driver
// - Uncomment one of the following touchscreen drivers
//#define DRV_TOUCH_NONE          // No touchscreen support
//#define DRV_TOUCH_SDL           // LINUX: Use SDL touch driver
#define DRV_TOUCH_TSLIB           // LINUX: Use tslib touch driver



// -----------------------------------------------------------------------------------------
// Enable of optional features
// - For memory constrained devices such as Arduino, it is best to
//   set the following features to 0 (to disable) unless they are
//   required.

#define GSLC_FEATURE_COMPOUND       1   // Compound elements (eg. XSelNum)
#define GSLC_FEATURE_XGAUGE_RADIAL  1   // XGauge control with radial support
#define GSLC_FEATURE_XGAUGE_RAMP    1   // XGauge control with ramp support
#define GSLC_FEATURE_XTEXTBOX_EMBED 0   // XTextbox control with embedded color

// Error reporting
// - Set DEBUG_ERR to 1 to enable error reporting via the console
#define DEBUG_ERR                   1   // Enable by default

// -----------------------------------------------------------------------------------------

// Graphics display driver-specific additional configuration
#if defined(DRV_DISP_SDL1)
  // Define default device paths for framebuffer & touchscreen
  #define GSLC_DEV_FB       "/dev/fb1"
  #define GSLC_DEV_TOUCH    "/dev/input/touchscreen"
  #define GSLC_DEV_VID_DRV  "fbcon"

  // Enable SDL startup workaround? (1 to enable, 0 to disable)
  #define DRV_SDL_FIX_START 1

  // Show SDL mouse (1 to show, 0 to hide)
  #define DRV_SDL_MOUSE_SHOW 0

  #define GSLC_LOCAL_STR      1
  #define GSLC_USE_FLOAT      1


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

  #define GSLC_LOCAL_STR      1
  #define GSLC_USE_FLOAT      1


#endif // DRV_DISP_*


// -----------------------------------------------------------------------------------------

// Touch Driver-specific additional configuration
#if defined(DRV_TOUCH_SDL)
  #define DRV_TOUCH_IN_DISP   // Use the display driver (SDL) for touch events


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
#define GSLC_USE_PROGMEM 0


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_CONFIG_LINUX_H_
