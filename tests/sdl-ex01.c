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

int main( int argc, char* args[] )
{
  microSDL_tsElem  sElem;

  // Initialize
  microSDL_InitEnv();
  microSDL_Init();

  // Create page elements
  sElem = microSDL_ElemCreateBox(E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  //xxx microSDL_ElemSetStyle(&sElem,m_colBlack,m_colWhite);
  microSDL_ElemSetStyleMain(&sElem,m_colWhite,m_colBlack,m_colBlack);
  microSDL_ElemAdd(sElem);

  // Start up display on main page
  microSDL_SetPageCur(E_PG_MAIN);
  microSDL_ElemDrawPageCur();
  
  // Main code here...
  SDL_Delay(2000);
  
  // Close down display
  microSDL_Quit();
}

