//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 11 (Arduino):
//   - show the usage of the touch handler class, this class allows easy user adaption of new touch screens
//   - Accept touch input, text button
//   - Expected behavior: Clicking on button is shown by a lighter button color
//

// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"


//specific touch handler class
//in order to adopt for new touch displays:
//  - copying this file to your home dir
//  - rename it 
//  - edit it
//  - include your adopted touch handler  
//  - instantiate your touch handler
//  - done
#include "GUIslice_th_XPT2046.h"


// Defines for resources

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT};
enum {E_FONT_BTN};

bool    m_bQuit = false;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_FONT            1
#define MAX_ELEM_PG_MAIN    2

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];


//instantiate the touch handler
#if defined(__STM32F1__)
  //usefull values for STM32  
  SPIClass touchSPI(2);
  TouchHandler_XPT2046 touchHandler = TouchHandler_XPT2046(/*spi=*/ touchSPI, /*spi_cs_pin=*/ PB12);
#else  
  //values for Arduino, to be confirmed
  SPIClass touchSPI(2);
  TouchHandler_XPT2046 touchHandler = TouchHandler_XPT2046(/*spi=*/ touchSPI, /*spi_cs_pin=*/ 0);
#endif


// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

void setup()
{
  gslc_tsElemRef* pElemRef = NULL;

  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used


  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // register touchHandler and start it
  gslc_InitTouchHandler(&touchHandler);

  // Load Fonts
  if (!gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1)) { return; }

  // -----------------------------------
  // Create page elements
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){220,150,80,40},(char*)"Quit",0,E_FONT_BTN,&CbBtnQuit);  //place the botton out of the middle to be able to check if flipping / swapping is correct

  // -----------------------------------
  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;
}

void loop()
{
  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will do nothing.
  
  //if (m_bQuit) {
  //  gslc_Quit(&m_gui);
  //  while (1) { }
  //}
  
}



