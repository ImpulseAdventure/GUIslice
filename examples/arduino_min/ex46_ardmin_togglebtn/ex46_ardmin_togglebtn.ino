// FILE: [ex46_ard_togglebtn.ino]
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
// - Example 46 (Arduino): [minimum RAM version]
//   - Demonstrates toggle buttons, Android (Rectangle) and ios (circular) styles 
//   - Shows callback notifications for toogle buttons
//   - Provide example of additional Adafruit-GFX fonts
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

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
#include <Adafruit_GFX.h>
// Note that these files are located within the Adafruit-GFX library folder:
#include "Fonts/FreeSans9pt7b.h"


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

// Define the maximum number of elements per page
// - To enable the same code to run on devices that support storing
//   data into Flash (PROGMEM) and those that don't, we can make the
//   number of elements in Flash dependent upon GSLC_USE_PROGMEM
// - This should allow both Arduino and ARM Cortex to use the same code
#define MAX_ELEM_PG_MAIN 8  // # Elems total on page
#if (GSLC_USE_PROGMEM)
  #define MAX_ELEM_PG_MAIN_PROG   8                                         // # Elems in Flash
#else
  #define MAX_ELEM_PG_MAIN_PROG   0                                         // # Elems in Flash
#endif
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM

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
  gslc_ElemCreateBox_P(&m_gui,E_ELEM_BOX1,E_PG_MAIN,10,60,300,150,GSLC_COL_BLACK,GSLC_COL_WHITE,
    true,true,NULL,NULL);
   
  // Create E_ELEM_BOX3 box
  gslc_ElemCreateBox_P(&m_gui,E_ELEM_BOX3,E_PG_MAIN,10,47,150,30,GSLC_COL_WHITE,GSLC_COL_WHITE,
    true,true,NULL,NULL);
  
  // Create E_LBL_NOTIFICATIONS text label
  gslc_ElemCreateTxt_P(&m_gui,E_LBL_NOTIFICATIONS,E_PG_MAIN,17,52,130,30,"Notifications:",&m_asFont[E_FONT_SANS9],
     GSLC_COL_ORANGE,GSLC_COL_WHITE,GSLC_COL_WHITE,GSLC_ALIGN_MID_LEFT,false,true);
  
  // Create E_LBL_SHOW text label
  gslc_ElemCreateTxt_P(&m_gui,E_LBL_SHOW,E_PG_MAIN,20,100,176,23,"Show app notications:",&m_asFont[E_FONT_SANS9],
    GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_WHITE,GSLC_ALIGN_MID_LEFT,false,true);
   
  // create toggle button E_BTN_SHOW
  gslc_ElemXTogglebtnCreate_P(&m_gui,E_BTN_SHOW,E_PG_MAIN,230,100,60,30,
    GSLC_COL_GRAY_LT2,GSLC_COL_GREEN_DK3,GSLC_COL_RED_DK1,true, // circular
    false,&CbBtnCommon);
  m_pElemShowApp = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_BTN_SHOW); // Save for quick access
  
  // Create E_LBL_PLAY text label
  gslc_ElemCreateTxt_P(&m_gui,E_LBL_PLAY,E_PG_MAIN,20,150,194,23,"Play notification sounds:",&m_asFont[E_FONT_SANS9],
    GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_WHITE,GSLC_ALIGN_MID_LEFT,false,true);
   
  // create toggle button E_BTN_PLAY
  gslc_ElemXTogglebtnCreate_P(&m_gui,E_BTN_PLAY,E_PG_MAIN,230,150,60,30,
    GSLC_COL_GRAY_LT2,GSLC_COL_GREEN_DK3,GSLC_COL_RED_DK1,false, // rectangular
    false,&CbBtnCommon);
  m_pElemPlayApp = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_BTN_PLAY); // Save for quick access
  
  // Create E_LBL_APP text label
  gslc_ElemCreateTxt_P(&m_gui,E_LBL_APP,E_PG_MAIN,0,0,320,32,"My App",&m_asFont[E_FONT_SANS9],
    GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_MID,false,true);

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
    if (!gslc_FontSet(&m_gui,E_FONT_SANS9,GSLC_FONTREF_PTR,&FreeSans9pt7b,1)) { return; }

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
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
