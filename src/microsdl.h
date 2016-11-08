#ifndef _MICROSDL_H_
#define _MICROSDL_H_

// =======================================================================
// microSDL library
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.4    (2016/11/08)
// =======================================================================

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"


// -----------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------

#define MSDL_ELEM_STRLEN_MAX  80  // Max string length of text elements

// Enable for bitmap transparency and definition of color to use
#define MSDL_BMP_TRANS_EN     1               // 1 = enabled, 0 = disabled
#define MSDL_BMP_TRANS_RGB    0xFF,0x00,0xFF  // RGB color (default:pink)

// -----------------------------------------------------------------------
// Enumerations
// -----------------------------------------------------------------------

  /// Element ID enumerations
  /// - The Element ID is the primary means for user code to
  ///   reference a graphic element.
  /// - Application code can assign arbitrary Element ID values
  ///   in the range of 0...32767
  /// - Specifying MSDL_ID_AUTO to ElemCreate() requests that
  ///   microSDL auto-assign an ID value for the Element. These
  ///   auto-assigned values will begin at MSDL_ID_AUTO_BASE.
  /// - Negative Element ID values are reserved 
  enum {
    // Public usage
    MSDL_ID_USER_BASE       = 0,      ///< Starting Element ID for user assignments
    MSDL_ID_NONE            = -1999,  ///< No Element ID has been assigned
    MSDL_ID_AUTO,                     ///< Auto-assigned Element ID requested
    MSDL_ID_TEMP,                     ///< ID for Temporary Element
    // Internal usage
    MSDL_ID_AUTO_BASE       = 32768,  ///< Starting Element ID to start auto-assignment
                                      ///< (when MSDL_ID_AUTO is specified)            
  };


  /// Page ID enumerations
  /// - The Page ID is the primary means for user code to
  ///   reference a specific page of elements.
  /// - Application code can assign arbitrary Page ID values
  ///   in the range of 0...32767
  /// - Negative Page ID values are reserved   
  enum {
    // Public usage
    MSDL_PAGE_USER_BASE     = 0,      ///< Starting Page ID for user assignments
    MSDL_PAGE_ALL           = -2999,  ///< Page ID used to indicate an element is
                                      ///< assigned to appear on all pages.
    // Internal usage
    MSDL_PAGE_NONE,                   ///< No Page ID has been assigned
  };

  /// View ID enumerations
  /// - The View ID is the primary means for user code to
  ///   reference a specific viewport.
  /// - Application code can assign arbitrary View ID values
  ///   in the range of 0...32767
  /// - Negative View ID values are reserved   
  enum {
    // Public usage
    MSDL_VIEW_ID_USER_BASE  = 0,      ///< Starting Page ID for user assignments
    MSDL_VIEW_ID_SCREEN     = -3999,  ///< Default Viewport ID used for main screen
    // Internal usage
    MSDL_VIEW_ID_NONE,                ///< No Viewport ID has been assigned
  };

  /// Group ID enumerations
  enum {
    // Public usage
    MSDL_GROUP_ID_USER_BASE = 0,      ///< Starting Group ID for user assignments
    // Internal usage
    MSDL_GROUP_ID_NONE      = -6999,  ///< No Group ID has been assigned
  };
  
  /// Font ID enumerations
  /// - The Font ID is the primary means for user code to
  ///   reference a specific font.
  /// - Application code can assign arbitrary Font ID values
  ///   in the range of 0...32767
  /// - Negative Font ID values are reserved    
  enum {
    // Public usage
    MSDL_FONT_USER_BASE     = 0,      ///< Starting Font ID for user assignments
    MSDL_FONT_NONE          = -4999,  ///< No Font ID has been assigned
  };
  

  /// View Index enumerations
  /// - The View Index is used for internal purposes as an offset
  //    into the GUI's array of views
  enum {
    // Internal usage
    MSDL_VIEW_IND_NONE      = -5999,  ///< No Viewport Index is available
    MSDL_VIEW_IND_SCREEN,             ///< Default Viewport Index used for main screen
  };
  
  /// Element Index enumerations
  /// - The Element Index is used for internal purposes as an offset
  //    into the GUI's array of elements
  enum {
    // Internal usage
    MSDL_IND_NONE           = -9999,  ///< No Element Index is available
    MSDL_IND_TEMP           = 0,      ///< Temporary element at index 0
    MSDL_IND_FIRST          = 1,      ///< User elements start at index 1
  };


/// Element type
typedef enum {
    // Core elements:
    MSDL_TYPE_NONE,         ///< No element type specified
    MSDL_TYPE_BKGND,        ///< Background element type
    MSDL_TYPE_BTN,          ///< Button element type
    MSDL_TYPE_TXT,          ///< Text label element type
    MSDL_TYPE_BOX,          ///< Box / frame element type
    MSDL_TYPE_LINE,         ///< Line element type
    // Extended elements:
    MSDL_TYPEX_GAUGE,       ///< Guage / progressbar extended element
    MSDL_TYPEX_CHECKBOX,    ///< Checkbox extended element
    MSDL_TYPEX_SLIDER,      ///< Slider extended element
    MSDL_TYPEX_SELNUM,      ///< SelNum extended element
} microSDL_teType;

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
#define MSDL_COL_BLUE_DK    (SDL_Color) {0,0,255}       ///< Blue (dark)
#define MSDL_COL_BLUE_LT    (SDL_Color) {128,128,255}   ///< Blue (light)
#define MSDL_COL_RED_DK     (SDL_Color) {255,0,0}       ///< Red (dark)
#define MSDL_COL_RED_LT     (SDL_Color) {255,128,128}   ///< Red (light)
#define MSDL_COL_GREEN_DK   (SDL_Color) {0,255,0}       ///< Green (dark)
#define MSDL_COL_GREEN_LT   (SDL_Color) {128,255,128}   ///< Green (light)
#define MSDL_COL_YELLOW     (SDL_Color) {255,255,0}     ///< Yellow
#define MSDL_COL_YELLOW_DK  (SDL_Color) {64,64,0}       ///< Yellow (dark)
#define MSDL_COL_BLACK      (SDL_Color) {0,0,0}         ///< Black
#define MSDL_COL_GRAY_DK    (SDL_Color) {64,64,64}      ///< Gray (dark)
#define MSDL_COL_GRAY       (SDL_Color) {128,128,128}   ///< Gray
#define MSDL_COL_GRAY_LT    (SDL_Color) {192,192,192}   ///< Gray (light)
#define MSDL_COL_WHITE      (SDL_Color) {255,255,255}   ///< White
#define MSDL_COL_PURPLE     (SDL_Color) {128,0,128}     ///< Purple
#define MSDL_COL_CYAN       (SDL_Color) {0,255,255}     ///< Cyan
#define MSDL_COL_MAGENTA    (SDL_Color) {255,0,255}     ///< Magenta
#define MSDL_COL_TEAL       (SDL_Color) {0,128,128}     ///< Teal
#define MSDL_COL_ORANGE     (SDL_Color) {255,165,0}     ///< Orange
#define MSDL_COL_BROWN      (SDL_Color) {165,42,42}     ///< Brown

/// Touch event type for element touch tracking
typedef enum  {
    MSDL_TOUCH_DOWN_IN,    /// Touch event (down) inside element, start tracking
    MSDL_TOUCH_MOVE_IN,    /// Touch event (move/drag) inside tracked element
    MSDL_TOUCH_MOVE_OUT,   /// Touch event (move/drag) outside tracked element
    MSDL_TOUCH_UP_IN,      /// Touch event (up) inside tracked element
    MSDL_TOUCH_UP_OUT,     /// Touch event (up) outside tracked element
} microSDL_teTouch;

// -----------------------------------------------------------------------
// Structures
// -----------------------------------------------------------------------

// Callback function for element drawing
typedef bool (*MSDL_CB_DRAW)(void* pvGui,void* pvElem);

// Callback function for element touch tracking
typedef bool (*MSDL_CB_TOUCH)(void* pvGui,void *pvElem,microSDL_teTouch eTouch,int nX,int nY);

///
/// Element Struct
/// - Represents a single graphic element in the microSDL environment
/// - A page is made up of a number of elments
/// - Each element is created with a user-specified ID for further
///   accesses (or MSDL_ID_AUTO for it to be auto-generated)
/// - Display order of elements in a page is based upon the creation order
/// - An element can be associated with all pages with nPage=MSDL_PAGE_ALL
/// - Extensions to the core element types is provided through the
///   pXData reference and pfuncX* callback functions.
///
typedef struct {
  bool                bValid;         ///< Element was created properly
  int                 nId;            ///< Element ID specified by user
  int                 nPage;          ///< Page ID containing this element
  microSDL_teType     nType;          ///< Element type enumeration
  SDL_Rect            rElem;          ///< Rect region containing element
  int                 nGroup;         ///< Group ID that the element belongs to
  
  bool                bGlowing;       ///< Element is currently glowing

  SDL_Surface*        pSurf;          ///< Surface ptr to draw (normal)
  SDL_Surface*        pSurfGlow;      ///< Surface ptr to draw (glowing)

  bool                bClickEn;       ///< Element accepts touch events
  bool                bFrameEn;       ///< Element is drawn with frame
  bool                bFillEn;        ///< Element is drawn with inner fill.
                                      ///< This is also used during redraw to determine
                                      ///< if elements underneath are visible and must
                                      ///< be redrawn as well.

  SDL_Color           colElemFrame;   ///< Color for frame
  SDL_Color           colElemFill;    ///< Color for background fill
  SDL_Color           colElemGlow;    ///< Color to use when touch cause glowing

  bool                bNeedRedraw;    ///< Element needs to be redrawn

  char                acStr[MSDL_ELEM_STRLEN_MAX+1];  ///< Text string to overlay
  SDL_Color           colElemText;    ///< Color of overlay text
  unsigned            eTxtAlign;      ///< Alignment of overlay text
  unsigned            nTxtMargin;     ///< Margin of overlay text within rect region
  TTF_Font*           pTxtFont;       ///< Font ptr for overlay text
 
  // Sub-element tracking
  int                 nSubIdTrack;    ///< Sub-element ID being tracked (TODO)
  
  void*               pXData;         ///< Ptr to extended data structure
  
  /// Callback func ptr for drawing
  MSDL_CB_DRAW        pfuncXDraw;
  
  /// Callback func ptr for touch
  MSDL_CB_TOUCH       pfuncXTouch;
  
  
} microSDL_tsElem;



/// Font reference structure
typedef struct {
  int         nId;      ///< Font ID specified by user
  TTF_Font*   pFont;    ///< Ptr to the Font (TTF_Font)
} microSDL_tsFont;


/// Viewport structure
/// - Viewports allow drawing operations to be done within
///   a local coordinate system and clipped to a region.
typedef struct {
  int         nId;        ///< View ID specified by user
  SDL_Rect    rView;      ///< Rect defining the viewport
  unsigned    nOriginX;   ///< Position within rView for local origin
  unsigned    nOriginY;   ///< Position within rView for local origin
} microSDL_tsView;

/// GUI structure
/// - Contains all GUI state and content
typedef struct {

  int               nPageIdCur;   ///< Currently active Page ID

  microSDL_tsFont*  psFont;       ///< Collection of loaded fonts
  unsigned          nFontMax;     ///< Maximum number of fonts to allocate
  unsigned          nFontCnt;     ///< Number of fonts allocated

  microSDL_tsElem*  psElem;           ///< Collection of graphic elements
                                      ///< (across all pages)
  unsigned          nElemMax;         ///< Maximum number of elements to allocate
  unsigned          nElemCnt;         ///< Number of elements allocated
  unsigned          nElemAutoIdNext;  ///< Next Element ID for auto-assignment

  microSDL_tsView*  psView;               ///< Collection of viewports
  unsigned          nViewMax;             ///< Maximum number of viewports to allocate
  unsigned          nViewCnt;             ///< Number of viewports allocated
  int               nViewIndCur;          ///< Currently-active viewport index

  int               nTrackElemIdStart;    ///< Element ID currently being touch-tracked.
                                          ///< Set to MSDL_ID_NONE if no elements are
                                          ///< currently being tracked.
  
  int               nClickLastX;          ///< Last touch event X coord
  int               nClickLastY;          ///< Last touch event Y coord
  unsigned          nClickLastPress;      ///< Last touch event pressure (0=none))

  // Touchscreen library interface
  #ifdef INC_TS
  struct tsdev*     ts;                   ///< Ptr to touchscreen device
  #endif

  // Redraw
  bool              bPageNeedRedraw;      ///< Page require a redraw
  bool              bPageNeedFlip;        ///< Screen requires a page flip
  
  // Primary surface definitions
  SDL_Surface*      surfScreen;           ///< Surface ptr for screen
  SDL_Surface*      surfBkgnd;            ///< Surface ptr for background
  
} microSDL_tsGui;




// ------------------------------------------------------------------------
// General Functions
// ------------------------------------------------------------------------


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
void microSDL_InitEnv(microSDL_tsGui* pGui);


///
/// Initialize the microSDL library
/// - Configures the primary screen surface
/// - Initializes font support
///
/// PRE:
/// - The environment variables should be configured before
///   calling microSDL_Init(). This can be done with microSDL_InitEnv()
///   or manually in user function.
///
/// POST:
/// - microSDL_m_surfScreen is initialized
///
/// \param[in]  pGui:      Pointer to GUI
/// \param[in]  psElem:    Pointer to Element array
/// \param[in]  nMaxElem:  Size of Element array
/// \param[in]  psFont:    Pointer to Font array
/// \param[in]  nMaxFont:  Size of Font array
/// \param[in]  psView:    Pointer to View array
/// \param[in]  nMaxView:  Size of View array
///
/// \return true if success, false if fail
///
bool microSDL_Init(microSDL_tsGui* pGui,microSDL_tsElem* psElem,unsigned nMaxElem,microSDL_tsFont* psFont,unsigned nMaxFont,microSDL_tsView* psView,unsigned nMaxView);


///
/// Exit the microSDL environment
/// - Calls SDL Quit to clean up any initialized subsystems
///   and also deletes any created elements or fonts
///
/// \param[in]  pGui:    Pointer to GUI
///
/// \return None
///
void microSDL_Quit(microSDL_tsGui* pGui);


///
/// Perform main microSDL handling functions
/// - Handles any touch events
/// - Performs any necessary screen redraw
///
/// \param[in]  pGui:    Pointer to GUI
///
/// \return None
///
void microSDL_Update(microSDL_tsGui* pGui);

// ------------------------------------------------------------------------
// Graphics General Functions
// ------------------------------------------------------------------------

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
SDL_Surface *microSDL_LoadBmp(microSDL_tsGui* pGui,char* pStrFname);



///
/// Configure the background image to use the provided surface.
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pSurf:       Surface to use for the background image
///
/// \return true if success, false if fail
///
bool microSDL_SetBkgnd(microSDL_tsGui* pGui,SDL_Surface* pSurf);

///
/// Configure the background to use a bitmap image
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pStrFname:   Filename to BMP file
///
/// \return true if success, false if fail
///
bool microSDL_SetBkgndImage(microSDL_tsGui* pGui,char* pStrFname);

///
/// Configure the background to use a solid color
/// - The background is used when redrawing the entire page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB Color to use
///
/// \return true if success, false if fail
///
bool microSDL_SetBkgndColor(microSDL_tsGui* pGui,SDL_Color nCol);


///
/// Copy one surface to another.
/// - This is typically used to copy a shape
///   to the main screen surface.
///
/// \param[in]  pGui:          Pointer to GUI
/// \param[in]  nX:            Destination X coordinate of copy
/// \param[in]  nY:            Destination Y coordinate of copy
/// \param[in]  pSrc:          Ptr to source surface (eg. a loaded image)
/// \param[in]  pDest:         Ptr to destination surface (typically the screen)
///
/// \return none
///
void microSDL_ApplySurface(microSDL_tsGui* pGui,int x, int y, SDL_Surface* pSrc, SDL_Surface* pDest);


///
/// Determine if a coordinate is inside of a rectangular region.
/// - This routine is useful in determining if a touch
///   coordinate is inside of a button.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nSelX:       X coordinate to test
/// \param[in]  nSelY:       X coordinate to test
/// \param[in]  rRect:       Rectangular region to compare against
///
/// \return true if inside region, false otherwise
///
bool microSDL_IsInRect(microSDL_tsGui* pGui,int nSelX,int nSelY,SDL_Rect rRect);

///
/// Determine if a coordinate is inside of a width x height region.
/// - This routine is useful in determining if a relative coordinate
///   is within a given W x H dimension
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nSelX:       X coordinate to test
/// \param[in]  nSelY:       X coordinate to test
/// \param[in]  nWidth:      Width to test against
/// \param[in]  nHeight:     Height to test against
///
/// \return true if inside region, false otherwise
///
bool microSDL_IsInWH(microSDL_tsGui* pGui,int nSelX,int nSelY,Uint16 nWidth,Uint16 nHeight);

///
/// Update the visible screen with any drawing changes made
/// - On some hardware this can trigger a double-buffering
///   page flip.
/// - This call forces a page flip irrespective of the
///   flag set by microSDL_PageFlipSet().
///
/// PRE:
/// - microSDL_Init() has been called
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return None
///
void microSDL_Flip(microSDL_tsGui* pGui);


///
/// Indicate whether the screen requires page flip
/// - This is generally called with bNeeded=true whenever
///   drawing has been done to the active page. Page flip
///   is actually performed later when calling PageFlipGo().
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bNeeded:     True if screen requires page flip
///
/// \return None
///
void microSDL_PageFlipSet(microSDL_tsGui* pGui,bool bNeeded);


///
/// Get state of pending page flip state
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return True if screen requires page flip
///
bool microSDL_PageFlipGet(microSDL_tsGui* pGui);


///
/// Update the visible screen if page has been marked for flipping
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return None
///
void microSDL_PageFlipGo(microSDL_tsGui* pGui);




// ------------------------------------------------------------------------
// Graphics Primitive Functions
// ------------------------------------------------------------------------


///
/// Set a pixel on the active screen to the given color with lock
/// - Calls upon microSDL_SetPixelRaw() but wraps with a surface lock lock
/// - If repeated access is needed, use microSDL_SetPixelRaw() instead
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          Pixel X coordinate to set
/// \param[in]  nY:          Pixel Y coordinate to set
/// \param[in]  nPixelCol:   Color pixel value ot assign
/// \param[in]  bMapEn:      Support viewport remapping
///
/// \return none
///
void microSDL_SetPixel(microSDL_tsGui* pGui,Sint16 nX,Sint16 nY,SDL_Color nCol,bool bMapEn);


///
/// Draw an arbitrary line using Bresenham's algorithm
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX0:         X coordinate of line startpoint
/// \param[in]  nY0:         Y coordinate of line startpoint
/// \param[in]  nX1:         X coordinate of line endpoint
/// \param[in]  nY1:         Y coordinate of line endpoint
/// \param[in]  nCol:        Color RGB value for the line
///
/// \return none
///
void microSDL_Line(microSDL_tsGui* pGui,Sint16 nX0,Sint16 nY0,Sint16 nX1,Sint16 nY1,SDL_Color nCol);


///
/// Draw a horizontal line
/// - Note that direction of line is in +ve X axis
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          X coordinate of line startpoint
/// \param[in]  nY:          Y coordinate of line startpoint
/// \param[in]  nW:          Width of line (in +X direction)
/// \param[in]  nCol:        Color RGB value for the line
///
/// \return none
///
void microSDL_LineH(microSDL_tsGui* pGui,Sint16 nX, Sint16 nY, Uint16 nW,SDL_Color nCol);


///
/// Draw a vertical line
/// - Note that direction of line is in +ve Y axis
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          X coordinate of line startpoint
/// \param[in]  nY:          Y coordinate of line startpoint
/// \param[in]  nH:          Height of line (in +Y direction)
/// \param[in]  nCol:        Color RGB value for the line
///
/// \return none
///
void microSDL_LineV(microSDL_tsGui* pGui,Sint16 nX, Sint16 nY, Uint16 nH,SDL_Color nCol);


///
/// Draw a framed rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to frame
/// \param[in]  nCol:        Color RGB value for the frame
///
/// \return none
///
void microSDL_FrameRect(microSDL_tsGui* pGui,SDL_Rect rRect,SDL_Color nCol);


///
/// Draw a filled rectangle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  rRect:       Rectangular region to fill
/// \param[in]  nCol:        Color RGB value to fill
///
/// \return none
///
void microSDL_FillRect(microSDL_tsGui* pGui,SDL_Rect rRect,SDL_Color nCol);


///
/// Expand or contract a rectangle in width and/or height (equal
/// amounts on both side), based on the centerpoint of the rectangle.
///
/// \param[in]  rRect:       Rectangular region before resizing
/// \param[in]  nExpandW:    Number of pixels to expand the width (if positive)
///                of contract the width (if negative)
/// \param[in]  nExpandH:    Number of pixels to expand the height (if positive)
///                of contract the height (if negative)
/// \return SDL_Rect() with resized dimensions
///
SDL_Rect microSDL_ExpandRect(SDL_Rect rRect,Sint16 nExpandW,Sint16 nExpandH);


///
/// Draw a framed circle
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nMidX:       Center X coordinate
/// \param[in]  nMidY:       Center Y coordinate
/// \param[in]  nRadius:     Radius of circle
/// \param[in]  nCol:        Color RGB value for the frame
///
/// \return none
///
void microSDL_FrameCircle(microSDL_tsGui* pGui,Sint16 nMidX,Sint16 nMidY,
  Uint16 nRadius,SDL_Color nCol);

// -----------------------------------------------------------------------
// Font Functions
// -----------------------------------------------------------------------


///
/// Load a font into the local font cache and assign
/// font ID (nFontId).
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nFontId:     ID to use when referncing this font
/// \param[in]  acFontName:  Filename path to the font
/// \param[in]  nFontSz:     Typeface size to use
///
/// \return true if load was successful, false otherwise
///
bool microSDL_FontAdd(microSDL_tsGui* pGui,int nFontId, const char* acFontName, unsigned nFontSz);


///
/// Fetch a font from its ID value
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nFontId:     ID value used to reference the font (supplied
///                originally to microSDL_FontAdd()
///
/// \return A TTF_Font pointer or NULL if error
///
TTF_Font* microSDL_FontGet(microSDL_tsGui* pGui,int nFontId);




// ------------------------------------------------------------------------
// Page Functions
// ------------------------------------------------------------------------

///
/// Fetch the current page ID
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return Page ID
///

int microSDL_GetPageCur(microSDL_tsGui* pGui);


///
/// Select a new page for display
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nPageId:     Page ID to select as current
///
/// \return none
///
void microSDL_SetPageCur(microSDL_tsGui* pGui,int nPageId);


///
/// Update the need-redraw status for the current page
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bRedraw:     True if redraw required, false otherwise
///
/// \return none
///
void microSDL_PageRedrawSet(microSDL_tsGui* pGui,bool bRedraw);


///
/// Get the need-redraw status for the current page
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return True if redraw required, false otherwise
///
bool microSDL_PageRedrawGet(microSDL_tsGui* pGui);


///
/// Redraw all elements on the active page. Only the
/// elements that have been marked as needing redraw are
/// rendered unless the entire page has been marked as
/// needing redraw (in which case everything is drawn)
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return none
///
void microSDL_PageRedrawGo(microSDL_tsGui* pGui);


// ------------------------------------------------------------------------
// Element General Functions
// ------------------------------------------------------------------------


///
/// Look up the Element Index from the Element ID
///
/// \param[in]  asELem:      Pointer to array of elements
/// \param[in]  nNumElem:    Number of elements in array
/// \param[in]  nElemId:     ID of the element to find
///
/// \return Index of element or MSDL_ID_NONE if not found
///
int microSDL_ElemFindIndFromId(microSDL_tsElem* asElem,unsigned nNumElem,int nElemId);


/// Find an Element Index from coordinates
/// - Ignore any elements not marked as clickable
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return Index of element or MSDL_IND_NONE if not found
///
int microSDL_ElemFindIndFromCoord(microSDL_tsGui* pGui,int nX, int nY);


/// Get an Element ID from an element structure
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to element structure
///
/// \return ID of element or MSDL_ID_NONE if not found
///
int microSDL_ElemGetIdFromElem(microSDL_tsGui* pGui,microSDL_tsElem* pElem);


// ------------------------------------------------------------------------
// Element Creation Functions
// ------------------------------------------------------------------------


///
/// Create a Text Element
/// - Draws a text string with filled background
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining text background size
/// \param[in]  pStr:        String to copy into element
/// \param[in]  nFontId:     Font ID to use for text display
///
/// \return The Element ID or MSDL_ID_NONE if failure
///
int microSDL_ElemCreateTxt(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* pStr,int nFontId);


///
/// Create a textual Button Element
/// - Creates a clickable element that has a textual label
///   with frame and fill
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining text background size
/// \param[in]  acStr:       String to copy into element
/// \param[in]  nFontId:     Font ID to use for text display
/// \param[in]  cbTouch:     Callback for touch events
///
/// \return The Element ID or MSDL_ID_NONE if failure
///
int microSDL_ElemCreateBtnTxt(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acStr,int nFontId,MSDL_CB_TOUCH cbTouch);


///
/// Create a graphical Button Element
/// - Creates a clickable element that uses a BMP image with
///   no frame or fill
/// - Transparency is supported by bitmap color (0xFF00FF)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining image size
/// \param[in]  acImg:       Filename of BMP image to load (unselected state)
/// \param[in]  acImgSel:    Filename of BMP image to load (selected state)
/// \param[in]  cbTouch:     Callback for touch events
///
/// \return The Element ID or MSDL_ID_NONE if failure
///
int microSDL_ElemCreateBtnImg(microSDL_tsGui* pGui,int nElemId,int nPage,
  SDL_Rect rElem,const char* acImg,const char* acImgSel,MSDL_CB_TOUCH cbTouch);


///
/// Create a Box Element
/// - Draws a box with frame and fill
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining box size
///
/// \return The Element ID or MSDL_ID_NONE if failure
///
int microSDL_ElemCreateBox(microSDL_tsGui* pGui,int nElemId,int nPage,SDL_Rect rElem);


///
/// Create an image Element
/// - Draws an image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to assign (0..32767 or MSDL_ID_AUTO to autogen)
/// \param[in]  nPage:       Page ID to attach element to
/// \param[in]  rElem:       Rectangle coordinates defining box size
/// \param[in]  acImg:       Filename of BMP image to load
///
/// \return The Element ID or MSDL_ID_NONE if failure
///
int microSDL_ElemCreateImg(microSDL_tsGui* pGui,int nElemId,int nPage,SDL_Rect rElem,
  const char* acImg);



// ------------------------------------------------------------------------
// Element Drawing Functions
// ------------------------------------------------------------------------


///
/// Draw a graphic element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     ID of element
///
/// \return none
///
void microSDL_ElemDraw(microSDL_tsGui* pGui,int nElemId);


// ------------------------------------------------------------------------
// Element Update Functions
// ------------------------------------------------------------------------

///
/// Set the fill state for an Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  bFillEn:     True if filled, false otherwise
///
/// \return none
///
void microSDL_ElemSetFillEn(microSDL_tsGui* pGui,int nElemId,bool bFillEn);


///
/// Update the common color selection for an Element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  colFrame:    Color for the frame
/// \param[in]  colFill:     Color for the fill
/// \param[in]  colGlow:     Color when glowing
///
/// \return none
///
void microSDL_ElemSetCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colFrame,SDL_Color colFill,SDL_Color colGlow);


///
/// Set the group ID for an element
/// - Typically used to associate radio button elements together
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  nGroupId:    Group ID to assign
///
/// \return none
///
void microSDL_ElemSetGroup(microSDL_tsGui* pGui,int nElemId,int nGroupId);


///
/// Get the group ID for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
///
/// \return Group ID or MSDL_GROUP_ID_NONE if unassigned
///
int microSDL_ElemGetGroup(microSDL_tsGui* pGui,int nElemId);


/// Set the alignment of a textual element (horizontal and vertical)
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nId:         Element ID to update
/// \param[in]  nAlign:      Alignment to specify:
///                           - MSDL_ALIGN_TOP_LEFT
///                           - MSDL_ALIGN_TOP_MID
///                           - MSDL_ALIGN_TOP_RIGHT
///                           - MSDL_ALIGN_MID_LEFT
///                           - MSDL_ALIGN_MID_MID
///                           - MSDL_ALIGN_MID_RIGHT
///                           - MSDL_ALIGN_BOT_LEFT
///                           - MSDL_ALIGN_BOT_MID
///                           - MSDL_ALIGN_BOT_RIGHT
///
/// \return none
///
void microSDL_ElemSetTxtAlign(microSDL_tsGui* pGui,int nElemId,unsigned nAlign);


///
/// Update the text string associated with an Element ID
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  pStr:        String to copy into element
///
/// \return none
///
void microSDL_ElemSetTxtStr(microSDL_tsGui* pGui,int nElemId,const char* pStr);

///
/// Update the text string color associated with an Element ID
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  colVal:      RGB color to change to
///
/// \return none
///
void microSDL_ElemSetTxtCol(microSDL_tsGui* pGui,int nElemId,SDL_Color colVal);


///
/// Update the Font selected for an Element's text
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  nFontId:     Font ID to select
///
/// \return none
///
void microSDL_ElemUpdateFont(microSDL_tsGui* pGui,int nElemId,int nFontId);


///
/// Update the need-redraw status for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  bRedraw:     True if redraw required, false otherwise
///
/// \return none
///
void microSDL_ElemSetRedraw(microSDL_tsGui* pGui,int nElemId,bool bRedraw);


///
/// Update the glowing indicator for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  bGlowing:    True if element is glowing
///
/// \return none
///
void microSDL_ElemSetGlow(microSDL_tsGui* pGui,int nElemId,bool bGlowing);


///
/// Get the glowing indicator for an element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to examine
///
/// \return True if element is glowing
///
bool microSDL_ElemGetGlow(microSDL_tsGui* pGui,int nElemId);

///
/// Assign the drawing callback function for an element
/// - This allows the user to override the default rendering for
///   an element, enabling the creation of a custom element
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemId:     Element ID to update
/// \param[in]  funcCb:      Function pointer to drawing routine
///
/// \return none
///
void microSDL_ElemSetDrawFunc(microSDL_tsGui* pGui,int nElemId,MSDL_CB_DRAW funcCb);


// ------------------------------------------------------------------------
// Viewport Functions
// ------------------------------------------------------------------------

///
/// Create a Viewport
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewId:     View ID to assign (0..32767)
/// \param[in]  rView:       Rectangle coordinates defining viewport position
/// \param[in]  nOriginX:    Relative X coordinates of origin
/// \param[in]  nOriginY:    Relative Y coordinates of origin
///
/// \return The Viewport ID or MSDL_VIEW_ID_NONE if failure
///
int microSDL_ViewCreate(microSDL_tsGui* pGui,int nViewId,SDL_Rect rView,unsigned nOriginX,unsigned nOriginY);


///
/// Set the origin for a Viewport
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewId:     Viewport ID to update
/// \param[in]  nOriginX:    Relative X coordinates of origin
/// \param[in]  nOriginY:    Relative Y coordinates of origin
///
/// \return true if success, false if failure
///
bool microSDL_ViewSetOrigin(microSDL_tsGui* pGui,int nViewId,unsigned nOriginX,unsigned nOriginY);

///
/// Sets the currently active view
/// - This function is used to swap between drawing within
///   a viewport (using local coordinates and clipping) and
///   the main view (the screen).
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewId:     Viewport ID to select or MSDL_VIEW_ID_SCREEN for main display
///
/// \return none
///
void microSDL_ViewSet(microSDL_tsGui* pGui,int nViewId);


// ------------------------------------------------------------------------
// Tracking Functions
// ------------------------------------------------------------------------


///
/// Handles a touch event and performs the necessary
/// tracking, glowing and selection actions depending
/// on the press state.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          X coordinate of touch event
/// \param[in]  nY:          Y coordinate of touch event
/// \param[in]  nPress:      Pressure level of touch event (0 for none, else touch)
///
/// \return none
///
void microSDL_TrackClick(microSDL_tsGui* pGui,int nX,int nY,unsigned nPress);


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
bool microSDL_GetSdlClick(microSDL_tsGui* pGui,int* pnX, int* pnY, unsigned* pnPress);


// ------------------------------------------------------------------------
// Touchscreen Functions
// ------------------------------------------------------------------------

#ifdef INC_TS

///
/// Initialize the touchscreen device
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  acDev:       Device path to touchscreen
///                  eg. "/dev/input/touchscreen"
///
/// \return true if successful
///
bool microSDL_InitTs(microSDL_tsGui* pGui,const char* acDev);


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
int microSDL_GetTsClick(microSDL_tsGui* pGui,int* pnX, int* pnY, unsigned* pnPress);


#endif // INC_TS




// ------------------------------------------------------------------------
// Private Functions
// - The following functions are generally not required for
//   typical users of microSdl. However, for advanced usage
//   more direct access may be required.
// ------------------------------------------------------------------------



///
/// Convert an RGB color triplet into the surface pixel value
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nCol:        RGB value for conversion
///
/// \return A pixel value for the current screen format
///
Uint32 microSDL_GenPixelColor(microSDL_tsGui* pGui,SDL_Color nCol);


///
/// Get the pixel at (X,Y)
/// - Based on code from:
/// -   https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
///
/// PRE:
/// - Surface must be locked
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  surf:        SDL surface pointer
/// \param[in]  nX:          Pixel X coordinate
/// \param[in]  nY:          Pixel Y coordinate
///
/// \return Pixel color value from the coordinate or 0 if error
///
Uint32 microSDL_GetPixelRaw(microSDL_tsGui* pGui,SDL_Surface *surf,int nX,int nY);


///
/// Set the pixel at (X,Y) on a surface to the given pixel value
///
/// PRE:
/// - Surface must be locked
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  surf:        SDL surface pointer
/// \param[in]  nX:          Pixel X coordinate
/// \param[in]  nY:          Pixel Y coordinate
/// \param[in]  nPixelVal:   Pixel color value
///
/// \return none
///
void microSDL_PutPixelRaw(microSDL_tsGui* pGui,SDL_Surface *surf, int nX, int nY, Uint32 nPixelVal);


///
/// Set a pixel on the active screen to the given color
///
/// PRE:
/// - The surface must be locked first
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nX:          Pixel X coordinate to set
/// \param[in]  nY:          Pixel Y coordinate to set
/// \param[in]  nPixelCol:   Color pixel value ot assign
///
/// \return none
///
void microSDL_SetPixelRaw(microSDL_tsGui* pGui,Sint16 nX,Sint16 nY,Uint32 nPixelCol);


///
/// Lock an SDL surface so that direct pixel manipulation
/// can be done safely.
///
/// POST:
/// - Primary screen surface is locked
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return true if success, false otherwise
///
bool microSDL_Lock(microSDL_tsGui* pGui);


///
/// Unlock the SDL surface after pixel manipulation is
/// complete.
///
/// POST:
/// - Primary screen surface is unlocked
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return none
///
void microSDL_Unlock(microSDL_tsGui* pGui);


///
/// Create a new element with default styling
///
/// \param[in]  pGui:         Pointer to GUI
/// \param[in]  nElemId:      User-supplied ID for referencing this element
///                           (or MSDL_ID_AUTO to auto-generate)
/// \param[in]  nPageId:      The page ID on which this page should be associated
///                           (or MSDL_PAGE_ALL) for all pages
/// \param[in]  nType:        Enumeration that indicates the type of element
///                           that is requested for creation. The type adjusts
///                           the visual representation and default styling.
/// \param[in]  rElem:        Rectangle region framing the element
/// \param[in]  pStr:         Pointer to string for textual elements
/// \param[in]  nFontId:      Font ID for textual elements
///
/// \return Initialized structure
///
microSDL_tsElem microSDL_ElemCreate(microSDL_tsGui* pGui,int nElemId,int nPageId,microSDL_teType nType,
  SDL_Rect rElem,const char* pStr,int nFontId);


///
/// Add the Element to the list of generated elements
/// in the GUI environment
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  sElem:       Element to add to environment
///
/// \return true if success, false if fail
///
bool microSDL_ElemAdd(microSDL_tsGui* pGui,microSDL_tsElem sElem);


///
/// Perform range check on Element Index
/// - Range checking is done before Element array access
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemInd:    Element index
///
/// \return true if index is in range, false otherwise
///
bool microSDL_ElemIndValid(microSDL_tsGui* pGui,int nElemInd);

///
/// Fetch the element ID from the element index
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemInd:    Element Index
///
/// \return Element ID or MSDL_ID_NONE if not found
///
int microSDL_ElemGetIdFromInd(microSDL_tsGui* pGui,int nElemInd);


///
/// Fetch the temporary element
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return Contents of temporary element struct
///
microSDL_tsElem microSDL_ElemGetTemp(microSDL_tsGui* pGui);


///
/// Set an element to use a bitmap image
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Pointer to Element to update
/// \param[in]  acImage:     String of filename for image
/// \param[in]  acImageSel:  String of filename for image (selected)
///
/// \return none
///
void microSDL_ElemSetImage(microSDL_tsGui* pGui,microSDL_tsElem* pElem,const char* acImage,
  const char* acImageSel);


/// // TODO: FIXME doc
/// Draw an element on the screen
/// - Also updates the active display
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  pElem:       Ptr to Element to draw
///
/// \return true if success, false otherwise
///
bool microSDL_ElemDrawByRef(microSDL_tsGui* pGui,microSDL_tsElem* pElem);

///
/// Draw an element on the screen
/// - Also updates the active display
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nElemInd:    Element index to draw
///
/// \return true if success, false otherwise
///
bool microSDL_ElemDrawByInd(microSDL_tsGui* pGui,int nElemInd);


///
/// Free up any surfaces associated with the Elements
/// - Called by microSDL_Quit()
/// 
/// \param[in]  pGui:         Pointer to GUI
/// 
/// \return none
///
void microSDL_ElemCloseAll(microSDL_tsGui* pGui);



///
/// Perform range check on Viewport Index
/// - Range checking is done before View array access
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewInd:    Viewport index
///
/// \return true if index is in range, false otherwise
///
bool microSDL_ViewIndValid(microSDL_tsGui* pGui,int nViewInd);

///
/// Look up the Viewport Index from the Viewport ID
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nViewId:     ID of the viewport to find
///
/// \return Index of viewport or MSDL_VIEW_IND_NONE if not found
///
int microSDL_ViewFindIndFromId(microSDL_tsGui* pGui,int nViewId);

///
/// Remap a coordinate from local to global using the
/// viewport settings. The input coordinates will be
/// replaced by the global coordinates.
///
/// \param[in]    pGui:        Pointer to GUI
/// \param[in]    nViewId:     ID of the viewport
/// \param[inout] pnX:         Ptr to X coordinate
/// \param[inout] pnY:         Ptr to Y coordinate
///
/// \return none
///
void microSDL_ViewRemapPt(microSDL_tsGui* pGui,Sint16* pnX,Sint16* pnY);

///
/// Remap a rectangle from local to global using the
/// viewport settings. The input rectangle coordinates will be
/// replaced by the global coordinates.
///
/// \param[in]    pGui:        Pointer to GUI
/// \param[in]    nViewId:     ID of the viewport
/// \param[inout] prRect:      Ptr to Rectangular coordinates to update
///
/// \return none
///
void microSDL_ViewRemapRect(microSDL_tsGui* pGui,SDL_Rect* prRect);


///
/// Handle a mouse-down event and track any
/// button glowing state changes.
///
/// This routine is called by microSDL_TrackClick().
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  nTrackIdNew: Element ID to start tracking
/// \param[in]  nX:          X coordinate of event
/// \param[in]  nY:          Y coordinate of event
///
/// \return none
///
void microSDL_TrackTouchDownClick1(microSDL_tsGui* pGui,int nTrackIdNew,int nX,int nY);

///
/// Handle a mouse-up event and track any
/// button selection state changes
///
/// This routine is called by microSDL_TrackClick().
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bInTracked:  In bounds of tracked element
/// \param[in]  nX:          X coordinate of event
/// \param[in]  nY:          Y coordinate of event
///
/// \return none
///
void microSDL_TrackTouchUpClick1(microSDL_tsGui* pGui,bool bInTracked,int nX,int nY);

///
/// Handle a mouse-move event and track any
/// button glowing state changes
///
/// This routine is called by microSDL_TrackClick().
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  bInTracked:  In bounds of tracked element
/// \param[in]  nX:          X coordinate of event
/// \param[in]  nY:          Y coordinate of event
///
/// \return none
///
void microSDL_TrackTouchDownMove1(microSDL_tsGui* pGui,bool bInTracked,int nX,int nY);

///
/// Notify an element of a touch event. This is an optional
/// behavior useful in some extended element types.
///
/// \param[in]  pGui:        Pointer to GUI
/// \param[in]  eTouch:      Touch event type
/// \param[in]  nX:          X coordinate of event (absolute coordinate)
/// \param[in]  nY:          Y coordinate of event (absolute coordinate)
///
/// \return true if success, false if error
///
bool microSDL_NotifyElemTouch(microSDL_tsGui* pGui,microSDL_teTouch eTouch,int nX,int nY);


///
/// Close all loaded fonts
///
/// \param[in]  pGui:        Pointer to GUI
///
/// \return none
///
void microSDL_FontCloseAll(microSDL_tsGui* pGui);

///
/// Initialize a Font struct
///
/// \param[in]  pFont:       Pointer to Font
///
/// \return none
///
void microSDL_ResetFont(microSDL_tsFont* pFont);

///
/// Initialize an Element struct
///
/// \param[in]  pElem:       Pointer to Element
///
/// \return none
///
void microSDL_ResetElem(microSDL_tsElem* pElem);

///
/// Initialize a View struct
///
/// \param[in]  pView:       Pointer to Viewport
///
/// \return none
///
void microSDL_ResetView(microSDL_tsView* pView);

///
/// Ensure SDL initializes cleanly to workaround
/// possible issues if previous SDL application
/// failed to close down gracefully.
///
/// \param[in]  sTTY:       Terminal device (eg. "/dev/tty0")
///
/// \return true if success
///
bool microSDL_CleanStart(const char* sTTY);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _MICROSDL_H_
