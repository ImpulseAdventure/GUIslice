#ifndef _MICROSDL_H_
#define _MICROSDL_H_

// =======================================================================
// microSDL library
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.2 (2016/09/25)
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"


// -----------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------

#define ELEM_MAX          200 // Max number of elements
#define ELEM_STRLEN_MAX   80  // Max string length of text elements
#define FONT_MAX          20  // Max number of fonts to load

#define BMP_TRANS_EN      1
#define BMP_TRANS_RGB     0xFF,0x00,0xFF

// -----------------------------------------------------------------------
// Enumerations
// -----------------------------------------------------------------------

#define MSDL_IND_NONE   -1
#define MSDL_ID_NONE    -11
#define MSDL_ID_ANON    -12
#define MSDL_PAGE_NONE  -21
#define MSDL_PAGE_ALL   -22
#define MSDL_FONT_NONE  -31

// Element types
enum {MSDL_TYPE_NONE, MSDL_TYPE_BKGND, MSDL_TYPE_BTN, MSDL_TYPE_TXT,
    MSDL_TYPE_GAUGE, MSDL_TYPE_BOX, MSDL_TYPE_LINE};

// Element text alignment
#define MSDL_ALIGNV_TOP       0x10
#define MSDL_ALIGNV_MID       0x20
#define MSDL_ALIGNV_BOT       0x40
#define MSDL_ALIGNH_LEFT      0x01
#define MSDL_ALIGNH_MID       0x02
#define MSDL_ALIGNH_RIGHT     0x04
#define MSDL_ALIGN_TOP_LEFT   MSDL_ALIGNH_LEFT  | MSDL_ALIGNV_TOP
#define MSDL_ALIGN_TOP_MID    MSDL_ALIGNH_MID   | MSDL_ALIGNV_TOP
#define MSDL_ALIGN_TOP_RIGHT  MSDL_ALIGNH_RIGHT | MSDL_ALIGNV_TOP
#define MSDL_ALIGN_MID_LEFT   MSDL_ALIGNH_LEFT  | MSDL_ALIGNV_MID
#define MSDL_ALIGN_MID_MID    MSDL_ALIGNH_MID   | MSDL_ALIGNV_MID
#define MSDL_ALIGN_MID_RIGHT  MSDL_ALIGNH_RIGHT | MSDL_ALIGNV_MID
#define MSDL_ALIGN_BOT_LEFT   MSDL_ALIGNH_LEFT  | MSDL_ALIGNV_BOT
#define MSDL_ALIGN_BOT_MID    MSDL_ALIGNH_MID   | MSDL_ALIGNV_BOT
#define MSDL_ALIGN_BOT_RIGHT  MSDL_ALIGNH_RIGHT | MSDL_ALIGNV_BOT


// Basic color definition
#define m_colBlueDk     (SDL_Color) {0,0,255}
#define m_colBlueLt     (SDL_Color) {128,128,255}
#define m_colRedDk      (SDL_Color) {255,0,0}
#define m_colRedLt      (SDL_Color) {255,128,128}
#define m_colGreenDk    (SDL_Color) {0,255,0}
#define m_colGreenLt    (SDL_Color) {128,255,128}
#define m_colYellow     (SDL_Color) {255,255,0}
#define m_colYellowDk   (SDL_Color) {64,64,0}
#define m_colBlack      (SDL_Color) {0,0,0}
#define m_colGrayDk     (SDL_Color) {64,64,64}
#define m_colGray       (SDL_Color) {128,128,128}
#define m_colGrayLt     (SDL_Color) {192,192,192}
#define m_colWhite      (SDL_Color) {255,255,255}
#define m_colPurple     (SDL_Color) {128,0,128}
#define m_colCyan       (SDL_Color) {0,255,255}
#define m_colMagenta    (SDL_Color) {255,0,255}
#define m_colTeal       (SDL_Color) {0,128,128}
#define m_colOrange     (SDL_Color) {255,165,0}
#define m_colBrown      (SDL_Color) {165,42,42}


// -----------------------------------------------------------------------
// Structures
// -----------------------------------------------------------------------



//
// Element Struct
// - Represents a single graphic element in the microSDL environment
// - A page is made up of a number of elments
// - Each element is created with a user-specified ID for further
//   accesses (or MSDL_ID_ANON if it is a don't-care)
// - Display order of elements in a page is based upon the creation order
// - An element can be associated with all pages with nPage=MSDL_PAGE_ALL
//
typedef struct {
  bool            bValid;         // Element was created properly
  int             nId;            // Element ID specified by user
  unsigned        nPage;          // Page ID containing this element
  unsigned        nType;
  SDL_Rect        rElem;

  SDL_Surface*    pSurf;
  SDL_Surface*    pSurfSel;

  bool            bClickEn;
  bool            bFrameEn;
  bool            bFillEn;

  SDL_Color       colElemFrame;
  SDL_Color       colElemFill;
  SDL_Color       colElemFillSel;

  // TODO: bNeedRedraw

  char            acStr[ELEM_STRLEN_MAX+1];
  SDL_Color       colElemText;
  unsigned        eTxtAlign;
  unsigned        nTxtMargin;
  TTF_Font*       pTxtFont;

  // Gauge
  int             nGaugeMin;
  int             nGaugeMax;
  int             nGaugeVal;
  SDL_Color       colGauge;
  bool            bGaugeVert;

} microSDL_tsElem;


// Struct that represents a font reference
typedef struct {
  unsigned    nId;
  TTF_Font*   pFont;
} microSDL_tsFont;


// Struct that contains all GUI state and content
typedef struct {

    // The page that is currently active
  unsigned          nPageIdCur;

  // Collection of loaded fonts
  microSDL_tsFont   asFont[FONT_MAX];
  unsigned          nFontCnt;

  // Collection of graphic elements (across all pages)
  microSDL_tsElem   asElem[ELEM_MAX];
  unsigned          nElemCnt;
  unsigned          nElemAutoIdNext;

  // Current touch-tracking hover status
  int               nTrackElemHover;
  bool              bTrackElemHoverGlow;

  // Last graphic element clicked
  int               nTrackElemClicked;
  int               nClickLastX;
  int               nClickLastY;
  unsigned          nClickLastPress;

  // Touchscreen library interface
  #ifdef INC_TS
  struct tsdev*     ts;
  #endif

  // Primary surface definitions
  SDL_Surface*      surfScreen;
  SDL_Surface*      surfBkgnd;

} microSDL_tsGui;




// ------------------------------------------------------------------------
// General Functions
// ------------------------------------------------------------------------

// TODO: Document
void microSDL_GuiReset(microSDL_tsGui* pGui);

//
// Configure environment variables suitable for
// default microSDL operation with PiTFT on
// Raspberry Pi.
//
// NOTE: Users with other hardware may need to
// supply their own initialization routine
// as these defaults may not be suitable.
//
// RETURN:
// - None
//
void microSDL_InitEnv(microSDL_tsGui* pGui);


//
// Initialize the microSDL library
// - Configures the primary screen surface
// - Initializes any fonts
//
// PRE:
// - The environment variables should be configured before
//   calling microSDL_Init(). This can be done with microSDL_InitEnv()
//   or manually in user function.
//
// POST:
// - microSDL_m_surfScreen is initialized
//
// RETURN:
// - true if success, false if fail
//
bool microSDL_Init(microSDL_tsGui* pGui);

//
// Exit the microSDL environment
// - Calls SDL Quit to clean up any initialized subsystems
//   and also deletes any created elements
//
// RETURN:
// - None
//
void microSDL_Quit(microSDL_tsGui* pGui);


// ------------------------------------------------------------------------
// Graphics General Functions
// ------------------------------------------------------------------------

//
// Load a bitmap (*.bmp) from a file and create
// a new surface. Transparency is supported
// via use of the color pink (0xFF00FF).
//
// INPUT:
// - pStrFname:   String containing file path to *.bmp file
//
// RETURN:
// - SDL surface pointer or NULL if error
//
SDL_Surface *microSDL_LoadBmp(microSDL_tsGui* pGui,char* pStrFname);



//
// Configure the background image to use the provided surface.
// - The background is used when redrawing the entire page
//
// INPUT:
// - pSurf: Surface to use for the background image
//
// RETURN:
// - true if success, false if fail
//
bool microSDL_SetBkgnd(microSDL_tsGui* pGui,SDL_Surface* pSurf);

//
// Configure the background to use a bitmap image
// - The background is used when redrawing the entire page
//
// INPUT:
// - pStrFname: Filename to BMP file
//
// RETURN:
// - true if success, false if fail
//
bool microSDL_SetBkgndImage(microSDL_tsGui* pGui,char* pStrFname);

//
// Configure the background to use a solid color
// - The background is used when redrawing the entire page
//
// INPUT:
// - nCol: RGB Color to use
//
// RETURN:
// - true if success, false if fail
//
bool microSDL_SetBkgndColor(microSDL_tsGui* pGui,SDL_Color nCol);


//
// Copy one surface to another.
// - This is typically used to copy a shape
//   to the main screen surface.
//
void microSDL_ApplySurface(microSDL_tsGui* pGui,int x, int y, SDL_Surface* pSrc, SDL_Surface* pDest);


//
// Determine if a coordinate is inside of a rectangular region.
// - This routine is useful in determining if a touch
//   coordinate is inside of a button.
//
// RETURN:
// - true if inside region, false otherwise
//
bool microSDL_IsInRect(microSDL_tsGui* pGui,unsigned nSelX,unsigned nSelY,SDL_Rect rBtn);


//
// Update the visible screen with any changes amde
// - On some hardware this can trigger a double-buffering
//   page flip.
//
// PRE:
// - microSDL_Init() has been called
//
// RETURN:
// - None
//
void microSDL_Flip(microSDL_tsGui* pGui);




// ------------------------------------------------------------------------
// Graphics Primitive Functions
// ------------------------------------------------------------------------


//
// Set a pixel on the active screen to the given color with lock
// - Calls upon microSDL_SetPixelRaw() but wraps with a surface lock lock
// - If repeated access is needed, use microSDL_SetPixelRaw() instead
//
// INPUT:
// - nX:         Pixel X coordinate to set
// - nY:         Pixel Y coordinate to set
// - nPixelCol:  Color pixel value ot assign
//
// RETURN:
// - none
//
void microSDL_SetPixel(microSDL_tsGui* pGui,Sint16 nX,Sint16 nY,SDL_Color nCol);


//
// Draw an arbitrary line using Bresenham's algorithm
//
// INPUT:
// - nX0:    X coordinate of line startpoint
// - nY0:    Y coordinate of line startpoint
// - nX1:    X coordinate of line endpoint
// - nY1:    Y coordinate of line endpoint
// - nCol:   Color RGB value for the line
//
// RETURN:
// - none
//
void microSDL_Line(microSDL_tsGui* pGui,Sint16 nX0,Sint16 nY0,Sint16 nX1,Sint16 nY1,SDL_Color nCol);


//
// Draw a horizontal line
// - Note that direction of line is in +ve X axis
//
// INPUT:
// - nX:     X coordinate of line startpoint
// - nY:     Y coordinate of line startpoint
// - nW:     Width of line (in +X direction)
// - nCol:   Color RGB value for the line
//
// RETURN:
// - none
//
void microSDL_LineH(microSDL_tsGui* pGui,Sint16 nX, Sint16 nY, Uint16 nW,SDL_Color nCol);


//
// Draw a vertical line
// - Note that direction of line is in +ve Y axis
//
// INPUT:
// - nX:     X coordinate of line startpoint
// - nY:     Y coordinate of line startpoint
// - nH:     Height of line (in +Y direction)
// - nCol:   Color RGB value for the line
//
// RETURN:
// - none
//
void microSDL_LineV(microSDL_tsGui* pGui,Sint16 nX, Sint16 nY, Uint16 nH,SDL_Color nCol);


//
// Draw a framed rectangle
//
// INPUT:
// - nX:    X coordinate of rectangle corner (bottom-left)
// - nY:    Y coordinate of rectangle corner (bottom-left)
// - nW:    Width of rectangle from corner
// - nH:    Height of rectangle from corner
// - nCol:  Color RGB value for the frame
//
// RETURN:
// - none
//
void microSDL_FrameRect(microSDL_tsGui* pGui,Sint16 nX,Sint16 nY,Uint16 nW,Uint16 nH,SDL_Color nCol);


//
// Draw a filled rectangle
//
// INPUT:
// - rRect: Rectangular region to fill
// - nCol:  Color RGB value to fill
//
// RETURN:
// - none
//
void microSDL_FillRect(microSDL_tsGui* pGui,SDL_Rect rRect,SDL_Color nCol);


// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------

//
// Setup text / font support
//
// RETURN:
// - true if success, false if fail
//
bool microSDL_InitFont(microSDL_tsGui* pGui);


//
// Load a font into the local font cache and assign
// font ID (nFontId).
//
// INPUT:
// - nFontId:    ID to use when referncing this font
// - acFontName: filename path to the font
// - nFontSz:    typeface size to use
//
// RETURN:
// - true if load was successful, false otherwise
//
bool microSDL_FontAdd(microSDL_tsGui* pGui,unsigned nFontId, const char* acFontName, unsigned nFontSz);


//
// Fetch a font from its ID value
//
// INPUT:
// - nFontId:  ID value used to reference the font (supplied
//             originally to microSDL_FontAdd()
//
// RETURN:
// - A TTF_Font pointer or NULL if error
//
TTF_Font* microSDL_FontGet(microSDL_tsGui* pGui,unsigned nFontId);


//
// Close all loaded fonts
//
// RETURN:
// - none
//
void microSDL_FontCloseAll(microSDL_tsGui* pGui);



// ------------------------------------------------------------------------
// Page Functions
// ------------------------------------------------------------------------

//
// Fetch the current page ID
//
// RETURN:
// - Page ID
//

unsigned microSDL_GetPageCur(microSDL_tsGui* pGui);


//
// Select a new page for display
//
// INPUT:
// - nPageId:   Page ID to select as current
//
// RETURN:
// - none
//
void microSDL_SetPageCur(microSDL_tsGui* pGui,unsigned nPageId);


//
// Draw a specific page
//
// INPUT:
// - nPageId:   Page ID to draw
//
// RETURN:
// - none
//
void microSDL_ElemDrawPage(microSDL_tsGui* pGui,unsigned nPageId);


//
// Draw the current page
//
// RETURN:
// - none
//
void microSDL_ElemDrawPageCur(microSDL_tsGui* pGui);

// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------

//
// Look up the Element Index from the Element ID
//
// INPUT:
// - nElemId:    ID of the element to find
//
// RETURN:
// - Index of element or MSDL_ID_NONE if not found
//
int microSDL_ElemFindIndFromId(microSDL_tsGui* pGui,int nElemId);


// Find an Element Index from coordinates
// - Ignore any elements not marked as clickable
//
// RETURN:
// - Index of element or MSDL_ID_NONE if not found
//
int microSDL_ElemFindFromCoord(microSDL_tsGui* pGui,int nX, int nY);




// ------------------------------------------------------------------------
// Element Creation Functions
// ------------------------------------------------------------------------


//
// Create a Text Element
// - Draws a text string with filled background
//
// INPUT:
// - nElemId: Element ID to assign
// - nPage:   Page ID to attach element to
// - rElem:   Rectangle coordinates defining text background size
// - pStr:    String to copy into element
// - nFontId: Font ID to use for text display
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateTxt(microSDL_tsGui* pGui,int nElemId,unsigned nPage,
  SDL_Rect rElem,const char* pStr,unsigned nFontId);


//
// Create a textual Button Element
// - Creates a clickable element that has a textual label
//   with frame and fill
//
// INPUT:
// - nElemId: Element ID to assign
// - nPage:   Page ID to attach element to
// - rElem:   Rectangle coordinates defining text background size
// - acStr:   String to copy into element
// - nFontId: Font ID to use for text display
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateBtnTxt(microSDL_tsGui* pGui,int nElemId,unsigned nPage,
  SDL_Rect rElem,const char* acStr,unsigned nFontId);


//
// Create a graphical Button Element
// - Creates a clickable element that uses a BMP image with
//   no frame or fill
// - Transparency is supported by bitmap color (0xFF00FF)
//
// INPUT:
// - nElemId:   Element ID to assign
// - nPage:     Page ID to attach element to
// - rElem:     Rectangle coordinates defining image size
// - acImg:     Filename of BMP image to load (unselected state)
// - acImgSel:  Filename of BMP image to load (selected state)
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateBtnImg(microSDL_tsGui* pGui,int nElemId,unsigned nPage,
  SDL_Rect rElem,const char* acImg,const char* acImgSel);


//
// Create a Box Element
// - Draws a box with frame and fill
//
// INPUT:
// - nElemId: Element ID to assign
// - nPage:   Page ID to attach element to
// - rElem:   Rectangle coordinates defining box size
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateBox(microSDL_tsGui* pGui,int nElemId,unsigned nPage,SDL_Rect rElem);


//
// Create an image Element
// - Draws an image
//
// INPUT:
// - nElemId: Element ID to assign
// - nPage:   Page ID to attach element to
// - rElem:   Rectangle coordinates defining box size
// - acImg:   Filename of BMP image to load
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateImg(microSDL_tsGui* pGui,int nElemId,unsigned nPage,SDL_Rect rElem,
  const char* acImg);


//
// Create a Gauge Element
// - Draws a horizontal or vertical box with a filled region
//   corresponding to the proportion that nVal represents
//   between nMin and nMax.
//
// INPUT:
// - nElemId:   Element ID to assign
// - nPage:     Page ID to attach element to
// - rElem:     Rectangle coordinates defining gauge size
// - nMin:      Minimum value of gauge for nVal comparison
// - nMax:      Maximum value of gauge for nVal comparison
// - colGauge:  Color to fill the gauge with
// - bVert:     Flag to indicate vertical vs horizontal action
//              (1 = vertical, 0 = horizontal)
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateGauge(microSDL_tsGui* pGui,int nElemId,unsigned nPage,
  SDL_Rect rElem,int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert);


//
// Add the Element to the list of generated elements
// in the GUI environment
//
// INPUT:
// - sElem:   Element to add to environment
//
// RETURN:
// - true if success, false if fail
//
bool microSDL_ElemAdd(microSDL_tsGui* pGui,microSDL_tsElem sElem);


// ------------------------------------------------------------------------
// Element Drawing Functions
// ------------------------------------------------------------------------


//
// Draw a graphic element
//
// INPUT:
// - nElemId:   ID of element
//
// RETURN:
// - none
//
void microSDL_ElemDraw(microSDL_tsGui* pGui,int nElemId);


// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

//
// Set the fill state for an Element
//
// INPUT:
// - nElemId:     Element ID to update
// - bFillEn:     True if filled, false otherwise
//
// RETURN:
// - none
//
void microSDL_ElemSetFillEn(microSDL_tsGui* pGui,int nElemId,bool bFillEn);


//
// Update the common color selection for an Element
//
// INPUT:
// - nElemId:     Element ID to update
// - colFrame:    Color for the frame
// - colFill:     Color for the fill
// - colFillSel:  Color for the fill when selected
//
// RETURN:
// - none
//
void microSDL_ElemSetCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colFrame,SDL_Color colFill,SDL_Color colFillSel);




// Set the alignment of a textual element (horizontal and vertical)
//
// INPUT:
// - nId:     Element ID to update
// - nAlign:  Alignment to specify:
//              MSDL_ALIGN_TOP_LEFT
//              MSDL_ALIGN_TOP_MID
//              MSDL_ALIGN_TOP_RIGHT
//              MSDL_ALIGN_MID_LEFT
//              MSDL_ALIGN_MID_MID
//              MSDL_ALIGN_MID_RIGHT
//              MSDL_ALIGN_BOT_LEFT
//              MSDL_ALIGN_BOT_MID
//              MSDL_ALIGN_BOT_RIGHT
//
// RETURN:
// - none
//
void microSDL_ElemSetTxtAlign(microSDL_tsGui* pGui,int nElemId,unsigned nAlign);


//
// Update the text string associated with an Element ID
//
// INPUT:
// - nElemId:   Element ID to update
// - pStr:      String to copy into element
//
// RETURN:
// - none
//
void microSDL_ElemSetTxtStr(microSDL_tsGui* pGui,int nElemId,const char* pStr);

//
// Update the text string color associated with an Element ID
//
// INPUT:
// - nElemId:   Element ID to update
// - colVal:    RGB color to change to
//
// RETURN:
// - none
//
void microSDL_ElemSetTxtCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colVal);


//
// Update the Font selected for an Element's text
//
// INPUT:
// - nElemId:   Element ID to update
// - nFont:     Font ID to select
//
// RETURN:
// - none
//
void microSDL_ElemUpdateFont(microSDL_tsGui* pGui,int nElemId,unsigned nFont);


//
// Update a Gauge element's current value
// - Note that min & max values are assigned in create()
//
// INPUT:
// - nElemId:   Element ID to update
// - nVal:      New value to show in gauge
//
// RETURN:
// - none
//
void microSDL_ElemUpdateGauge(microSDL_tsGui* pGui,int nElemId,int nVal);



// ------------------------------------------------------------------------
// Tracking Functions
// ------------------------------------------------------------------------

//
// Fetch the index of the last clicked element
//
// RETURN:
// - Element Index or MSDL_IND_NONE if no new elements selected
//
int microSDL_GetTrackElemClicked(microSDL_tsGui* pGui);

//
// Resets the indicator of a last clicked element,
// which should be called after the previous
// element returned by microSDL_GetTrackElemClicked()
// has been handled.
//
// RETURN:
// - none
//
void microSDL_ClearTrackElemClicked(microSDL_tsGui* pGui);

//
// Handles a touch event and performs the necessary
// tracking, hover and selection actions depending
// on the press state.
//
// INPUT:
// - nX:      X coordinate of touch event
// - nY:      Y coordinate of touch event
// - nPress:  Pressure level of touch event (0 for none, else touch)
//
// RETURN:
// - none
//
void microSDL_TrackClick(microSDL_tsGui* pGui,int nX,int nY,unsigned nPress);


//
// Get the last touch event from the SDL_Event handler
//
// OUTPUT:
// - nX:      X coordinate of last touch event
// - nY:      Y coordinate of last touch event
// - nPress:  Pressure level of last touch event (0 for none, 1 for touch)
//
// RETURN:
// - true if an event was detected or false otherwise
//
bool microSDL_GetSdlClick(microSDL_tsGui* pGui,int &nX, int &nY, unsigned &nPress);


// ------------------------------------------------------------------------
// Touchscreen Functions
// ------------------------------------------------------------------------

#ifdef INC_TS

//
// Initialize the touchscreen device
//
// INPUT:
// - acDev: Device path to touchscreen
//          eg. "/dev/input/touchscreen"
//
// RETURN:
// - true if successful
//
bool microSDL_InitTs(microSDL_tsGui* pGui,const char* acDev);


//
// Get the last touch event from the tslib handler
//
// OUTPUT:
// - nX:      X coordinate of last touch event
// - nY:      Y coordinate of last touch event
// - nPress:  Pressure level of last touch event (0 for none, >0 for touch)
//
// RETURN:
// - non-zero if an event was detected or 0 otherwise
//
int microSDL_GetTsClick(microSDL_tsGui* pGui,int &nX, int &nY, unsigned &nPress);


#endif // INC_TS




// ------------------------------------------------------------------------
// Private Functions
// - The following functions are generally not required for
//   typical users of microSdl. However, for advanced usage
//   more direct access may be required.
// ------------------------------------------------------------------------


//
// Convert an RGB color triplet into the surface pixel value
//
// INPUT:
// - nCol: RGB value for conversion
//
// RETURN:
// - A pixel value for the current screen format
//
Uint32 microSDL_GenPixelColor(microSDL_tsGui* pGui,SDL_Color nCol);


//
// Get the pixel at (X,Y)
// - Based on code from:
// -   https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
//
// INPUT:
// - surf:  SDL surface pointer
// - nX:    Pixel X coordinate
// - nY:    Pixel Y coordinate
//
// PRE:
// - Surface must be locked
//
// RETURN:
// - Pixel color value from the coordinate or 0 if error
//
Uint32 microSDL_GetPixelRaw(microSDL_tsGui* pGui,SDL_Surface *surf,int nX,int nY);


//
// Set the pixel at (X,Y) on a surface to the given pixel value
//
// INPUT:
// - surf:       SDL surface pointer
// - nX:         Pixel X coordinate
// - nY:         Pixel Y coordinate
// - nPixelVal:  Pixel color value
//
// PRE:
// - Surface must be locked
//
// RETURN:
// - none
//
void microSDL_PutPixelRaw(microSDL_tsGui* pGui,SDL_Surface *surf, int nX, int nY, Uint32 nPixelVal);


//
// Set a pixel on the active screen to the given color
//
// INPUT:
// - nX:         Pixel X coordinate to set
// - nY:         Pixel Y coordinate to set
// - nPixelCol:  Color pixel value ot assign
//
// PRE:
// - The surface must be locked first
//
// RETURN:
// - none
//
void microSDL_SetPixelRaw(microSDL_tsGui* pGui,Sint16 nX,Sint16 nY,Uint32 nPixelCol);


//
// Lock an SDL surface so that direct pixel manipulation
// can be done safely.
//
// POST:
// - Primary screen surface is locked
//
// RETURN:
// - true if success, false otherwise
//
bool microSDL_Lock(microSDL_tsGui* pGui);


//
// Unlock the SDL surface after pixel manipulation is
// complete.
//
// POST:
// - Primary screen surface is unlocked
//
// RETURN:
// - none
//
void microSDL_Unlock(microSDL_tsGui* pGui);


//
// Create a new element with basic styling
//
// INPUT:
// - nElemId:   User-supplied ID for referencing this element
//              (or MSDL_ID_ANON to auto-generate)
// - nPageId:   The page ID on which this page should be associated
//              (or MSDL_PAGE_ALL) for all pages
// - nType:     Enumeration that indicates the type of element
//              that is requested for creation. The type adjusts
//              the visual representation and default styling.
// - rElem:     Rectangle region framing the element
// - pStr:      Pointer to string for textual elements
// - nFontId:   Font ID for textual elements
//
// RETURN:
// - Initialized structure
//
microSDL_tsElem microSDL_ElemCreate(microSDL_tsGui* pGui,int nElemId,unsigned nPageId,unsigned nType,
  SDL_Rect rElem,const char* pStr,unsigned nFontId);


//
// Perform range check on Element Index
//
// RETURN:
// - true if index is in range, false otherwise
//
bool microSDL_ElemIndValid(microSDL_tsGui* pGui,int nElemInd);

//
// Fetch the element ID from the element index
//
// INPUT:
// - nElemInd:    Element Index
//
// RETURN:
// - Element ID or MSDL_ID_NONE if not found
//
int microSDL_ElemGetIdFromInd(microSDL_tsGui* pGui,int nElemInd);


//
// Set an element to use a bitmap image
//
// INPUT:
// - nElemId:     Element ID to update
// - acImage:     String of filename for image
// - acImageSel:  String of filename for image (selected)
//
// RETURN:
// - none
//
// TODO: Replace sElem
void microSDL_ElemSetImage(microSDL_tsGui* pGui,microSDL_tsElem* sElem,const char* acImage,
  const char* acImageSel);


//
// Draw an element on the screen
// - Also updates the active display
//
// INPUT:
// - nElemInd:   Element index to draw
//
// RETURN:
// - true if success, false otherwise
//
bool microSDL_ElemDrawByInd(microSDL_tsGui* pGui,int nElemInd);


//
// Draw a gauge element on the screen
// - Called from microSDL_ElemDraw()
//
// INPUT:
// - sElem:  Element structure
//
// RETURN:
// - true if success, false otherwise
//
bool microSDL_ElemDraw_Gauge(microSDL_tsGui* pGui,microSDL_tsElem sElem);


//
// Free up any surfaces associated with the Elements
// - Called by microSDL_Quit()
//
void microSDL_ElemCloseAll(microSDL_tsGui* pGui);


//
// Handle a mouse-down event and track any
// button hover state changes.
//
// This routine is called by microSDL_TrackClick().
//
// INPUT:
// - nX:    X coordinate of event
// - nY:    Y coordinate of event
//
// RETURN:
// - none
//
void microSDL_TrackTouchDownClick(microSDL_tsGui* pGui,int nX,int nY);

//
// Handle a mouse-up event and track any
// button selection state changes
//
// This routine is called by microSDL_TrackClick().
//
// INPUT:
// - nX:    X coordinate of event
// - nY:    Y coordinate of event
//
// RETURN:
// - none
//
void microSDL_TrackTouchUpClick(microSDL_tsGui* pGui,int nX,int nY);


//
// Handle a mouse-move event and track any
// button hover state changes
//
// This routine is called by microSDL_TrackClick().
//
// INPUT:
// - nX:    X coordinate of event
// - nY:    Y coordinate of event
//
// RETURN:
// - none
//
void microSDL_TrackTouchDownMove(microSDL_tsGui* pGui,int nX,int nY);



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MICROSDL_H_
