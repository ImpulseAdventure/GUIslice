//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 05 (Arduino):
//     Multiple page handling
//     Background image
//     Compound elements
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
// - To support a reasonable number of GUI elements, it is recommended to
//   use a CPU that provides more than 2KB of SRAM.
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
#define MAX_FONT            3
gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsXGauge               m_sXGauge;
gslc_tsXSelNum              m_sXSelNum[3];

#define MAX_PAGE            2
#define MAX_ELEM_PG_MAIN    9
#define MAX_ELEM_PG_EXTRA   7
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asElemMain[MAX_ELEM_PG_MAIN];
gslc_tsElem                 m_asElemExtra[MAX_ELEM_PG_EXTRA];

#define MAX_STR             8


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
  
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asElemMain,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_EXTRA,m_asElemExtra,MAX_ELEM_PG_EXTRA);
  
  // -----------------------------------
  // Background
  // Disabled by default
  //static const char m_strImgBkgnd[] = IMG_BKGND;
  //gslc_SetBkgndImage(&m_gui,gslc_GetImageFromSD(m_strImgBkgnd,GSLC_IMGREF_FMT_BMP24)); 
   
  // -----------------------------------
  // PAGE: MAIN

  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,50,280,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create title
  static const char mstr1[] PROGMEM = "GUIslice Demo";
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){10,10,310,40},
    (char*)mstr1,strlen_P(mstr1),E_FONT_TITLE);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);    
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pElem,false);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);

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
  static const char mstr4[] PROGMEM = "Count";   
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,60,50,10},
    (char*)mstr4,strlen_P(mstr4),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);      
  static char mstr5[8] = "";  // Placeholder for counter  
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){100,60,50,10},
    mstr5,8,E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_YELLOW);
  

  // Create progress bar
  static const char mstr6[] PROGMEM = "Progress";   
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,80,50,10},
    (char*)mstr6,strlen_P(mstr6),E_FONT_TXT);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);      
  pElem = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_tsRect){100,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);
    

  // Add compound element
  pElem = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP1,E_PG_MAIN,&m_sXSelNum[0],
    (gslc_tsRect){160,60,120,50},E_FONT_BTN);  

  // -----------------------------------
  // PAGE: EXTRA

  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){40,40,240,160});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);


  // Create Back button with text label
  static const char mstr7[] PROGMEM = "Back";    
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_BACK,E_PG_EXTRA,
    (gslc_tsRect){50,170,50,20},(char*)mstr7,strlen_P(mstr7),E_FONT_BTN,&CbBtnCommon);
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);       


  // Create a few labels
  int16_t    nPosY = 50;
  int16_t    nSpaceY = 20;
  static const char mstr8[] PROGMEM = "Data 1";  
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){60,nPosY,50,10},
    (char*)mstr8,strlen_P(mstr8),E_FONT_TXT); nPosY += nSpaceY;
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);      


  static const char mstr9[] PROGMEM = "Data 2";    
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){60,nPosY,50,10},
    (char*)mstr9,strlen_P(mstr9),E_FONT_TXT); nPosY += nSpaceY;
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);   


  static const char mstr10[] PROGMEM = "Data 3";    
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){60,nPosY,50,10},
    (char*)mstr10,strlen_P(mstr10),E_FONT_TXT); nPosY += nSpaceY;
  gslc_ElemSetTxtMem(pElem,GSLC_TXT_MEM_PROG);   


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
  char              acTxt[MAX_STR];

  // -----------------------------------
  // Initialize
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  // - In this example, we are loading the same font but at
  //   different point sizes. We could also refer to other
  //   font files as well.
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,"",1);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,"",1);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TITLE,"",3);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }


  // -----------------------------------
  // Create page elements
  // -----------------------------------
  InitOverlays();


  // -----------------------------------
  // Start display

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Save some element references for quick access
  gslc_tsElem*  pElemCnt        = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_tsElem*  pElemProgress   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);
  
  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {

    m_nCount++;

    // -----------------------------------
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
    
  } // bQuit


  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

}

void loop() { }


