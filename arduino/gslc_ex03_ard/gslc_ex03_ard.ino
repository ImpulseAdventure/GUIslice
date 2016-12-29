//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 03 (Arduino):
//     Accept touch input, graphic button
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//   - IMPORTANT: This sketch uses graphics located on an external SD card
//     accessed via a SPI interface. The GUIslice_config.h needs to set
//     #define ADAGFX_SD_EN 1
// - This example assumes that two image files have been created in the
//   root of the SD card directory: "exit_n24.bmp" and "exit_g24.bmp"
//   representing the quit button normal and glowing states. The file
//   format is 24-bit BMP. These files have been included for reference in
//   /arduino/res/
// - To support a reasonable number of GUI elements, it is recommended to
//   use an Arduino CPU that provides more than 2KB of SRAM.

#include "GUIslice.h"
#include "GUIslice_drv.h"



// Defines for resources
#define IMG_BTN_QUIT      "exit_n24.bmp"
#define IMG_BTN_QUIT_SEL  "exit_g24.bmp"


// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT};

bool        m_bQuit = false;

// Instantiate the GUI
gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;

#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    6
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];


// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}


bool InitOverlays()
{
  gslc_tsElem*  pElem = NULL;
  
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);  
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with image label
  pElem = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,(gslc_tsRect){258,70,32,32},
          gslc_GetImageFromSD(IMG_BTN_QUIT,GSLC_IMGREF_FMT_BMP24),
          gslc_GetImageFromSD(IMG_BTN_QUIT_SEL,GSLC_IMGREF_FMT_BMP24),
          &CbBtnQuit);
  gslc_ElemSetFillEn(pElem,true); // On slow displays disable transparency to prevent full redraw

  return true;
}

void setup()
{

  // -----------------------------------
  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,NULL,0)) { exit(1); }  

  // -----------------------------------
  // Create the graphic elements
  InitOverlays(); 
  
  // -----------------------------------
  // Start display

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {
    
    // Periodically call GUIslice update function
    gslc_Update(&m_gui);
    
  } // bQuit


  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

}

void loop() {}


