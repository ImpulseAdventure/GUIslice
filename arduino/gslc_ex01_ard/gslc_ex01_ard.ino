//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/guislice-gui.html
// - Example 01 (Arduino):
//     Display a box
//     No touchscreen required
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
// - To support a reasonable number of GUI elements, it is recommended to
//   use a CPU that provides more than 2KB of SRAM.
//

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX};

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    5

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

void setup()
{
  gslc_tsElem*  pElem = NULL;

  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,NULL,0)) { exit(1); }  

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);  
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create page elements
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // Periodically call GUIslice update function    
  gslc_Update(&m_gui);
  
  // Main code here...
  delay(2000);
  
  // Close down display
  gslc_Quit(&m_gui);

}

void loop() { }


