#ifndef _MICROSDL_DRV_H_
#define _MICROSDL_DRV_H_

// =======================================================================
// microSDL library (driver layer for SDL1.2)
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.5.1    (2016/11/16)
// =======================================================================

// =======================================================================
// Driver Layer for SDL1.2
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "microsdl.h"

#include <stdio.h>

#include "SDL/SDL.h"
#include "SDL/SDL_getenv.h"
#include "SDL/SDL_ttf.h"

  
// =======================================================================
// API support definitions
// - These defines indicate whether the driver includes optimized
//   support for various APIs. If a define is set to 0, then the
//   microSDL core emulation will be used instead.
// - At the very minimum, the pixel set routine must always be available
//   (microSDL_DrvDrawSetPixelRaw).
// =======================================================================
  
#define DRV_HAS_DRAW_LINE           0 ///< Support microSDL_DrvDrawLine()
#define DRV_HAS_DRAW_RECT_FRAME     0 ///< Support microSDL_DrvDrawFrameRect()
#define DRV_HAS_DRAW_RECT_FILL      1 ///< Support microSDL_DrvDrawFillRect()
#define DRV_HAS_DRAW_CIRCLE_FRAME   0 ///< Support microSDL_DrvDrawFrameCircle()
#define DRV_HAS_DRAW_CIRCLE_FILL    0 ///< Support microSDL_DrvDrawFillCircle()
#define DRV_HAS_DRAW_TEXT           1 ///< Support microSDL_DrvDrawTxt()
  
  
// =======================================================================
// Public APIs to microSDL core library
// - These functions define the renderer / driver-dependent
//   implementations for the core drawing operations within
//   microSDL.
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
///   calling microSDL_DrvInit(). This can be done with microSDL_DrvInitEnv()
///   or manually in user function.
///
///
/// \param[in]  pGui:      Pointer to GUI
///
/// \return true if success, false if fail
///
bool microSDL_DrvInit(microSDL_tsGui* pGui);

///
/// Configure environment variables suitable for
/// default microSDL operation with PiTFT on
/// Raspberry Pi.
///
/// - NOTE: Users with other hardware may need to
///   supply their own initialization routine
///   as these defaults may not be suitable.
///
/// \param[in]  pGui:    Pointer to GUI
///
/// \return None
///
void microSDL_DrvInitEnv(microSDL_tsGui* pGui);  



// -----------------------------------------------------------------------
// Image/surface handling Functions
// -----------------------------------------------------------------------


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
bool microSDL_DrvScreenLock(microSDL_tsGui* pGui);


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
void microSDL_DrvScreenUnlock(microSDL_tsGui* pGui);

///
/// Load a bitmap (*.bmp) from a file and create
/// a new surface. Transparency is supported
/// via use of the color pink (0xFF00FF).
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pStrFname:   String containing file path to *.bmp file
///
/// \return SDL surface pointer or NULL if error
///
SDL_Surface* microSDL_DrvLoadBmp(microSDL_tsGui* pGui,char* pStrFname);


///
/// Configure the background to use a bitmap image
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pStrFname:   Filename to BMP file
///
/// \return true if success, false if fail
///
bool microSDL_DrvSetBkgndImage(microSDL_tsGui* pGui,char* pStrFname);


///
/// Configure the background to use a solid color
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB Color to use
///
/// \return true if success, false if fail
///
bool microSDL_DrvSetBkgndColor(microSDL_tsGui* pGui,microSDL_Color nCol);

///
/// Set an element's normal-state image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  acImage:     String of filename for image
///
/// \return none
///
void microSDL_DrvSetElemImageNorm(microSDL_tsGui* pGui,microSDL_tsElem* pElem,const char* acImage);

///
/// Set an element's glow-state image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  acImage:     String of filename for image
///
/// \return none
///  
void microSDL_DrvSetElemImageGlow(microSDL_tsGui* pGui,microSDL_tsElem* pElem,const char* acImage);


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
void microSDL_DrvPasteSurface(microSDL_tsGui* pGui,int nX, int nY, void* pvSrc, void* pvDest);


/// 
/// Release an image surface
/// 
/// \param[in]  pvSurf:         Void ptr to surface / image
/// 
/// \return none
///
void microSDL_DrvSurfaceDestruct(void* pvSurf);  


/// 
/// Set the clipping rectangle for future drawing updates
/// 
/// \param[in]  pGui:          Pointer to GUI  
/// \param[in]  pRect:         Rectangular region to constrain edits
/// 
/// \return none
///
void microSDL_DrvSetClipRect(microSDL_tsGui* pGui,microSDL_Rect* pRect);


// -----------------------------------------------------------------------
// Font handling Functions
// -----------------------------------------------------------------------

/// 
/// Release all fonts defined in the GUI
/// 
/// \param[in]  pGui:          Pointer to GUI  
/// 
/// \return none
///
void microSDL_DrvFontsDestruct(microSDL_tsGui* pGui);


///
/// Draw the text string associated with an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
///
/// \return true if success, false if failure
///  
bool microSDL_DrvDrawTxt(microSDL_tsGui* pGui,microSDL_tsElem* pElem);


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
void microSDL_DrvPageFlipNow(microSDL_tsGui* pGui);


// -----------------------------------------------------------------------
// Graphics Primitives Functions
// -----------------------------------------------------------------------  


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
uint32_t microSDL_DrvAdaptColorRaw(microSDL_tsGui* pGui,microSDL_Color nCol);


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
uint32_t microSDL_DrvDrawGetPixelRaw(microSDL_tsGui* pGui,int nX,int nY);


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
void microSDL_DrvDrawSetPixelRaw(microSDL_tsGui* pGui,int nX,int nY,uint32_t nPixelCol);


///
/// Draw a filled rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to fill
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return none
///
void microSDL_DrvDrawFillRect(microSDL_tsGui* pGui,microSDL_Rect rRect,microSDL_Color nCol);




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
bool microSDL_DrvGetTouch(microSDL_tsGui* pGui,int* pnX, int* pnY, unsigned* pnPress);



// =======================================================================
// Private Functions
// - These functions are not included in the scope of APIs used by
//   the core microSDL library. Instead, these functions are used
//   to support the operations within this driver layer.
// =======================================================================

// -----------------------------------------------------------------------
// Configuration Functions
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
bool microSDL_DrvCleanStart(const char* sTTY);


// -----------------------------------------------------------------------
// Conversion Functions
// -----------------------------------------------------------------------

///
/// Translate a microSDL_Rect into an SDL_Rect
///
/// \param[in]  rRect:    microSDL_Rect
///
/// \return Converted SDL_Rect
///
SDL_Rect  microSDL_DrvAdaptRect(microSDL_Rect rRect);

///
/// Translate a microSDL_Color into an SDL_Color
///
/// \param[in]  sCol:    microSDL_Color
///
/// \return Converted SDL_Color
///
SDL_Color  microSDL_DrvAdaptColor(microSDL_Color sCol);



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MICROSDL_DRV_H_

