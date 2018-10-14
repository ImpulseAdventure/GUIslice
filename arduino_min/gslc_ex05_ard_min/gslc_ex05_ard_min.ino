//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 05 (Arduino): [minimum RAM version]
//   - Multiple page handling
//   - Background image
//   - Compound elements
//   - NOTE: The XSelNum compound element requires GSLC_FEATURE_COMPOUND enabled
//   - Demonstrates the use of ElemCreate*_P() functions
//     These RAM-reduced examples take advantage of the internal
//     Flash storage (via PROGMEM).
//   - NOTE: This sketch requires moderate program storage in Flash.
//     As a result, it may not run on basic Arduino devices (eg. ATmega328)
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
// - To enable the same code to run on devices that support storing
//   data into Flash (PROGMEM) and those that don't, we can make the
//   number of elements in Flash dependent upon GSLC_USE_PROGMEM
// - This should allow both Arduino and ARM Cortex to use the same code
#define MAX_ELEM_PG_MAIN        9                                         // # Elems total on Main page
#define MAX_ELEM_PG_EXTRA       7                                         // # Elems total on Extra page
#if (GSLC_USE_PROGMEM)
  #define MAX_ELEM_PG_MAIN_PROG   7                                       // # Elems in Flash
  #define MAX_ELEM_PG_EXTRA_PROG  5                                       // # Elems in Flash
#else
  #define MAX_ELEM_PG_MAIN_PROG   0                                       // # Elems in Flash
  #define MAX_ELEM_PG_EXTRA_PROG  0                                       // # Elems in Flash
#endif
#define MAX_ELEM_PG_MAIN_RAM    MAX_ELEM_PG_MAIN  - MAX_ELEM_PG_MAIN_PROG   // # Elems in RAM
#define MAX_ELEM_PG_EXTRA_RAM   MAX_ELEM_PG_EXTRA - MAX_ELEM_PG_EXTRA_PROG  // # Elems in RAM

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
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui,pElemRef);
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
  gslc_ElemCreateBox_P(&m_gui,100,E_PG_MAIN,20,50,280,150,GSLC_COL_WHITE,GSLC_COL_BLACK,true,true,NULL,NULL);

  // Create title
  gslc_ElemCreateTxt_P(&m_gui,101,E_PG_MAIN,10,10,310,40,"GUIslice Demo",&m_asFont[2], // E_FONT_TITLE
          GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_MID,false,false);

  // Create Quit button with text label
  gslc_ElemCreateBtnTxt_P(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,100,140,50,20,"Quit",&m_asFont[0],
    GSLC_COL_WHITE,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK1,GSLC_ALIGN_MID_MID,true,true,&CbBtnCommon,NULL);



  // Create Extra button with text label
  gslc_ElemCreateBtnTxt_P(&m_gui,E_ELEM_BTN_EXTRA,E_PG_MAIN,170,140,50,20,"Extra",&m_asFont[0],
    GSLC_COL_WHITE,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK1,GSLC_ALIGN_MID_MID,true,true,&CbBtnCommon,NULL);


  // Create counter
  gslc_ElemCreateTxt_P(&m_gui,102,E_PG_MAIN,40,60,50,10,"Count",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  static char mstr5[8] = "????";  // Placeholder for counter
  gslc_ElemCreateTxt_P_R(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,100,60,50,10,mstr5,8,&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);


  // Create progress bar
  gslc_ElemCreateTxt_P(&m_gui,103,E_PG_MAIN,40,80,50,10,"Progress",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_tsRect){100,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress = pElemRef; // Save for quick access


  // Add compound element
  pElemRef = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP1,E_PG_MAIN,&m_sXSelNum[0],
    (gslc_tsRect){160,60,120,50},E_FONT_BTN);

  // -----------------------------------
  // PAGE: EXTRA

  // Create background box
  gslc_ElemCreateBox_P(&m_gui,200,E_PG_EXTRA,40,40,240,160,GSLC_COL_WHITE,GSLC_COL_BLACK,true,true,NULL,NULL);

  // Create Back button with text label
  gslc_ElemCreateBtnTxt_P(&m_gui,E_ELEM_BTN_BACK,E_PG_EXTRA,50,170,50,20,"Back",&m_asFont[0],
    GSLC_COL_WHITE,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK1,GSLC_ALIGN_MID_MID,true,true,&CbBtnCommon,NULL);

  // Create a few labels
  gslc_ElemCreateTxt_P(&m_gui,201,E_PG_EXTRA,60,50,50,10,"Data 1",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  gslc_ElemCreateTxt_P(&m_gui,202,E_PG_EXTRA,60,70,50,10,"Data 2",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  gslc_ElemCreateTxt_P(&m_gui,203,E_PG_EXTRA,60,90,50,10,"Data 3",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

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
  // - NOTE: If we are using the ElemCreate*_P() macros then it is important to note
  //   the font pointer (array index) as it will be provided to certain
  //   ElemCreate*_P() functions (eg. ElemCreateTxt_P).
  if (!gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_PTR,NULL,1)) { return; }    // m_asFont[0]
  if (!gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_PTR,NULL,1)) { return; }    // m_asFont[1]
  if (!gslc_FontAdd(&m_gui,E_FONT_TITLE,GSLC_FONTREF_PTR,NULL,1)) { return; }  // m_asFont[2]

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
