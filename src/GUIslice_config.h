#ifndef _GUISLICE_CONFIG_H_
#define _GUISLICE_CONFIG_H_

// =======================================================================
// GUIslice library (user-specified configuration)
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// =======================================================================
//
// The MIT License
//
// Copyright 2016 Calvin Hass
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
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

  
// Specify the display driver library
// - Uncomment one of the following display drivers
//
#define DRV_DISP_SDL1              // SDL 1.2 library
//#define DRV_DISP_SDL2            // SDL 2.0 library

  
// Specify the touchscreen driver
// - Uncomment one of the following touchscreen drivers
//#define DRV_TOUCH_NONE           // No touchscreen support
//#define DRV_TOUCH_SDL            // Use SDL touch driver
#define DRV_TOUCH_TSLIB            // Use tslib touch driver
  
 
  
// Driver-specific additional configuration  
#if defined(DRV_DISP_SDL1)
  // Define default device paths for framebuffer & touchscreen  
  #define GSLC_DEV_FB    "/dev/fb1"
  #define GSLC_DEV_TOUCH "/dev/input/touchscreen"

  // Enable SDL startup workaround?
  #define DRV_SDL_FIX_START
  // Error reporting
  #define DEBUG_ERR   1


#elif defined(DRV_DISP_SDL2)
  // Define default device paths for framebuffer & touchscreen
  // - The following assumes display driver (eg. fbtft) reads from fb1
  // - Raspberry Pi can support hardware acceleration onto fb0
  // - To use SDL2.0 with hardware acceleration with such displays,
  //   use fb0 as the target and then run fbcp to mirror fb0 to fb1
  #define GSLC_DEV_FB    "/dev/fb0"
  #define GSLC_DEV_TOUCH "/dev/input/touchscreen"  

  // Error reporting
  #define DEBUG_ERR   1

#endif
  
  
  
// Define default maximum string length for elements
#define GSLC_ELEM_STRLEN_MAX  20  // Max string length of text elements


// Debug modes
//#define DBG_LOG           // Enable debugging log output
//#define DBG_TOUCH         // Enable debugging of touch-presses
//#define DBG_FRAME_RATE    // Enable diagnostic frame rate reporting
//#define DBG_DRAW_IMM      // Enable immediate rendering of drawing primitives
  
// Enable for bitmap transparency and definition of color to use
#define GSLC_BMP_TRANS_EN     1               // 1 = enabled, 0 = disabled
#define GSLC_BMP_TRANS_RGB    0xFF,0x00,0xFF  // RGB color (default:pink)




#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_CONFIG_H_

