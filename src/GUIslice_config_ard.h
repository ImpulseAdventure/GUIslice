#ifndef _GUISLICE_CONFIG_ARD_H_
#define _GUISLICE_CONFIG_ARD_H_

// =============================================================================
// GUIslice library (user configuration) for:
//     - Arduino
//     - Cortex-M0
//     - ESP8266 / ESP32
//     - nRF52
//     - STM32
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
// DISPLAY CONFIGURATION - SELECT
// =============================================================================

  // Specify the graphics driver library
  // - Uncomment one of the following graphics drivers DRV_DISP_*
  //   applicable to the device type in use

  // --------------------------------------------------------------
  // Arduino / ATmega / AVR / Cortex-M0 / nRF52:
      #define DRV_DISP_ADAGFX           // Adafruit-GFX library
  // --------------------------------------------------------------
  // ESP8266 / ESP32 / NodeMCU:
  //  #define DRV_DISP_ADAGFX           // Adafruit-GFX library
  //  #define DRV_DISP_TFT_ESPI         // Bodmer/TFT_eSPI library
  //  #define DRV_DISP_M5STACK          // m5stack/M5Stack library
  // --------------------------------------------------------------
  // STM32:
  //  #define DRV_DISP_ADAGFX           // Adafruit-GFX library
  //  #define DRV_DISP_ADAGFX_AS        // Adafruit-GFX-AS library, high speed using DMA
  // --------------------------------------------------------------


// =============================================================================
// TOUCH CONFIGURATION - SELECT
// =============================================================================

  // Specify the touchscreen driver
  // - Uncomment one of the following touchscreen drivers DRV_TOUCH_*
  //   applicable to the controller chip in use

  //  #define DRV_TOUCH_NONE            // No touchscreen support
      #define DRV_TOUCH_ADA_STMPE610    // Adafruit STMPE610 touch driver
  //  #define DRV_TOUCH_ADA_FT6206      // Adafruit FT6206 touch driver
  //  #define DRV_TOUCH_ADA_SIMPLE      // Adafruit Touchscreen
  //  #define DRV_TOUCH_TFT_ESPI        // TFT_eSPI integrated XPT2046 touch driver
  //  #define DRV_TOUCH_XPT2046         // Arduino build in XPT2046 touch driver (<XPT2046_touch.h>)
  //  #define DRV_TOUCH_HANDLER         // touch handler class


// =============================================================================
// DISPLAY CONFIGURATION - DETAILS
// - Graphics display driver-specific additional configuration
// =============================================================================

// -----------------------------------------------------------------------------
#if defined(DRV_DISP_ADAGFX)

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

  // Set Default rotation
  // - Note that if you change this you will likely have to change
  //   ADATOUCH_FLIP_X & ADATOUCH_FLIP_Y as well to ensure that the touch screen
  //   orientation matches the display rotation
  #define GSLC_ROTATE     1


#elif defined(DRV_DISP_ADAGFX_AS)

  //NOTE: this is a optimized driver for STM32 only

  // The Adafruit-GFX-AS library supports a number of displays
  // - Select a display sub-type by uncommenting one of the
  //   following DRV_DISP_ADAGFX_* lines
  #define DRV_DISP_ADAGFX_ILI9341_STM     // Adafruit ILI9341 (STM32 version)

  // For Adafruit-GFX drivers, define pin connections
  // - Define general pins (modify these example pin assignments to match your board)
  // - Please refer to "docs/GUIslice_config_guide.xlsx" for detailed examples

  //Note: Fixed pin setting for HW SPI1
  //  PA5  SCLK
  //  PA6  MISO
  //  PA7  MOSI

  // USE Arduino STM32 PIN Notations
  // - Define to use Arduino STM32 PIN Notations
  // #define STM32_NOTATION

  #if defined(STM32_NOTATION)
    // NOTE: Using Arduino STM32 pin notation
    #define ADAGFX_PIN_CS   PA4   // Display chip select
    #define ADAGFX_PIN_DC   PB1   // Display SPI data/command
    #define ADAGFX_PIN_RST  PB0   // Display Reset (set to -1 in order to use Adafruit-GFX drivers w/o reset)
    #define ADAGFX_PIN_SDCS       // SD card chip select
    #define ADAGFX_PIN_WR         // Display write pin (for parallel displays)
    #define ADAGFX_PIN_RD         // Display read pin (for parallel displays)
  #else
    // NOTE: Using Arduino pin notation
    #define ADAGFX_PIN_CS    10   // Display chip select
    #define ADAGFX_PIN_DC     9   // Display SPI data/command
    #define ADAGFX_PIN_RST    0   // Display Reset (some displays could use pin 11)
    #define ADAGFX_PIN_SDCS   4   // SD card chip select
    #define ADAGFX_PIN_WR    A1   // Display write pin (for parallel displays)
    #define ADAGFX_PIN_RD    A0   // Display read pin (for parallel displays)
  #endif

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


  // Set Default rotation
  // - Note that if you change this you will likely have to change
  //   ADATOUCH_FLIP_X & ADATOUCH_FLIP_Y as well to ensure that the touch screen
  //   orientation matches the display rotation
  #define GSLC_ROTATE     1

// -----------------------------------------------------------------------------
#elif defined(DRV_DISP_TFT_ESPI)

  // NOTE: When using the TFT_eSPI library, there are additional
  //       library-specific configuration files that may need
  //       customization (including pin configuration), such as
  //       "User_Setup_Select.h" (typically located in the
  //       Arduino /libraries/TFT_eSPI folder). Please refer to
  //       Bodmer's TFT_eSPI library for more details:
  //       https://github.com/Bodmer/TFT_eSPI

  // NOTE: To avoid potential SPI conflicts, it is recommended
  //       that SUPPORT_TRANSACTIONS is defined in TFT_eSPI's "User Setup"


  // Set Default rotation
  // - Note that if you change this you will likely have to change
  //   ADATOUCH_FLIP_X & ADATOUCH_FLIP_Y as well to ensure that the touch screen
  //   orientation matches the display rotation
  #define GSLC_ROTATE     1

// -----------------------------------------------------------------------------
#elif defined(DRV_DISP_M5STACK)

  #define ADAGFX_PIN_SDCS   4   // SD card chip select
  #define TFT_LIGHT_PIN    32   // display backlight

  // Set Default rotation
  // - Note that if you change this you will likely have to change
  //   ADATOUCH_FLIP_X & ADATOUCH_FLIP_Y as well to ensure that the touch screen
  //   orientation matches the display rotation
  #define GSLC_ROTATE     0

#else

  #error "Unknown driver for display DRV_DISP_..."

#endif // DRV_DISP_*


// =============================================================================
// TOUCH CONFIGURATION - DETAILS
// - Touch Driver-specific additional configuration
// =============================================================================

// -----------------------------------------------------------------------------
#if defined(DRV_TOUCH_NONE)

  //no touch defined

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
  //   (check for min and max values reported from program as you touch display
  //   corners)
  // - Note that X & Y directions reference the display's natural orientation
  #define ADATOUCH_X_MIN 230
  #define ADATOUCH_Y_MIN 260
  #define ADATOUCH_X_MAX 3800
  #define ADATOUCH_Y_MAX 3700

// -----------------------------------------------------------------------------
#elif defined(DRV_TOUCH_ADA_FT6206)
  // Define sensitivity coefficient (capacitive touch)
  #define ADATOUCH_SENSITIVITY  40

// -----------------------------------------------------------------------------
#elif defined(DRV_TOUCH_ADA_SIMPLE)

  // Define 4-wire resistive touchscreen pinout
  #define ADATOUCH_PIN_YP A2   // "Y+": Must be an analog pin, use "An" notation
  #define ADATOUCH_PIN_XM A3   // "X-": Must be an analog pin, use "An" notation
  #define ADATOUCH_PIN_YM 44   // "Y-": Can be a digital pin
  #define ADATOUCH_PIN_XP 45   // "X+": Can be a digital pin
  #define ADATOUCH_RX 300      // "rxplate"

  // Calibration values for touch display
  // - These values may need to be updated to match your display
  // - Typically used in resistive displays
  #define ADATOUCH_X_MIN 100
  #define ADATOUCH_Y_MIN 150
  #define ADATOUCH_X_MAX 900
  #define ADATOUCH_Y_MAX 900

  // Define pressure threshold for detecting a touch
  #define ADATOUCH_PRESS_MIN 10
  #define ADATOUCH_PRESS_MAX 1000

// -----------------------------------------------------------------------------
#elif defined(DRV_TOUCH_TFT_ESPI)
  // The TFT_eSPI display library also includes support for XPT2046 touch controller
  // Note that TFT_eSPI's "User_Setup" should define TOUCH_CS
  #define DRV_TOUCH_IN_DISP   // Use the display driver (TFT_eSPI) for touch events

  // Define the XPT2046 touch driver calibration values
  // - The following are some example defaults, but they should be updated
  //   to match your specific touch device.
  #define TFT_ESPI_TOUCH_CALIB { 321,3498,280,3593,3 }

// -----------------------------------------------------------------------------
#elif defined(DRV_TOUCH_XPT2046)
  // Arduino built in XPT2046 touch driver (<XPT2046_touch.h>)

  // SPI2 is used. Due to some known issues of the TFT SPI driver working on SPI1
  // it was not possible to share the touch with SPI1.
  // On the Arduino STM32 these are the following pins:
  //   PB13   SCLK
  //   PB14   MISO
  //   PB15   MOSI
  #define XPT2046_DEFINE_DPICLASS SPIClass XPT2046_spi(2); //Create an SPI instance on SPI2 port

  // Chip select pin for touch SPI2
  #define XPT2046_CS PB12

  // Calibration values for touch display
  // - These values may need to be updated to match your display
  // - empirically found for XPT2046
  #define ADATOUCH_X_MIN 398
  #define ADATOUCH_Y_MIN 280
  #define ADATOUCH_X_MAX 3877
  #define ADATOUCH_Y_MAX 3805

  // Define pressure threshold for detecting a touch
  #define ADATOUCH_PRESS_MIN 0

// -----------------------------------------------------------------------------
#elif defined(DRV_TOUCH_HANDLER)
  // touch handler class

// -----------------------------------------------------------------------------
#else

  #error "Unknown driver for touch DRV_TOUCH_..."

#endif // DRV_TOUCH_*


// -----------------------------------------------------------------------------

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


// =============================================================================
// COMMON CONFIGURATION
// =============================================================================

  // Error reporting
  // - Set DEBUG_ERR to 1 to enable error reporting via the Serial connection
  // - Enabling DEBUG_ERR increases FLASH memory consumption which may be
  //   limited on the baseline Arduino (ATmega328P) devices.
  #if defined(__AVR__)
    #define DEBUG_ERR               0   // Disable by default on low-mem Arduino
  #else
    // For all other devices, DEBUG_ERR is enabled by default.
    // Since this mode increases FLASH memory considerably, it may be
    // necessary to disable this feature.
    #define DEBUG_ERR               1   // Enable debug reporting
  #endif


  // Enable of optional features
  // - For memory constrained devices such as Arduino, it is best to
  //   set the following features to 0 (to disable) unless they are
  //   required.

  #define GSLC_FEATURE_COMPOUND       0   // Compound elements (eg. XSelNum)
  #define GSLC_FEATURE_XGAUGE_RADIAL  0   // XGauge control with radial support
  #define GSLC_FEATURE_XGAUGE_RAMP    0   // XGauge control with ramp support
  #define GSLC_FEATURE_XTEXTBOX_EMBED 0   // XTextbox control with embedded color


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


  // In "Local String" mode, memory within internal element array is
  // used for strings. This mode incurs a memory cost for all elements,
  // irrespective of whether strings are used or their length. This
  // mode may make string definition convenient but is not memory-efficient.
  // Therefore, it is not recommended for limited memory devices such as
  // Arduino.
  // - When using element local string storage (GSLC_LOCAL_STR=1),
  //   GSLC_LOCAL_STR_LEN defines the fixed length buffer used for every element
  #define GSLC_LOCAL_STR      0   // 1=Use local strings (in element array), 0=External
  #define GSLC_LOCAL_STR_LEN  30  // Max string length of text elements

  #define GSLC_USE_FLOAT      0   // 1=Use floating pt library, 0=Fixed-point lookup tables


  // Debug diagnostic modes
  // - Uncomment any of the following to enable specific debug modes
  //#define DBG_LOG           // Enable debugging log output
  //#define DBG_TOUCH         // Enable debugging of touch-presses
  //#define DBG_FRAME_RATE    // Enable diagnostic frame rate reporting
  //#define DBG_DRAW_IMM      // Enable immediate rendering of drawing primitives
  //#define DBG_DRIVER        // Enable graphics driver debug reporting


// =============================================================================
// INTERNAL CONFIGURATION
// - Users should not need to modify the following
// =============================================================================

  // Display device string is only used in LINUX drivers
  #define GSLC_DEV_TOUCH ""   // No device path used


  // Define compatibility for non-AVR to call PROGMEM functions
  #if defined(__AVR__)
    #define GSLC_USE_PROGMEM 1
  #else
    #define GSLC_USE_PROGMEM 0
  #endif


// =============================================================================

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_CONFIG_ARD_H_
