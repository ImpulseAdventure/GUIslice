//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 17 (Arduino):
//   - Show the usage of dynamic rotation
//   - Accept touch input, text button
//   - Expected behavior: Clicking on button rotates the screen
//

// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"


// Defines for resources

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_ROTATE};
enum {E_FONT_BTN};

bool      m_bRotate = false;
uint8_t   m_nRotation = 1;

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



// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Button callbacks
bool CbBtnRotate(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bRotate = true;
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

  // Load Fonts
  if (!gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1)) { return; }

  // -----------------------------------
  // Create page elements
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,220,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create rotate button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_ROTATE,E_PG_MAIN,
    (gslc_tsRect){20,60,80,40},(char*)"Rotate",0,E_FONT_BTN,&CbBtnRotate);  //place the botton out of the middle to be able to check if flipping / swapping is correct

  // -----------------------------------
  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bRotate = false;
}


void loop()
{
  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we change the rotation

  if (m_bRotate) {
    gslc_GuiRotate(&m_gui, m_nRotation);
    m_nRotation = (m_nRotation+1) % 4;

    m_bRotate = false;

  }
  //delay(1000);

}



