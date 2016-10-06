//
// MicroSDL GUI Library Examples
// - Calvin Hass ( http://www.impulseadventure.com/elec/ )
// - Example 02: Accept touch input
//

#include "microsdl.h"


// Defines for resources
#define IMG_BTN_QUIT      "./res/btn-exit32x32.bmp"
#define IMG_BTN_QUIT_SEL  "./res/btn-exit_sel32x32.bmp"


// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT};

// Instantiate the GUI
#define MAX_ELEM  30
microSDL_tsGui  m_gui;
microSDL_tsElem m_asElem[MAX_ELEM];



int main( int argc, char* args[] )
{
  bool              bOk = true;
  bool              bQuit = false;  
  int               nRet;
  int               nClickX,nClickY;
  unsigned          nClickPress;
  int               nElemId;
  int               nTrackElemClicked;

  // -----------------------------------
  // Initialize

  microSDL_InitEnv(&m_gui);
  microSDL_Init(&m_gui,m_asElem,MAX_ELEM);

  microSDL_InitTs(&m_gui,"/dev/input/touchscreen");


  // -----------------------------------
  // Create page elements

  // Create background box
  nElemId = microSDL_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetCol(&m_gui,nElemId,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Create Quit button with image label
  nElemId = microSDL_ElemCreateBtnImg(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
          (SDL_Rect){264,8,32,32},IMG_BTN_QUIT,IMG_BTN_QUIT_SEL);

  // -----------------------------------
  // Start display

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);
  microSDL_ElemDrawPageCur(&m_gui);

  // -----------------------------------
  // Main event loop

  bQuit = false;
  while (!bQuit) {
  
    // Poll for touchscreen presses
    if (microSDL_GetTsClick(&m_gui,nClickX,nClickY,nClickPress)) {
 
      // Track the touch event and find any associated object
      microSDL_TrackClick(&m_gui,nClickX,nClickY,nClickPress);
      nTrackElemClicked = microSDL_GetTrackElemClicked(&m_gui);

      // Any selectable object clicked? (MSDL_ID_NONE if no)
      if (nTrackElemClicked == E_ELEM_BTN_QUIT) {
        // Quit button pressed
        bQuit = true;
      }
  
      // Clear click event
      microSDL_ClearTrackElemClicked(&m_gui);
  
    } // Touchscreen press
  } // bQuit


  // -----------------------------------
  // Close down display

  microSDL_Quit(&m_gui);
}

