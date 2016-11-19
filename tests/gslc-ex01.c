//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 01: Display a box
//

#include "GUIslice.h"

#include <unistd.h>      // For sleep()

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX};

// Instantiate the GUI
gslc_tsGui  m_gui;

#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    30
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];


int main( int argc, char* args[] )
{
  gslc_tsElem*  pElem = NULL;

  // Initialize
  gslc_InitEnv(&m_gui);
  if (!gslc_Init(&m_gui,m_asPage,MAX_PAGE,NULL,0,NULL,0)) { exit(1); }  

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);  
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create page elements
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_Rect){10,50,300,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // Periodically call GUIslice update function    
  gslc_Update(&m_gui);
  
  // Main code here...
  sleep(2);
  
  // Close down display
  gslc_Quit(&m_gui);

  return 0;
}

