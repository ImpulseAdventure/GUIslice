// =======================================================================
// GUIslice library (driver layer for bodmer/TFT_eSPI)
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
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
// =======================================================================
/// \file GUIslice_drv_tft_espi.cpp

// Compiler guard for requested driver
#include "GUIslice_config.h" // Sets DRV_DISP_*
#if defined(DRV_DISP_TFT_ESPI)

// =======================================================================
// Driver Layer for bodmer/TFT_eSPI
// - https://github.com/Bodmer/TFT_eSPI
// =======================================================================

// GUIslice library
#include "GUIslice_drv_tft_espi.h"

#include <stdio.h>

#include <TFT_eSPI.h>

// Optional SPIFFS support
#if (GSLC_SPIFFS_EN)
  // https://github.com/Bodmer/TFT_eFEX
  #include <TFT_eFEX.h> // Include the extension graphics functions library
#endif

#include <SPI.h>


#if defined(DRV_TOUCH_ADA_STMPE610)
  #include <SPI.h>
  #include <Wire.h>
  #include "Adafruit_STMPE610.h"
#elif defined(DRV_TOUCH_ADA_FT6206)
  #include <Wire.h>
  #include "Adafruit_FT6206.h"
#elif defined(DRV_TOUCH_ADA_SIMPLE)
  #include <stdint.h>
  #include <TouchScreen.h>
#elif defined(DRV_TOUCH_XPT2046_STM)
  // NOTE: This file is located in the Arduino_STM32 library:
  //       Arduino_STM32/STM32F1/libraries/Serasidis_XPT2046_touch/src/XPT2046_touch.h
  #include <XPT2046_touch.h>
#elif defined(DRV_TOUCH_XPT2046_PS)
  #include <XPT2046_Touchscreen.h>
#elif defined(DRV_TOUCH_HANDLER)
  #include <GUIslice_th.h>
#endif

// ------------------------------------------------------------------------
// Load storage drivers
// - Support SD card interface
// ------------------------------------------------------------------------
#if (GSLC_SD_EN)
  #if (GSLC_SD_EN == 1)
    // Use built-in SD library
    // - Only supports HW SPI
    #include <FS.h>
    #include <SD.h>
  #elif (GSLC_SD_EN == 2)
    // Use greiman/SdFat library
    // TODO
    #error "GSLC_SD_EN=2 not yet implemented in TFT_eSPI mode"
  #endif
#endif
 

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


  // Define display driver naming
  const char* m_acDrvDisp = "TFT_eSPI";

// ------------------------------------------------------------------------
// Use default pin settings as defined in TFT_eSPI/User_Setup.h
TFT_eSPI m_disp = TFT_eSPI();
#if (GSLC_SPIFFS_EN)
  // Create TFT_eFEX object "fex" with pointer to "m_disp" object
  TFT_eFEX fex = TFT_eFEX(&m_disp);
#endif

// ------------------------------------------------------------------------
#if defined(DRV_TOUCH_ADA_STMPE610)
  #if (ADATOUCH_I2C_HW) // Use I2C
    const char* m_acDrvTouch = "STMPE610(I2C-HW)";
    Adafruit_STMPE610 m_touch = Adafruit_STMPE610();
  #elif (ADATOUCH_SPI_HW) // Use hardware SPI
    const char* m_acDrvTouch = "STMPE610(SPI-HW)";
    Adafruit_STMPE610 m_touch = Adafruit_STMPE610(ADATOUCH_PIN_CS);
  #elif (ADATOUCH_SPI_SW) // Use software SPI
    const char* m_acDrvTouch = "STMPE610(SPI-SW)";
    Adafruit_STMPE610 m_touch = Adafruit_STMPE610(ADATOUCH_PIN_CS, ADATOUCH_PIN_SDI, ADATOUCH_PIN_SDO, ADATOUCH_PIN_SCK);
  #else // No interface flag set
    #error "DRV_TOUCH_ADA_STMPE610 but no ADATOUCH_I2C_* or ADATOUCH_SPI_* set in config"
  #endif
  #define DRV_TOUCH_INSTANCE
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_ADA_FT6206)
  const char* m_acDrvTouch = "FT6206(I2C)";
  // Always use I2C
  Adafruit_FT6206 m_touch = Adafruit_FT6206();
  #define DRV_TOUCH_INSTANCE
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_ADA_SIMPLE)
  const char* m_acDrvTouch = "SIMPLE(Analog)";
  TouchScreen m_touch = TouchScreen(ADATOUCH_PIN_XP, ADATOUCH_PIN_YP, ADATOUCH_PIN_XM, ADATOUCH_PIN_YM, ADATOUCH_RX);
  #define DRV_TOUCH_INSTANCE
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_XPT2046_STM)
  const char* m_acDrvTouch = "XPT2046_STM(SPI-HW)";
  // Create an SPI class for XPT2046 access
  XPT2046_DEFINE_DPICLASS;
  // XPT2046 driver from Arduino_STM32 by Serasidis (<XPT2046_touch.h>)
  XPT2046_touch m_touch(XPT2046_CS, XPT2046_spi); // Chip Select pin, SPI instance
  #define DRV_TOUCH_INSTANCE
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_XPT2046_PS)
  const char* m_acDrvTouch = "XPT2046_PS(SPI-HW)";
  #if defined(XPT2046_IRQ)
    // Use SPI, with IRQs
    XPT2046_Touchscreen m_touch(XPT2046_CS, XPT2046_IRQ); // Chip Select pin, IRQ pin
  #else
    // Use SPI, no IRQs
    XPT2046_Touchscreen m_touch(XPT2046_CS); // Chip Select pin
  #endif  
  #define DRV_TOUCH_INSTANCE
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_TFT_ESPI)
  const char* m_acDrvTouch = "TFT_eSPI(XPT2046)";
  // Define the XPT2046 calibration data
  uint16_t m_anCalData[5] = TFT_ESPI_TOUCH_CALIB;
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_HANDLER)
  const char* m_acDrvTouch = "Handler";
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_INPUT)
  const char* m_acDrvTouch = "INPUT";
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_NONE)
  const char* m_acDrvTouch = "NONE";
// ------------------------------------------------------------------------
#endif // DRV_TOUCH_*



// =======================================================================
// Public APIs to GUIslice core library
// =======================================================================

// -----------------------------------------------------------------------
// Configuration Functions
// -----------------------------------------------------------------------

bool gslc_DrvInit(gslc_tsGui* pGui)
{

  // Report any debug info if enabled
  #if defined(DBG_DRIVER)
  // TODO
  #endif

  // Initialize any library-specific members
  if (pGui->pvDriver) {
    gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);

    pDriver->nColBkgnd = GSLC_COL_BLACK;

    pDriver->pvFontLast = NULL;

    // These displays can accept partial redraw as they retain the last
    // image in the controller graphics RAM
    pGui->bRedrawPartialEn = true;

    m_disp.init();

    // Now that we have initialized the display, we can assign
    // the rotation parameters and clipping region
    gslc_DrvRotate(pGui,GSLC_ROTATE);

    // Initialize SD card usage
    #if (GSLC_SD_EN)
      #if (!defined(ADAGFX_PIN_SDCS))
        #error "ERROR: ADAGFX_PIN_SDCS must be defined in config for SD support"
      #endif
      if (!SD.begin(ADAGFX_PIN_SDCS)) {
        GSLC_DEBUG_PRINT("ERROR: DrvInit() SD init failed\n", 0);
        return false;
      }
    #endif

    #if (GSLC_SPIFFS_EN)
      // Initialize SPIFFS file system
      if (!SPIFFS.begin()) {
        GSLC_DEBUG_PRINT("ERROR: DrvInit() SPIFFS init failed\n", 0);
      }
    #endif    
  }
  return true;
}

void* gslc_DrvGetDriverDisp(gslc_tsGui* pGui)
{
  return (void*)(&m_disp);
}

void gslc_DrvDestruct(gslc_tsGui* pGui)
{
}

const char* gslc_DrvGetNameDisp(gslc_tsGui* pGui)
{
  return m_acDrvDisp;
}

const char* gslc_DrvGetNameTouch(gslc_tsGui* pGui)
{
  return m_acDrvTouch;
}


// -----------------------------------------------------------------------
// Image/surface handling Functions
// -----------------------------------------------------------------------

void* gslc_DrvLoadImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef)
{
  // GUIslice adapter for Adafruit-GFX doesn't preload the
  // images into RAM (to keep RAM requirements low), so we
  // don't need to do any further processing here. Instead,
  // the loading is done during render.
  if (sImgRef.eImgFlags == GSLC_IMGREF_NONE) {
    return NULL;
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_FILE) {
    return NULL;  // No image preload done
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_SD) {
    return NULL;  // No image preload done
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_RAM) {
    return NULL;  // No image preload done
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_PROG) {
    return NULL;  // No image preload done
  }

  // Default
  return NULL;
}


bool gslc_DrvSetBkgndImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef)
{
  // Dispose of previous background
  if (pGui->sImgRefBkgnd.eImgFlags != GSLC_IMGREF_NONE) {
    gslc_DrvImageDestruct(pGui->sImgRefBkgnd.pvImgRaw);
    pGui->sImgRefBkgnd = gslc_ResetImage();
  }

  pGui->sImgRefBkgnd = sImgRef;
  pGui->sImgRefBkgnd.pvImgRaw = gslc_DrvLoadImage(pGui,sImgRef);
  if (pGui->sImgRefBkgnd.pvImgRaw == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: DrvSetBkgndImage(%s) failed\n","");
    return false;
  }

  return true;
}


bool gslc_DrvSetBkgndColor(gslc_tsGui* pGui,gslc_tsColor nCol)
{
  if (pGui->pvDriver) {
    gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);
    pDriver->nColBkgnd = nCol;
  }
  return true;
}


bool gslc_DrvSetElemImageNorm(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef)
{
  // This driver doesn't preload the image to memory,
  // so we just save the reference for loading upon render
  pElem->sImgRefNorm = sImgRef;
  return true; // TODO
}


bool gslc_DrvSetElemImageGlow(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef)
{
  // This driver doesn't preload the image to memory,
  // so we just save the reference for loading upon render
  pElem->sImgRefGlow = sImgRef;
  return true; // TODO
}


void gslc_DrvImageDestruct(void* pvImg)
{
}

bool gslc_DrvSetClipRect(gslc_tsGui* pGui,gslc_tsRect* pRect)
{
  // NOTE: The clipping rect is currently saved in the
  // driver struct, but the drawing code does not currently
  // use it.
  gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  if (pRect == NULL) {
    // Default to entire display
    pDriver->rClipRect = {0,0,pGui->nDispW,pGui->nDispH};
  } else {
    pDriver->rClipRect = *pRect;
  }

  // Rendering within clipping regions is provided by TFT_eSPI's
  // setViewport() API. Enabling this functionality provides
  // greatly enhanced redraw performance when updating the
  // entire page.
  // The setViewport() API is only available in recent versions of
  // TFT_eSPI (v2.3.2+).
  #if (TFT_ESPI_FEATURES & 0x0001) // Bit 0 = Viewport capability
    m_disp.setViewport(pDriver->rClipRect.x,pDriver->rClipRect.y,pDriver->rClipRect.w,pDriver->rClipRect.h,false);
  #endif

  return true;
}



// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------

const void* gslc_DrvFontAdd(gslc_teFontRefType eFontRefType,const void* pvFontRef,uint16_t nFontSz)
{
  if (eFontRefType == GSLC_FONTREF_PTR  || eFontRefType  == GSLC_FONTREF_FNAME) {
    // Return pointer to Adafruit-GFX GFXfont structure
    return pvFontRef;
  }
  else {
    // Arduino mode currently only supports font definitions from memory
    GSLC_DEBUG2_PRINT("ERROR: DrvFontAdd(%s) failed - Arduino only supports memory-based fonts and fonts stored in SPIFFS\n","");
    return NULL;
  }
}

void gslc_DrvFontsDestruct(gslc_tsGui* pGui)
{
  // Nothing to deallocate
}


// NOTE: Please see the comments associated with gslc_DrvDrawTxt().
//       In summary, DrvGetTxtSize() is not able to gather the complete
//       text sizing information from TFT_eSPI as the APIs are not available.
//
bool gslc_DrvGetTxtSize(gslc_tsGui* pGui,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,
        int16_t* pnTxtX,int16_t* pnTxtY,uint16_t* pnTxtSzW,uint16_t* pnTxtSzH)
{
  uint16_t  nTxtLen    = 0;
  uint16_t  nTxtHeight = 0;
  uint16_t  nTxtScale  = pFont->nSize;
  // TFT_eSPI font API differs from Adafruit-GFX's setFont() API
  if (pFont->pvFont == NULL) {
    m_disp.setTextFont(1);
  } else {
    m_disp.setFreeFont((const GFXfont *)pFont->pvFont);
  }
  m_disp.setTextSize(nTxtScale);
  nTxtLen = m_disp.textWidth((char*)pStr);
  nTxtHeight = m_disp.fontHeight(1); // Use freefont "textfont" value
  *pnTxtX = 0;  // Unused
  *pnTxtY = 0;  // Unused
  *pnTxtSzW = nTxtLen;
  *pnTxtSzH = nTxtHeight;

  return true;
}

bool gslc_DrvDrawTxtAlign(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,int8_t eTxtAlign,
        gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt, gslc_tsColor colBg=GSLC_COL_BLACK)
{
  gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(colTxt);
  uint16_t nColBgRaw = gslc_DrvAdaptColorToRaw(colBg);
  uint16_t nTxtScale = pFont->nSize;

  #ifdef SMOOTH_FONT
    m_disp.setTextColor(nColRaw,nColBgRaw);
  #else
    m_disp.setTextColor(nColRaw);
  #endif

  // TFT_eSPI font API differs from Adafruit-GFX's setFont() API
  if (pFont->pvFont == NULL) {
    m_disp.setTextFont(1);
  } else {
    #ifdef SMOOTH_FONT
      if (pFont->eFontRefType  == GSLC_FONTREF_FNAME){
        if (pFont->pvFont != pDriver->pvFontLast) {
          m_disp.loadFont((const char*)pFont->pvFont);
          pDriver->pvFontLast = pFont->pvFont;
        }
      } else {
        m_disp.setFreeFont((const GFXfont *)pFont->pvFont);
      }
    #else
      m_disp.setFreeFont((const GFXfont *)pFont->pvFont);
    #endif
  }
  m_disp.setTextSize(nTxtScale);

  // Default to mid-mid datum
  int8_t  nDatum = MC_DATUM;
  int16_t nTxtX = nX0 + (nX1-nX0)/2;
  int16_t nTxtY = nY0 + (nY1-nY0)/2;

  // Override the datum depending on alignment mode
  switch(eTxtAlign) {
    case GSLC_ALIGN_TOP_LEFT:   nDatum = TL_DATUM; nTxtX = nX0; nTxtY = nY0; break;
    case GSLC_ALIGN_TOP_MID:    nDatum = TC_DATUM; nTxtY = nY0; break;
    case GSLC_ALIGN_TOP_RIGHT:  nDatum = TR_DATUM; nTxtX = nX1; nTxtY = nY0; break;
    case GSLC_ALIGN_MID_LEFT:   nDatum = ML_DATUM; nTxtX = nX0; break;
    case GSLC_ALIGN_MID_MID:    nDatum = MC_DATUM; break;
    case GSLC_ALIGN_MID_RIGHT:  nDatum = MR_DATUM; nTxtX = nX1; break;
    case GSLC_ALIGN_BOT_LEFT:   nDatum = BL_DATUM; nTxtX = nX0; nTxtY = nY1; break;
    case GSLC_ALIGN_BOT_MID:    nDatum = BC_DATUM; nTxtY = nY1; break;
    case GSLC_ALIGN_BOT_RIGHT:  nDatum = BR_DATUM; nTxtX = nX1; nTxtY = nY1; break;
    default:                    nDatum = MC_DATUM; break;
  }
  m_disp.setTextDatum(nDatum);

  m_disp.drawString(pStr,nTxtX,nTxtY);

  // For now, always return true
  return true;
}

// NOTE: As TFT_eSPI performs some complex logic in determining the font
//       baseline and associated adjustments and these are not provided
//       via an API, calling DrvGetTxtSize() is insufficient to determine
//       the appropriate position corrections required (eg. when centering
//       text). Therefore, DrvDrawTxt() will not result in proper text
//       alignment in all cases. Instead, it is recommended that
//       DrvDrawTxtAlign() is used instead, which will depend on the datum
//       adjustment code within TFT_eSPI. This mode of operation is
//       selected by default in GUIslice_drv_tft_espi.h by setting
//       DRV_OVERRIDE_TXT_ALIGN to 1.

// This method is not recommended for use with TFT_eSPI. DrvDrawTxtAlign()
// should be used instead.
bool gslc_DrvDrawTxt(gslc_tsGui* pGui,int16_t nTxtX,int16_t nTxtY,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt, gslc_tsColor colBg=GSLC_COL_BLACK)
{
  gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  uint16_t nTxtScale = pFont->nSize;
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(colTxt);
  uint16_t nColBgRaw = gslc_DrvAdaptColorToRaw(colBg);
  #ifdef SMOOTH_FONT
      if (pFont->eFontRefType  == GSLC_FONTREF_FNAME){
        if (pFont->pvFont != pDriver->pvFontLast) {
          m_disp.loadFont((const char*)pFont->pvFont);
          pDriver->pvFontLast = pFont->pvFont;
        }
        m_disp.setTextColor(nColRaw,nColBgRaw);
      } else {
        m_disp.setTextColor(nColRaw);
      }
  #else
  m_disp.setTextColor(nColRaw);
  #endif
  // m_disp.setCursor(nTxtX,nTxtY);
  m_disp.setTextSize(nTxtScale);

  // Default to top-left datum
  m_disp.setTextDatum(TL_DATUM);

  if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_RAM) {
    // String in SRAM; can access buffer directly
    // m_disp.println(pStr);
    m_disp.drawString(pStr,nTxtX,nTxtY);
  } else if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_PROG) {
    // String in PROGMEM (flash); must access via pgm_* calls
    char    ch;
    int     nXOffset = 0;
    while ((ch = pgm_read_byte(pStr++))) {
      // m_disp.print(ch);
      nXOffset += m_disp.drawChar(ch,nTxtX+nXOffset,nTxtY);
    }
    m_disp.println();
  }

  return true;
}

// -----------------------------------------------------------------------
// Screen Management Functions
// -----------------------------------------------------------------------

void gslc_DrvPageFlipNow(gslc_tsGui* pGui)
{
  #if defined(DRV_DISP_ADAGFX_ILI9341) || defined(DRV_DISP_ADAGFX_ILI9341_8BIT) || \
    defined(DRV_DISP_ADAGFX_ILI9341_STM) || defined(DRV_DISP_ADAGFX_ST7735) || \
    defined(DRV_DISP_ADAGFX_HX8347) || defined(DRV_DISP_ADAGFX_HX8357)
    // Nothing to do as we're not double-buffered

  #elif defined(DRV_DISP_ADAGFX_SSD1306)
    // Show the display buffer on the hardware.
    // NOTE: You _must_ call display after making any drawing commands
    // to make them visible on the display hardware!
    m_disp.display();

    // TODO: Might need to call m_disp.clearDisplay() now?

  #endif
}


// -----------------------------------------------------------------------
// Graphics Primitives Functions
// -----------------------------------------------------------------------

inline void gslc_DrvDrawPoint_base(int16_t nX, int16_t nY, uint16_t nColRaw)
{
  m_disp.drawPixel(nX,nY,nColRaw);
}

inline void gslc_DrvDrawLine_base(int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,uint16_t nColRaw)
{
  m_disp.drawLine(nX0,nY0,nX1,nY1,nColRaw);
}


bool gslc_DrvDrawPoint(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // Perform clipping
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  if (!gslc_ClipPt(&pDriver->rClipRect,nX,nY)) { return true; }
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  gslc_DrvDrawPoint_base(nX, nY, nColRaw);
  return true;
}


bool gslc_DrvDrawPoints(gslc_tsGui* pGui,gslc_tsPt* asPt,uint16_t nNumPt,gslc_tsColor nCol)
{
  return false;
}

bool gslc_DrvDrawFillRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // Perform clipping
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  if (!gslc_ClipRect(&pDriver->rClipRect,&rRect)) { return true; }
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.fillRect(rRect.x,rRect.y,rRect.w,rRect.h,nColRaw);
  return true;
}

bool gslc_DrvDrawFillRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol)
{
#if (DRV_HAS_DRAW_RECT_ROUND_FILL)
  // TODO: Support GSLC_CLIP_EN
  // - Would need to determine how to clip the rounded corners
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.fillRoundRect(rRect.x,rRect.y,rRect.w,rRect.h,nRadius,nColRaw);
#endif
  return true;
}


bool gslc_DrvDrawFrameRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol)
{
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
#if (GSLC_CLIP_EN)
  // Perform clipping
  // - TODO: Optimize the following, perhaps with new ClipLineHV()
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  int16_t nX0, nY0, nX1, nY1;
  // Top
  nX0 = rRect.x;
  nY0 = rRect.y;
  nX1 = rRect.x + rRect.w - 1;
  nY1 = nY0;
  if (gslc_ClipLine(&pDriver->rClipRect, &nX0, &nY0, &nX1, &nY1)) { gslc_DrvDrawLine_base(nX0, nY0, nX1, nY1, nColRaw); }
  // Bottom
  nX0 = rRect.x;
  nY0 = rRect.y + rRect.h - 1;
  nX1 = rRect.x + rRect.w - 1;
  nY1 = nY0;
  if (gslc_ClipLine(&pDriver->rClipRect, &nX0, &nY0, &nX1, &nY1)) { gslc_DrvDrawLine_base(nX0, nY0, nX1, nY1, nColRaw); }
  // Left
  nX0 = rRect.x;
  nY0 = rRect.y;
  nX1 = nX0;
  nY1 = rRect.y + rRect.h - 1;
  if (gslc_ClipLine(&pDriver->rClipRect, &nX0, &nY0, &nX1, &nY1)) { gslc_DrvDrawLine_base(nX0, nY0, nX1, nY1, nColRaw); }
  // Right
  nX0 = rRect.x + rRect.w - 1;
  nY0 = rRect.y;
  nX1 = nX0;
  nY1 = rRect.y + rRect.h - 1;
  if (gslc_ClipLine(&pDriver->rClipRect, &nX0, &nY0, &nX1, &nY1)) { gslc_DrvDrawLine_base(nX0, nY0, nX1, nY1, nColRaw); }
#else
  m_disp.drawRect(rRect.x,rRect.y,rRect.w,rRect.h,nColRaw);
#endif
  return true;
}

bool gslc_DrvDrawFrameRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol)
{
#if (DRV_HAS_DRAW_RECT_ROUND_FRAME)
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);

  // TODO: Support GSLC_CLIP_EN
  // - Would need to determine how to clip the rounded corners
  m_disp.drawRoundRect(rRect.x,rRect.y,rRect.w,rRect.h,nRadius,nColRaw);
#endif
  return true;
}



bool gslc_DrvDrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  if (!gslc_ClipLine(&pDriver->rClipRect,&nX0,&nY0,&nX1,&nY1)) { return true; }
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  gslc_DrvDrawLine_base(nX0,nY0,nX1,nY1,nColRaw);
  return true;
}

bool gslc_DrvDrawFrameCircle(gslc_tsGui*,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // TODO
#endif

#if (DRV_HAS_DRAW_CIRCLE_FRAME)
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.drawCircle(nMidX,nMidY,nRadius,nColRaw);
#endif
  return true;
}

bool gslc_DrvDrawFillCircle(gslc_tsGui*,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // TODO
#endif

#if (DRV_HAS_DRAW_CIRCLE_FILL)
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.fillCircle(nMidX,nMidY,nRadius,nColRaw);
#endif
  return true;
}


bool gslc_DrvDrawFrameTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol)
{
#if (DRV_HAS_DRAW_TRI_FRAME)

#if (GSLC_CLIP_EN)
  // TODO
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.drawTriangle(nX0,nY0,nX1,nY1,nX2,nY2,nColRaw);
#endif
  return true;
}

bool gslc_DrvDrawFillTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol)
{
#if (DRV_HAS_DRAW_TRI_FILL)

#if (GSLC_CLIP_EN)
  // TODO
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.fillTriangle(nX0,nY0,nX1,nY1,nX2,nY2,nColRaw);
#endif
  return true;
}



// ----- REFERENCE CODE begin
// The following code was based upon the following reference code but modified to
// adapt for use in GUIslice.
//
//   URL:              https://github.com/adafruit/Adafruit-GFX-Library/blob/master/Adafruit_GFX.cpp
//   Original author:  Adafruit
//   Function:         drawBitmap()

// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer using the foreground color defined in the
// header (unset bits are transparent).

// GUIslice modified the raw memory format to add a header:
// Image array format:
// - Width[15:8],  Width[7:0],
// - Height[15:8], Height[7:0],
// - ColorR[7:0],  ColorG[7:0],
// - ColorB[7:0],  0x00,
// - Monochrome bitmap follows...
//
void gslc_DrvDrawMonoFromMem(gslc_tsGui* pGui,int16_t nDstX, int16_t nDstY,
 const unsigned char *pBitmap,bool bProgMem)
 {
  const unsigned char*  bmap_base = pBitmap;
  int16_t         w,h;
  gslc_tsColor    nCol;

  // Read header
  w       = ( (bProgMem)? pgm_read_byte(bmap_base++) : *(bmap_base++) ) << 8;
  w      |= ( (bProgMem)? pgm_read_byte(bmap_base++) : *(bmap_base++) ) << 0;
  h       = ( (bProgMem)? pgm_read_byte(bmap_base++) : *(bmap_base++) ) << 8;
  h      |= ( (bProgMem)? pgm_read_byte(bmap_base++) : *(bmap_base++) ) << 0;
  nCol.r  =   (bProgMem)? pgm_read_byte(bmap_base++) : *(bmap_base++);
  nCol.g  =   (bProgMem)? pgm_read_byte(bmap_base++) : *(bmap_base++);
  nCol.b  =   (bProgMem)? pgm_read_byte(bmap_base++) : *(bmap_base++);
  bmap_base++;

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t nByte = 0;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) nByte <<= 1;
      else {
        if (bProgMem) {
          nByte = pgm_read_byte(bmap_base + j * byteWidth + i / 8);
        } else {
          nByte = bmap_base[j * byteWidth + i / 8];
        }
      }
      if(nByte & 0x80) {
        gslc_DrvDrawPoint(pGui,nDstX+i,nDstY+j,nCol);
      }
    }
  }
}
// ----- REFERENCE CODE end

void gslc_DrvDrawBmp24FromMem(gslc_tsGui* pGui,int16_t nDstX, int16_t nDstY,const unsigned char* pBitmap,bool bProgMem)
{
  const uint16_t* pImage = (const uint16_t*)pBitmap;
  int16_t h = *(pImage++);
  int16_t w = *(pImage++);

  // Swap the colour byte order when rendering
  m_disp.setSwapBytes(true); 
  #if (GSLC_BMP_TRANS_EN)
    uint16_t nTransRaw = gslc_DrvAdaptColorToRaw(pGui->sTransCol);
    if (bProgMem) {
      m_disp.pushImage(nDstX, nDstY, w, h, (const uint16_t*) pImage, nTransRaw); 
    } else {
      m_disp.pushImage(nDstX, nDstY, w, h, (uint16_t*) pImage, nTransRaw); 
    }
  #else
    if (bProgMem) {
      m_disp.pushImage(nDstX, nDstY, w, h, (const uint16_t*) pImage); 
    } else {
      m_disp.pushImage(nDstX, nDstY, w, h, (uint16_t*) pImage); 
    }
  #endif // GSLC_BMP_TRANS_EN
}

#if (GSLC_SD_EN)
// ----- REFERENCE CODE begin
// The following code was based upon the following reference code but modified to
// adapt for use in GUIslice.
//
//   URL:              https://github.com/adafruit/Adafruit_ILI9341/blob/master/examples/spitftbitmap/spitftbitmap.ino
//   Original author:  Adafruit
//   Function:         bmpDraw()

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
uint16_t gslc_DrvRead16SD(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t gslc_DrvRead32SD(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void gslc_DrvDrawBmp24FromSD(gslc_tsGui* pGui,const char *filename, uint16_t x, uint16_t y)
{
  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*GSLC_SD_BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  (void)startTime; // Unused

  if((x >= pGui->nDispW) || (y >= pGui->nDispH)) return;

  //Serial.println();
  //Serial.print("Loading image '");
  //Serial.print(filename);
  //Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == 0) {
    GSLC_DEBUG2_PRINT("ERROR: DrvDrawBmp24FromSD() file not found [%s]",filename);
    return;
  }
  // Parse BMP header
  if(gslc_DrvRead16SD(bmpFile) == 0x4D42) { // BMP signature
    uint32_t nFileSize = gslc_DrvRead32SD(bmpFile);
    (void)nFileSize; // Unused
    //Serial.print("File size: "); Serial.println(nFileSize);
    (void)gslc_DrvRead32SD(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = gslc_DrvRead32SD(bmpFile); // Start of image data
    //Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    uint32_t nHdrSize = gslc_DrvRead32SD(bmpFile);
    (void)nHdrSize; // Unused
    //Serial.print("Header size: "); Serial.println(nHdrSize);
    bmpWidth  = gslc_DrvRead32SD(bmpFile);
    bmpHeight = gslc_DrvRead32SD(bmpFile);
    if(gslc_DrvRead16SD(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = gslc_DrvRead16SD(bmpFile); // bits per pixel
      //Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (gslc_DrvRead32SD(bmpFile) == 0)) { // 0 = uncompressed
        goodBmp = true; // Supported BMP format -- proceed!
        //Serial.print("Image size: ");
        //Serial.print(bmpWidth);
        //Serial.print('x');
        //Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= pGui->nDispW) w = pGui->nDispW  - x;
        if((y+h-1) >= pGui->nDispH) h = pGui->nDispH - y;

        // Set TFT address window to clipped image bounds
        //xxx tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            //xxx tft.pushColor(tft.Color565(r,g,b));
            gslc_tsColor nCol = (gslc_tsColor){r,g,b};
            bool bDrawBit = true;
            if (GSLC_BMP_TRANS_EN) {
              gslc_tsColor nColTrans = pGui->sTransCol;
              if ((nCol.r == nColTrans.r) && (nCol.g == nColTrans.g) && (nCol.b == nColTrans.b)) {
                bDrawBit = false;
              }
            }
            if (bDrawBit) {
              gslc_DrvDrawPoint(pGui,x+col,y+row,nCol);
            }

          } // end pixel
        } // end scanline
        //Serial.print("Loaded in ");
        //Serial.print(millis() - startTime);
        //Serial.println(" ms");
      } // end goodBmp
    }
  }
  bmpFile.close();
  if(!goodBmp) {
    GSLC_DEBUG2_PRINT("ERROR: DrvDrawBmp24FromSD() BMP format unknown [%s]",filename);
  }
}
// ----- REFERENCE CODE end
#endif // GSLC_SD_EN


bool gslc_DrvDrawImage(gslc_tsGui* pGui,int16_t nDstX,int16_t nDstY,gslc_tsImgRef sImgRef)
{
  #if defined(DBG_DRIVER)
  char addr[9];
  GSLC_DEBUG_PRINT("DBG: DrvDrawImage() with ImgBuf address=","");
  sprintf(addr,"%08X",(unsigned int)sImgRef.pImgBuf);
  GSLC_DEBUG_PRINT("%s\n",addr);
  #endif

  // GUIslice adapter library for Adafruit-GFX does not pre-load
  // image data into memory before calling DrvDrawImage(), so
  // we to handle the loading now (when rendering).
  if (sImgRef.eImgFlags == GSLC_IMGREF_NONE) {
    return true;  // Nothing to do
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_FILE) {
    // Load image from SPIFFS
    #if (GSLC_SPIFFS_EN)
      if ((sImgRef.eImgFlags & GSLC_IMGREF_FMT) == GSLC_IMGREF_FMT_JPG) {
        // Draw Jpeg from SPIFFS file system
        return gslc_DrvDrawJpegFromFile(pGui,nDstX,nDstY,sImgRef);
      } else if ((sImgRef.eImgFlags & GSLC_IMGREF_FMT) == GSLC_IMGREF_FMT_BMP24) {
        // Draw Bitmap from SPIFFS file system
        return gslc_DrvDrawBmpFromFile(pGui,nDstX,nDstY,sImgRef);
      } else {
        return false; // TODO: not yet supported
      }
    #else
      GSLC_DEBUG_PRINT("ERROR: GetImageFromFile() not supported as Config:GSLC_SPIFFS_EN=0\n", 0);
      return false; // TODO: not yet supported
    #endif // GSLC_SPIFFS_EN
  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_RAM) {
    if ((sImgRef.eImgFlags & GSLC_IMGREF_FMT) == GSLC_IMGREF_FMT_RAW1) {
      // Draw a monochrome bitmap from SRAM
      // - Dimensions and output color are defined in arrray header
      gslc_DrvDrawMonoFromMem(pGui,nDstX,nDstY,sImgRef.pImgBuf,false);
      return true;
    } else if ((sImgRef.eImgFlags & GSLC_IMGREF_FMT) == GSLC_IMGREF_FMT_BMP24) {
      // 24-bit Bitmap in ram
      gslc_DrvDrawBmp24FromMem(pGui,nDstX,nDstY,sImgRef.pImgBuf,false);
      return true;
    } else {
      return false; // TODO: not yet supported
    }

  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_PROG) {
    // TODO: Probably need to fix this to work with PROGMEM,
    //       but check (GSLC_USE_PROGMEM) first
    if ((sImgRef.eImgFlags & GSLC_IMGREF_FMT) == GSLC_IMGREF_FMT_RAW1) {
      // Draw a monochrome bitmap from program memory
      // - Dimensions and output color are defined in array header
      gslc_DrvDrawMonoFromMem(pGui,nDstX,nDstY,sImgRef.pImgBuf,true);
      return true;
    } else if ((sImgRef.eImgFlags & GSLC_IMGREF_FMT) == GSLC_IMGREF_FMT_BMP24) {
      // 24-bit Bitmap in flash
      gslc_DrvDrawBmp24FromMem(pGui,nDstX,nDstY,sImgRef.pImgBuf,true);
      return true;
    } else {
      return false; // TODO: not yet supported
    }

  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_SD) {
    // Load image from SD media
    #if (GSLC_SD_EN)
      if ((sImgRef.eImgFlags & GSLC_IMGREF_FMT) == GSLC_IMGREF_FMT_BMP24) {
        // 24-bit Bitmap
        gslc_DrvDrawBmp24FromSD(pGui,sImgRef.pFname,nDstX,nDstY);
        return true;
      } else {
        // Unsupported format
        return false;
      }
    #else
      // SD card access not enabled
      GSLC_DEBUG_PRINT("ERROR: GSLC_SD_EN not enabled\n","");
      return false;
    #endif

  } else {
    // Unsupported source
    GSLC_DEBUG_PRINT("DBG: DrvDrawImage() unsupported source eImgFlags=%d\n", sImgRef.eImgFlags);
    return false;
  }
}

#if (GSLC_SPIFFS_EN)
bool gslc_DrvDrawBmpFromFile(gslc_tsGui* pGui,int16_t nDstX,int16_t nDstY,gslc_tsImgRef sImgRef)
{
  const char* pStrFname = sImgRef.pFname;

  // Load BMP image from file system
  // NOTE: No return value is provided upon decoder failure,
  //       so we always proceed as if it is OK.
  fex.drawBmp(pStrFname, nDstX, nDstY);
  return true;
}

bool gslc_DrvDrawJpegFromFile(gslc_tsGui* pGui,int16_t nDstX,int16_t nDstY,gslc_tsImgRef sImgRef)
{
  const char* pStrFname = sImgRef.pFname;

  // Load JPEG image from file system
#if defined(ESP32)
  // use optimized ESP32 native decoder
  // drawJpgFile() can return false upon a decoding failure (eg. for
  // unsupported progressive JPEG images), so we trap it here.
  if (!fex.drawJpgFile(SPIFFS, pStrFname, nDstX, nDstY)) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawJpegFromFile() failed on [%s]",pStrFname);
    return false;
  }
#else 
  // use library decoder
  // NOTE: No return value is provided upon decoder failure,
  //       so we always proceed as if it is OK.
  fex.drawJpeg(pStrFname, nDstX, nDstY);
#endif

  return true;
}
#endif // GSLC_SPIFFS_EN

void gslc_DrvDrawBkgnd(gslc_tsGui* pGui)
{
  if (pGui->pvDriver) {
    gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);

    // Check to see if an image has been assigned to the background
    if (pGui->sImgRefBkgnd.eImgFlags == GSLC_IMGREF_NONE) {
      // No image assigned, so assume flat color background
      // TODO: Create a new eImgFlags enum to signal that the
      //       background should be a flat color instead of
      //       an image.

      // NOTE: We don't call m_disp.fillScreen() here as
      //       that API doesn't support clipping. Since
      //       we may be redrawing the page with a clipping
      //       region enabled, it is important that we don't
      //       redraw the entire screen.
      gslc_tsRect rRect = (gslc_tsRect) { 0, 0, pGui->nDispW, pGui->nDispH };
      gslc_DrvDrawFillRect(pGui, rRect, pDriver->nColBkgnd);
    } else {
      // An image should be loaded
      // TODO: For now, re-use the DrvDrawImage(). Later, consider
      //       extending to support different background drawing
      //       capabilities such as stretching and tiling of background
      //       image.
      gslc_DrvDrawImage(pGui, 0, 0, pGui->sImgRefBkgnd);
    }
  }
}


// -----------------------------------------------------------------------
// Touch Functions (via display driver)
// -----------------------------------------------------------------------

// Confirm that TOUCH_CS has been defined otherwise the
// m_disp.getTouch() call won't be defined in TFT_eSPI
#if defined(DRV_TOUCH_TFT_ESPI)
  #if !defined(TOUCH_CS)
    #error "To use DRV_TOUCH_TFT_ESPI, TOUCH_CS needs to be defined in TFT_eSPI User_Setup"
  #endif
#endif

#if defined(DRV_TOUCH_IN_DISP)

// Enable touch filtering from TFT_eSPI (comment to disable)
#define DRV_TOUCH_TFT_ESPI_FILTER


// Import touch filtering code from TFT_eSPI
#if defined(DRV_TOUCH_TFT_ESPI_FILTER)
  // - NOTE: We can't use TFT_eSPI's validTouch() as it is declared private
  //   so instead we have replicated code here.
  // ----- REFERENCE CODE begin
  //   Reference code: https://github.com/Bodmer/TFT_eSPI/blob/master/Extensions/Touch.cpp
  #define TFT_eSPI_RAWERR 20 // Deadband error allowed in successive position samples
  uint8_t TFT_eSPI_validTouch(uint16_t *x, uint16_t *y, uint16_t threshold) {
  uint16_t x_tmp, y_tmp, x_tmp2, y_tmp2;

  // Wait until pressure stops increasing to debounce pressure
  uint16_t z1 = 1;
  uint16_t z2 = 0;
  while (z1 > z2)
  {
    z2 = z1;
    z1 = m_disp.getTouchRawZ();
    delay(1);
  }

  //  Serial.print("Z = ");Serial.println(z1);

  if (z1 <= threshold) return false;

  m_disp.getTouchRaw(&x_tmp, &y_tmp);

  //  Serial.print("Sample 1 x,y = "); Serial.print(x_tmp);Serial.print(",");Serial.print(y_tmp);
  //  Serial.print(", Z = ");Serial.println(z1);

  delay(1); // Small delay to the next sample
  if (m_disp.getTouchRawZ() <= threshold) return false;

  delay(2); // Small delay to the next sample
  m_disp.getTouchRaw(&x_tmp2, &y_tmp2);

  //  Serial.print("Sample 2 x,y = "); Serial.print(x_tmp2);Serial.print(",");Serial.println(y_tmp2);
  //  Serial.print("Sample difference = ");Serial.print(abs(x_tmp - x_tmp2));Serial.print(",");Serial.println(abs(y_tmp - y_tmp2));

  if (abs(x_tmp - x_tmp2) > TFT_eSPI_RAWERR) return false;
  if (abs(y_tmp - y_tmp2) > TFT_eSPI_RAWERR) return false;

  *x = x_tmp;
  *y = y_tmp;

  return true;
  }
  // ----- REFERENCE CODE end
#endif // DRV_TOUCH_TFT_ESPI_FILTER

void* gslc_DrvGetDriverTouch(gslc_tsGui* pGui)
{
  // As the touch driver instance is optional, we need to check for
  // its existence before returning a pointer to it.
  #if defined(DRV_TOUCH_INSTANCE)
    return (void*)(&m_touch);
  #else
    return NULL;
  #endif
}


bool gslc_DrvInitTouch(gslc_tsGui* pGui, const char* acDev) {
  if (pGui == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: DrvInitTouch(%s) called with NULL ptr\n", "");
    return false;
  }

  // Perform any driver-specific touchscreen init here

  // Initialize the touch calibration data
  #if defined(DRV_TOUCH_TFT_ESPI)
    // NOTE: TFT_eSPI calibration unused as GUIslice calibration is used instead
    m_disp.setTouch(m_anCalData);
  #endif

  // Load calibration settings for resistive displays from config
  #if defined(DRV_TOUCH_CALIB)
    pGui->nTouchCalXMin = ADATOUCH_X_MIN;
    pGui->nTouchCalXMax = ADATOUCH_X_MAX;
    pGui->nTouchCalYMin = ADATOUCH_Y_MIN;
    pGui->nTouchCalYMax = ADATOUCH_Y_MAX;
    #if defined(ADATOUCH_PRESS_MIN)
      pGui->nTouchCalPressMin = ADATOUCH_PRESS_MIN;
      pGui->nTouchCalPressMax = ADATOUCH_PRESS_MAX;
    #else
      // For backward compatibility, if these config settings
      // were not included in the config file, provide defaults.
      pGui->nTouchCalPressMin = 200;
      pGui->nTouchCalPressMax = 4000;
    #endif
  #endif // DRV_TOUCH_CALIB

  // Support touch controllers with swapped X & Y
  #if defined(ADATOUCH_REMAP_YX)
    // Capture swap setting from config file
    pGui->bTouchRemapYX = ADATOUCH_REMAP_YX;
  #else
    // For backward compatibility with older config files
    // that have not defined this config option
    pGui->bTouchRemapYX = false;
  #endif

  // NOTE: TFT_eSPI constructor already initializes the touch
  // driver if TOUCH_CS is defined in the TFT_eSPI library's "User_Setup.h"

  // Nothing further to do with driver
  return true;
}

bool gslc_DrvGetTouch(gslc_tsGui* pGui, int16_t* pnX, int16_t* pnY, uint16_t* pnPress, gslc_teInputRawEvent* peInputEvent, int16_t* pnInputVal)
{

  if ((pGui == NULL) || (pGui->pvDriver == NULL)) {
    GSLC_DEBUG2_PRINT("ERROR: DrvGetTouch(%s) called with NULL ptr\n", "");
    return false;
  }

  // Use TFT_eSPI's integrated XPT2046 touch handler
  // - Note that the TFT_eSPI getTouch() calls are not used here because
  //   they don't currently support dynamic rotation. Without dynamic rotation
  //   support, the coordinate transform will not be correct after issuing a
  //   setRotation() call to the TFT_eSPI display library.
  // - Therefore, we will instead use the raw XPT2046 readings (from TFT_eSPI)
  //   and let GUIslice perform the calibration and dynamic rotation.
  // - Note that these readings will naturally be quite noisy w/o filtering.
  // - Reference: https://github.com/Bodmer/TFT_eSPI/issues/365

  // - TODO: Consider merging this logic with gslc_TDrvGetTouch() and remove
  //         the special case for DRV_TOUCH_IN_DISP

  // As some touch hardware drivers don't appear to return
  // an indication of "touch released" with a coordinate, we
  // must detect the release transition here and send the last
  // known coordinate but with pressure=0. To do this, we are
  // allocating a static variable to maintain the last touch
  // coordinate.
  // TODO: This code can be reworked / simplified
  static int16_t  m_nLastRawX     = 0;
  static int16_t  m_nLastRawY     = 0;
  static uint16_t m_nLastRawPress = 0;
  static bool     m_bLastTouched  = false;

  bool bValid = false;  // Indicate a touch event to GUIslice core?

  // Define maximum bounds for display in native orientation
  int nDispOutMaxX,nDispOutMaxY;
  nDispOutMaxX = pGui->nDisp0W-1;
  nDispOutMaxY = pGui->nDisp0H-1;

  uint16_t  nRawX,nRawY; //XPT2046 returns values up to 4095
  uint16_t  nRawPress;   //XPT2046 returns values up to 4095

  // Retrieve the raw touch coordinates from the XPT2046

  // Provide optional touch filtering
  #if defined(DRV_TOUCH_TFT_ESPI_FILTER)
    // Use touch filtering code from TFT_eSPI
    uint8_t nSamples = 5;
    uint8_t nSamplesValid = 0;
    while (nSamples--) {
      if (TFT_eSPI_validTouch(&nRawX, &nRawY, pGui->nTouchCalPressMin)) nSamplesValid++;
    }
    if (nSamplesValid < 1) {
      nRawPress = 0; // Invalidate the reading
    }
    else {
      // Force a value within range
      nRawPress = pGui->nTouchCalPressMin + 1;
    }
  #else
    // No additional touch filtering
    // NOTE: On some displays this will lead to very noisy touch readings
    nRawPress = m_disp.getTouchRawZ();
    m_disp.getTouchRaw(&nRawX, &nRawY);
  #endif // DRV_TOUCH_TFT_ESPI_FILTER

  if ((nRawPress > pGui->nTouchCalPressMin) && (nRawPress < pGui->nTouchCalPressMax)) {

    m_nLastRawX = nRawX;
    m_nLastRawY = nRawY;
    m_nLastRawPress = nRawPress;
    m_bLastTouched = true;
    bValid = true;
  } else {
    if (!m_bLastTouched) {
      // Wasn't touched before; do nothing
    } else {
      // Touch release
      // Indicate old coordinate but with pressure=0
      m_nLastRawPress = 0;
      m_bLastTouched = false;
      bValid = true;
    }
  }

  // If an event was detected, signal it back to GUIslice
  if (bValid) {

    int nRawX,nRawY;
    int nInputX,nInputY;
    int nOutputX,nOutputY;

    // Input assignment
    nRawX = m_nLastRawX;
    nRawY = m_nLastRawY;

    // Handle any hardware swapping in native orientation
    // This is done prior to any flip/swap as a result of
    // rotation away from the native orientation.
    // In most cases, the following is not used, but there
    // may be touch modules that have swapped their X&Y convention.
    if (pGui->bTouchRemapYX) {
      nRawX = m_nLastRawY;
      nRawY = m_nLastRawX;
    }

    nInputX = nRawX;
    nInputY = nRawY;

    // For resistive displays, perform constraint and scaling
    #if defined(DRV_TOUCH_CALIB)
      if (pGui->bTouchRemapEn) {
        // Perform scaling from input to output
        // - Calibration done in native orientation (GSLC_ROTATE=0)
        // - Input to map() is done with raw unswapped X,Y
        // - map() and constrain() done with native dimensions and
        //   native calibration
        // - Swap & Flip done to output of map/constrain according
        //   to GSLC_ROTATE
        //
        #if defined(DBG_TOUCH)
          GSLC_DEBUG_PRINT("DBG: remapX: (%d,%d,%d,%d,%d)\n", nInputX, pGui->nTouchCalXMin, pGui->nTouchCalXMax, 0, nDispOutMaxX);
          GSLC_DEBUG_PRINT("DBG: remapY: (%d,%d,%d,%d,%d)\n", nInputY, pGui->nTouchCalYMin, pGui->nTouchCalYMax, 0, nDispOutMaxY);
        #endif
        nOutputX = map(nInputX, pGui->nTouchCalXMin, pGui->nTouchCalXMax, 0, nDispOutMaxX);
        nOutputY = map(nInputY, pGui->nTouchCalYMin, pGui->nTouchCalYMax, 0, nDispOutMaxY);
        // Perform constraining to OUTPUT boundaries
        // NOTE: TFT_eSPI's getTouch readings occasionally return spurious touch
        //       events. These are often at the limit of the calibrated X/Y
        //       space. Post constraint, this may appear as events with one
        //       coordinate clipped to 0, for example. Bodmer provides
        //       one additional check in that if the mapped coordinate
        //       exceeds the positive bounds of the display (eg. 320px)
        //       then he marks the touch event invalid. Doing this may
        //       block some of the spurious events but not all (it would be
        //       calibration dependent). If we do decide to add in a similar
        //       check here, then we would want to force (bValid=false) here
        //       and then only proceed after re-checking (bValid==true).
        nOutputX = constrain(nOutputX, 0, nDispOutMaxX);
        nOutputY = constrain(nOutputY, 0, nDispOutMaxY);
      } else {
        // No scaling from input to output
        nOutputX = nInputX;
        nOutputY = nInputY;
      }
    #else
      // No scaling from input to output
      nOutputX = nInputX;
      nOutputY = nInputY;
    #endif  // DRV_TOUCH_CALIB
  
    #ifdef DBG_TOUCH
    GSLC_DEBUG_PRINT("DBG: PreRotate: x=%u y=%u\n", nOutputX, nOutputY);
    GSLC_DEBUG_PRINT("DBG: RotateCfg: remap=%u nSwapXY=%u nFlipX=%u nFlipY=%u\n",
      pGui->bTouchRemapEn,pGui->nSwapXY,pGui->nFlipX,pGui->nFlipY);
    #endif // DBG_TOUCH

    // Perform remapping due to current orientation
    if (pGui->bTouchRemapEn) {
      // Perform any requested swapping of input axes
      if (pGui->nSwapXY) {
        int16_t nOutputXTmp = nOutputX;
        nOutputX = nOutputY;
        nOutputY = nOutputXTmp;
        // Perform any requested output axis flipping
        // TODO: Collapse these cases
        if (pGui->nFlipX) {
          nOutputX = nDispOutMaxY - nOutputX;
        }
        if (pGui->nFlipY) {
          nOutputY = nDispOutMaxX - nOutputY;
        }
      } else {
        // Perform any requested output axis flipping
        if (pGui->nFlipX) {
          nOutputX = nDispOutMaxX - nOutputX;
        }
        if (pGui->nFlipY) {
          nOutputY = nDispOutMaxY - nOutputY;
        }
      }
    }

    // Final assignment
    *pnX          = nOutputX;
    *pnY          = nOutputY;
    *pnPress      = m_nLastRawPress;
    *peInputEvent = GSLC_INPUT_TOUCH;
    *pnInputVal   = 0;

    // Print output for debug
    #ifdef DBG_TOUCH
    if (m_nLastRawPress > 0) {
    GSLC_DEBUG_PRINT("DBG: Touch Press=%u Raw[%d,%d] Out[%d,%d]\n",
        m_nLastRawPress,m_nLastRawX,m_nLastRawY,nOutputX,nOutputY);
    }
    #endif

    // Return with indication of new value
    return true;
  }

  // No new value
  return false;
}

#endif // DRV_TOUCH_IN_DISP

// ------------------------------------------------------------------------
// Touch Functions (via external touch driver)
// ------------------------------------------------------------------------


#if defined(DRV_TOUCH_ADA_SIMPLE)

  // --------------------------------------------------------------------------
  // Enable Adafruit_TouchScreen workarounds
  // --------------------------------------------------------------------------

  // NOTE: The Adafruit_TouchScreen is not natively compatible with certain
  //       devices (eg. ESP32) and also doesn't safeguard against pin-sharing
  //       conflicts. For these and some other issues, the following workarounds
  //       are optionally enabled.

  // Enable workaround for ambiguity in Adafruit_TouchScreen pressure readings
  // - See https://github.com/ImpulseAdventure/GUIslice/issues/96
  #define FIX_4WIRE_Z // Comment out to disable

  // Enable workaround for Adafruit_TouchScreen getPoint() altering
  // the pin state and not restoring it. Without working around this,
  // the touch handler may interfere with displays that share pins.
  #define FIX_4WIRE_PIN_STATE // Comment out to disable

  // For ESP32 devices, a workaround is required for the
  // Adafruit_TouchScreen since it makes an assumption that
  // the ADC resolution is 10-bit. This workaround enables the
  // Adafruit library to operate the same was as for AVR devices.
  #if defined(ESP32)
    #define FIX_4WIRE_ADC_10 // Comment out to disable
  #endif

  // Enable workaround for TFT_eSPI with 8-bit parallel TFTs that share
  // certain pins (eg. RDX, WRX) with the touch overlay. TFT_eSPI appears
  // to force the TFT chip select low (permanently) in ESP32_PARALLEL mode
  // which can cause unexpected TFT corruption during the analog read
  // process. This workaround wraps the touch reads with a TFT chip-select
  // disable.
  // - Note that this workaround assumes that TFT_CS has been defined
  //   in the TFT_eSPI library.
  // - TFT_eSPI does not officially support UNO-style parallel shields
  //   according to https://github.com/Bodmer/TFT_eSPI/issues/345
  // - This workaround does enable successful operation with some of
  //   these shields, but it is not guaranteed to work with all.
  // - By default, this workaround has been disabled
  //#define FIX_4WIRE_FORCE_CS // Comment out to disable

  // --------------------------------------------------------------------------

  // Disable certain workarounds for Adafruit_TouchScreen in STM32 mode
  // as we haven't implemented the equivalent pin save/restore code yet.
  #if defined(ARDUINO_ARCH_STM32) || defined(__STM32F1__)
    #undef FIX_4WIRE_PIN_STATE
    #undef FIX_4WIRE_FORCE_CS
  #endif
  
  // --------------------------------------------------------------------------

  #if defined(FIX_4WIRE_PIN_STATE)
  // NOTE: The Adafruit_TouchScreen library alters the state of several
  //       pins during the course of reading the touch coordinates and
  //       pressure. Unfortunately, it does not restore the prior state
  //       which can impact other processes such as graphics drivers which
  //       may share the same pins. The following routines are responsible
  //       for saving and restoring the pin state and will wrap the
  //       touch polling logic. If a future release of the Adafruit_TouchScreen
  //       library addresses this issue, this wrapper logic can be removed.
  //       For further reference, please refer to Issue #96.

  /// Structure used to retain a port state (mode and level)
  /// so that it can be restored later.
  struct gslc_tsPinState
  {
    int     nMode;     // OUTPUT, INPUT, INPUT_PULLUP
    bool    bIsHigh;   // Is an output and HIGH?
  };

  /// Return the current pinMode() for a pin
  int gslc_TDrvGetPinMode(uint8_t nPin)
  {
    if (nPin >= NUM_DIGITAL_PINS) {
      return (-1);
    }
#if defined(ESP32) || defined(ESP8266)
    uint32_t nBit            = digitalPinToBitMask(nPin);
    uint32_t nPort           = digitalPinToPort(nPin);
    volatile uint32_t *nReg  = portModeRegister(nPort);
#else
    uint8_t nBit            = digitalPinToBitMask(nPin);
    uint8_t nPort           = digitalPinToPort(nPin);
    volatile uint8_t *nReg  = portModeRegister(nPort);
#endif

    // Determine if port is an output
    if (*nReg & nBit) {
      return (OUTPUT);
    }

    // Determine if port is an input and whether pullup is active
#if defined(ESP32) || defined(ESP8266)
    volatile  uint32_t *nOut = portOutputRegister(nPort);
#else
    volatile  uint8_t *nOut = portOutputRegister(nPort);
#endif
    return ((*nOut & nBit) ? INPUT_PULLUP : INPUT);
  }

  /// Fetch the current pin mode and level
  inline void gslc_TDrvSavePinState(int nPin, gslc_tsPinState &sPinState)
  {
    sPinState.nMode = gslc_TDrvGetPinMode(nPin);
    sPinState.bIsHigh = digitalRead(nPin);
  }

  /// Restore the pin mode and level
  inline void gslc_TDrvRestorePinState(int nPin,gslc_tsPinState sPinState)
  {
    pinMode(nPin,sPinState.nMode);
    if (sPinState.nMode == OUTPUT) digitalWrite(nPin,sPinState.bIsHigh);
  }

  #endif // FIX_4WIRE_PIN_STATE

  // --------------------------------------------------------------------------

#endif // DRV_TOUCH_ADA_SIMPLE


#if defined(DRV_TOUCH_TYPE_EXTERNAL)

bool gslc_TDrvInitTouch(gslc_tsGui* pGui,const char* acDev) {

  // Capture default calibration settings for resistive displays
  #if defined(DRV_TOUCH_CALIB)
    pGui->nTouchCalXMin = ADATOUCH_X_MIN;
    pGui->nTouchCalXMax = ADATOUCH_X_MAX;
    pGui->nTouchCalYMin = ADATOUCH_Y_MIN;
    pGui->nTouchCalYMax = ADATOUCH_Y_MAX;
    #if defined(ADATOUCH_PRESS_MIN)
      pGui->nTouchCalPressMin = ADATOUCH_PRESS_MIN;
      pGui->nTouchCalPressMax = ADATOUCH_PRESS_MAX;
    #else
      // For backward compatibility, if these config settings
      // were not included in the config file, provide defaults.
      pGui->nTouchCalPressMin = 200;
      pGui->nTouchCalPressMax = 4000;
    #endif
  #endif // DRV_TOUCH_CALIB

  // Support touch controllers with swapped X & Y
  #if defined(ADATOUCH_REMAP_YX)
    // Capture swap setting from config file
    pGui->bTouchRemapYX = ADATOUCH_REMAP_YX;
  #else
    // For backward compatibility with older config files
    // that have not defined this config option
    pGui->bTouchRemapYX = false;
  #endif

  #if defined(DRV_TOUCH_ADA_STMPE610)
    #if (ADATOUCH_I2C_HW)
    if (!m_touch.begin(ADATOUCH_I2C_ADDR)) {
    #else
    if (!m_touch.begin()) {
    #endif
      GSLC_DEBUG2_PRINT("ERROR: TDrvInitTouch() failed to init STMPE610\n",0);
      return false;
    } else {
      return true;
    }
  #elif defined(DRV_TOUCH_ADA_FT6206)
    if (!m_touch.begin(ADATOUCH_SENSITIVITY)) {
      GSLC_DEBUG2_PRINT("ERROR: TDrvInitTouch() failed to init FT6206\n",0);
      return false;
    } else {
      return true;
    }
  #elif defined(DRV_TOUCH_ADA_SIMPLE)
    #if defined(ESP32)
      // ESP32 defaults to 12-bit resolution whereas Adafruit_Touchscreen
      // hardcodes a 10-bit range. Workaround for now is to change the
      // ADC resolution to 10-bit.
      // References:
      // - https://github.com/adafruit/Adafruit_TouchScreen/issues/15
      #if defined(FIX_4WIRE_ADC_10)
        analogReadResolution(10);
      #endif // FIX_4WIRE_ADC_10
    #endif
    return true;
  #elif defined(DRV_TOUCH_XPT2046_STM)
    m_touch.begin();
    return true;
  #elif defined(DRV_TOUCH_XPT2046_PS)
    m_touch.begin();
    // Since this XPT2046 library supports "touch rotation", and defaults
    // to landscape orientation, rotate to traditional portrait orientation
    // for consistency with other handlers.
    //
    // Unfortunately, this API (from 2018/01/04) is not available in the
    // latest tagged release of XPT2046 in the Library Manager. Therefore,
    // we can't use this API and instead need to hardcode the mapping
    // during the DrvGetTouch() function.
    //m_touch.setRotation(0);
    return true;
  #elif defined(DRV_TOUCH_INPUT)
    // Nothing more to initialize for GPIO input control mode
    return true;
  #elif defined(DRV_TOUCH_HANDLER)
    return true;
  #else
    // ERROR: Unsupported driver mode
    GSLC_DEBUG_PRINT("ERROR: TDrvInitTouch() driver not supported yet\n",0);
    return false;
  #endif

}

bool gslc_TDrvGetTouch(gslc_tsGui* pGui,int16_t* pnX,int16_t* pnY,uint16_t* pnPress,gslc_teInputRawEvent* peInputEvent,int16_t* pnInputVal)
{

  #if defined(DRV_TOUCH_NONE)
    return false;
  #endif

  // As the STMPE610 hardware driver doesn't appear to return
  // an indication of "touch released" with a coordinate, we
  // must detect the release transition here and send the last
  // known coordinate but with pressure=0. To do this, we are
  // allocating a static variable to maintain the last touch
  // coordinate.
  // TODO: This code can be reworked / simplified
  static int16_t  m_nLastRawX     = 0;
  static int16_t  m_nLastRawY     = 0;
  static uint16_t m_nLastRawPress = 0;
  static bool     m_bLastTouched  = false;

  bool bValid = false;  // Indicate a touch event to GUIslice core?

  // Define maximum bounds for display in native orientation
  int nDispOutMaxX,nDispOutMaxY;
  nDispOutMaxX = pGui->nDisp0W-1;
  nDispOutMaxY = pGui->nDisp0H-1;

  // ----------------------------------------------------------------
  #if defined(DRV_TOUCH_ADA_STMPE610)

  uint16_t  nRawX,nRawY;
  uint8_t   nRawPress;

  if (m_touch.touched()) {

    if (m_touch.bufferEmpty()) {
      // Nothing to do
    } else {
      while (!m_touch.bufferEmpty()) {
        // Continued press; update next reading
        // TODO: Is there a risk that the touch hardware could
        //       maintain a non-empty state for an extended period of time?
        m_touch.readData(&nRawX,&nRawY,&nRawPress);
        m_nLastRawX = nRawX;
        m_nLastRawY = nRawY;
        m_nLastRawPress = nRawPress;
        m_bLastTouched = true;
        bValid = true;
      }

    }
    // Clear interrupts
    m_touch.writeRegister8(STMPE_INT_STA, 0xFF);

  } else {
    if (!m_bLastTouched) {
      // Wasn't touched before; do nothing
    } else {
      // Touch release
      // Indicate old coordinate but with pressure=0
      m_nLastRawPress = 0;
      m_bLastTouched = false;
      bValid = true;
    }
    // Flush the FIFO
    while (!m_touch.bufferEmpty()) {
      m_touch.readData(&nRawX,&nRawY,&nRawPress);
    }
  }

  // ----------------------------------------------------------------
  #elif defined(DRV_TOUCH_ADA_FT6206)

  if (m_touch.touched()) {
    TS_Point ptTouch = m_touch.getPoint();
    // FT6206 coordinates appear to have flipped both axes vs other controllers
    // - Confirmed by comments in Adafruit_FT6206 library example code:
    //   "rotate coordinate system. flip it around to match the screen."
    m_nLastRawX = nDispOutMaxX-ptTouch.x;
    m_nLastRawY = nDispOutMaxY-ptTouch.y;
    m_nLastRawPress = 255;  // Select arbitrary non-zero value
    m_bLastTouched = true;
    bValid = true;

  } else {
    if (!m_bLastTouched) {
      // Wasn't touched before; do nothing
    } else {
      // Touch release
      // Indicate old coordinate but with pressure=0
      m_nLastRawPress = 0;
      m_bLastTouched = false;
      bValid = true;
    }
  }

  // ----------------------------------------------------------------
  #elif defined(DRV_TOUCH_ADA_SIMPLE)

  uint16_t  nRawX,nRawY;
  int16_t   nRawPress;

  #if defined(FIX_4WIRE_PIN_STATE)
    // Saved pin state
    gslc_tsPinState   sPinStateXP, sPinStateXM, sPinStateYP, sPinStateYM;

    // As Adafruit_TouchScreen polling will alter the pin state and some
    // of these pins may be shared with the display, we need to save and
    // then later restore the pin state.
    gslc_TDrvSavePinState(ADATOUCH_PIN_XP, sPinStateXP);
    gslc_TDrvSavePinState(ADATOUCH_PIN_XM, sPinStateXM);
    gslc_TDrvSavePinState(ADATOUCH_PIN_YP, sPinStateYP);
    gslc_TDrvSavePinState(ADATOUCH_PIN_YM, sPinStateYM);

    #if defined(FIX_4WIRE_FORCE_CS)
      // Preserve the TFT chip select and deassert it during touchscreen reads
      gslc_tsPinState sPinStateCS;
      gslc_TDrvSavePinState(TFT_CS, sPinStateCS);
      digitalWrite(TFT_CS, HIGH);
    #endif // FIX_4WIRE_FORCE_CS

  #endif // FIX_4WIRE_PIN_STATE
  
  // Perform the polling of touch coordinate & pressure
  TSPoint p = m_touch.getPoint();

  // Select reasonable touch pressure threshold range.
  // Note that the Adafruit_TouchScreen library appears to
  // return the following:
  // - 0:     If no touch (results from integer overflow, div/0)
  // - 0:     If touch active but filtered due to noise
  // - small: If touch active and hard
  // - large: If touch active and soft
  // Note that the "pressure" (z) value is inverted in interpretation
  if ((p.z > pGui->nTouchCalPressMin) && (p.z < pGui->nTouchCalPressMax)) {
    nRawX = p.x;
    nRawY = p.y;
    nRawPress = p.z;
    m_nLastRawX = nRawX;
    m_nLastRawY = nRawY;
    m_nLastRawPress = nRawPress;
    m_bLastTouched = true;
    bValid = true;
  } else {
    if (!m_bLastTouched) {
      // Wasn't touched before; do nothing
    } else {

      #if !defined(FIX_4WIRE_Z) // Original behavior without touch pressure workaround

      // Indicate old coordinate but with pressure=0
      m_nLastRawPress = 0;
      m_bLastTouched = false;
      bValid = true;
      #ifdef DBG_TOUCH
      GSLC_DEBUG_PRINT("DBG: Touch End  =%u Raw[%d,%d] *****\n",
          m_nLastRawPress,m_nLastRawX,m_nLastRawY);
      #endif

      #else // Apply touch pressure workaround

      // Unfortunately, the Adafruit_TouchScreen has a few issues that
      // make it hard to deal with reliably. The most difficult problem
      // involves the ambiguous return state from getTouch().
      // Without handling this in a special way, we might see spurious
      // touch-release events.
      //
      // Upon entering this clause, we can infer Adafruit_TouchScreen returned z=0
      // - This either means:
      //   a) Touch was released (z is 0 due to integer overflow, div/0)
      //   b) Touch still active but filtered due to noisy read
      //
      // Because of case (b) returning the same signature as case (a), we
      // need to take an additional step to differentiate the two cases
      // otherwise we might interpret spurious "touch release" events.
      //
      // In order to differentiate these cases, we can call the Adafruit
      // getPressure() API since it does not include the filtering for (b).
      // Therefore, if we see that the pressure is non-zero, and less than
      // the max pressure threshold, we can re-interpret our original reading
      // as (b), wherein we would still want to treat as a touch pressed event.

      // Read the touch pressure
      // Note that we will need to restore the pin status later
      // once we are done with our polling.
      uint16_t nPressCur = m_touch.pressure();

      if ((nPressCur > pGui->nTouchCalPressMin) && (nPressCur < pGui->nTouchCalPressMax)) {
        // The unfiltered result is that the display is still pressed
        // Therefore we are likely in case (b) and should return our
        // last saved result (with touch pressure still active)
        bValid = true;
        #ifdef DBG_TOUCH
        // Give indication that workaround applied: continue press
        GSLC_DEBUG_PRINT("DBG: Touch Cont =%u Raw[%d,%d]\n",
            m_nLastRawPress,m_nLastRawX,m_nLastRawY);
        #endif
      } else {
        // The unfiltered result is that the display is not pressed
        // Therefore we are likely in case (a) and should force
        // the touch pressure to be deactivated

        // Indicate old coordinate but with pressure=0
        m_nLastRawPress = 0;
        m_bLastTouched = false;
        bValid = true;
        #ifdef DBG_TOUCH
        GSLC_DEBUG_PRINT("DBG: Touch End  =%u Raw[%d,%d] *****\n",
            m_nLastRawPress,m_nLastRawX,m_nLastRawY);
        #endif
      } // nPressCur
      #endif // FIX_4WIRE_Z

      // TODO: Implement touch debouncing

    } // m_bLastTouched
  }

  #if defined(FIX_4WIRE_PIN_STATE)
    // Now that we have completed our polling into Adafruit_TouchScreen,
    // we need to restore the original pin state.
    gslc_TDrvRestorePinState(ADATOUCH_PIN_XP, sPinStateXP);
    gslc_TDrvRestorePinState(ADATOUCH_PIN_XM, sPinStateXM);
    gslc_TDrvRestorePinState(ADATOUCH_PIN_YP, sPinStateYP);
    gslc_TDrvRestorePinState(ADATOUCH_PIN_YM, sPinStateYM);

    #if defined(FIX_4WIRE_FORCE_CS)
      // Restore TFT chip select
      gslc_TDrvRestorePinState(TFT_CS, sPinStateCS);
    #endif // FIX_4WIRE_FORCE_CS

  #endif // FIX_4WIRE_PIN_STATE

  // ----------------------------------------------------------------
  #elif defined(DRV_TOUCH_XPT2046_STM)
    // NOTE: XPT2046_STM returns pressure (z) values with a reversed
    //       convention versus other touch libraries (ie. a small
    //       non-zero z value means light touch, whereas a large
    //       value means a hard / wide touch).

    uint16_t  nRawX,nRawY; //XPT2046 returns values up to 4095
    uint16_t  nRawPress;   //XPT2046 returns values up to 4095

    TS_Point p = m_touch.getPoint();

    if ((p.z > pGui->nTouchCalPressMin) && (p.z < pGui->nTouchCalPressMax)) {
      nRawX = p.x;
      nRawY = p.y;
      nRawPress = p.z;
      m_nLastRawX = nRawX;
      m_nLastRawY = nRawY;
      m_nLastRawPress = nRawPress;
      m_bLastTouched = true;
      bValid = true;
    }
    else {
      if (!m_bLastTouched) {
        // Wasn't touched before; do nothing
      }
      else {
        // Touch release
        // Indicate old coordinate but with pressure=0
        m_nLastRawPress = 0;
        m_bLastTouched = false;
        bValid = true;
      }
    }

  // ----------------------------------------------------------------
  #elif defined(DRV_TOUCH_XPT2046_PS)
    uint16_t  nRawX,nRawY; //XPT2046 returns values up to 4095
    uint16_t  nRawPress;   //XPT2046 returns values up to 4095

    TS_Point p = m_touch.getPoint();

    if ((p.z > pGui->nTouchCalPressMin) && (p.z < pGui->nTouchCalPressMax)) {
      // PaulStoffregen/XPT2046 appears to use a different orientation
      // than other libraries. Therefore, we will remap it here
      // to match the default portrait orientation.
      nRawX = 4095-p.y;
      nRawY = p.x;

      nRawPress = p.z;
      m_nLastRawX = nRawX;
      m_nLastRawY = nRawY;
      m_nLastRawPress = nRawPress;
      m_bLastTouched = true;
      bValid = true;
    }
    else {
      if (!m_bLastTouched) {
        // Wasn't touched before; do nothing
      }
      else {
        // Touch release
        // Indicate old coordinate but with pressure=0
        m_nLastRawPress = 0;
        m_bLastTouched = false;
        bValid = true;
      }
    }

  // ----------------------------------------------------------------
  #elif defined(DRV_TOUCH_HANDLER)

    uint16_t  nRawX,nRawY;
    uint16_t  nRawPress; 
  
    TouchHandler *pTH = gslc_getTouchHandler();
    THPoint p(0,0,0);
    //if no TouchHandler was defined use (0,0,0)
    if (pTH!=NULL)
        p = pTH->getPoint();
    
    if (p.z > 0) {
      nRawX=p.x;
      nRawY=p.y;
      nRawPress=p.z;
      m_nLastRawX = nRawX;
      m_nLastRawY = nRawY;
      m_nLastRawPress = nRawPress;
      m_bLastTouched = true;
      bValid = true;
      //Serial.print("pTH= ");Serial.print(p.x);Serial.print(" ");Serial.print(p.y);Serial.print(" ");Serial.println(p.z);
    }
    else {
      if (!m_bLastTouched) {
        // Wasn't touched before; do nothing
      } else {
        // Touch release
        // Indicate old coordinate but with pressure=0
        m_nLastRawPress = 0;
        m_bLastTouched = false;
        bValid = true;
      }
    }

  // ----------------------------------------------------------------
  #elif defined(DRV_TOUCH_INPUT)
    // No more to do for GPIO-only mode since gslc_Update() already
    // looks for GPIO inputs before calling TDrvGetTouch().
    // bValid will default to false

  // ----------------------------------------------------------------
  #endif // DRV_TOUCH_*


  // If an event was detected, signal it back to GUIslice
  if (bValid) {

    int nRawX,nRawY;
    int nInputX,nInputY;
    int nOutputX,nOutputY;

    // Input assignment
    nRawX = m_nLastRawX;
    nRawY = m_nLastRawY;

    // Handle any hardware swapping in native orientation
    // This is done prior to any flip/swap as a result of
    // rotation away from the native orientation.
    // In most cases, the following is not used, but there
    // may be touch modules that have swapped their X&Y convention.
    if (pGui->bTouchRemapYX) {
      nRawX = m_nLastRawY;
      nRawY = m_nLastRawX;
    }

    nInputX = nRawX;
    nInputY = nRawY;

    // For resistive displays, perform constraint and scaling
    #if defined(DRV_TOUCH_CALIB)
      if (pGui->bTouchRemapEn) {
        // Perform scaling from input to output
        // - Calibration done in native orientation (GSLC_ROTATE=0)
        // - Input to map() is done with raw unswapped X,Y
        // - map() and constrain() done with native dimensions and
        //   native calibration
        // - Swap & Flip done to output of map/constrain according
        //   to GSLC_ROTATE
        //
        #if defined(DBG_TOUCH)
          GSLC_DEBUG_PRINT("DBG: remapX: (%d,%d,%d,%d,%d)\n", nInputX, pGui->nTouchCalXMin, pGui->nTouchCalXMax, 0, nDispOutMaxX);
          GSLC_DEBUG_PRINT("DBG: remapY: (%d,%d,%d,%d,%d)\n", nInputY, pGui->nTouchCalYMin, pGui->nTouchCalYMax, 0, nDispOutMaxY);
        #endif
        nOutputX = map(nInputX, pGui->nTouchCalXMin, pGui->nTouchCalXMax, 0, nDispOutMaxX);
        nOutputY = map(nInputY, pGui->nTouchCalYMin, pGui->nTouchCalYMax, 0, nDispOutMaxY);
        // Perform constraining to OUTPUT boundaries
        nOutputX = constrain(nOutputX, 0, nDispOutMaxX);
        nOutputY = constrain(nOutputY, 0, nDispOutMaxY);
      } else {
        // No scaling from input to output
        nOutputX = nInputX;
        nOutputY = nInputY;
      }
    #else
      // No scaling from input to output
      nOutputX = nInputX;
      nOutputY = nInputY;
    #endif  // DRV_TOUCH_CALIB
  
    #ifdef DBG_TOUCH
    GSLC_DEBUG_PRINT("DBG: PreRotate: x=%u y=%u\n", nOutputX, nOutputY);
    #if defined(DRV_TOUCH_CALIB)
      GSLC_DEBUG_PRINT("DBG: RotateCfg: remap=%u nSwapXY=%u nFlipX=%u nFlipY=%u\n",
        pGui->bTouchRemapEn,pGui->nSwapXY,pGui->nFlipX,pGui->nFlipY);
    #endif // DRV_TOUCH_CALIB
    #endif // DBG_TOUCH

    // Perform remapping due to current orientation
    if (pGui->bTouchRemapEn) {
      // Perform any requested swapping of input axes
      if (pGui->nSwapXY) {
        int16_t nOutputXTmp = nOutputX;
        nOutputX = nOutputY;
        nOutputY = nOutputXTmp;
        // Perform any requested output axis flipping
        // TODO: Collapse these cases
        if (pGui->nFlipX) {
          nOutputX = nDispOutMaxY - nOutputX;
        }
        if (pGui->nFlipY) {
          nOutputY = nDispOutMaxX - nOutputY;
        }
      } else {
        // Perform any requested output axis flipping
        if (pGui->nFlipX) {
          nOutputX = nDispOutMaxX - nOutputX;
        }
        if (pGui->nFlipY) {
          nOutputY = nDispOutMaxY - nOutputY;
        }
      }
    }

    // Final assignment
    *pnX          = nOutputX;
    *pnY          = nOutputY;
    *pnPress      = m_nLastRawPress;
    *peInputEvent = GSLC_INPUT_TOUCH;
    *pnInputVal   = 0;

    // Print output for debug
    #ifdef DBG_TOUCH
    GSLC_DEBUG_PRINT("DBG: Touch Press=%u Raw[%d,%d] Out[%d,%d]\n",
        m_nLastRawPress,m_nLastRawX,m_nLastRawY,nOutputX,nOutputY);
    #endif

    // Return with indication of new value
    return true;
  }

  // No new value
  return false;
}

#endif // DRV_TOUCH_*


// -----------------------------------------------------------------------
// Dynamic Screen rotation and Touch axes swap/flip functions
// -----------------------------------------------------------------------

/// Change display rotation and any associated touch orientation
bool gslc_DrvRotate(gslc_tsGui* pGui, uint8_t nRotation)
{
  bool bChange = true;

  // Determine if the new orientation has swapped axes
  // versus the native orientation (0)
  bool bSwap = false;
  if ((nRotation == 1) || (nRotation == 3)) {
    bSwap = true;
  }
  (void)bSwap; // Unused

  // Did the orientation change?
  if (nRotation == pGui->nRotation) {
    // Orientation did not change -- indicate this by returning
    // false so that we can avoid a redraw
    bChange = false;
  }

  // Update the GUI rotation member
  pGui->nRotation = nRotation;

  // Inform the display to adjust the orientation and
  // update the saved display dimensions

  // DRV_DISP_TFT_ESPI
  // Capture display dimensions in native orientation
  m_disp.setRotation(0);
  pGui->nDisp0W = m_disp.width();
  pGui->nDisp0H = m_disp.height();
  // Capture display dimensions in selected orientation
  m_disp.setRotation(pGui->nRotation);
  pGui->nDispW = m_disp.width();
  pGui->nDispH = m_disp.height();

  // Update the clipping region
  gslc_tsRect rClipRect = {0,0,pGui->nDispW,pGui->nDispH};
  gslc_DrvSetClipRect(pGui,&rClipRect);

  // Now update the touch remapping
  #if !defined(DRV_TOUCH_NONE)
    // Correct touch mapping according to current rotation mode
    pGui->nSwapXY = TOUCH_ROTATION_SWAPXY(pGui->nRotation);
    pGui->nFlipX = TOUCH_ROTATION_FLIPX(pGui->nRotation);
    pGui->nFlipY = TOUCH_ROTATION_FLIPY(pGui->nRotation);
  #endif // !DRV_TOUCH_NONE

  // Mark the current page ask requiring redraw
  // if the rotation value changed
  if (bChange) {
    gslc_PageRedrawSet( pGui, true );
  }

  return true;
}


// =======================================================================
// Private Functions
// =======================================================================


// Convert from RGB struct to native screen format
// TODO: Use 32bit return type?
uint16_t gslc_DrvAdaptColorToRaw(gslc_tsColor nCol)
{
  uint16_t nColRaw = 0;

  #if defined(DRV_COLORMODE_MONO)
    // Monochrome
    if ((nCol.r == 0) && (nCol.g == 0) && (nCol.b == 0)) { // GSLC_COL_BLACK
      nColRaw = 0;  // BLACK
    } else {
      nColRaw = 1;  // WHITE
    }

  #elif defined(DRV_COLORMODE_BGR565)
    nColRaw |= (((nCol.b & 0xF8) >> 3) << 11); // Mask: 1111 1000 0000 0000
    nColRaw |= (((nCol.g & 0xFC) >> 2) <<  5); // Mask: 0000 0111 1110 0000
    nColRaw |= (((nCol.r & 0xF8) >> 3) <<  0); // Mask: 0000 0000 0001 1111

  #else
    // Default to DRV_COLORMODE_RGB565
    nColRaw |= (((nCol.r & 0xF8) >> 3) << 11); // Mask: 1111 1000 0000 0000
    nColRaw |= (((nCol.g & 0xFC) >> 2) <<  5); // Mask: 0000 0111 1110 0000
    nColRaw |= (((nCol.b & 0xF8) >> 3) <<  0); // Mask: 0000 0000 0001 1111

  #endif // DRV_COLORMODE_*

  return nColRaw;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // Compiler guard for requested driver
