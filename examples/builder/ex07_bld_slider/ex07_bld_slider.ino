//<File !Start!>
// FILE: [ex07_bld_slider.ino]
// Created by GUIslice Builder version: [0.13.0]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
//   - Sliders with dynamic color control and position callback
//   - Expected behavior: Clicking on "SAVE" button terminates program,
//     and no action when clicking on "Selected Room" button
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. 
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
#include "elem/XSlider.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
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
enum {E_DRAW_LINE1,E_DRAW_LINE2,E_ELEM_BOX1,E_ELEM_BTN_KITCHEN
      ,E_ELEM_BTN_QUIT,E_ELEM_COLOR,E_ELEM_TEXT8,E_LBL_BLUE,E_LBL_GREEN
      ,E_LBL_RED,E_LBL_ROOM,E_LBL_SETLED,E_LBL_TITLE,E_SLIDER_B
      ,E_SLIDER_G,E_SLIDER_R};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_TXT15,E_FONT_TXT5,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                1

#define MAX_ELEM_PG_MAIN 16                                         // # Elems total on page
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
gslc_tsXSlider                  m_sXSlider1;
gslc_tsXSlider                  m_sXSlider2;
gslc_tsXSlider                  m_sXSlider3;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

bool      m_bQuit = false;

// Current RGB value for color box
// - Globals defined here for convenience so that callback
//   can update R,G,B components independently
uint8_t   m_nPosR = 255;
uint8_t   m_nPosG = 128;
uint8_t   m_nPosB = 0;

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemColor      = NULL;
gslc_tsElemRef*  m_pElemSave       = NULL;
gslc_tsElemRef*  m_pElemSlider1    = NULL;
gslc_tsElemRef*  m_pElemSlider2    = NULL;
gslc_tsElemRef*  m_pElemSlider3    = NULL;
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
        gslc_ElemSetTxtStr(&m_gui,m_pElemSave,"Saved");
        gslc_ElemSetCol(&m_gui,m_pElemSave,GSLC_COL_RED,GSLC_COL_BLACK,GSLC_COL_BLACK);
        break;

      case E_ELEM_BTN_KITCHEN:
        //TODO- Replace with button handling code
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

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>
    case E_SLIDER_R:
      // Fetch the slider position
      m_nPosR = gslc_ElemXSliderGetPos(pGui,m_pElemSlider1);
      break;
    case E_SLIDER_G:
      // Fetch the slider position
      m_nPosG = gslc_ElemXSliderGetPos(pGui,m_pElemSlider2);
      break;
    case E_SLIDER_B:
      // Fetch the slider position
      m_nPosB = gslc_ElemXSliderGetPos(pGui,m_pElemSlider3);
      break;

//<Slider Enums !End!>
    default:
      break;
  }

  // Calculate the new RGB value
  gslc_tsColor colRGB = (gslc_tsColor){m_nPosR,m_nPosG,m_nPosB};

  // Update the color box
  gslc_ElemSetCol(pGui,m_pElemColor,GSLC_COL_WHITE,colRGB,GSLC_COL_WHITE);

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
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){10,50,300,170});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_LBL_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_TITLE,E_PG_MAIN,(gslc_tsRect){22,0,272,28},
    (char*)"Home Automation",0,E_FONT_TXT15);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,(gslc_tsColor){128,128,240});
   
  // Create E_ELEM_COLOR box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_COLOR,E_PG_MAIN,(gslc_tsRect){20,115,130,100});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,(gslc_tsColor){255,128,0},GSLC_COL_WHITE);
  m_pElemColor = pElemRef;
  
  // Create E_ELEM_BTN_QUIT button with modifiable text label
  static char m_strbtn1[8] = "SAVE";
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){250,60,50,30},
    (char*)m_strbtn1,8,E_FONT_TXT5,&CbBtnCommon);
  m_pElemSave = pElemRef;
  
  // Create E_LBL_ROOM text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_ROOM,E_PG_MAIN,(gslc_tsRect){20,65,86,12},
    (char*)"Selected Room:",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);
  
  // create E_ELEM_BTN_KITCHEN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_KITCHEN,E_PG_MAIN,
    (gslc_tsRect){140,60,80,30},(char*)"Kitchen...",0,E_FONT_TXT5,&CbBtnCommon);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK2,GSLC_COL_GRAY_DK3,GSLC_COL_BLUE_DK1);
  
  // Create E_LBL_SETLED text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_SETLED,E_PG_MAIN,(gslc_tsRect){160,115,74,12},
    (char*)"Set LED RGB:",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  // Create E_LBL_RED text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_RED,E_PG_MAIN,(gslc_tsRect){160,130,26,12},
    (char*)"Red:",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3);

  // Create slider E_SLIDER_R 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_R,E_PG_MAIN,&m_sXSlider1,
          (gslc_tsRect){210,130,80,20},0,255,255,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_RED,10,5,GSLC_COL_RED);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSlider1 = pElemRef;
  
  // Create E_LBL_GREEN text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_GREEN,E_PG_MAIN,(gslc_tsRect){160,160,38,12},
    (char*)"Green:",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3);

  // Create slider E_SLIDER_G 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_G,E_PG_MAIN,&m_sXSlider2,
          (gslc_tsRect){210,160,80,20},0,255,128,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_GREEN,10,5,GSLC_COL_GREEN);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSlider2 = pElemRef;
  
  // Create E_LBL_BLUE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_LBL_BLUE,E_PG_MAIN,(gslc_tsRect){160,190,32,12},
    (char*)"Blue:",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3);

  // Create slider E_SLIDER_B 
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_B,E_PG_MAIN,&m_sXSlider3,
          (gslc_tsRect){210,190,80,20},0,255,0,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_BLUE,10,5,GSLC_COL_BLUE);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pElemSlider3 = pElemRef;
  
  // Create E_ELEM_TEXT8 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT8,E_PG_MAIN,(gslc_tsRect){215,230,98,10},
    (char*)"GUIslice Example",0,E_FONT_TXT5);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_GRAY_DK2,GSLC_COL_BLACK);

  // Create E_DRAW_LINE1 line 
  pElemRef = gslc_ElemCreateLine(&m_gui,E_DRAW_LINE1,E_PG_MAIN,20,100,300,100);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_WHITE);

  // Create E_DRAW_LINE2 line 
  pElemRef = gslc_ElemCreateLine(&m_gui,E_DRAW_LINE2,E_PG_MAIN,235,60,235,95);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_WHITE,GSLC_COL_WHITE);
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
    if (!gslc_FontSet(&m_gui,E_FONT_TXT15,GSLC_FONTREF_PTR,NULL,3)) { return; }
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

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  //TODO - Add update code for any text, gauges, or sliders
  
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
    while (1) { }
  }
}
