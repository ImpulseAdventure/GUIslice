// =======================================================================
// GUIslice library (driver layer for UTFT)
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
/// \file GUIslice_drv_utft.cpp


// Compiler guard for requested driver
#include "GUIslice_config.h" // Sets DRV_DISP_*
#if defined(DRV_DISP_UTFT)

// =======================================================================
// Driver Layer for UTFT
// =======================================================================

// GUIslice library
#include "GUIslice_drv_utft.h"

#include <stdio.h>

// ------------------------------------------------------------------------
// Load display drivers
// ------------------------------------------------------------------------
#if defined(DRV_DISP_UTFT)
  #include <UTFT.h>
#else
  #error "CONFIG: Need to enable a supported DRV_DISP_* option in GUIslice config"
#endif


// ------------------------------------------------------------------------
// Load touch drivers
// ------------------------------------------------------------------------
#if defined(DRV_TOUCH_URTOUCH)
  #if defined(DRV_TOUCH_URTOUCH_OLD)
    #include <UTouch.h> // Select old version of URTouch
  #else
    #include <URTouch.h>
  #endif
#endif

// ------------------------------------------------------------------------


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



// ------------------------------------------------------------------------
#if defined(DRV_DISP_UTFT)
  const char* m_acDrvDisp = "UTFT";
  UTFT m_disp(DRV_DISP_UTFT_INIT);

// ------------------------------------------------------------------------
#endif // DRV_DISP_*

// ------------------------------------------------------------------------
#if defined(DRV_TOUCH_URTOUCH)
  #if defined(DRV_TOUCH_URTOUCH_OLD)
    const char* m_acDrvTouch = "URTOUCH_OLD";
    UTouch m_touch(DRV_TOUCH_URTOUCH_INIT);
  #else
    const char* m_acDrvTouch = "URTOUCH";
    URTouch m_touch(DRV_TOUCH_URTOUCH_INIT);
  #endif
  #define DRV_TOUCH_INSTANCE
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_INPUT)
  const char* m_acDrvTouch = "INPUT";
// ------------------------------------------------------------------------
#elif defined(DRV_TOUCH_NONE)
  const char* m_acDrvTouch = "NONE";
// ------------------------------------------------------------------------
#endif // DRV_TOUCH_*

// -----------------------------------------------------------------------
// Font Definitions
// -----------------------------------------------------------------------
  extern uint8_t SmallFont[];
  extern uint8_t BigFont[];

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

    // These displays can accept partial redraw as they retain the last
    // image in the controller graphics RAM
    pGui->bRedrawPartialEn = true;

    // Support any additional initialization prior to display init

    // Perform any display initialization
    #if defined(DRV_DISP_UTFT)
      m_disp.InitLCD();
      m_disp.clrScr();

    #endif

    // Now that we have initialized the display, we can assign
    // the rotation parameters and clipping region
    gslc_DrvRotate(pGui,GSLC_ROTATE);


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

  // TODO: For ILI9341, perhaps we can leverage m_disp.setAddrWindow(x0, y0, x1, y1)?
  return true;
}



// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------

const void* gslc_DrvFontAdd(gslc_teFontRefType eFontRefType, const void* pvFontRef, uint16_t nFontSz)
{
  // Arduino mode currently only supports font definitions from memory
  if (eFontRefType != GSLC_FONTREF_PTR) {
    GSLC_DEBUG2_PRINT("ERROR: DrvFontAdd(%s) failed - Arduino only supports memory-based fonts\n", "");
    return NULL;
  }
  // For UTFT, we will force "SmallFont" to be the "default" font
  // - The Adafruit-GFX convention for default/built-in font is to pass NULL font pointer
  //   along with a size specifier
  if (pvFontRef == NULL) {
    //GSLC_DEBUG2_PRINT("DBG: DrvFontAdd() forcing SmallFont\n", "");
    pvFontRef = SmallFont;
  }
  return pvFontRef;
}

void gslc_DrvFontsDestruct(gslc_tsGui* pGui)
{
  // Nothing to deallocate
}

bool gslc_DrvGetTxtSize(gslc_tsGui* pGui,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,
        int16_t* pnTxtX,int16_t* pnTxtY,uint16_t* pnTxtSzW,uint16_t* pnTxtSzH)
{
  //uint16_t  nTxtScale = 0;
  m_disp.setFont((uint8_t*)pFont->pvFont);
  uint16_t nTxtLen = 0;

  // Get length
  if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_RAM) {
    // Fetch the text bounds
    nTxtLen = strlen((char*)pStr);
  } else if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_PROG) {
#if (GSLC_USE_PROGMEM)
    nTxtLen = strlen_P(pStr);
#else
    // NOTE: Should not get here
    // - The text string has been marked as being stored in
    //   FLASH via PROGMEM (typically for Arduino) but
    //   the current device does not support the PROGMEM
    //   methodology.
    // - Degrade back to using SRAM directly

    // Fetch the text bounds
    nTxtLen = strlen((char*)pStr);
#endif
  }

  // Estimate the length of the monospaced font
  *pnTxtSzW = nTxtLen * m_disp.getFontXsize();
  *pnTxtSzH = 1 * m_disp.getFontYsize(); // TODO: Handle multi-line

  // No baseline info available
  *pnTxtX = 0;
  *pnTxtY = 0;

  // TODO m_disp.setFont();
  return true;

}

bool gslc_DrvDrawTxt(gslc_tsGui* pGui,int16_t nTxtX,int16_t nTxtY,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt, gslc_tsColor colBg=GSLC_COL_BLACK)
{
  //uint16_t  nTxtScale = pFont->nSize;
  uint16_t  nColRaw = gslc_DrvAdaptColorToRaw(colTxt);
  char      ch;
  int16_t   nTxtXStart;

  // Initialize the font and positioning
  m_disp.setFont((uint8_t*)pFont->pvFont);
  m_disp.setColor(nColRaw);
  // Default to transparent text rendering
  m_disp.setBackColor(VGA_TRANSPARENT);

  // TODO m_disp.setCursor(nTxtX,nTxtY);
  // TODO m_disp.setTextSize(nTxtScale);

  // Driver-specific overrides

  // Default to accessing RAM directly (GSLC_TXT_MEM_RAM)
  bool bProg = false;
  if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_PROG) {
    bProg = true;
  }

  // Save the original starting X coordinate for line wraps
  nTxtXStart = nTxtX;

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
    // Call UTFT for rendering
    // Note that UTFT:printChar() is public but not documented
    m_disp.printChar(ch,nTxtX,nTxtY);
    // Advance the current position
    nTxtX += m_disp.getFontXsize();

    // Handle multi-line text:
    // If we just output a newline, Adafruit-GFX will automatically advance
    // the Y cursor but reset the X cursor to 0. Therefore we need to
    // readjust the X cursor to our aligned bounding box.
    if (ch == '\n') {
      nTxtX = nTxtXStart;
      nTxtY += m_disp.getFontYsize();
    }

  } // while(1)

  // Restore the font
  // TODO m_disp.setFont();

  return true;
}

// -----------------------------------------------------------------------
// Screen Management Functions
// -----------------------------------------------------------------------

void gslc_DrvPageFlipNow(gslc_tsGui* pGui)
{
  #if defined(DRV_DISP_ADAGFX_SSD1306)
    // Show the display buffer on the hardware.
    // NOTE: You _must_ call display after making any drawing commands
    // to make them visible on the display hardware!
    m_disp.display();
    // TODO: Might need to call m_disp.clearDisplay() now?

  #else
    // Nothing to do as we're not double-buffered

  #endif
}


// -----------------------------------------------------------------------
// Graphics Primitives Functions
// -----------------------------------------------------------------------

inline void gslc_DrvDrawPoint_base(int16_t nX, int16_t nY, uint16_t nColRaw)
{
  m_disp.setColor(nColRaw);
  m_disp.drawPixel(nX, nY);
}

inline void gslc_DrvDrawLine_base(int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,uint16_t nColRaw)
{
  m_disp.setColor(nColRaw);
  m_disp.drawLine(nX0,nY0,nX1,nY1);
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
  m_disp.setColor(nColRaw);
  m_disp.fillRect(rRect.x, rRect.y, rRect.x + rRect.w - 1, rRect.y + rRect.h - 1);
  return true;
}

bool gslc_DrvDrawFillRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol)
{
  // TODO: Support GSLC_CLIP_EN
  // - Would need to determine how to clip the rounded corners
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.setColor(nColRaw);
  // TODO: Handle radius?
  m_disp.fillRoundRect(rRect.x, rRect.y, rRect.x + rRect.w - 1, rRect.y + rRect.h - 1);
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
  m_disp.setColor(nColRaw);
  m_disp.drawRect(rRect.x,rRect.y,rRect.x+rRect.w-1,rRect.y+rRect.h-1);
#endif
  return true;
}

bool gslc_DrvDrawFrameRoundRect(gslc_tsGui* pGui,gslc_tsRect rRect,int16_t nRadius,gslc_tsColor nCol)
{
#if (DRV_HAS_DRAW_RECT_ROUND_FRAME)
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);

  // TODO: Support GSLC_CLIP_EN
  // - Would need to determine how to clip the rounded corners
  m_disp.setColor(nColRaw);
  // TODO: Handle radius?
  m_disp.drawRoundRect(rRect.x,rRect.y,rRect.x+rRect.w-1,rRect.y+rRect.h-1);
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
  m_disp.setColor(nColRaw);
  m_disp.drawCircle(nMidX, nMidY, nRadius);
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
  m_disp.setColor(nColRaw);
  m_disp.fillCircle(nMidX, nMidY, nRadius);
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
  // AdaFruit GFX doesn't have a routine for this so we output pixel by pixel
  const uint16_t* pImage = (const uint16_t*)pBitmap;
  int16_t h, w;
  if (bProgMem) {
    h = pgm_read_word(pImage++);
    w = pgm_read_word(pImage++);
  } else {
    h = *(pImage++);
    w = *(pImage++);
  }
  #if defined(DBG_DRIVER)
  GSLC_DEBUG_PRINT("DBG: DrvDrawBmp24FromMem() w=%d h=%d\n", w, h);
  #endif
  int row, col;
  for (row=0; row<h; row++) { // For each scanline...
    for (col=0; col<w; col++) { // For each pixel...
      if (bProgMem) {
        //To read from Flash Memory, pgm_read_XXX is required.
        //Since image is stored as uint16_t, pgm_read_word is used as it uses 16bit address
        gslc_DrvDrawPoint_base(nDstX+col, nDstY+row, pgm_read_word(pImage++));
      } else {
        gslc_DrvDrawPoint_base(nDstX+col, nDstY+row, *(pImage++));
      }
    } // end pixel
  }
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
uint16_t gslc_DrvRead16SD(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t gslc_DrvRead32SD(File &f) {
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
    return false; // Not supported

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
#if (GSLC_USE_PROGMEM)
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
#endif
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
    GSLC_DEBUG2_PRINT("DBG: DrvDrawImage() unsupported source eImgFlags=%d\n", sImgRef.eImgFlags);
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
      gslc_DrvDrawImage(pGui,0,0,pGui->sImgRefBkgnd);
    }
  }
}


// -----------------------------------------------------------------------
// Touch Functions (via display driver)
// -----------------------------------------------------------------------


bool gslc_DrvInitTouch(gslc_tsGui* pGui,const char* acDev) {
  if (pGui == NULL) {
    GSLC_DEBUG2_PRINT("ERROR: DrvInitTouch(%s) called with NULL ptr\n","");
    return false;
  }
  // TODO
  // Perform any driver-specific touchscreen init here
  return true;
}

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

bool gslc_DrvGetTouch(gslc_tsGui* pGui,int16_t* pnX,int16_t* pnY,uint16_t* pnPress,gslc_teInputRawEvent* peInputEvent,int16_t* pnInputVal)
{
  // TODO
  return false;
}

// ------------------------------------------------------------------------
// Touch Functions (via external touch driver)
// ------------------------------------------------------------------------



#if defined(DRV_TOUCH_TYPE_EXTERNAL)

bool gslc_TDrvInitTouch(gslc_tsGui* pGui,const char* acDev) {

  // Capture default calibration settings for resistive displays
  #if defined(DRV_TOUCH_CALIB)
    pGui->nTouchCalXMin = ADATOUCH_X_MIN;
    pGui->nTouchCalXMax = ADATOUCH_X_MAX;
    pGui->nTouchCalYMin = ADATOUCH_Y_MIN;
    pGui->nTouchCalYMax = ADATOUCH_Y_MAX;
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

  #if defined(DRV_TOUCH_URTOUCH)
    m_touch.InitTouch();
    m_touch.setPrecision(PREC_MEDIUM);
    // Disable touch remapping since URTouch handles it
    gslc_SetTouchRemapEn(pGui, false);
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
  #if defined(DRV_TOUCH_URTOUCH)

    // Note that we rely on URTouch's calibration
    // - This is detected by URTouch / URTouch_Calibration
    // - The calibration settings are stored in URTouch/URTouchCD.h

    int16_t   nRawX,nRawY;
    uint16_t  nRawPress = 0;
    bool bTouchOk = true;

    if (!m_touch.dataAvailable()) {
      bTouchOk = false;
    }

    if (bTouchOk) {
      m_touch.read();
      nRawX = m_touch.getX();
      nRawY = m_touch.getY();
      if ((nRawX == -1) || (nRawY == -1)) {
        bTouchOk = false;
      }
    }

    if (bTouchOk) {
      nRawPress = 255; // Dummy non-zero value
      m_nLastRawX = nRawX;
      m_nLastRawY = nRawY;
      m_nLastRawPress = nRawPress;
      m_bLastTouched = true;
      bValid = true;
    } else {
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
  #elif defined(DRV_TOUCH_INPUT)
    // No more to do for GPIO-only mode since gslc_Update() already
    // looks for GPIO inputs before calling TDrvGetTouch().
    // bValid will default to false


  // Assign defaults
  *pnX = 0;
  *pnY = 0;
  *pnPress = 0;

  *peInputEvent = GSLC_INPUT_NONE;
  *pnInputVal = 0;

  #ifdef DRV_DISP_ADAGFX_SEESAW
    // Keep track of last value to support simple debouncing
    static uint32_t nButtonsLast = 0xFFFFFFFF;     // Saved last value (static to preserve b/w calls)
    uint32_t nButtonsCur = m_seesaw.readButtons(); // Current value (note active low)
    if ((nButtonsLast & TFTSHIELD_BUTTON_UP) && !(nButtonsCur & TFTSHIELD_BUTTON_UP)) {
      *peInputEvent = GSLC_INPUT_PIN_ASSERT;
      *pnInputVal = GSLC_PIN_BTN_UP;
    } else if ((nButtonsLast & TFTSHIELD_BUTTON_DOWN) && !(nButtonsCur & TFTSHIELD_BUTTON_DOWN)) {
      *peInputEvent = GSLC_INPUT_PIN_ASSERT;
      *pnInputVal = GSLC_PIN_BTN_DOWN;
    } else if ((nButtonsLast & TFTSHIELD_BUTTON_LEFT) && !(nButtonsCur & TFTSHIELD_BUTTON_LEFT)) {
      *peInputEvent = GSLC_INPUT_PIN_ASSERT;
      *pnInputVal = GSLC_PIN_BTN_LEFT;
    } else if ((nButtonsLast & TFTSHIELD_BUTTON_RIGHT) && !(nButtonsCur & TFTSHIELD_BUTTON_RIGHT)) {
      *peInputEvent = GSLC_INPUT_PIN_ASSERT;
      *pnInputVal = GSLC_PIN_BTN_RIGHT;
    } else if ((nButtonsLast & TFTSHIELD_BUTTON_IN) && !(nButtonsCur & TFTSHIELD_BUTTON_IN)) {
      *peInputEvent = GSLC_INPUT_PIN_ASSERT;
      *pnInputVal = GSLC_PIN_BTN_SEL;
    }
    // Save button state so that transitions can be detected
    // during the next pass.
    nButtonsLast = nButtonsCur;
  #endif


  // If we reached here, then we had a button event
  return true;

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
  bool bSupportRotation = true;

  // Determine if the new orientation has swapped axes
  // versus the native orientation (0)
  bool bSwap = false;
  if ((nRotation == 1) || (nRotation == 3)) {
    bSwap = true;
  }
  (void)bSwap; // May be Unused in some driver modes

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
  #if defined(DRV_DISP_UTFT)
    pGui->nDisp0W = m_disp.getDisplayXSize();
    pGui->nDisp0H = m_disp.getDisplayYSize();
    // Temporarily disable support for rotation
    // TODO m_disp.setRotation(pGui->nRotation);
    pGui->nDispW = m_disp.getDisplayXSize();
    pGui->nDispH = m_disp.getDisplayYSize();

  #else
    // Report error for unsupported display mode
    // - If we don't trap this condition, the GUI dimensions will be incorrect
    #error "ERROR: DRV_DISP_* mode not supported in DrvRotate initialization"

  #endif

  // Update the clipping region
  gslc_tsRect rClipRect = { 0,0,pGui->nDispW,pGui->nDispH };
  gslc_DrvSetClipRect(pGui, &rClipRect);

  if (!bSupportRotation) {
    // No support for rotation, so override rotation indicator to 0
    // This will also ensure that nSwapXY / nFlipX / nFlipY all remain 0
    pGui->nRotation = 0;
    // Ensure no redraw forced due to change in rotation value
    bChange = false;
  }

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
