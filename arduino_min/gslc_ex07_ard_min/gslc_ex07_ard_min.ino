//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 07 (Arduino): [minimum RAM version]
//   - Sliders with dynamic color control and position callback
//   - Expected behavior: Clicking on "SAVE" button terminates program,
//     and no action when clicking on "Selected Room" button
//   - Demonstrates the use of ElemCreate*_P() functions.
//     These RAM-reduced examples take advantage of the internal
//     Flash storage (via PROGMEM).
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
      E_SLIDER_R,E_SLIDER_G,E_SLIDER_B,E_ELEM_BTN_ROOM};
enum {E_FONT_TXT,E_FONT_TITLE};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Instantiate the GUI
#define MAX_PAGE                1
#define MAX_FONT                2

// Define the maximum number of elements per page
// - To enable the same code to run on devices that support storing
//   data into Flash (PROGMEM) and those that don't, we can make the
//   number of elements in Flash dependent upon GSLC_USE_PROGMEM
// - This should allow both Arduino and ARM Cortex to use the same code
#define MAX_ELEM_PG_MAIN          17                                        // # Elems total
#if (GSLC_USE_PROGMEM)
  #define MAX_ELEM_PG_MAIN_PROG   16                                        // # Elems in Flash
#else
  #define MAX_ELEM_PG_MAIN_PROG   0                                         // # Elems in Flash
#endif
#define MAX_ELEM_PG_MAIN_RAM      MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN_RAM];   // Storage for all elements in RAM
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];    // References for all elements in GUI

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
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
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
  // Note: must use title Font ID
  gslc_ElemCreateTxt_P(&m_gui,98,E_PG_MAIN,2,2,320,50,"Home Automation",&m_asFont[1],
          TMP_COL1,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_MID,false,false);
  gslc_ElemCreateTxt_P(&m_gui,99,E_PG_MAIN,0,0,320,50,"Home Automation",&m_asFont[1],
          TMP_COL2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_MID,false,false);


  // Create background box
  gslc_ElemCreateBox_P(&m_gui,200,E_PG_MAIN,10,50,300,180,GSLC_COL_WHITE,GSLC_COL_BLACK,true,true,NULL,NULL);

  // Create dividers
  gslc_ElemCreateBox_P(&m_gui,201,E_PG_MAIN,20,100,280,1,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,true,true,NULL,NULL);
  gslc_ElemCreateBox_P(&m_gui,202,E_PG_MAIN,235,60,1,35,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,true,true,NULL,NULL);


  // Create color box
  // - This element is created in RAM instead of Flash as we want
  //   dynamic control over its color fill
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_COLOR,E_PG_MAIN,(gslc_tsRect){20,90+30,130,100});
  gslc_tsColor colRGB = (gslc_tsColor){m_nPosR,m_nPosG,m_nPosB};
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,colRGB,GSLC_COL_WHITE);

  // Create Quit button with text label
  gslc_ElemCreateBtnTxt_P(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,250,60,50,30,"SAVE",&m_asFont[0],
    GSLC_COL_WHITE,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK1,GSLC_ALIGN_MID_MID,true,true,&CbBtnQuit,NULL);

  // Create dummy selector
  gslc_ElemCreateTxt_P(&m_gui,100,E_PG_MAIN,20,65,100,20,"Selected Room:",&m_asFont[0],
          GSLC_COL_GRAY_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  // Create room button
  gslc_ElemCreateBtnTxt_P(&m_gui,E_ELEM_BTN_ROOM,E_PG_MAIN,140,65,80,20,"Kitchen...",&m_asFont[0],
    GSLC_COL_WHITE,GSLC_COL_GRAY_DK2,GSLC_COL_GRAY_DK3,GSLC_COL_GRAY_DK2,GSLC_COL_BLUE_DK1,GSLC_ALIGN_MID_MID,true,true,NULL,NULL);

  // Create sliders
  // - Define element arrangement
  uint16_t  nSlideW   = 80;
  uint16_t  nSlideH   = 20;
  int16_t   nLabelX   = 160;
  uint16_t  nLabelW   = 30;
  uint16_t  nLabelH   = 20;
  int16_t   nSlideX   = nLabelX + nLabelW + 20;

  gslc_ElemCreateTxt_P(&m_gui,105,E_PG_MAIN,160,115,120,20,"Set LED RGB:",&m_asFont[0],
          GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  // Create three sliders (R,G,B) and assign callback function
  // that is invoked upon change. The common callback will update
  // the color box.

  // Static text label
  gslc_ElemCreateTxt_P(&m_gui,106,E_PG_MAIN,160,140,30,20,"Red:",&m_asFont[0],
          GSLC_COL_GRAY_LT3,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // Slider
  gslc_ElemXSliderCreate_P(&m_gui,E_SLIDER_R,E_PG_MAIN,210,140,80,20,
    0,255,255,5,false,GSLC_COL_RED,GSLC_COL_BLACK);
  pElemRef = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_SLIDER_R);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_RED_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);

  // Static text label
  gslc_ElemCreateTxt_P(&m_gui,107,E_PG_MAIN,160,170,30,20,"Green:",&m_asFont[0],
          GSLC_COL_GRAY_LT3,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // Slider
  gslc_ElemXSliderCreate_P(&m_gui,E_SLIDER_G,E_PG_MAIN,210,170,80,20,
    0,255,128,5,false,GSLC_COL_GREEN,GSLC_COL_BLACK);
  pElemRef = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_SLIDER_G);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_GREEN_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);

  // Static text label
  gslc_ElemCreateTxt_P(&m_gui,108,E_PG_MAIN,160,200,30,20,"Blue:",&m_asFont[0],
          GSLC_COL_GRAY_LT3,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  // Slider
  gslc_ElemXSliderCreate_P(&m_gui,E_SLIDER_B,E_PG_MAIN,210,200,80,20,
    0,255,0,5,false,GSLC_COL_BLUE,GSLC_COL_BLACK);
  pElemRef = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_SLIDER_B);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_BLUE_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);

  gslc_ElemCreateTxt_P(&m_gui,109,E_PG_MAIN,250,230,60,10,"GUIslice Example",0,
          GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_RIGHT,false,false);

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
  // - NOTE: If we are using the ElemCreate*_P() macros then it is important to note
  //   the font pointer (array index) as it will be provided to certain
  //   ElemCreate*_P() functions (eg. ElemCreateTxt_P).
  if (!gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1)) { return; }   // m_asFont[0]
  if (!gslc_FontAdd(&m_gui,E_FONT_TITLE,GSLC_FONTREF_PTR,NULL,3)) { return; } // m_asFont[1]

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
