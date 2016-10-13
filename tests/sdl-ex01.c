//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 01: Display a box
//

#include "microsdl.h"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX};

// Instantiate the GUI
#define MAX_ELEM  30
microSDL_tsGui  m_gui;
microSDL_tsElem m_asElem[MAX_ELEM];


int main( int argc, char* args[] )
{
  int     nElemId;

  // Initialize
  microSDL_InitEnv(&m_gui);
  if (!microSDL_Init(&m_gui,m_asElem,MAX_ELEM,NULL,0,NULL,0)) { exit(1); }

  // Background flat color
  microSDL_SetBkgndColor(&m_gui,MSDL_COL_GRAY_DK);

  // Create page elements
  nElemId = microSDL_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetCol(&m_gui,nElemId,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);
  microSDL_ElemDrawPageCur(&m_gui);
  
  // Main code here...
  SDL_Delay(2000);
  
  // Close down display
  microSDL_Quit(&m_gui);

  return 0;
}

