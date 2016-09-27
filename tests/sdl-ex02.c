//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 02: Accept touch input
//

#include "microsdl.h"


// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT};
enum {E_FONT_BTN};


int main( int argc, char* args[] )
{
  bool              bOk = true;
  bool              bQuit = false;  
  int               nRet;
  int               nClickX,nClickY;
  unsigned          nClickPress;
  int               nElemId;
  int               nTrackElemClicked;
  microSDL_tsElem   sElem;  

  // -----------------------------------
  // Initialize

  microSDL_InitEnv();
  microSDL_Init();

  microSDL_InitFont();
  microSDL_InitTs("/dev/input/touchscreen");

  // Load Fonts
  bOk = microSDL_FontAdd(E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { printf("ERROR: microSDL_FontAdd() failed\n"); return false; }


  // -----------------------------------
  // Create page elements

  // Create background box
  sElem = microSDL_ElemCreateBox(E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  //xxx microSDL_ElemSetStyle(&sElem,m_colBlack,m_colWhite,m_colBlack,m_colWhite);
  microSDL_ElemSetStyleMain(&sElem,m_colWhite,m_colBlack,m_colBlack);
  microSDL_ElemAdd(sElem);

  // Create Quit button with text label
  sElem = microSDL_ElemCreateBtnTxt(E_ELEM_BTN_QUIT,E_PG_MAIN,
  (SDL_Rect){120,100,80,40},"Quit",E_FONT_BTN);
  microSDL_ElemAdd(sElem);

  // -----------------------------------
  // Start display

  // Start up display on main page
  microSDL_SetPageCur(E_PG_MAIN);
  microSDL_ElemDrawPageCur();

  // -----------------------------------
  // Main event loop

  bQuit = false;
  while (!bQuit) {
  
    // Poll for touchscreen presses
    if (microSDL_GetTsClick(nClickX,nClickY,nClickPress)) {
 
      // Track the touch event and find any associated object
      microSDL_TrackClick(nClickX,nClickY,nClickPress);
      nTrackElemClicked = microSDL_GetTrackElemClicked();

      // Any selectable object clicked?
      if (nTrackElemClicked != MSDL_ID_NONE) {

        // Convert element index to element ID
        nElemId = microSDL_ElemGetId(nTrackElemClicked);
        if (nElemId == E_ELEM_BTN_QUIT) {
          // Quit button pressed
          bQuit = true;
        }
  
        // Clear click event
        microSDL_ClearTrackElemClicked();
  
      } // Object clicked
    } // Touchscreen press
  } // bQuit


  // -----------------------------------
  // Close down display

  microSDL_FontCloseAll();
  microSDL_Quit();
}

