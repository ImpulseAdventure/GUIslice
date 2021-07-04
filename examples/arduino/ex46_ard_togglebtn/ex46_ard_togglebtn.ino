// FILE: [ex46_ard_togglebtn.ino]
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
// - Example 46 (Arduino): 
//   - Demonstrates toggle buttons, Android (Rectangle) and ios (circular) styles 
//   - Shows callback notifications for toogle buttons
//   - Provide example of additional Adafruit-GFX fonts
//
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
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
#include "elem/XTogglebtn.h"

//#define EXTRA_FONTS

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
#if defined(EXTRA_FONTS)
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeSans9pt7b.h"
#endif


// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
enum {E_PG_MAIN};
enum {E_BTN_PLAY,E_BTN_SHOW,E_ELEM_BOX1,E_ELEM_BOX3,E_LBL_APP
      ,E_LBL_NOTIFICATIONS,E_LBL_PLAY,E_LBL_SHOW};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_SANS9,MAX_FONT};

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
#define MAX_PAGE                1

#define MAX_ELEM_PG_MAIN 8                                          // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsXTogglebtn               m_sXTogglebtn1;
gslc_tsXTogglebtn               m_sXTogglebtn2;

#define MAX_STR                 100

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
gslc_tsElemRef*  m_pElemPlayApp    = NULL;
gslc_tsElemRef*  m_pElemShowApp    = NULL;

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = pElemRef->pElem;
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
      case E_BTN_SHOW:
        if (gslc_ElemXTogglebtnGetState(pGui, m_pElemShowApp))
          Serial.println("Show Notifications");
        else 
          Serial.println("Don't Show Notifications");
        break;
      case E_BTN_PLAY:
        if (gslc_ElemXTogglebtnGetState(pGui, m_pElemPlayApp))
          Serial.println("Play Notification sounds");
        else
          Serial.println("Don't Play Notification sounds");
        break;

      default:
        break;
    } // switch
  }
  return true;
}

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,(gslc_tsColor){55,0,190});

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){10,60,300,150});
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_WHITE);
   
  // Create E_ELEM_BOX3 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX3,E_PG_MAIN,(gslc_tsRect){10,47,150,30});
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_WHITE,GSLC_COL_WHITE);
  
  // Create E_LBL_NOTIFICATIONS text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_NOTIFICATIONS,E_PG_MAIN,(gslc_tsRect){17,52,130,30},
    (char*)"Notifications",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_ORANGE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_WHITE,GSLC_COL_WHITE);
  
  // Create E_LBL_SHOW text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_SHOW,E_PG_MAIN,(gslc_tsRect){20,100,176,23},
    (char*)"Show app notications:",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_WHITE,GSLC_COL_WHITE);
   
  // create toggle button E_BTN_SHOW
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_BTN_SHOW,E_PG_MAIN,&m_sXTogglebtn1,
    (gslc_tsRect){230,100,60,30},
    GSLC_COL_GRAY_LT2,GSLC_COL_GREEN_DK3,GSLC_COL_RED_DK1,true, // circular
    false,&CbBtnCommon);
  gslc_ElemSetGlowCol(&m_gui,pElemRef,GSLC_COL_RED_LT1,GSLC_COL_BLACK,GSLC_COL_WHITE);
  m_pElemShowApp = pElemRef;
  
  // Create E_LBL_PLAY text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_PLAY,E_PG_MAIN,(gslc_tsRect){20,150,194,23},
    (char*)"Play notification sounds:",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_WHITE,GSLC_COL_WHITE);
   
  // create toggle button E_BTN_PLAY
  pElemRef = gslc_ElemXTogglebtnCreate(&m_gui,E_BTN_PLAY,E_PG_MAIN,&m_sXTogglebtn2,
    (gslc_tsRect){230,150,60,30},
    GSLC_COL_GRAY_LT2,GSLC_COL_GREEN_DK3,GSLC_COL_RED_DK1,false, // rectangular
    false,&CbBtnCommon);
  gslc_ElemSetGlowCol(&m_gui,pElemRef,GSLC_COL_RED_LT1,GSLC_COL_BLACK,GSLC_COL_WHITE);
  m_pElemPlayApp = pElemRef;
  
  // Create E_LBL_APP text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_APP,E_PG_MAIN,(gslc_tsRect){0,0,320,32},
    (char*)"My App",0,E_FONT_SANS9);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);

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
  #if defined(EXTRA_FONTS)
    if (!gslc_FontSet(&m_gui,E_FONT_SANS9,GSLC_FONTREF_PTR,&FreeSans9pt7b,1)) { return; }
  #else
    if (!gslc_FontSet(&m_gui,E_FONT_SANS9,GSLC_FONTREF_PTR,NULL,1)) { return; }
  #endif

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  
  // Update the default focus colors due to white background
  gslc_SetFocusCol(&m_gui,GSLC_COL_WHITE,GSLC_COL_MAGENTA,GSLC_COL_RED);

  InitGUI();

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
}
