//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 05 (Arduino):
//   - Multiple page handling
//   - Background image
//   - Compound elements
//   - NOTE: The XSelNum compound element requires GSLC_FEATURE_COMPOUND enabled
//   - NOTE: This is the simple version of the example without
//     optimizing for memory consumption. Therefore, it may not
//     run on Arduino devices with limited memory. A "minimal"
//     version is located in the "arduino_min" folder which includes
//     FLASH memory optimization for reduced memory devices.
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
// - To support a larger number of GUI elements, it is recommended to
//   use a CPU that provides more than 2KB of SRAM (eg. ATmega2560).
//
#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"


// Defines for resources
#define IMG_BKGND       "back1_24.bmp"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN,E_PG_EXTRA};
enum {E_ELEM_BTN_QUIT,E_ELEM_BTN_EXTRA,E_ELEM_BTN_BACK,
      E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,
      E_ELEM_COMP1,E_ELEM_COMP2,E_ELEM_COMP3};
enum {E_FONT_BTN,E_FONT_TXT,E_FONT_TITLE};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Instantiate the GUI
#define MAX_PAGE                2
#define MAX_FONT                3

// Define the maximum number of elements per page
#define MAX_ELEM_PG_MAIN        9                 // # Elems total on Main page
#define MAX_ELEM_PG_EXTRA       7                 // # Elems total on Extra page
#define MAX_ELEM_PG_MAIN_RAM    MAX_ELEM_PG_MAIN  // # Elems in RAM
#define MAX_ELEM_PG_EXTRA_RAM   MAX_ELEM_PG_EXTRA // # Elems in RAM

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asMainElem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef              m_asMainElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                 m_asExtraElem[MAX_ELEM_PG_EXTRA_RAM];
gslc_tsElemRef              m_asExtraElemRef[MAX_ELEM_PG_EXTRA];

gslc_tsXGauge               m_sXGauge;
gslc_tsXSelNum              m_sXSelNum[3];


#define MAX_STR             8

  // Save some element pointers for quick access
  gslc_tsElemRef*  m_pElemCnt = NULL;
  gslc_tsElemRef*  m_pElemProgress = NULL;

// Define debug message function
static int16_t DebugOut(char ch) { Serial.write(ch); return 0; }

// Button callbacks
// - Show example of common callback function
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;
  int16_t nElemId = pElem->nId;
  if (eTouch == GSLC_TOUCH_UP_IN) {
    if (nElemId == E_ELEM_BTN_QUIT) {
      m_bQuit = true;
    } else if (nElemId == E_ELEM_BTN_EXTRA) {
      gslc_SetPageCur(&m_gui,E_PG_EXTRA);
    } else if (nElemId == E_ELEM_BTN_BACK) {
      gslc_SetPageCur(&m_gui,E_PG_MAIN);
    }
  }
  return true;
}


// Create the default elements on each page
bool InitOverlays()
{
  gslc_tsElemRef*  pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asMainElem,MAX_ELEM_PG_MAIN_RAM,m_asMainElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_EXTRA,m_asExtraElem,MAX_ELEM_PG_EXTRA_RAM,m_asExtraElemRef,MAX_ELEM_PG_EXTRA);

  // -----------------------------------
  // Background
  // - Background image from SD card disabled, uncomment to enable
  // - Ensure that GSLC_SD_EN is set to 1 in GUIslice_config.h
  //static const char m_strImgBkgnd[] = IMG_BKGND;
  //gslc_SetBkgndImage(&m_gui,gslc_GetImageFromSD(m_strImgBkgnd,GSLC_IMGREF_FMT_BMP24));

  // -----------------------------------
  // PAGE: MAIN

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,50,280,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create title
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){10,10,310,40},
    (char*)"GUIslice Demo",0,E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){100,140,50,20},(char*)"Quit",0,E_FONT_BTN,&CbBtnCommon);


  // Create Extra button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA,E_PG_MAIN,
    (gslc_tsRect){170,140,50,20},(char*)"Extra",0,E_FONT_BTN,&CbBtnCommon);

  // Create counter
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,60,50,10},
    (char*)"Count:",0,E_FONT_TXT);
  static char mstr1[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){100,60,50,10},
    mstr1,sizeof(mstr1),E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);
  m_pElemCnt = pElemRef; // Save for quick access


  // Create progress bar
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,80,50,10},
    (char*)"Progress:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_tsRect){100,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress = pElemRef; // Save for quick access


  // Add compound element
  pElemRef = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP1,E_PG_MAIN,&m_sXSelNum[0],
    (gslc_tsRect){160,60,120,50},E_FONT_BTN);

  // -----------------------------------
  // PAGE: EXTRA

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){40,40,240,160});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Back button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_BACK,E_PG_EXTRA,
    (gslc_tsRect){50,170,50,20},(char*)"Back",0,E_FONT_BTN,&CbBtnCommon);


  // Create a few labels
  int16_t    nPosY = 50;
  int16_t    nSpaceY = 20;
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){60,nPosY,50,10},
    (char*)"Data 1",0,E_FONT_TXT); nPosY += nSpaceY;
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){60,nPosY,50,10},
    (char*)"Data 2",0,E_FONT_TXT); nPosY += nSpaceY;
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){60,nPosY,50,10},
    (char*)"Data 3",0,E_FONT_TXT); nPosY += nSpaceY;

  // Add compound element
  pElemRef = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP2,E_PG_EXTRA,&m_sXSelNum[1],
    (gslc_tsRect){130,60,120,50},E_FONT_BTN);

  pElemRef = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP3,E_PG_EXTRA,&m_sXSelNum[2],
    (gslc_tsRect){130,120,120,50},E_FONT_BTN);


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
  if (!gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1)) { return; }
  if (!gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1)) { return; }
  if (!gslc_FontAdd(&m_gui,E_FONT_TITLE,GSLC_FONTREF_PTR,NULL,1)) { return; }

  // Create page elements
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;
}

void loop()
{
  char              acTxt[MAX_STR];

  // Save some element references for easy access
  gslc_tsElemRef*  pElemCnt        = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_tsElemRef*  pElemProgress   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);

  m_nCount++;

  // Perform drawing updates
  // - Note: we can make the updates conditional on the active
  //   page by checking gslc_GetPageCur() first.

  snprintf(acTxt,MAX_STR,"%u",m_nCount);
  gslc_ElemSetTxtStr(&m_gui,pElemCnt,acTxt);

  gslc_ElemXGaugeUpdate(&m_gui,pElemProgress,((m_nCount/2)%100));

  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // Slow down updates
  delay(100);

  // In a real program, we would detect the button press and take an action.
  // For this Arduino demo, we will pretend to exit by emulating it with an
  // infinite loop. Note that interrupts are not disabled so that any debug
  // messages via Serial have an opportunity to be transmitted.
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    while (1) { }
  }
}


