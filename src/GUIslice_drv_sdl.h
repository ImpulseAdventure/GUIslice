#ifndef _GUISLICE_DRV_SDL_H_
#define _GUISLICE_DRV_SDL_H_

// =======================================================================
// GUIslice library (driver layer for SDL 1.2 & 2.0)
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
// Driver Layer for SDL
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "GUIslice.h"

#include <stdio.h>

#if defined(DRV_DISP_SDL1)
  #include <SDL/SDL.h>
  #include <SDL/SDL_getenv.h>
  #include <SDL/SDL_ttf.h>
#endif
#if defined(DRV_DISP_SDL2)
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_ttf.h>
#endif

// Includes for optional tslib touch handling
#if defined(DRV_TOUCH_TSLIB)
  #include "tslib.h"
#endif

// =======================================================================
// API support definitions
// - These defines indicate whether the driver includes optimized
//   support for various APIs. If a define is set to 0, then the
//   GUIslice core emulation will be used instead.
// - At the very minimum, the point draw routine must be available:
//   gslc_DrvDrawPoint()
// =======================================================================

#define DRV_HAS_DRAW_POINT          1 ///< Support gslc_DrvDrawPoint()

#if defined(DRV_DISP_SDL1)
  #define DRV_HAS_DRAW_POINTS         1 ///< Support gslc_DrvDrawPoints()
  #define DRV_HAS_DRAW_LINE           0 ///< Support gslc_DrvDrawLine()
  #define DRV_HAS_DRAW_RECT_FRAME     0 ///< Support gslc_DrvDrawFrameRect()
  #define DRV_HAS_DRAW_RECT_FILL      1 ///< Support gslc_DrvDrawFillRect()
  #define DRV_HAS_DRAW_CIRCLE_FRAME   0 ///< Support gslc_DrvDrawFrameCircle()
  #define DRV_HAS_DRAW_CIRCLE_FILL    0 ///< Support gslc_DrvDrawFillCircle()
  #define DRV_HAS_DRAW_TRI_FRAME      0 ///< Support gslc_DrvDrawFrameTriangle()
  #define DRV_HAS_DRAW_TRI_FILL       0 ///< Support gslc_DrvDrawFillTriangle()
  #define DRV_HAS_DRAW_TEXT           1 ///< Support gslc_DrvDrawTxt()
#endif

#if defined(DRV_DISP_SDL2)
  #define DRV_HAS_DRAW_POINTS         1 ///< Support gslc_DrvDrawPoints()
  #define DRV_HAS_DRAW_LINE           1 ///< Support gslc_DrvDrawLine()
  #define DRV_HAS_DRAW_RECT_FRAME     1 ///< Support gslc_DrvDrawFrameRect()
  #define DRV_HAS_DRAW_RECT_FILL      1 ///< Support gslc_DrvDrawFillRect()
  #define DRV_HAS_DRAW_CIRCLE_FRAME   0 ///< Support gslc_DrvDrawFrameCircle()
  #define DRV_HAS_DRAW_CIRCLE_FILL    0 ///< Support gslc_DrvDrawFillCircle()
  #define DRV_HAS_DRAW_TRI_FRAME      0 ///< Support gslc_DrvDrawFrameTriangle()
  #define DRV_HAS_DRAW_TRI_FILL       0 ///< Support gslc_DrvDrawFillTriangle()
  #define DRV_HAS_DRAW_TEXT           1 ///< Support gslc_DrvDrawTxt()
#endif

#define DRV_OVERRIDE_TXT_ALIGN      0 ///< Driver provides text alignment

// =======================================================================
// Driver-specific members
// =======================================================================
typedef struct {

  #if defined(DRV_DISP_SDL1)
  SDL_Surface*        pSurfScreen;      ///< Surface ptr for screen
  #endif

  #if defined(DRV_DISP_SDL2)
  SDL_Window*         pWind;            ///< SDL2 Window
  SDL_Renderer*       pRender;          ///< SDL2 Rendering engine
  #endif

  #if defined(DRV_TOUCH_TSLIB)
  struct tsdev*       pTsDev;           ///< Ptr to touchscreen device
  #endif

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
///   calling gslc_DrvInit().
///
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return true if success, false if fail
///
bool gslc_DrvInit(gslc_tsGui* pGui);


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
/// \return Image pointer (surface/texture/path) or NULL if error
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
/// \return true if success, false if error
///
bool gslc_DrvSetClipRect(gslc_tsGui* pGui,gslc_tsRect* pRect);


// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------


///
/// Load a font from a resource and return pointer to it
///
/// \param[in]  eFontRefType:   Font reference type (GSLC_FONTREF_FNAME for SDL)
/// \param[in]  pvFontRef:      Font reference pointer (Pointer to the font filename)
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
/// Copy the background image to destination screen
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return true if success, false if fail
///
void gslc_DrvDrawBkgnd(gslc_tsGui* pGui);


// -----------------------------------------------------------------------
// Touch Functions
// -----------------------------------------------------------------------

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
bool gslc_DrvGetTouch(gslc_tsGui* pGui,int16_t* pnX, int16_t* pnY, uint16_t* pnPress);



// =======================================================================
// Private Functions
// - These functions are not included in the scope of APIs used by
//   the core GUIslice library. Instead, these functions are used
//   to support the operations within this driver layer.
// =======================================================================

// -----------------------------------------------------------------------
// Private Configuration Functions
// -----------------------------------------------------------------------

///
/// Ensure SDL initializes cleanly to workaround
/// possible issues if previous SDL application
/// failed to close down gracefully.
///
/// \param[in]  sTTY:       Terminal device (eg. "/dev/tty0")
///
/// \return true if success
///
bool gslc_DrvCleanStart(const char* sTTY);


///
/// Report driver debug info (before initialization)
///
/// \return none
///
void gslc_DrvReportInfoPre();

///
/// Report driver debug info (after initialization)
///
/// \return none
///
void gslc_DrvReportInfoPost();

// -----------------------------------------------------------------------
// Private Conversion Functions
// -----------------------------------------------------------------------

///
/// Translate a gslc_tsRect into an SDL_Rect
///
/// \param[in]  rRect:    gslc_tsRect
///
/// \return Converted SDL_Rect
///
SDL_Rect  gslc_DrvAdaptRect(gslc_tsRect rRect);

///
/// Translate a gslc_tsColor into an SDL_Color
///
/// \param[in]  sCol:    gslc_tsColor
///
/// \return Converted SDL_Color
///
SDL_Color  gslc_DrvAdaptColor(gslc_tsColor sCol);


// -----------------------------------------------------------------------
// Private Drawing Functions
// -----------------------------------------------------------------------

#if defined(DRV_DISP_SDL1)
///
/// Lock an SDL surface so that direct pixel manipulation
/// can be done safely. This function is called before any
/// direct pixel updates.
///
/// POST:
/// - Primary screen surface is locked
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return true if success, false otherwise
///
bool gslc_DrvScreenLock(gslc_tsGui* pGui);


///
/// Unlock the SDL surface after pixel manipulation is
/// complete. This function is called after all pixel updates
/// are done.
///
/// POST:
/// - Primary screen surface is unlocked
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return none
///
void gslc_DrvScreenUnlock(gslc_tsGui* pGui);


///
/// Convert an RGB color triplet into the surface pixel value.
/// This is called to produce the native pixel value required by
/// the raw pixel manipulation routines.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB value for conversion
///
/// \return A pixel value for the current screen format
///
uint32_t gslc_DrvAdaptColorRaw(gslc_tsGui* pGui,gslc_tsColor nCol);


///
/// Get the pixel at (X,Y) from the active screen
///
/// PRE:
/// - Screen surface must be locked
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          Pixel X coordinate
/// \param[in]  nY:          Pixel Y coordinate
///
/// \return Pixel color value from the coordinate or 0 if error
///
uint32_t gslc_DrvDrawGetPixelRaw(gslc_tsGui* pGui,int16_t nX,int16_t nY);


///
/// Set a pixel on the active screen to the given color
///
/// PRE:
/// - Screen surface must be locked
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          Pixel X coordinate to set
/// \param[in]  nY:          Pixel Y coordinate to set
/// \param[in]  nPixelCol:   Raw color pixel value to assign
///
/// \return none
///
void gslc_DrvDrawSetPixelRaw(gslc_tsGui* pGui,int16_t nX,int16_t nY,uint32_t nPixelCol);

///
/// Copy one image region to another.
/// - This is typically used to copy an image to the main screen surface
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  nX:            Destination X coordinate of copy
/// \param[in]  nY:            Destination Y coordinate of copy
/// \param[in]  pvSrc:         Void Ptr to source surface (eg. a loaded image)
/// \param[in]  pvDest:        Void Ptr to destination surface (typically the screen)
///
/// \return none
///
void gslc_DrvPasteSurface(gslc_tsGui* pGui,int16_t nX, int16_t nY, void* pvSrc, void* pvDest);

#endif // DRV_DISP_SDL1



// -----------------------------------------------------------------------
// Private Touchscreen Functions (if using SDL)
// -----------------------------------------------------------------------

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
/// Get the last touch event from the SDL handler
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[out] pnX:         Ptr to X coordinate of last touch event
/// \param[out] pnY:         Ptr to Y coordinate of last touch event
/// \param[out] pnPress:     Ptr to  Pressure level of last touch event (0 for none, >0 for touch)
///
/// \return true if an event was detected or 0 otherwise
///
bool gslc_DrvGetTouch(gslc_tsGui* pGui,int16_t* pnX, int16_t* pnY, uint16_t* pnPress);



// -----------------------------------------------------------------------
// Touchscreen Functions (if using tslib)
// -----------------------------------------------------------------------

#if defined(DRV_TOUCH_TSLIB)
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
/// Get the last touch event from the tslib handler
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[out] pnX:         Ptr to X coordinate of last touch event
/// \param[out] pnY:         Ptr to Y coordinate of last touch event
/// \param[out] pnPress:     Ptr to Pressure level of last touch event (0 for none, >0 for touch)
///
/// \return non-zero if an event was detected or 0 otherwise
///
int gslc_TDrvGetTouch(gslc_tsGui* pGui,int16_t* pnX, int16_t* pnY, uint16_t* pnPress);

#endif // DRV_TOUCH_TSLIB


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_DRV_SDL_H_

