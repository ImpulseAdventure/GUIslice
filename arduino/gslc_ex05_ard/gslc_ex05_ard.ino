//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/guislice-gui.html
// - Example 05 (Arduino):
//   - Multiple page handling
//   - Background image
//   - Compound elements
//   - Demonstrates the use of ElemCreate*_P() functions
//     NOTE: This sketch requires CPUs with >2KB of RAM
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
  #define MAX_ELEM_PG_MAIN_PROG   4                                       // # Elems in Flash
  #define MAX_ELEM_PG_EXTRA_PROG  4                                       // # Elems in Flash  
#else
  #define MAX_ELEM_PG_MAIN_PROG   0                                       // # Elems in Flash
  #define MAX_ELEM_PG_EXTRA_PROG  0                                       // # Elems in Flash  
#endif
#define MAX_ELEM_PG_MAIN_RAM    MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM
#define MAX_ELEM_PG_EXTRA_RAM   MAX_ELEM_PG_MAIN - MAX_ELEM_PG_MAIN_PROG  // # Elems in RAM

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
  gslc_tsElem*  m_pElemCnt = NULL;
  gslc_tsElem*  m_pElemProgress = NULL;

// Button callbacks
// - Show example of common callback function
bool CbBtnCommon(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElem* pElem = (gslc_tsElem*)(pvElem);
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
  gslc_tsElem*  pElem = NULL;
  
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asMainElem,MAX_ELEM_PG_MAIN_RAM,m_asMainElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_EXTRA,m_asExtraElem,MAX_ELEM_PG_EXTRA_RAM,m_asExtraElemRef,MAX_ELEM_PG_EXTRA);
  
  // -----------------------------------
  // Background
  // - Background image from SD card disabled, uncomment to enable
  // - Ensure that ADAGFX_SD_EN is set to 1 in GUIslice_config.h
  //static const char m_strImgBkgnd[] = IMG_BKGND;
  //gslc_SetBkgndImage(&m_gui,gslc_GetImageFromSD(m_strImgBkgnd,GSLC_IMGREF_FMT_BMP24)); 
   
  // -----------------------------------
  // PAGE: MAIN

  // Create background box
  gslc_ElemCreateBox_P(&m_gui,100,E_PG_MAIN,20,50,280,150,GSLC_COL_WHITE,GSLC_COL_BLACK,true,true);  

  // Create title
  gslc_ElemCreateTxt_P(&m_gui,101,E_PG_MAIN,10,10,310,40,"GUIslice Demo",&m_asFont[2], // E_FONT_TITLE
          GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_MID,false,false);  

  // Create Quit button with text label
  static const char mstr2[] PROGMEM = "Quit";  
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){100,140,50,20},(char*)mstr2,strlen_P(mstr2),E_FONT_BTN,&CbBtnCommon);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    

 
  // Create Extra button with text label
  static const char mstr3[] PROGMEM = "Extra";  
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA,E_PG_MAIN,
    (gslc_tsRect){170,140,50,20},(char*)mstr3,strlen_P(mstr3),E_FONT_BTN,&CbBtnCommon);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    

  // Create counter
  gslc_ElemCreateTxt_P(&m_gui,102,E_PG_MAIN,40,60,50,10,"Count",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  static char mstr5[8] = "";  // Placeholder for counter  
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){100,60,50,10},
    mstr5,8,E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_YELLOW);
  m_pElemCnt = pElem; // Save for quick access
  

  // Create progress bar
  gslc_ElemCreateTxt_P(&m_gui,103,E_PG_MAIN,40,80,50,10,"Progress",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  pElem = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_tsRect){100,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);
  m_pElemProgress = pElem; // Save for quick access    
    

  // Add compound element
  pElem = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP1,E_PG_MAIN,&m_sXSelNum[0],
    (gslc_tsRect){160,60,120,50},E_FONT_BTN);  

  // -----------------------------------
  // PAGE: EXTRA

  // Create background box
  gslc_ElemCreateBox_P(&m_gui,200,E_PG_EXTRA,40,40,240,160,GSLC_COL_WHITE,GSLC_COL_BLACK,true,true);  

  // Create Back button with text label
  static const char mstr7[] PROGMEM = "Back";    
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_BACK,E_PG_EXTRA,
    (gslc_tsRect){50,170,50,20},(char*)mstr7,strlen_P(mstr7),E_FONT_BTN,&CbBtnCommon);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);       


  // Create a few labels
  gslc_ElemCreateTxt_P(&m_gui,201,E_PG_EXTRA,60,50,50,10,"Data 1",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  gslc_ElemCreateTxt_P(&m_gui,202,E_PG_EXTRA,60,70,50,10,"Data 2",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);
  gslc_ElemCreateTxt_P(&m_gui,203,E_PG_EXTRA,60,90,50,10,"Data 3",&m_asFont[1], // E_FONT_TXT
          GSLC_COL_YELLOW,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_ALIGN_MID_LEFT,false,true);

  // Add compound element
  pElem = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP2,E_PG_EXTRA,&m_sXSelNum[1],
    (gslc_tsRect){130,60,120,50},E_FONT_BTN);

  pElem = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP3,E_PG_EXTRA,&m_sXSelNum[2],
    (gslc_tsRect){130,120,120,50},E_FONT_BTN);    

    
  return true;
}


void setup()
{
  bool              bOk = true;

  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  // - In this example, we are loading the same font but at
  //   different point sizes. We could also refer to other
  //   font files as well.
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,"",1); // m_asFont[0]
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,"",1); // m_asFont[1]
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TITLE,"",3); // m_asFont[2]
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }

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
  gslc_tsElem*  pElemCnt        = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_tsElem*  pElemProgress   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);
    
  m_nCount++;

  // Perform drawing updates
  // - Note: we can make the updates conditional on the active
  //   page by checking gslc_GetPageCur() first.

  snprintf(acTxt,MAX_STR,"%u",m_nCount);
  gslc_ElemSetTxtStr(pElemCnt,acTxt);

  gslc_ElemXGaugeUpdate(pElemProgress,((m_nCount/2)%100));
    
  // Periodically call GUIslice update function
  gslc_Update(&m_gui);

  // Slow down updates
  delay(100);
  
  // Upon Quit, close down GUI and terminate loop
  if (m_bQuit) {
    gslc_Quit(&m_gui);
    exit(1);  // In Arduino, this essentially starts infinite loop
  }
}


