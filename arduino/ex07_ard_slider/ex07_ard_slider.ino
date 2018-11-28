//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 07 (Arduino):
//   - Sliders with dynamic color control and position callback
//   - Expected behavior: Clicking on "SAVE" button terminates program,
//     and no action when clicking on "Selected Room" button
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"


// Defines for resources

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_COLOR,
      E_SLIDER_R,E_SLIDER_G,E_SLIDER_B};
enum {E_FONT_TXT,E_FONT_TITLE};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Instantiate the GUI
#define MAX_PAGE                1
#define MAX_FONT                2

// Define the maximum number of elements per page
#define MAX_ELEM_PG_MAIN          17                // # Elems total
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN  // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];   // Storage for all elements in RAM
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];    // References for all elements in GUI

gslc_tsXSlider              m_sXSlider_R,m_sXSlider_G,m_sXSlider_B;

// Current RGB value for color box
// - Globals defined here for convenience so that callback
//   can update R,G,B components independently
uint8_t   m_nPosR = 255;
uint8_t   m_nPosG = 128;
uint8_t   m_nPosB = 0;

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Quit button callback
bool CbBtnQuit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Callback function for when a slider's position has been updated
// - After a slider position has been changed, update the color box
// - Note that all three sliders use the same callback for
//   convenience. From the element's ID we can determine which
//   slider was updated.
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;
  //gslc_tsXSlider* pSlider = (gslc_tsXSlider*)(pElem->pXData);

  // Fetch the new RGB component from the slider
  switch (pElem->nId) {
    case E_SLIDER_R:
      m_nPosR = gslc_ElemXSliderGetPos(pGui,pElemRef);
      break;
    case E_SLIDER_G:
      m_nPosG = gslc_ElemXSliderGetPos(pGui,pElemRef);
      break;
    case E_SLIDER_B:
      m_nPosB = gslc_ElemXSliderGetPos(pGui,pElemRef);
      break;
    default:
      break;
  }

  // Calculate the new RGB value
  gslc_tsColor colRGB = (gslc_tsColor){m_nPosR,m_nPosG,m_nPosB};

  // Update the color box
  gslc_tsElemRef* pElemColor = gslc_PageFindElemById(pGui,E_PG_MAIN,E_ELEM_COLOR);
  gslc_ElemSetCol(pGui,pElemColor,GSLC_COL_WHITE,colRGB,GSLC_COL_WHITE);

  return true;
}


// Create page elements
bool InitOverlays()
{
  gslc_tsElemRef*  pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN_RAM,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create Title with offset shadow
  #define TMP_COL1 (gslc_tsColor){ 32, 32, 60}
  #define TMP_COL2 (gslc_tsColor){128,128,240}
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){2,2,320,50},
    (char*)"Home Automation",0,E_FONT_TITLE);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,TMP_COL1);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){0,0,320,50},
    (char*)"Home Automation",0,E_FONT_TITLE);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,TMP_COL2);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);


  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,300,180});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create dividers
  pElemRef = gslc_ElemCreateLine(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,20,100,300,100);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY_DK3,GSLC_COL_GRAY_DK3);
  pElemRef = gslc_ElemCreateLine(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,235,60,235,95);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY_DK3,GSLC_COL_GRAY_DK3);


  // Create color box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_COLOR,E_PG_MAIN,(gslc_tsRect){20,90+30,130,100});
  gslc_tsColor colRGB = (gslc_tsColor){m_nPosR,m_nPosG,m_nPosB};
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,colRGB,GSLC_COL_WHITE);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){250,60,50,30},(char*)"SAVE",0,E_FONT_TXT,&CbBtnQuit);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);

  // Create dummy selector
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,65,100,20},
    (char*)"Selected Room:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,
   (gslc_tsRect){140,65,80,20},(char*)"Kitchen...",0,E_FONT_TXT,NULL);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK2,GSLC_COL_GRAY_DK3,GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);

  // Create sliders
  // - Define element arrangement
  int16_t   nCtrlY    = 115;
  int16_t   nCtrlGap  = 30;
  uint16_t  nSlideW   = 80;
  uint16_t  nSlideH   = 20;
  int16_t   nLabelX   = 160;
  uint16_t  nLabelW   = 30;
  uint16_t  nLabelH   = 20;
  int16_t   nSlideX   = nLabelX + nLabelW + 20;

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){nLabelX,nCtrlY,120,20},
    (char*)"Set LED RGB:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  nCtrlY += 25;

  // Create three sliders (R,G,B) and assign callback function
  // that is invoked upon change. The common callback will update
  // the color box.
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){nLabelX,nCtrlY,nLabelW,nLabelH},
    (char*)"Red:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3);

  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_R,E_PG_MAIN,&m_sXSlider_R,
          (gslc_tsRect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,m_nPosR,5,false);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_RED,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_RED_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  nCtrlY += nCtrlGap;

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){nLabelX,nCtrlY,nLabelW,nLabelH},
    (char*)"Green:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3);

  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_G,E_PG_MAIN,&m_sXSlider_G,
          (gslc_tsRect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,m_nPosG,5,false);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GREEN,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_GREEN_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  nCtrlY += nCtrlGap;


  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){nLabelX,nCtrlY,nLabelW,nLabelH},
    (char*)"Blue:",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT3);

  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_B,E_PG_MAIN,&m_sXSlider_B,
          (gslc_tsRect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,m_nPosB,5,false);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_BLUE_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  nCtrlY += nCtrlGap;


  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){250,230,60,10},
    (char*)"GUIslice Example",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_BLACK);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_RIGHT);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);

  return true;
}


void setup()
{
  // Initialize debug output
  Serial.begin(9600);
  gslc_InitDebug(&DebugOut);
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // Load Fonts
  if (!gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1)) { return; }
  if (!gslc_FontAdd(&m_gui,E_FONT_TITLE,GSLC_FONTREF_PTR,NULL,3)) { return; }

  // Create pages display
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;
  return;
}

void loop()
{
  // General counter
  m_nCount++;

  // Periodically call GUIslice update function
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

