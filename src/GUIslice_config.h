#ifndef _GUISLICE_CONFIG_H_
#define _GUISLICE_CONFIG_H_

// =======================================================================
// GUIslice library (user configuration) selection by device
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
// \file GUIslice_config.h


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// -----------------------------------------------------------------------------------------

// Import user configuration depending on device type
#if defined(__linux__)
  #include "GUIslice_config_linux.h"
#elif defined(__AVR__) || defined(ARDUINO_SAMD_ZERO)
  #include "GUIslice_config_ard.h"
#elif defined(ESP8266) || defined(ESP32)
  #include "GUIslice_config_ard.h"
#elif defined(NRF52)
  #include "GUIslice_config_ard.h"
#elif defined(ARDUINO_STM32_FEATHER) || defined(__STM32F1__)
  #include "GUIslice_config_ard.h"
#else
  #error "Unknown device platform"
#endif


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_CONFIG_H_
