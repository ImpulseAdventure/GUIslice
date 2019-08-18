//<File !Start!>
// FILE: [ex42_bld_ring.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XRingGauge.h"
#include "elem/XSlider.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeSans12pt7b.h"
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN};
enum {E_ELEM_BOX1,E_ELEM_BTN_QUIT,E_ELEM_SLIDER1,E_ELEM_XRING};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_SANS12,E_FONT_TXT5,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                1

#define MAX_ELEM_PG_MAIN 4                                          // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsXRingGauge               m_sXRingGauge1;
gslc_tsXSlider                  m_sXSlider1;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool    m_bQuit = false;

int16_t m_nSliderPos = 60;

uint16_t m_nLoops = 0;
int16_t m_nCount = 0;
bool m_bCountUp = true;
uint32_t m_nTimeLast = 0;


// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemQuit       = NULL;
gslc_tsElemRef*  m_pElemSlider1    = NULL;
gslc_tsElemRef*  m_pElemXRingGauge1= NULL;
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
      case E_ELEM_BTN_QUIT:
        m_bQuit = true;
        gslc_ElemSetTxtStr(&m_gui,m_pElemQuit,"DONE");
        gslc_ElemSetCol(&m_gui,m_pElemQuit,GSLC_COL_RED,GSLC_COL_BLACK,GSLC_COL_BLACK);
        break;

//<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;
  int16_t         nVal;

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>
    case E_ELEM_SLIDER1:
      // Fetch the slider position
      m_nSliderPos = gslc_ElemXSliderGetPos(pGui,m_pElemSlider1);
      break;

//<Slider Enums !End!>
    default:
      break;
  }

  return true;
}
//<Tick Callback !Start!>
//<Tick Callback !End!>

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_BTN_QUIT button with modifiable text label
  static char m_strbtn1[7] = "QUIT";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){235,5,80,30},
    (char*)m_strbtn1,7,E_FONT_TXT5,&CbBtnCommon);
  m_pElemQuit = pElemRef;

  // Create ring gauge E_ELEM_XRING 
  static char m_sRingText1[11] = "";
  pElemRef = gslc_ElemXRingGaugeCreate(&m_gui,E_ELEM_XRING,E_PG_MAIN,&m_sXRingGauge1,
          (gslc_tsRect){80,80,100,100},
          (char*)m_sRingText1,11,E_FONT_SANS12);
  gslc_ElemXRingGaugeSetValRange(&m_gui, pElemRef, 0, 100);
  gslc_ElemXRingGaugeSetVal(&m_gui, pElemRef, 60); // Set initial value
  gslc_ElemXRingGaugeSetThickness(&m_gui,pElemRef, 15);
  gslc_ElemXRingGaugeSetColorActiveGradient(&m_gui, pElemRef, GSLC_COL_BLUE_LT4, GSLC_COL_RED);
  m_pElemXRingGauge1 = pElemRef;

  // Create slider E_ELEM_SLIDER1 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_ELEM_SLIDER1,E_PG_MAIN,&m_sXSlider1,
          (gslc_tsRect){200,80,100,20},0,100,60,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_BLUE_DK1,10,5,GSLC_COL_BLUE_DK1);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSlider1 = pElemRef;
//<InitGUI !End!>

  return true;
}

void setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  Serial.begin(9600);
  // Wait for USB Serial 
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  gslc_InitDebug(&DebugOut);

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_FONT_SANS12,GSLC_FONTREF_PTR,&FreeSans12pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_TXT5,GSLC_FONTREF_PTR,NULL,1)) { return; }
//<Load_Fonts !End!>

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

//<Startup !Start!>
//<Startup !End!>

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{
  char acStr[10];

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  // No filtering enabled -- update the XRingGauge immediately

  // Update the XRingGauge position with the slider position
  gslc_ElemXRingGaugeSetVal(&m_gui, m_pElemXRingGauge1, m_nSliderPos);

  // Update the XRingGauge text string with a percentage
  snprintf(acStr, 10, "%d%%", m_nSliderPos);
  gslc_ElemSetTxtStr(&m_gui, m_pElemXRingGauge1, acStr);

  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) {}
  }
}
