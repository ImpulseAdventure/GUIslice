#ifndef _GUISLICE_DRV_H_
#define _GUISLICE_DRV_H_

// =======================================================================
// GUIslice library (generic driver layer include)
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
// Generic Driver Layer
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined(DRV_DISP_SDL1)
  #include "GUIslice_drv_sdl.h"
#elif defined(DRV_DISP_SDL2)
  #include "GUIslice_drv_sdl.h"
#elif defined(DRV_DISP_ADAGFX)
  #include "GUIslice_drv_adagfx.h"
#elif defined(DRV_DISP_TFT_ESPI)
  #include "GUIslice_drv_tft_espi.h"
#elif defined(DRV_DISP_M5STACK)
  #include "GUIslice_drv_m5stack.h"
#else
  #error "Driver needs to be specified in GUIslice_config_*.h (DRV_DISP_*)"
#endif



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_DRV_H_

