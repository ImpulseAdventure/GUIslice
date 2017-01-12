// =======================================================================
// GUIslice library (driver layer for Adafruit-GFX)
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
// Driver Layer for Adafruit-GFX
// =======================================================================

// GUIslice library
#include "GUIslice_drv_adagfx.h"

#include <stdio.h>

#include <Adafruit_GFX.h>
#include <gfxfont.h>

#if defined(DRV_DISP_ADAGFX_ILI9341)
  #include <Adafruit_ILI9341.h>
  #if (ADAGFX_SD_EN)
    #include <SD.h>   // Include support for SD card access
  #endif
  #include <SPI.h>
#elif defined(DRV_DISP_ADAGFX_SSD1306)
  #include <Adafruit_SSD1306.h>
#endif

#if defined(DRV_TOUCH_ADA_STMPE610)
  #include <SPI.h>
  #include <Wire.h>
  #include "Adafruit_STMPE610.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



// ------------------------------------------------------------------------
#if defined(DRV_DISP_ADAGFX_ILI9341) 
  #if defined(ADAGFX_SPI_HW) // Use hardware SPI or software SPI (with custom pins)
    Adafruit_ILI9341 m_disp = Adafruit_ILI9341(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_RST);    
  #else
    Adafruit_ILI9341 m_disp = Adafruit_ILI9341(ADAGFX_PIN_CS, ADAGFX_PIN_DC, ADAGFX_PIN_MOSI, ADAGFX_PIN_CLK, ADAGFX_PIN_RST, ADAGFX_PIN_MISO);
  #endif

// ------------------------------------------------------------------------
#elif defined(DRV_DISP_ADAGFX_SSD1306)
  #if defined(ADAGFX_SPI_HW) // Use hardware SPI or software SPI (with custom pins)
    Adafruit_SSD1306 m_disp(ADAGFX_PIN_DC, ADAGFX_PIN_RST, ADAGFX_PIN_CS);
  #else
    Adafruit_SSD1306 m_disp(ADAGFX_PIN_MOSI, ADAGFX_PIN_CLK, ADAGFX_PIN_DC, ADAGFX_PIN_RESET, ADAGFX_PIN_CS);
  #endif

// ------------------------------------------------------------------------
#endif // DRV_DISP_ADAGFX_*



// ------------------------------------------------------------------------
#if defined(DRV_TOUCH_ADA_STMPE610) 
  #if defined(ADATOUCH_I2C_HW) // Use I2C
    Adafruit_STMPE610 m_touch = Adafruit_STMPE610();
  #elif defined(ADATOUCH_SPI_HW) // Use hardware SPI
    Adafruit_STMPE610 m_touch = Adafruit_STMPE610(ADATOUCH_PIN_CS);
  #elif defined(ADATOUCH_SPI_SW) // Use software SPI
    Adafruit_STMPE610 m_touch = Adafruit_STMPE610(ADATOUCH_PIN_CS, ADATOUCH_PIN_SDI, ADATOUCH_PIN_SDO, ADATOUCH_PIN_SCK);   
  #endif
// ------------------------------------------------------------------------
#endif // DRV_TOUCH_ADA_*



// =======================================================================
// Public APIs to GUIslice core library
// =======================================================================

// -----------------------------------------------------------------------
// Configuration Functions
// -----------------------------------------------------------------------

bool gslc_DrvInit(gslc_tsGui* pGui)
{
  // Initialize any library-specific members
  if (pGui->pvDriver) {
    gslc_tsDriver*  pDriver = (gslc_tsDriver*)(pGui->pvDriver);

    pDriver->nColRawBkgnd = gslc_DrvAdaptColorToRaw(GSLC_COL_BLACK);

    // These displays can accept partial redraw as they retain the last
    // image in the controller graphics RAM
    pGui->bRedrawPartialEn = true;

    #if defined(DRV_DISP_ADAGFX_ILI9341)
      m_disp.begin();      
      // Rotate display from native portrait orientation to landscape
      // NOTE: The touch events in gslc_TDrvGetTouch() will also need rotation
      m_disp.setRotation(1);
      pGui->nDispW = ILI9341_TFTHEIGHT;
      pGui->nDispH = ILI9341_TFTWIDTH;
      
    #elif defined(DRV_DISP_ADAGFX_SSD1306)
      m_disp.begin(SSD1306_SWITCHCAPVCC);
      pGui->nDispW = LCDWIDTH;
      pGui->nDispH = LCDHEIGHT;
    #endif

    // Defaults for clipping region
    gslc_tsRect rClipRect = {0,0,pGui->nDispW,pGui->nDispH};
    gslc_DrvSetClipRect(pGui,&rClipRect);
      
    // Initialize SD card usage
    #if (ADAGFX_SD_EN)
    if (!SD.begin(ADAGFX_PIN_SDCS)) {
      return false; // ERROR
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

void* gslc_DrvFontAdd(const char* acFontName,uint16_t nFontSz)
{
  // Nothing required to load, so return
  return NULL;
}

void gslc_DrvFontsDestruct(gslc_tsGui* pGui)
{
  // Nothing to deallocate
}

bool gslc_DrvGetTxtSize(gslc_tsGui* pGui,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,uint16_t* pnTxtSzW,uint16_t* pnTxtSzH)
{
  uint16_t  nTxtLen   = 0;  
  uint16_t  nTxtScale = pFont->nSize;
  m_disp.setTextSize(nTxtScale);
  
  //int16_t   nDummyX,nDummyY;
  //m_disp.getTextBounds((char*)pStr,0,0,&nDummyX,&nDummyY,pnTxtSzW,pnTxtSzH); 
  // TODO: FIXME: getTextBounds seems to return bad value.
  //       Would also need to handle pStr in PROGMEM
  
  // Workaround uses the dimensions of default default font in Adafruit-GFX library
  if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_RAM) {
    nTxtLen = strlen(pStr);
  } else if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_PROG) {
    nTxtLen = strlen_P(pStr);
  }
  *pnTxtSzW = (nTxtLen*6*nTxtScale);
  *pnTxtSzH = 8;
  return true;
}

bool gslc_DrvDrawTxt(gslc_tsGui* pGui,int16_t nTxtX,int16_t nTxtY,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt)
{
  uint16_t nTxtScale = pFont->nSize;  
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(colTxt);
  m_disp.setTextColor(nColRaw);
  m_disp.setCursor(nTxtX,nTxtY);
  m_disp.setTextSize(nTxtScale);

  if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_RAM) {
    // String in SRAM; can access buffer directly
    m_disp.println(pStr);
  } else if ((eTxtFlags & GSLC_TXT_MEM) == GSLC_TXT_MEM_PROG) {
    // String in PROGMEM (flash); must access via pgm_* calls
    char ch;
    while ((ch = pgm_read_byte(pStr++))) {
      m_disp.print(ch);
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
  #if defined(DRV_DISP_ADAGFX_ILI9341)
    // Nothing to do as we're not double-buffered

  #elif defined(DRV_DISP_ADAGFX_SSD1306)
    // Show the display buffer on the hardware.
    // NOTE: You _must_ call display after making any drawing commands
    // to make them visible on the display hardware!
    m_disp.display();
    
  #endif
}


// -----------------------------------------------------------------------
// Graphics Primitives Functions
// -----------------------------------------------------------------------  


bool gslc_DrvDrawPoint(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_tsColor nCol)
{
#if (ADAGFX_CLIP)
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
#if (ADAGFX_CLIP)
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
#if (ADAGFX_CLIP)
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
#if (ADAGFX_CLIP)
  gslc_tsDriver* pDriver = (gslc_tsDriver*)(pGui->pvDriver);  
  if (!gslc_ClipLine(&pDriver->rClipRect,&nX0,&nY0,&nX1,&nY1)) { return true; }      
#endif
  
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.drawLine(nX0,nY0,nX1,nY1,nColRaw);
  return true;
}

bool gslc_DrvDrawFrameCircle(gslc_tsGui*,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol)
{
#if (ADAGFX_CLIP)
  // TODO
#endif
  
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);  
  m_disp.drawCircle(nMidX,nMidY,nRadius,nColRaw);
  return true;
}

bool gslc_DrvDrawFillCircle(gslc_tsGui*,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol)
{
#if (ADAGFX_CLIP)
  // TODO
#endif
  
  uint16_t nColRaw = gslc_DrvAdaptColorToRaw(nCol);
  m_disp.fillCircle(nMidX,nMidY,nRadius,nColRaw);
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
void gslc_DrvDrawMonoFromMem(gslc_tsGui* pGui,int16_t x, int16_t y,
 const unsigned char *bitmap,bool bProgMem)
 {
  const unsigned char*  bmap_base = bitmap;
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
  uint8_t nByte;

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
        gslc_DrvDrawPoint(pGui,x+i,y+j,nCol);
      }
    }
  }   
}
// ----- REFERENCE CODE end


#if (ADAGFX_SD_EN)
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
  uint8_t  sdbuffer[3*ADAGFX_SD_BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
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
    //Serial.print("File size: "); Serial.println(gslc_DrvRead32SD(bmpFile));
    (void)gslc_DrvRead32SD(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = gslc_DrvRead32SD(bmpFile); // Start of image data
    //Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    //Serial.print("Header size: "); Serial.println(gslc_DrvRead32SD(bmpFile));
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
#endif // ADAGFX_SD_EN


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
    #if (ADAGFX_SD_EN)
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

#if defined(DRV_TOUCH_ADA_STMPE610) || defined(DRV_TOUCH_ADA_FT6206)

bool gslc_TDrvInitTouch(gslc_tsGui* pGui,const char* acDev) {
  #if defined(DRV_TOUCH_ADA_STMPE610)
    if (!m_touch.begin(ADATOUCH_I2C_ADDR)) {
      return false;
    } else {
      return true;
    }
  #endif

  return false;
}


bool gslc_TDrvGetTouch(gslc_tsGui* pGui,int16_t* pnX, int16_t* pnY, uint16_t* pnPress)
{
  uint16_t  nRawX,nRawY;
  uint8_t   nRawPress;

  #if defined(DRV_TOUCH_ADA_STMPE610)

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

  // If an event was detected, signal it back to GUIslice
  if (bValid) {
    // Clip the input range
    m_nLastRawX = constrain(m_nLastRawX,ADATOUCH_X_MIN,ADATOUCH_X_MAX);
    m_nLastRawY = constrain(m_nLastRawY,ADATOUCH_Y_MIN,ADATOUCH_Y_MAX);
    // Scale the range, swap and flip coords
    // - The swap and flip is done to rotate a native 240x320 display
    //   to a landscape orientation.
    // - TODO: Provide configuration options to support different orientations
    *pnX = map(m_nLastRawY,ADATOUCH_Y_MIN,ADATOUCH_Y_MAX,0,(pGui->nDispW-1));
    *pnY = (pGui->nDispH-1)-map(m_nLastRawX,ADATOUCH_X_MIN,ADATOUCH_X_MAX,0,(pGui->nDispH-1));
    *pnPress = m_nLastRawPress;
    
    // Return with indication of new value
    return true;
  } else {
    // No new value
    return false;
  }
  
  #endif // DRV_TOUCH_*

  return false;
}

#endif // DRV_TOUCH_*


// =======================================================================
// Private Functions
// =======================================================================

// Convert from RGB struct to native screen format
// TODO: Use 32bit return type?
uint16_t gslc_DrvAdaptColorToRaw(gslc_tsColor nCol)
{
  uint16_t nColRaw = 0;
  
  #if defined(DRV_DISP_ADAGFX_ILI9341)
    // RGB565
    nColRaw |= (((nCol.r & 0xF8) >> 3) << 11); // Mask: 1111 1000 0000 0000
    nColRaw |= (((nCol.g & 0xFC) >> 2) <<  5); // Mask: 0000 0111 1110 0000
    nColRaw |= (((nCol.b & 0xF8) >> 3) <<  0); // Mask: 0000 0000 0001 1111
  
  #elif defined(DRV_DISP_ADAGFX_SSD1306)
    if ((nCol.r == 0) || (nCol.g == 0) || (nCol.b == 0)) { // GSLC_COL_BLACK
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
