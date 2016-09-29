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

// Main GUI instance
microSDL_tsGui  m_gui;

int main( int argc, char* args[] )
{
  microSDL_tsElem  sElem;

  // Initialize
  microSDL_InitEnv(&m_gui);
  microSDL_Init(&m_gui);

  // Create page elements
  sElem = microSDL_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetCol(&m_gui,sElem.nId,m_colWhite,m_colBlack,m_colBlack);

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);
  microSDL_ElemDrawPageCur(&m_gui);
  
  // Main code here...
  SDL_Delay(2000);
  
  // Close down display
  microSDL_Quit(&m_gui);
}

