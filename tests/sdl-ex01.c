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
microSDL_tsGui  m_gui;

#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    30
microSDL_tsPage             m_asPage[MAX_PAGE];
microSDL_tsElem             m_asPageElem[MAX_ELEM_PG_MAIN];


int main( int argc, char* args[] )
{
  microSDL_tsElem*  pElem = NULL;

  // Initialize
  microSDL_InitEnv(&m_gui);
  if (!microSDL_Init(&m_gui,m_asPage,MAX_PAGE,NULL,0,NULL,0)) { exit(1); }  

  microSDL_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);  
  
  // Background flat color
  microSDL_SetBkgndColor(&m_gui,MSDL_COL_GRAY_DK);

  // Create page elements
  pElem = microSDL_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetCol(pElem,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);

  // Periodically call microSDL update function    
  microSDL_Update(&m_gui);
  
  // Main code here...
  SDL_Delay(2000);
  
  // Close down display
  microSDL_Quit(&m_gui);

  return 0;
}

