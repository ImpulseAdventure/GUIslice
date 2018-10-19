// =======================================================================
// GUIslice library (driver layer for Adafruit-GFX)
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


// Compiler guard for requested driver
#include "GUIslice_config.h" // Sets DRV_DISP_*
#if defined(DRV_DISP_ADAGFX) || defined(DRV_DISP_ADAGFX_AS)

// =======================================================================
// Driver Layer for Adafruit-GFX
// =======================================================================

// GUIslice library
#include "GUIslice_drv_adagfx.h"

#include <stdio.h>

#if defined(DRV_DISP_ADAGFX)
  #include <Adafruit_GFX.h>
  #include <gfxfont.h>
#elif defined(DRV_DISP_ADAGFX_AS)
  #include <Adafruit_GFX_AS.h>
#endif

#if defined(DRV_DISP_ADAGFX_ILI9341)
  #include <Adafruit_ILI9341.h>
  #if (GSLC_SD_EN)
    #include <SD.h>   // Include support for SD card access
  #endif
  #include <SPI.h>
#elif defined(DRV_DISP_ADAGFX_ILI9341_8BIT)
  #include <Adafruit_TFTLCD.h>
  #if (GSLC_SD_EN)
    #include <SD.h>   // Include support for SD card access
  #endif
  #include <SPI.h>
#elif defined(DRV_DISP_ADAGFX_ILI9341_STM)
  #include <Adafruit_ILI9341_STM.h>
  #if (GSLC_SD_EN)
    #include <SD.h>   // Include support for SD card access
  #endif
  #include <SPI.h>
#elif defined(DRV_DISP_ADAGFX_SSD1306)
  #include <Adafruit_SSD1306.h>
  // TODO: Select either SPI or I2C. For now, assume SPI
  #include <SPI.h>
  #include <Wire.h>
#elif defined(DRV_DISP_ADAGFX_ST7735)
  #include <Adafruit_ST7735.h>
  #include <SPI.h>
#elif defined(DRV_DISP_ADAGFX_HX8357)
  #include <Adafruit_HX8357.h>
  // TODO: Select either SPI or I2C. For now, assume SPI
  #if (GSLC_SD_EN)
    #include <SD.h>   // Include support for SD card access
  #endif
  #include <SPI.h>
#elif defined(DRV_DISP_ADAGFX_PCD8544)
  #include <Adafruit_PCD8544.h>
  #include <SPI.h>
#endif

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
#elif defined(DRV_TOUCH_XPT2046)
  #include <XPT2046_touch.h>
#elif defined(DRV_TOUCH_HANDLER)
  #include <GUIslice_th.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



// ------------------------------------------------------------------------
#if defined(DRV_DISP_ADAGFX_ILI9341)
  #if (ADAGFX_SPI_HW) // Use hardware SPI or software SPI (with custom pins)
    Adafruit_ILI9341 m_disp = Adafruit_ILI9341(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_RST);
  #else
    Adafruit_ILI9341 m_disp = Adafruit_ILI9341(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_MOSI, ADAGFX_PIN_CLK, ADAGFX_PIN_RST, ADAGFX_PIN_MISO);
  #endif

// ------------------------------------------------------------------------
#elif defined(DRV_DISP_ADAGFX_ILI9341_8BIT)
  Adafruit_TFTLCD m_disp = Adafruit_TFTLCD (ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_WR, ADAGFX_PIN_RD, ADAGFX_PIN_RST);

// ------------------------------------------------------------------------
#elif defined(DRV_DISP_ADAGFX_ILI9341_STM)
  #if (ADAGFX_SPI_HW) // Use hardware SPI or software SPI (with custom pins)
    //PIN_RST=-1 doesn't give same behavior as 2-param function variant, therefore use different functions
    #if ADAGFX_PIN_RST==-1
      Adafruit_ILI9341_STM m_disp = Adafruit_ILI9341_STM(ADAGFX_PIN_CS, ADAGFX_PIN_DC);
    #else
      Adafruit_ILI9341_STM m_disp = Adafruit_ILI9341_STM(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_RST);
    #endif
  #else
    Adafruit_ILI9341_STM m_disp = Adafruit_ILI9341_STM(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_MOSI, ADAGFX_PIN_CLK, ADAGFX_PIN_RST, ADAGFX_PIN_MISO);
  #endif

// ------------------------------------------------------------------------
#elif defined(DRV_DISP_ADAGFX_SSD1306)
  #if (ADAGFX_SPI_HW) // Use hardware SPI or software SPI (with custom pins)
    Adafruit_SSD1306 m_disp(ADAGFX_PIN_DC, ADAGFX_PIN_RST, ADAGFX_PIN_CS);
  #else
    Adafruit_SSD1306 m_disp(ADAGFX_PIN_MOSI, ADAGFX_PIN_CLK, ADAGFX_PIN_DC, ADAGFX_PIN_RESET, ADAGFX_PIN_CS);
  #endif

// ------------------------------------------------------------------------
#elif defined(DRV_DISP_ADAGFX_ST7735)
  #if (ADAGFX_SPI_HW) // Use hardware SPI or software SPI (with custom pins)
    Adafruit_ST7735 m_disp(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_RST);
  #else
    Adafruit_ST7735 m_disp(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_MOSI, ADAGFX_PIN_CLK, ADAGFX_PIN_RESET);
  #endif

// ------------------------------------------------------------------------
#elif defined(DRV_DISP_ADAGFX_HX8357)
  #if (ADAGFX_SPI_HW) // Use hardware SPI or software SPI (with custom pins)
    Adafruit_HX8357 m_disp(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_RST);
  #else
    Adafruit_HX8357 m_disp(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_MOSI, ADAGFX_PIN_CLK, ADAGFX_PIN_RESET);
  #endif

// ------------------------------------------------------------------------
#elif defined(DRV_DISP_ADAGFX_PCD8544)
  #if (ADAGFX_SPI_HW) // Use hardware SPI or software SPI (with custom pins)
    Adafruit_PCD8544 m_disp(ADAGFX_PIN_DC, ADAGFX_PIN_CS, ADAGFX_PIN_RST);
  #else
    Adafruit_PCD8544 m_disp(ADAGFX_PIN_CLK, ADAGFX_PIN_MOSI, DAGFX_PIN_DC, ADAGFX_PIN_CS, ADAGFX_PIN_RST);
  #endif

// ------------------------------------------------------------------------
#endif // DRV_DISP_ADAGFX_*



// ------------------------------------------------------------------------
#if defined(DRV_TOUCH_ADA_STMPE610)
  #if (ADATOUCH_I2C_HW) // Use I2C
    Adafruit_STMPE610 m_touch = Adafruit_STMPE610();
  #elif (ADATOUCH_SPI_HW) // Use hardware SPI
    Adafruit_STMPE610 m_touch = Adafruit_STMPE610(ADATOUCH_PIN_CS);
  #elif (ADATOUCH_SPI_SW) // Use software SPI
    Adafruit_STMPE610 m_touch = Adafruit_STMPE610(ADATOUCH_PIN_CS, ADATOUCH_PIN_SDI, ADATOUCH_PIN_SDO, ADATOUCH_PIN_SCK);
  #endif
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_ADA_FT6206)
    // Always use I2C
    Adafruit_FT6206 m_touch = Adafruit_FT6206();
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_ADA_SIMPLE)
  #if defined(ADATOUCH_PIN_XP) && defined(ADATOUCH_PIN_YP) && defined(ADATOUCH_PIN_XM) && defined(ADATOUCH_PIN_YM) && defined(ADATOUCH_RX)
  TouchScreen m_touch = TouchScreen(ADATOUCH_PIN_XP, ADATOUCH_PIN_YP, ADATOUCH_PIN_XM, ADATOUCH_PIN_YM, ADATOUCH_RX);
  #else
  // Config fields not defined, so use default pinout
  TouchScreen m_touch = TouchScreen(45, A2, A3, 44, 300);
  #endif // defined(ADATOUCH_*)
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_XPT2046)
  // Create an SPI class for XPT2046 access
  XPT2046_DEFINE_DPICLASS;
  // Arduino built in XPT2046 touch driver (<XPT2046_touch.h>)
  XPT2046_touch m_touch(XPT2046_CS, XPT2046_spi); // Chip Select pin, SPI instance
#elif defined(DRV_TOUCH_HANDLER)

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

    pDriver->nColRawBkgnd = gslc_DrvAdaptColorToRaw(GSLC_COL_BLACK);

    // These displays can accept partial redraw as they retain the last
    // image in the controller graphics RAM
    pGui->bRedrawPartialEn = true;

    #if defined(DRV_DISP_ADAGFX_ILI9341) || defined(DRV_DISP_ADAGFX_ILI9341_STM)
      m_disp.begin();

      m_disp.readcommand8(ILI9341_RDMODE);
      m_disp.readcommand8(ILI9341_RDMADCTL);
      m_disp.readcommand8(ILI9341_RDPIXFMT);
      m_disp.readcommand8(ILI9341_RDIMGFMT);
      m_disp.readcommand8(ILI9341_RDSELFDIAG);

      // Rotate display from native portrait orientation to landscape
      // NOTE: The touch events in gslc_TDrvGetTouch() will also need rotation
      m_disp.setRotation( pGui->nRotation );
      if (pGui->nRotation == 0 || pGui->nRotation == 2) {
        pGui->nDispW = ILI9341_TFTWIDTH;
        pGui->nDispH = ILI9341_TFTHEIGHT;
      }
      else {
        pGui->nDispW = ILI9341_TFTHEIGHT;
        pGui->nDispH = ILI9341_TFTWIDTH;
      }

    #elif defined(DRV_DISP_ADAGFX_ILI9341_8BIT)
      uint16_t identifier = m_disp.readID();
      m_disp.begin(identifier);
      m_disp.setRotation( pGui->nRotation );
      if (pGui->nRotation == 0 || pGui->nRotation == 2) {
        pGui->nDispW = ILI9341_TFTWIDTH;
        pGui->nDispH = ILI9341_TFTHEIGHT;
      }
      else {
        pGui->nDispW = ILI9341_TFTHEIGHT;
        pGui->nDispH = ILI9341_TFTWIDTH;
      }

    #elif defined(DRV_DISP_ADAGFX_SSD1306)
      m_disp.begin(SSD1306_SWITCHCAPVCC);
      pGui->nDispW = SSD1306_LCDWIDTH;
      pGui->nDispH = SSD1306_LCDHEIGHT;

    #elif defined(DRV_DISP_ADAGFX_ST7735)
      m_disp.initR(INITR_144GREENTAB);  // 1.44"
      //m_disp.initR(INITR_BLACKTAB);     // 1.8" [TODO]
      pGui->nDispW = m_disp.width();
      pGui->nDispH = m_disp.height();

    #elif defined(DRV_DISP_ADAGFX_HX8357)
      m_disp.begin(HX8357D);
      // Rotate display from native portrait orientation to landscape
      // NOTE: The touch events in gslc_TDrvGetTouch() will also need rotation
      m_disp.setRotation( pGui->nRotation );
      pGui->nDispW = HX8357_TFTHEIGHT;
      pGui->nDispH = HX8357_TFTWIDTH;

    #elif defined(DRV_DISP_ADAGFX_PCD8544)
      m_disp.begin();
      //m_disp.setContrast(50); Set the contrast level

    #endif

    // Defaults for clipping region
    gslc_tsRect rClipRect = {0,0,pGui->nDispW,pGui->nDispH};
    gslc_DrvSetClipRect(pGui,&rClipRect);

    // Initialize SD card usage
    #if (GSLC_SD_EN)
    if (!SD.begin(ADAGFX_PIN_SDCS)) {
      GSLC_DEBUG_PRINT("ERROR: DrvInit() SD init failed\n",0);
      return false;
    }
    #endif

  }
  return true;
}


void gslc_DrvDestruct(gslc_tsGui* pGui)
{
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
    GSLC_DEBUG_PRINT("ERROR: DrvSetBkgndImage(%s) failed\n","");
    return false;
  }

  return true;
}


bool gslc_DrvSetBkgndColor(gslc_tsGui* pGui,gslc_tsColor nCol)
{
  if (pGui->pvDriver) {
    gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);
    pDriver->nColRawBkgnd = gslc_DrvAdaptColorToRaw(nCol);
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

  // TODO: For ILI9341, perhaps we can leverage m_disp.setAddrWindow(x0, y0, x1, y1)?
  return false;
}



// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------

const void* gslc_DrvFontAdd(gslc_teFontRefType eFontRefType,const void* pvFontRef,uint16_t nFontSz)
{
  // Arduino mode currently only supports font definitions from memory
  if (eFontRefType != GSLC_FONTREF_PTR) {
    GSLC_DEBUG_PRINT("ERROR: DrvFontAdd(%s) failed - Arduino only supports memory-based fonts\n","");
    return NULL;
  }
  // Return pointer to Adafruit-GFX GFXfont structure
  return pvFontRef;
}

void gslc_DrvFontsDestruct(gslc_tsGui* pGui)
{
  // Nothing to deallocate
}

bool gslc_DrvGetTxtSize(gslc_tsGui* pGui,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,
        int16_t* pnTxtX,int16_t* pnTxtY,uint16_t* pnTxtSzW,uint16_t* pnTxtSzH)
{
  uint16_t  nTxtLen   = 0;
  uint16_t  nTxtScale = pFont->nSize;

  m_disp.setFont((const GFXfont *)pFont->pvFont);
  m_disp.setTextSize(nTxtScale);

  if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_RAM) {
    // Fetch the text bounds
    m_disp.getTextBounds((char*)pStr,0,0,pnTxtX,pnTxtY,pnTxtSzW,pnTxtSzH);

  } else if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_PROG) {
#if (GSLC_USE_PROGMEM)
    nTxtLen = strlen_P(pStr);
    char tempStr[nTxtLen+1];
    strncpy_P(tempStr,pStr,nTxtLen);
    tempStr[nTxtLen] = '\0';  // Force termination

    // Fetch the text bounds
    m_disp.getTextBounds(tempStr,0,0,pnTxtX,pnTxtY,pnTxtSzW,pnTxtSzH);
#else
    // NOTE: Should not get here
    // - The text string has been marked as being stored in
    //   FLASH via PROGMEM (typically for Arduino) but
    //   the current device does not support the PROGMEM
    //   methodology.
    // - Degrade back to using SRAM directly

    // Fetch the text bounds
    m_disp.getTextBounds((char*)pStr,0,0,pnTxtX,pnTxtY,pnTxtSzW,pnTxtSzH);
#endif
  }

  m_disp.setFont();
  return true;
}

bool gslc_DrvDrawTxt(gslc_tsGui* pGui,int16_t nTxtX,int16_t nTxtY,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt)
{
  uint16_t  nTxtScale = pFont->nSize;
  uint16_t  nColRaw = gslc_DrvAdaptColorToRaw(colTxt);
  char      ch;

  // Initialize the font and positioning
  m_disp.setFont((const GFXfont *)pFont->pvFont);
  m_disp.setTextColor(nColRaw);
  m_disp.setCursor(nTxtX,nTxtY);
  m_disp.setTextSize(nTxtScale);

  // Default to accessing RAM directly (GSLC_TXT_MEM_RAM)
  bool bProg = false;
  if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_PROG) {
    bProg = true;
  }

  while (1) {
    // Fetch the next character
    if (!bProg) {
      // String in SRAM; can access buffer directly
      ch = *(pStr++);
    } else {
      // String in PROGMEM (flash); must access via pgm_* calls
      ch = pgm_read_byte(pStr++);
    }

    // Detect string terminator
    if (ch == 0) {
      break;
    }

    // Render the character
    m_disp.print(ch);

    // Handle multi-line text:
    // If we just output a newline, Adafruit-GFX will automatically advance
    // the Y cursor but reset the X cursor to 0. Therefore we need to
    // readjust the X cursor to our aligned bounding box.
    if (ch == '\n') {
      int16_t nCurPosY = m_disp.getCursorY();
      m_disp.setCursor(nTxtX,nCurPosY);
    }

  } // while(1)

  // Restore the font
  m_disp.setFont();

  return true;
}

// -----------------------------------------------------------------------
// Screen Management Functions
// -----------------------------------------------------------------------

void gslc_DrvPageFlipNow(gslc_tsGui* pGui)
{
  #if defined(DRV_DISP_ADAGFX_ILI9341) || defined(DRV_DISP_ADAGFX_ILI9341_8BIT) || defined(DRV_DISP_ADAGFX_ILI9341_STM) || defined(DRV_DISP_ADAGFX_ST7735) || defined(DRV_DISP_ADAGFX_HX8357)
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


bool gslc_DrvDrawPoint(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // Perform clipping
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  if (!gslc_ClipPt(&pDriver->rClipRect,nX,nY)) { return true; }
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.drawPixel(nX,nY,nColRaw);
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

bool gslc_DrvDrawFrameRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // Perform clipping
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  if (!gslc_ClipRect(&pDriver->rClipRect,&rRect)) { return true; }
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.drawRect(rRect.x,rRect.y,rRect.w,rRect.h,nColRaw);
  return true;
}


bool gslc_DrvDrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);
  if (!gslc_ClipLine(&pDriver->rClipRect,&nX0,&nY0,&nX1,&nY1)) { return true; }
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.drawLine(nX0,nY0,nX1,nY1,nColRaw);
  return true;
}

bool gslc_DrvDrawFrameCircle(gslc_tsGui*,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // TODO
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.drawCircle(nMidX,nMidY,nRadius,nColRaw);
  return true;
}

bool gslc_DrvDrawFillCircle(gslc_tsGui*,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // TODO
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.fillCircle(nMidX,nMidY,nRadius,nColRaw);
  return true;
}


bool gslc_DrvDrawFrameTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // TODO
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.drawTriangle(nX0,nY0,nX1,nY1,nX2,nY2,nColRaw);
  return true;
}

bool gslc_DrvDrawFillTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol)
{
#if (GSLC_CLIP_EN)
  // TODO
#endif

  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.fillTriangle(nX0,nY0,nX1,nY1,nX2,nY2,nColRaw);
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
  uint8_t nByte=0;

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
uint16_t gslc_DrvRead16SD(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t gslc_DrvRead32SD(File f) {
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

  if((x >= pGui->nDispW) || (y >= pGui->nDispH)) return;

  //Serial.println();
  //Serial.print("Loading image '");
  //Serial.print(filename);
  //Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == 0) {
    GSLC_DEBUG_PRINT("ERROR: DrvDrawBmp24FromSD() file not found [%s]",filename);
    return;
  }
  // Parse BMP header
  if(gslc_DrvRead16SD(bmpFile) == 0x4D42) { // BMP signature
    uint32_t nFileSize = gslc_DrvRead32SD(bmpFile);
    //Serial.print("File size: "); Serial.println(nFileSize);
    (void)gslc_DrvRead32SD(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = gslc_DrvRead32SD(bmpFile); // Start of image data
    //Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    uint32_t nHdrSize = gslc_DrvRead32SD(bmpFile);
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
            gslc_tsColor nColTrans = (gslc_tsColor){GSLC_BMP_TRANS_RGB};
            bool bDrawBit = true;
            if (GSLC_BMP_TRANS_EN) {
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
    GSLC_DEBUG_PRINT("ERROR: DrvDrawBmp24FromSD() BMP format unknown [%s]",filename);
  }
}
// ----- REFERENCE CODE end
#endif // GSLC_SD_EN


bool gslc_DrvDrawImage(gslc_tsGui* pGui,int16_t nDstX,int16_t nDstY,gslc_tsImgRef sImgRef)
{
  // GUIslice adapter library for Adafruit-GFX does not pre-load
  // image data into memory before calling DrvDrawImage(), so
  // we to handle the loading now (when rendering).
  if (sImgRef.eImgFlags == GSLC_IMGREF_NONE) {
    return true;  // Nothing to do

  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_FILE) {
    return false; // Not supported

  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_RAM) {
    if ((sImgRef.eImgFlags & GSLC_IMGREF_FMT) == GSLC_IMGREF_FMT_RAW1) {
      // Draw a monochrome bitmap from SRAM
      // - Dimensions and output color are defined in arrray header
      gslc_DrvDrawMonoFromMem(pGui,nDstX,nDstY,sImgRef.pImgBuf,false);
      return true;
    } else {
      return false; // TODO: not yet supported
    }

  } else if ((sImgRef.eImgFlags & GSLC_IMGREF_SRC) == GSLC_IMGREF_SRC_PROG) {
    // TODO: Probably need to fix this to work with PROGMEM,
    //       but check (GSLC_USE_PROGMEM) first
    if ((sImgRef.eImgFlags & GSLC_IMGREF_FMT) == GSLC_IMGREF_FMT_RAW1) {
      // Draw a monochrome bitmap from program memory
      // - Dimensions and output color are defined in arrray header
      gslc_DrvDrawMonoFromMem(pGui,nDstX,nDstY,sImgRef.pImgBuf,true);
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
      return false;
    #endif

  } else {
    // Unsupported source
    return false;
  }
}


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
      uint16_t nColRaw = pDriver->nColRawBkgnd;
      m_disp.fillScreen(nColRaw);
	} else {
      // An image should be loaded
      // TODO: For now, re-use the DrvDrawImage(). Later, consider
      //       extending to support different background drawing
      //       capabilities such as stretching and tiling of background
      //       image.
      gslc_DrvDrawImage(pGui,0,0,pGui->sImgRefBkgnd);
    }
  }
}


// -----------------------------------------------------------------------
// Touch Functions (via display driver)
// -----------------------------------------------------------------------


bool gslc_DrvInitTouch(gslc_tsGui* pGui,const char* acDev) {
  // TODO
  return true;
}


bool gslc_DrvGetTouch(gslc_tsGui* pGui,int16_t* pnX, int16_t* pnY, uint16_t* pnPress)
{
  // TODO
  return false;
}

// ------------------------------------------------------------------------
// Touch Functions (via external touch driver)
// ------------------------------------------------------------------------

#if defined(DRV_TOUCH_ADA_STMPE610) || defined(DRV_TOUCH_ADA_FT6206) || defined(DRV_TOUCH_ADA_SIMPLE) || defined(DRV_TOUCH_XPT2046) || defined(DRV_TOUCH_HANDLER)

bool gslc_TDrvInitTouch(gslc_tsGui* pGui,const char* acDev) {
  #if defined(DRV_TOUCH_ADA_STMPE610)
    if (!m_touch.begin(ADATOUCH_I2C_ADDR)) {
      GSLC_DEBUG_PRINT("ERROR: TDrvInitTouch() failed to init STMPE610\n",0);
      return false;
    } else {
      return true;
    }
  #elif defined(DRV_TOUCH_ADA_FT6206)
    if (!m_touch.begin(ADATOUCH_SENSITIVITY)) {
      GSLC_DEBUG_PRINT("ERROR: TDrvInitTouch() failed to init FT6206\n",0);
      return false;
    } else {
      return true;
    }
  #elif defined(DRV_TOUCH_ADA_SIMPLE)
    return true;
  #elif defined(DRV_TOUCH_XPT2046)
    m_touch.begin();
    return true;
  #elif defined(DRV_TOUCH_HANDLER)
    return true;
  #else
    // ERROR: Unsupported driver mode
    GSLC_DEBUG_PRINT("ERROR: TDrvInitTouch() driver not supported yet\n",0);
    return false;
  #endif

}


bool gslc_TDrvGetTouch(gslc_tsGui* pGui,int16_t* pnX, int16_t* pnY, uint16_t* pnPress)
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
    m_nLastRawX = ptTouch.x;
    m_nLastRawY = ptTouch.y;
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
  uint8_t   nRawPress;

  TSPoint p = m_touch.getPoint();

  pinMode(ADATOUCH_PIN_XM, OUTPUT);
  pinMode(ADATOUCH_PIN_YP, OUTPUT);

  // Select reasonable touch pressure thresholds
  // Note that the minimum is not "> 0" as some
  // displays may produce a (small) non-zero value
  // when not touched.
  #if defined(ADATOUCH_PRESS_MIN) && defined(ADATOUCH_PRESS_MAX)
  if ((p.z > ADATOUCH_PRESS_MIN) && (p.z < ADATOUCH_PRESS_MAX)) {
  #else
  if ((p.z > 10) && (p.z < 1000)) {
  #endif

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
      // Touch release
      // Indicate old coordinate but with pressure=0
      m_nLastRawPress = 0;
      m_bLastTouched = false;
      bValid = true;
    }
  }

  // ----------------------------------------------------------------
  #elif defined(DRV_TOUCH_XPT2046)

    uint16_t  nRawX,nRawY; //XPT2046 returns values up to 4095
    uint16_t  nRawPress;   //XPT2046 returns values up to 4095

    TS_Point p = m_touch.getPoint();

    #if defined(ADATOUCH_PRESS_MIN)
    if (p.z > ADATOUCH_PRESS_MIN) {
    #else
    if (p.z > 0) {
    #endif
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
  #endif // DRV_TOUCH_*


  // If an event was detected, signal it back to GUIslice
  if (bValid) {

    int nRawX,nRawY;
    int nInputX,nInputY;
    int nDispOutMaxX,nDispOutMaxY;
    int nOutputX,nOutputY;

    // Input assignment
    nRawX = m_nLastRawX;
    nRawY = m_nLastRawY;

    // Perform any requested swapping of input axes
    if( pGui->nSwapXY ) {
      nInputX = nRawY;
      nInputY = nRawX;
    } else {
      nInputX = nRawX;
      nInputY = nRawY;
    }

    // Define maximum bounds for display
    nDispOutMaxX = pGui->nDispW-1;
    nDispOutMaxY = pGui->nDispH-1;


    // For resistive displays, perform constraint and scaling
    #if defined(DRV_TOUCH_ADA_STMPE610) || defined(DRV_TOUCH_ADA_SIMPLE) || defined(DRV_TOUCH_XPT2046)
      // Perform constraining to input boundaries
      nInputX = constrain(nInputX,ADATOUCH_X_MIN,ADATOUCH_X_MAX);
      nInputY = constrain(nInputY,ADATOUCH_Y_MIN,ADATOUCH_Y_MAX);
      // Perform scaling from input to output
      nOutputX = map(nInputX,ADATOUCH_X_MIN,ADATOUCH_X_MAX,0,nDispOutMaxX);
      nOutputY = map(nInputY,ADATOUCH_Y_MIN,ADATOUCH_Y_MAX,0,nDispOutMaxY);
    #else
      // No scaling from input to output
      nOutputX = nInputX;
      nOutputY = nInputY;
    #endif

    // Perform any requested output axis flipping
    if( pGui->nFlipX ) {
      nOutputX = nDispOutMaxX - nOutputX;
    }
    if( pGui->nFlipY ) {
      nOutputY = nDispOutMaxY - nOutputY;
    }

    // Final assignment
    *pnX      = nOutputX;
    *pnY      = nOutputY;
    *pnPress  = m_nLastRawPress;

    // Print output for debug
    #ifdef DBG_TOUCH
    GSLC_DEBUG_PRINT("DBG: Touch Press=%u Raw[%d,%d] Out[%d,%d]\n",
        m_nLastRawPress,m_nLastRawX,m_nLastRawY,nOutputX,nOutputY);
    #endif

    // Debug output
    //Serial.print("p: ");Serial.print(nOutputX);Serial.print(",");Serial.print(nOutputY);Serial.print(",");Serial.println(m_nLastRawPress);
    //Serial.print("nDispOutMaxX: ");Serial.println(nDispOutMaxX);
    //Serial.print("nDispOutMaxY: ");Serial.println(nDispOutMaxY);

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

///
/// Change rotation and axes swap/flip
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nRotation:   Screen Rotation value (0, 1, 2 or 3)
/// \param[in]  nSwapXY:     Touchscreen Swap X/Y axes
/// \param[in]  nFlipX:      Touchscreen Flip X axis
/// \param[in]  nFlipY:      Touchscreen Flip Y axis
///
/// \return true if successful
///
bool gslc_DrvRotateSwapFlip(gslc_tsGui* pGui, uint8_t nRotation, uint8_t nSwapXY, uint8_t nFlipX, uint8_t nFlipY )
{
    bool redraw     = pGui->nRotation != nRotation;
    pGui->nRotation = nRotation;
    pGui->nSwapXY   = nSwapXY;
    pGui->nFlipX    = nFlipX;
    pGui->nFlipY    = nFlipY;

    m_disp.setRotation( pGui->nRotation );

    // Redraw the current page if rotation value changed
    if( redraw ) {
        gslc_PageRedrawSet( pGui, true );
        gslc_PageRedrawGo( pGui );
    }

    return( true );
}


// =======================================================================
// Private Functions
// =======================================================================


// Convert from RGB struct to native screen format
// TODO: Use 32bit return type?
uint16_t gslc_DrvAdaptColorToRaw(gslc_tsColor nCol)
{
  uint16_t nColRaw = 0;

  #if defined(DRV_DISP_ADAGFX_ILI9341) || defined(DRV_DISP_ADAGFX_ILI9341_8BIT) || defined(DRV_DISP_ADAGFX_ILI9341_STM) || defined(DRV_DISP_ADAGFX_ST7735) || defined(DRV_DISP_ADAGFX_HX8357)
    // RGB565
    nColRaw |= (((nCol.r & 0xF8) >> 3) << 11); // Mask: 1111 1000 0000 0000
    nColRaw |= (((nCol.g & 0xFC) >> 2) <<  5); // Mask: 0000 0111 1110 0000
    nColRaw |= (((nCol.b & 0xF8) >> 3) <<  0); // Mask: 0000 0000 0001 1111

  #elif defined(DRV_DISP_ADAGFX_SSD1306)
    if ((nCol.r == 0) && (nCol.g == 0) && (nCol.b == 0)) { // GSLC_COL_BLACK
      nColRaw = 0;  // BLACK
    } else {
      nColRaw = 1;  // WHITE
    }

  #endif // DRV_DISP_ADAGFX_*

  return nColRaw;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // Compiler guard for requested driver
