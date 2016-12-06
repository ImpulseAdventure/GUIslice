#ifndef _GUISLICE_DRV_SDL_H_
#define _GUISLICE_DRV_SDL_H_

// =======================================================================
// GUIslice library (driver layer for SDL 1.2 & 2.0)
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
// Driver Layer for SDL
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "GUIslice.h"

#include <stdio.h>
  
#ifdef DRV_TYPE_SDL1
  #include <SDL/SDL.h>
  #include <SDL/SDL_getenv.h>
  #include <SDL/SDL_ttf.h>
#endif
#ifdef DRV_TYPE_SDL2  
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_ttf.h>
#endif
  
// Includes for optional tslib touch handling
#ifdef DRV_INC_TS
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
  
#ifdef DRV_TYPE_SDL1
  #define DRV_HAS_DRAW_POINTS         1 ///< Support gslc_DrvDrawPoints()  
  #define DRV_HAS_DRAW_LINE           0 ///< Support gslc_DrvDrawLine()
  #define DRV_HAS_DRAW_RECT_FRAME     0 ///< Support gslc_DrvDrawFrameRect()
  #define DRV_HAS_DRAW_RECT_FILL      1 ///< Support gslc_DrvDrawFillRect()
  #define DRV_HAS_DRAW_CIRCLE_FRAME   0 ///< Support gslc_DrvDrawFrameCircle()
  #define DRV_HAS_DRAW_CIRCLE_FILL    0 ///< Support gslc_DrvDrawFillCircle()
  #define DRV_HAS_DRAW_TEXT           1 ///< Support gslc_DrvDrawTxt()
#endif
  
#ifdef DRV_TYPE_SDL2
  #define DRV_HAS_DRAW_POINTS         1 ///< Support gslc_DrvDrawPoints()
  #define DRV_HAS_DRAW_LINE           1 ///< Support gslc_DrvDrawLine()
  #define DRV_HAS_DRAW_RECT_FRAME     1 ///< Support gslc_DrvDrawFrameRect()
  #define DRV_HAS_DRAW_RECT_FILL      1 ///< Support gslc_DrvDrawFillRect()
  #define DRV_HAS_DRAW_CIRCLE_FRAME   0 ///< Support gslc_DrvDrawFrameCircle()
  #define DRV_HAS_DRAW_CIRCLE_FILL    0 ///< Support gslc_DrvDrawFillCircle()
  #define DRV_HAS_DRAW_TEXT           1 ///< Support gslc_DrvDrawTxt()
#endif  
  
// =======================================================================
// Driver-specific members
// =======================================================================
typedef struct {
  
  #ifdef DRV_TYPE_SDL1  
  SDL_Surface*        pSurfScreen;      ///< Surface ptr for screen
  #endif

  #ifdef DRV_TYPE_SDL2
  SDL_Window*         pWind;            ///< SDL2 Window
  SDL_Renderer*       pRender;          ///< SDL2 Rendering engine
  #endif

  #ifdef DRV_INC_TS
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
/// Example environment variable configuration suitable for
/// GUIslice operation with PiTFT on Raspberry Pi.
///
/// - NOTE: Users with other hardware may need to
///   supply their own initialization routine
///   as these defaults may not be suitable.
///
/// \param[in]  acDevFb:    Path to framebuffer device (NULL if none)
/// \param[in]  acDevTouch: Path to touchscreen device (NULL if none)
///
/// \return None
///
void gslc_DrvInitEnv(const char* acDevFb,const char* acDevTouch);


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
/// Load a bitmap (*.bmp) from a file and create
/// a new image resource. Transparency is enabled by GSLC_BMP_TRANS_EN
/// through use of color (GSLC_BMP_TRANS_RGB).
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pStrFname:   String containing file path to *.bmp file
///
/// \return Image pointer (surface/texture) or NULL if error
///
void* gslc_DrvLoadBmp(gslc_tsGui* pGui,const char* pStrFname);

///
/// Configure the background to use a bitmap image
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pStrFname:   Filename to BMP file
///
/// \return true if success, false if fail
///
bool gslc_DrvSetBkgndImage(gslc_tsGui* pGui,char* pStrFname);


///
/// Configure the background to use a solid color
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB Color to use
///
/// \return true if success, false if fail
///
bool gslc_DrvSetBkgndColor(gslc_tsGui* pGui,gslc_Color nCol);

///
/// Set an element's normal-state image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  acImage:     String of filename for image
///
/// \return true if success, false if error
///
bool gslc_DrvSetElemImageNorm(gslc_tsGui* pGui,gslc_tsElem* pElem,const char* acImage);

///
/// Set an element's glow-state image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  acImage:     String of filename for image
///
/// \return true if success, false if error
///  
bool gslc_DrvSetElemImageGlow(gslc_tsGui* pGui,gslc_tsElem* pElem,const char* acImage);


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
void gslc_DrvSetClipRect(gslc_tsGui* pGui,gslc_Rect* pRect);


// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------

///
/// Load a font from a file and return pointer to it
///
/// \param[in]  acFontName:  Filename path to the font
/// \param[in]  nFontSz:     Typeface size to use
///
/// \return Void ptr if load was successful, NULL otherwise
///
void* gslc_DrvFontAdd(const char* acFontName,unsigned nFontSz);


/// 
/// Release all fonts defined in the GUI
/// 
/// \param[in]  pGui:          Pointer to GUI  
/// 
/// \return none
///
void gslc_DrvFontsDestruct(gslc_tsGui* pGui);


///
/// Draw the text string associated with an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
///
/// \return true if success, false if failure
///  
bool gslc_DrvDrawTxt(gslc_tsGui* pGui,gslc_tsElem* pElem);


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
bool gslc_DrvDrawPoint(gslc_tsGui* pGui,int nX,int nY,gslc_Color nCol);

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
bool gslc_DrvDrawPoints(gslc_tsGui* pGui,gslc_Pt* asPt,unsigned nNumPt,gslc_Color nCol);

///
/// Draw a framed rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to frame
/// \param[in]  nCol:        Color RGB value to frame
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFrameRect(gslc_tsGui* pGui,gslc_Rect rRect,gslc_Color nCol);


///
/// Draw a filled rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to fill
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return true if success, false if error
///
bool gslc_DrvDrawFillRect(gslc_tsGui* pGui,gslc_Rect rRect,gslc_Color nCol);


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
bool gslc_DrvDrawLine(gslc_tsGui* pGui,int16_t nX0,int16_t nY0,int16_t nX1,int16_t nY1,gslc_Color nCol);


///
/// Copy all of source image to destination screen at specified coordinate
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nDstX:       Destination X coord for copy
/// \param[in]  nDstY:       Destination Y coord for copy
/// \param[in]  pImage:      Void ptr to image (eg. surface / texture)
///
/// \return true if success, false if fail
///
bool gslc_DrvDrawImage(gslc_tsGui* pGui,int nDstX,int nDstY,void* pImage);


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
bool gslc_DrvGetTouch(gslc_tsGui* pGui,int* pnX, int* pnY, unsigned* pnPress);



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


// -----------------------------------------------------------------------
// Private Conversion Functions
// -----------------------------------------------------------------------

///
/// Translate a gslc_Rect into an SDL_Rect
///
/// \param[in]  rRect:    gslc_Rect
///
/// \return Converted SDL_Rect
///
SDL_Rect  gslc_DrvAdaptRect(gslc_Rect rRect);

///
/// Translate a gslc_Color into an SDL_Color
///
/// \param[in]  sCol:    gslc_Color
///
/// \return Converted SDL_Color
///
SDL_Color  gslc_DrvAdaptColor(gslc_Color sCol);


// -----------------------------------------------------------------------
// Private Drawing Functions
// -----------------------------------------------------------------------

#ifdef DRV_TYPE_SDL1
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
uint32_t gslc_DrvAdaptColorRaw(gslc_tsGui* pGui,gslc_Color nCol);


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
uint32_t gslc_DrvDrawGetPixelRaw(gslc_tsGui* pGui,int nX,int nY);


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
void gslc_DrvDrawSetPixelRaw(gslc_tsGui* pGui,int nX,int nY,uint32_t nPixelCol);

///
/// Copy one image region to another.
/// - This is typically used to copy an image to the main screen surface
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  nX:            Destination X coordinate of copy
/// \param[in]  nY:            Destination Y coordinate of copy
/// \param[in]  pSrc:          Ptr to source surface (eg. a loaded image)
/// \param[in]  pDest:         Ptr to destination surface (typically the screen)
///
/// \return none
/// 
void gslc_DrvPasteSurface(gslc_tsGui* pGui,int nX, int nY, void* pvSrc, void* pvDest);

#endif // DRV_TYPE_SDL1


// -----------------------------------------------------------------------
// Private Touchscreen Functions (if using tslib)
// ----------------------------------------------------------------------- 

#ifdef DRV_INC_TS
///
/// Get the last touch event from the tslib handler
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[out] pnX:         Ptr to X coordinate of last touch event
/// \param[out] pnY:         Ptr to Y coordinate of last touch event
/// \param[out] pnPress:     Ptr to  Pressure level of last touch event (0 for none, >0 for touch)
///
/// \return non-zero if an event was detected or 0 otherwise
///
int gslc_DrvGetTsTouch(gslc_tsGui* pGui,int* pnX, int* pnY, unsigned* pnPress);

#endif // DRV_INC_TS


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _GUISLICE_DRV_SDL_H_

