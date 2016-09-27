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

#define MSDL_ID_NONE    -11
#define MSDL_ID_ANON    -12
#define MSDL_PAGE_NONE  -21
#define MSDL_PAGE_ALL   -22
#define MSDL_FONT_NONE  -31

// Element types
enum {MSDL_TYPE_NONE, MSDL_TYPE_BKGND, MSDL_TYPE_BTN, MSDL_TYPE_TXT,
    MSDL_TYPE_GAUGE, MSDL_TYPE_BOX, MSDL_TYPE_LINE};

// Element text alignment
enum {MSDL_ALIGN_NONE,
      MSDL_ALIGN_H_LEFT,MSDL_ALIGN_H_MID,MSDL_ALIGN_H_RIGHT,
      MSDL_ALIGN_V_TOP,MSDL_ALIGN_V_MID,MSDL_ALIGN_V_BOT};

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

  char            acStr[ELEM_STRLEN_MAX+1];
  SDL_Color       colElemText;
  unsigned        eTxtAlignH;
  unsigned        eTxtAlignV;
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




// ------------------------------------------------------------------------
// General Functions
// ------------------------------------------------------------------------

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
void microSDL_InitEnv();


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
bool microSDL_Init();

//
// Exit the microSDL environment
// - Calls SDL Quit to clean up any initialized subsystems
//   and also deletes any created elements
//
// RETURN:
// - None
//
void microSDL_Quit();


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
SDL_Surface *microSDL_LoadBmp(char* pStrFname);



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
bool microSDL_SetBkgnd(SDL_Surface* pSurf);

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
bool microSDL_SetBkgndImage(char* pStrFname);

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
bool microSDL_SetBkgndColor(SDL_Color nCol);


//
// Copy one surface to another.
// - This is typically used to copy a shape
//   to the main screen surface.
//
void microSDL_ApplySurface(int x, int y, SDL_Surface* pSrc, SDL_Surface* pDest);


//
// Determine if a coordinate is inside of a rectangular region.
// - This routine is useful in determining if a touch
//   coordinate is inside of a button.
//
// RETURN:
// - true if inside region, false otherwise
//
bool microSDL_IsInRect(unsigned nSelX,unsigned nSelY,SDL_Rect rBtn);


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
void microSDL_Flip();




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
void microSDL_SetPixel(Sint16 nX,Sint16 nY,SDL_Color nCol);


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
void microSDL_Line(Sint16 nX0,Sint16 nY0,Sint16 nX1,Sint16 nY1,SDL_Color nCol);


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
void microSDL_LineH(Sint16 nX, Sint16 nY, Uint16 nW,SDL_Color nCol);


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
void microSDL_LineV(Sint16 nX, Sint16 nY, Uint16 nH,SDL_Color nCol);


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
void microSDL_FrameRect(Sint16 nX,Sint16 nY,Uint16 nW,Uint16 nH,SDL_Color nCol);


// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------

//
// Setup text / font support
//
// RETURN:
// - true if success, false if fail
//
bool microSDL_InitFont();


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
bool microSDL_FontAdd(unsigned nFontId, const char* acFontName, unsigned nFontSz);


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
TTF_Font* microSDL_FontGet(unsigned nFontId);


//
// Close all loaded fonts
//
// RETURN:
// - none
//
void microSDL_FontCloseAll();



// ------------------------------------------------------------------------
// Page Functions
// ------------------------------------------------------------------------

//
// Fetch the current page ID
//
// RETURN:
// - Page ID
//

unsigned microSDL_GetPageCur();


//
// Select a new page for display
//
// INPUT:
// - nPageId:   Page ID to select as current
//
// RETURN:
// - none
//
void microSDL_SetPageCur(unsigned nPageId);


//
// Draw a specific page
//
// INPUT:
// - nPageId:   Page ID to draw
//
// RETURN:
// - none
//
void microSDL_ElemDrawPage(unsigned nPageId);


//
// Draw the current page
//
// RETURN:
// - none
//
void microSDL_ElemDrawPageCur();

// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------

//
// Look up the Element Index from the Element ID
//
// INPUT:
// - nId:    ID of the element to find
//
// RETURN:
// - Index of element or MSDL_ID_NONE if not found
//
int microSDL_ElemFindIndFromId(unsigned nId);


// Find an Element Index from coordinates
// - Ignore any elements not marked as clickable
//
// RETURN:
// - Index of element or MSDL_ID_NONE if not found
//
int microSDL_ElemFindFromCoord(int nX, int nY);




// ------------------------------------------------------------------------
// Element Creation Functions
// ------------------------------------------------------------------------


//
// Create a Text Element
// - Draws a text string with filled background
//
// INPUT:
// - nId:     Element ID to assign
// - nPage:   Page ID to attach element to
// - rElem:   Rectangle coordinates defining text background size
// - pStr:    String to copy into element
// - nFontId: Font ID to use for text display
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateTxt(int nId,unsigned nPage,
  SDL_Rect rElem,const char* pStr,unsigned nFontId);


//
// Create a textual Button Element
// - Creates a clickable element that has a textual label
//   with frame and fill
//
// INPUT:
// - nId:     Element ID to assign
// - nPage:   Page ID to attach element to
// - rElem:   Rectangle coordinates defining text background size
// - acStr:   String to copy into element
// - nFontId: Font ID to use for text display
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateBtnTxt(int nId,unsigned nPage,
  SDL_Rect rElem,const char* acStr,unsigned nFontId);


//
// Create a graphical Button Element
// - Creates a clickable element that uses a BMP image with
//   no frame or fill
// - Transparency is supported by bitmap color (0xFF00FF)
//
// INPUT:
// - nId:       Element ID to assign
// - nPage:     Page ID to attach element to
// - rElem:     Rectangle coordinates defining image size
// - acImg:     Filename of PNG image to load (unselected state)
// - acImgSel:  Filename of BMP image to load (selected state)
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateBtnImg(int nId,unsigned nPage,SDL_Rect rElem,const char* acImg,const char* acImgSel);


//
// Create a Box Element
// - Draws a box with frame and fill
//
// INPUT:
// - nId:     Element ID to assign
// - nPage:   Page ID to attach element to
// - rElem:   Rectangle coordinates defining box size
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateBox(int nId,unsigned nPage,SDL_Rect rElem);


//
// Create a Gauge Element
// - Draws a horizontal or vertical box with a filled region
//   corresponding to the proportion that nVal represents
//   between nMin and nMax.
//
// INPUT:
// - nId:     Element ID to assign
// - nPage:   Page ID to attach element to
// - rElem:   Rectangle coordinates defining gauge size
// - nMin:    Minimum value of gauge for nVal comparison
// - nMax:    Maximum value of gauge for nVal comparison
// - colGauge:  Color to fill the gauge with
// - bVert:     Flag to indicate vertical vs horizontal action
//              (1 = vertical, 0 = horizontal)
//
// RETURN:
// - The created Element structure
//
microSDL_tsElem microSDL_ElemCreateGauge(int nId,unsigned nPage,SDL_Rect rElem,int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert);


//
// Add the Element to the microSDL
bool microSDL_ElemAdd(microSDL_tsElem sElem);


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
void microSDL_ElemDraw(int nElemId);


// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

/* //xxx
void microSDL_ElemSetStyle(microSDL_tsElem* sElem,SDL_Color colFill,
  SDL_Color colFrame,SDL_Color colText,SDL_Color colFillSel);
*/
void microSDL_ElemSetStyleMain(microSDL_tsElem* sElem,SDL_Color colFrame,
  SDL_Color colFill,SDL_Color colFillSel);
void microSDL_ElemSetImage(microSDL_tsElem* sElem,const char* acImage,
  const char* acImageSel);

//
// Update the text string associated with an Element ID
//
// INPUT:
// - nId:     Element ID to update
// - pStr:    String to copy into element
//
// RETURN:
// - none
//
void microSDL_ElemUpdateTxt(unsigned nId,const char* pStr);

//
// Update the text string color associated with an Element ID
//
// INPUT:
// - nId:     Element ID to update
// - colVal:  RGB color to change to
//
// RETURN:
// - none
//
void microSDL_ElemUpdateTxtCol(unsigned nId,SDL_Color colVal);


//
// Update the Font selected for an Element's text
//
// INPUT:
// - nId:     Element ID to update
// - nFont:   Font ID to select
//
// RETURN:
// - none
//
void microSDL_ElemUpdateFont(unsigned nId,unsigned nFont);


//
// Update a Gauge element's current value
// - Note that min & max values are assigned in create()
//
// INPUT:
// - nId:     Element ID to update
// - nVal:    New value to show in gauge
//
// RETURN:
// - none
//
void microSDL_ElemUpdateGauge(unsigned nId,int nVal);



// ------------------------------------------------------------------------
// Tracking Functions
// ------------------------------------------------------------------------

//
// Fetch the index of the last clicked element
//
// RETURN:
// - Element Index or -1 if no new elements selected
//
int microSDL_GetTrackElemClicked();

//
// Resets the indicator of a last clicked element,
// which should be called after the previous
// element returned by microSDL_GetTrackElemClicked()
// has been handled.
//
void microSDL_ClearTrackElemClicked();

//
// Handles a touch event and performs the necessary
// tracking, hover and selection actions depending
// on the press state.
//
// INPUT:
// - 
void microSDL_TrackClick(int nX,int nY,unsigned nPress);


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
bool microSDL_GetSdlClick(int &nX, int &nY, unsigned &nPress);


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
bool microSDL_InitTs(const char* acDev);


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
int microSDL_GetTsClick(int &nX, int &nY, unsigned &nPress);


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
Uint32 microSDL_GenPixelColor(SDL_Color nCol);


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
Uint32 microSDL_GetPixelRaw(SDL_Surface *surf,int nX,int nY);


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
void microSDL_PutPixelRaw(SDL_Surface *surf, int nX, int nY, Uint32 nPixelVal);


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
void microSDL_SetPixelRaw(Sint16 nX,Sint16 nY,Uint32 nPixelCol);


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
bool microSDL_Lock();


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
void microSDL_Unlock();


//
// Create a new element with basic styling
//
// INPUT:
// - nId:       User-supplied ID for referencing this element
//              (or MSDL_ID_ANON for don't-care)
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
microSDL_tsElem microSDL_ElemCreate(int nId,unsigned nPageId,unsigned nType,
  SDL_Rect rElem,const char* pStr,unsigned nFontId);


//
// Fetch the element ID from the element index
//
// RETURN:
// - Index of element or MSDL_ID_NONE if not found
//
int microSDL_ElemGetId(int nElemInd);


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
bool microSDL_ElemDrawByInd(int nElemInd);


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
bool microSDL_ElemDraw_Gauge(microSDL_tsElem sElem);


//
// Free up any surfaces associated with the Elements
// - Called by microSDL_Quit()
//
void microSDL_ElemCloseAll();


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
void microSDL_TrackTouchDownClick(int nX,int nY);

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
void microSDL_TrackTouchUpClick(int nX,int nY);


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
void microSDL_TrackTouchDownMove(int nX,int nY);



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MICROSDL_H_
