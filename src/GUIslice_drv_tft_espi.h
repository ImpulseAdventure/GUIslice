#ifndef _GUISLICE_DRV_TFT_ESPI_H_
#define _GUISLICE_DRV_TFT_ESPI_H_

// =======================================================================
// GUIslice library (driver layer for bodmer/TFT_eSPI)
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
// Driver Layer for bodmer/TFT_eSPI
// - https://github.com/Bodmer/TFT_eSPI
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "GUIslice.h"

#include <stdio.h>




// =======================================================================
// API support definitions
// - These defines indicate whether the driver includes optimized
//   support for various APIs. If a define is set to 0, then the
//   GUIslice core emulation will be used instead.
// - At the very minimum, the point draw routine must be available:
//   gslc_DrvDrawPoint()
// =======================================================================

#define DRV_HAS_DRAW_POINT          1 ///< Support gslc_DrvDrawPoint()

#define DRV_HAS_DRAW_POINTS         0 ///< Support gslc_DrvDrawPoints()
#define DRV_HAS_DRAW_LINE           1 ///< Support gslc_DrvDrawLine()
#define DRV_HAS_DRAW_RECT_FRAME     1 ///< Support gslc_DrvDrawFrameRect()
#define DRV_HAS_DRAW_RECT_FILL      1 ///< Support gslc_DrvDrawFillRect()
#define DRV_HAS_DRAW_CIRCLE_FRAME   1 ///< Support gslc_DrvDrawFrameCircle()
#define DRV_HAS_DRAW_CIRCLE_FILL    1 ///< Support gslc_DrvDrawFillCircle()
#define DRV_HAS_DRAW_TRI_FRAME      1 ///< Support gslc_DrvDrawFrameTriangle()
#define DRV_HAS_DRAW_TRI_FILL       1 ///< Support gslc_DrvDrawFillTriangle()
#define DRV_HAS_DRAW_TEXT           1 ///< Support gslc_DrvDrawTxt()

#define DRV_OVERRIDE_TXT_ALIGN      1 ///< Driver provides text alignment

// =======================================================================
// Driver-specific members
// =======================================================================
typedef struct {
  uint16_t      nColRawBkgnd;   ///< Background color (if not image-based)

  gslc_tsRect   rClipRect;      ///< Clipping rectangle

} gslc_tsDriver;



// =======================================================================
// Public APIs to GUIslice core library
// - These functions define the renderer / driver-dependent
//   implementations for the core drawing operations within
//   GUIslice.
// =======================================================================


// -----------------------------------------------------------------------
// Configuration Functions
// -----------------------------------------------------------------------

///
/// Initialize the SDL library
/// - Performs clean startup workaround (if enabled)
/// - Configures video mode
/// - Initializes font support
///
/// PRE:
/// - The environment variables should be configured before
///   calling gslc_DrvInit(). This can be done with gslc_DrvInitEnv()
///   or manually in user function.
///
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return true if success, false if fail
///
bool gslc_DrvInit(gslc_tsGui* pGui);


///
/// Perform any touchscreen-specific initialization
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  acDev:       Device path to touchscreen
///                          eg. "/dev/input/touchscreen"
///
/// \return true if successful
///
bool gslc_DrvInitTs(gslc_tsGui* pGui,const char* acDev);


///
/// Free up any members associated with the driver
/// - Eg. renderers, windows, background surfaces, etc.
///
/// \param[in]  pGui:         Pointer to GUI
///
/// \return none
///
void gslc_DrvDestruct(gslc_tsGui* pGui);


// -----------------------------------------------------------------------
// Image/surface handling Functions
// -----------------------------------------------------------------------


///
/// Load a bitmap (*.bmp) and create a new image resource.
/// Transparency is enabled by GSLC_BMP_TRANS_EN
/// through use of color (GSLC_BMP_TRANS_RGB).
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  sImgRef:     Image reference
///
/// \return Image pointer (surface/texture) or NULL if error
///
void* gslc_DrvLoadImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef);


///
/// Configure the background to use a bitmap image
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  sImgRef:     Image reference
///
/// \return true if success, false if fail
///
bool gslc_DrvSetBkgndImage(gslc_tsGui* pGui,gslc_tsImgRef sImgRef);

///
/// Configure the background to use a solid color
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB Color to use
///
/// \return true if success, false if fail
///
bool gslc_DrvSetBkgndColor(gslc_tsGui* pGui,gslc_tsColor nCol);

///
/// Set an element's normal-state image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  sImgRef:     Image reference
///
/// \return true if success, false if error
///
bool gslc_DrvSetElemImageNorm(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef);

///
/// Set an element's glow-state image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  sImgRef:     Image reference
///
/// \return true if success, false if error
///
bool gslc_DrvSetElemImageGlow(gslc_tsGui* pGui,gslc_tsElem* pElem,gslc_tsImgRef sImgRef);


///
/// Release an image surface
///
/// \param[in]  pvImg:          Void ptr to image
///
/// \return none
///
void gslc_DrvImageDestruct(void* pvImg);


///
/// Set the clipping rectangle for future drawing updates
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  pRect:         Rectangular region to constrain edits
///
/// \return none
///
bool gslc_DrvSetClipRect(gslc_tsGui* pGui,gslc_tsRect* pRect);


// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------

///
/// Load a font from a resource and return pointer to it
///
/// \param[in]  eFontRefType:   Font reference type (GSLC_FONTREF_PTR for Arduino)
/// \param[in]  pvFontRef:      Font reference pointer (Pointer to the GFXFont array)
/// \param[in]  nFontSz:        Typeface size to use
///
/// \return Void ptr to driver-specific font if load was successful, NULL otherwise
///
const void* gslc_DrvFontAdd(gslc_teFontRefType eFontRefType,const void* pvFontRef,uint16_t nFontSz);


///
/// Release all fonts defined in the GUI
///
/// \param[in]  pGui:          Pointer to GUI
///
/// \return none
///
void gslc_DrvFontsDestruct(gslc_tsGui* pGui);


///
/// Get the extent (width and height) of a text string
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pFont:       Ptr to Font structure
/// \param[in]  pStr:        String to display
/// \param[in]  eTxtFlags:   Flags associated with text string
/// \param[out] pnTxtX:      Ptr to offset X of text
/// \param[out] pnTxtY:      Ptr to offset Y of text
/// \param[out] pnTxtSzW:    Ptr to width of text
/// \param[out] pnTxtSzH:    Ptr to height of text
///
/// \return true if success, false if failure
///
bool gslc_DrvGetTxtSize(gslc_tsGui* pGui,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,
        int16_t* pnTxtX,int16_t* pnTxtY,uint16_t* pnTxtSzW,uint16_t* pnTxtSzH);


///
/// Draw a text string at the given coordinate
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nTxtX:       X coordinate of top-left text string
/// \param[in]  nTxtY:       Y coordinate of top-left text string
/// \param[in]  pFont:       Ptr to Font
/// \param[in]  pStr:        String to display
/// \param[in]  eTxtFlags:   Flags associated with text string
/// \param[in]  colTxt:      Color to draw text
///
/// \return true if success, false if failure
///
bool gslc_DrvDrawTxt(gslc_tsGui* pGui,int16_t nTxtX,int16_t nTxtY,gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt);

///
/// Draw a text string in a bounding box using the specified alignment
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         X coordinate of top-left of bounding box
/// \param[in]  nY0:         Y coordinate of top-left of bounding box
/// \param[in]  nX1:         X coordinate of bot-right of bounding box
/// \param[in]  nY1:         Y coordinate of bot-right of bounding box
/// \param[in]  eTxtAlign:   Alignment mode]
/// \param[in]  pFont:       Ptr to Font
/// \param[in]  pStr:        String to display
/// \param[in]  eTxtFlags:   Flags associated with text string
/// \param[in]  colTxt:      Color to draw text
///
/// \return true if success, false if failure
///
bool gslc_DrvDrawTxtAlign(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,int8_t eTxtAlign,
        gslc_tsFont* pFont,const char* pStr,gslc_teTxtFlags eTxtFlags,gslc_tsColor colTxt);

// -----------------------------------------------------------------------
// Screen Management Functions
// -----------------------------------------------------------------------

///
/// Force a page flip to occur. This generally copies active
/// screen surface to the display.
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return none
///
void gslc_DrvPageFlipNow(gslc_tsGui* pGui);


// -----------------------------------------------------------------------
// Graphics Primitives Functions
// -----------------------------------------------------------------------

///
/// Draw a point
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          X coordinate of point
/// \param[in]  nY:          Y coordinate of point
/// \param[in]  nCol:        Color RGB value to draw
///
/// \return true if success, false if error
///
bool gslc_DrvDrawPoint(gslc_tsGui* pGui,int16_t nX,int16_t nY,gslc_tsColor nCol);

///
/// Draw a point
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  asPt:        Array of points to draw
/// \param[in]  nNumPt:      Number of points in array
/// \param[in]  nCol:        Color RGB value to draw
///
/// \return true if success, false if error
///
bool gslc_DrvDrawPoints(gslc_tsGui* pGui,gslc_tsPt* asPt,uint16_t nNumPt,gslc_tsColor nCol);

///
/// Draw a framed rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to frame
/// \param[in]  nCol:        Color RGB value to frame
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFrameRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol);


///
/// Draw a filled rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to fill
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFillRect(gslc_tsGui* pGui,gslc_tsRect rRect,gslc_tsColor nCol);


///
/// Draw a line
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         Line start (X coordinate)
/// \param[in]  nY0:         Line start (Y coordinate)
/// \param[in]  nX1:         Line finish (X coordinate)
/// \param[in]  nY1:         Line finish (Y coordinate)
/// \param[in]  nCol:        Color RGB value to draw
///
/// \return true if success, false if error
///
bool gslc_DrvDrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_tsColor nCol);


///
/// Draw a framed circle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nMidX:       Center of circle (X coordinate)
/// \param[in]  nMidY:       Center of circle (Y coordinate)
/// \param[in]  nRadius:     Radius of circle
/// \param[in]  nCol:        Color RGB value to frame
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFrameCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol);


///
/// Draw a filled circle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nMidX:       Center of circle (X coordinate)
/// \param[in]  nMidY:       Center of circle (Y coordinate)
/// \param[in]  nRadius:     Radius of circle
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFillCircle(gslc_tsGui* pGui,int16_t nMidX,int16_t nMidY,uint16_t nRadius,gslc_tsColor nCol);


///
/// Draw a framed triangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         X Coordinate #1
/// \param[in]  nY0:         Y Coordinate #1
/// \param[in]  nX1:         X Coordinate #2
/// \param[in]  nY1:         Y Coordinate #2
/// \param[in]  nX2:         X Coordinate #3
/// \param[in]  nY2:         Y Coordinate #3
/// \param[in]  nCol:        Color RGB value to frame
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFrameTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol);


///
/// Draw a filled triangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         X Coordinate #1
/// \param[in]  nY0:         Y Coordinate #1
/// \param[in]  nX1:         X Coordinate #2
/// \param[in]  nY1:         Y Coordinate #2
/// \param[in]  nX2:         X Coordinate #3
/// \param[in]  nY2:         Y Coordinate #3
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFillTriangle(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,
        int16_t nX1,int16_t nY1,int16_t nX2,int16_t nY2,gslc_tsColor nCol);


///
/// Copy all of source image to destination screen at specified coordinate
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nDstX:       Destination X coord for copy
/// \param[in]  nDstY:       Destination Y coord for copy
/// \param[in]  sImgRef:     Image reference
///
/// \return true if success, false if fail
///
bool gslc_DrvDrawImage(gslc_tsGui* pGui,int16_t nDstX,int16_t nDstY,gslc_tsImgRef sImgRef);


///
/// Draw a monochrome bitmap from a memory array
/// - Draw from the bitmap buffer using the foreground color
///   defined in the header (unset bits are transparent)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nDstX:       Destination X coord for copy
/// \param[in]  nDstY:       Destination Y coord for copy
/// \param[in]  pBitmap:     Pointer to bitmap buffer
/// \param[in]  bProgMem:    Bitmap is stored in Flash if true, RAM otherwise
///
/// \return none
///
void gslc_DrvDrawMonoFromMem(gslc_tsGui* pGui,int16_t nDstX, int16_t nDstY, const unsigned char *pBitmap,bool bProgMem);


///
/// Copy the background image to destination screen
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return true if success, false if fail
///
void gslc_DrvDrawBkgnd(gslc_tsGui* pGui);


// -----------------------------------------------------------------------
// Touch Functions (if using display driver library)
// -----------------------------------------------------------------------

#if defined(DRV_TOUCH_IN_DISP)
// Use TFT_eSPI's integrated XPT2046 touch driver

///
/// Perform any touchscreen-specific initialization
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  acDev:       Device path to touchscreen
///                          eg. "/dev/input/touchscreen"
///
/// \return true if successful
///
bool gslc_DrvInitTouch(gslc_tsGui* pGui,const char* acDev);


///
/// Get the last touch event from the internal XPT2046 touch handler
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[out] pnX:         Ptr to X coordinate of last touch event
/// \param[out] pnY:         Ptr to Y coordinate of last touch event
/// \param[out] pnPress:     Ptr to Pressure level of last touch event (0 for none, 1 for touch)
///
/// \return true if an event was detected or false otherwise
///
bool gslc_DrvGetTouch(gslc_tsGui* pGui,int16_t* pnX, int16_t* pnY, uint16_t* pnPress);

#endif // DRV_TOUCH_IN_DISP


// -----------------------------------------------------------------------
// Touch Functions (if using external touch driver library)
// -----------------------------------------------------------------------

#if defined(DRV_TOUCH_ADA_STMPE610) || defined(DRV_TOUCH_ADA_FT6206) || defined(DRV_TOUCH_ADA_SIMPLE) || defined(DRV_TOUCH_XPT2046)

///
/// Perform any touchscreen-specific initialization
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  acDev:       Device path to touchscreen
///                          eg. "/dev/input/touchscreen"
///
/// \return true if successful
///
bool gslc_TDrvInitTouch(gslc_tsGui* pGui,const char* acDev);


///
/// Get the last touch event from the SDL_Event handler
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[out] pnX:         Ptr to X coordinate of last touch event
/// \param[out] pnY:         Ptr to Y coordinate of last touch event
/// \param[out] pnPress:     Ptr to Pressure level of last touch event (0 for none, 1 for touch)
///
/// \return true if an event was detected or false otherwise
///
bool gslc_TDrvGetTouch(gslc_tsGui* pGui,int16_t* pnX, int16_t* pnY, uint16_t* pnPress);

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
bool gslc_DrvRotateSwapFlip(gslc_tsGui* pGui, uint8_t nRotation, uint8_t nSwapXY, uint8_t nFlipX, uint8_t nFlipY );

// =======================================================================
// Private Functions
// - These functions are not included in the scope of APIs used by
//   the core GUIslice library. Instead, these functions are used
//   to support the operations within this driver layer.
// =======================================================================

uint16_t gslc_DrvAdaptColorToRaw(gslc_tsColor nCol);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_DRV_TFT_ESPI_H_
