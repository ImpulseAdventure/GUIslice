//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 05: Multiple page handling
//               Compound elements
//

#include "GUIslice.h"
#include "GUIslice_ex.h"

#include <libgen.h>       // For path parsing

// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"
#define IMG_BKGND       "/res/bkgnd1_320x240.bmp"

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
#define MAX_FONT            10
gslc_tsGui                  m_gui;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsXGauge               m_sXGauge;
gslc_tsXSelNum              m_sXSelNum[3];

#define MAX_PAGE            2
#define MAX_ELEM_PG_MAIN    10
#define MAX_ELEM_PG_EXTRA   10
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asElemMain[MAX_ELEM_PG_MAIN];
gslc_tsElem                 m_asElemExtra[MAX_ELEM_PG_EXTRA];

#define MAX_STR             100

// Button callbacks
// - Show example of common callback function
bool CbBtnCommon(void* pvGui,void *pvElem,gslc_teTouch eTouch,int nX,int nY)
{
  gslc_tsElem* pElem = (gslc_tsElem*)(pvElem);
  int nElemId = pElem->nId;
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
// - strPath: Path to executable passed in to locate resource files
bool InitOverlays(char *strPath)
{
  gslc_tsElem*  pElem = NULL;
  
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asElemMain,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_EXTRA,m_asElemExtra,MAX_ELEM_PG_EXTRA);
  
  // -----------------------------------
  // Background
  char* strImgBkgndPath = (char*)malloc(strlen(strPath)+strlen(IMG_BKGND)+1);
  strcpy(strImgBkgndPath, strPath);
  strcat(strImgBkgndPath, IMG_BKGND);
  gslc_SetBkgndImage(&m_gui,strImgBkgndPath);
  free(strImgBkgndPath);
  
  // -----------------------------------
  // PAGE: MAIN

  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){20,50,280,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create title
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){10,10,310,40},
    "GUIslice Demo",E_FONT_TITLE);
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pElem,false);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);

  // Create Quit button with text label
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_Rect){100,140,50,20},"Quit",E_FONT_BTN,&CbBtnCommon);

  // Create Extra button with text label
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA,E_PG_MAIN,
    (gslc_Rect){170,140,50,20},"Extra",E_FONT_BTN,&CbBtnCommon);

  // Create counter
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){40,60,50,10},
    "Count:",E_FONT_TXT);
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_Rect){100,60,50,10},
    "",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_YELLOW);

  // Create progress bar
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){40,80,50,10},
    "Progress:",E_FONT_TXT);
  pElem = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_Rect){100,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);

  // Add compound element
  pElem = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP1,E_PG_MAIN,&m_sXSelNum[0],
    (gslc_Rect){160,60,120,50},E_FONT_BTN);  
  
  // -----------------------------------
  // PAGE: EXTRA

  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_Rect){40,40,240,160});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Back button with text label
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_BACK,E_PG_EXTRA,
    (gslc_Rect){50,170,50,20},"Back",E_FONT_BTN,&CbBtnCommon);

  // Create a few labels
  int16_t    nPosY = 50;
  int16_t    nSpaceY = 20;
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_Rect){60,nPosY,50,10},
    "Data 1",E_FONT_TXT); nPosY += nSpaceY;
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_Rect){60,nPosY,50,10},
    "Data 2",E_FONT_TXT); nPosY += nSpaceY;
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_Rect){60,nPosY,50,10},
    "Data 3",E_FONT_TXT); nPosY += nSpaceY;
  
  // Add compound element
  pElem = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP2,E_PG_EXTRA,&m_sXSelNum[1],
    (gslc_Rect){130,60,120,50},E_FONT_BTN);

  pElem = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP3,E_PG_EXTRA,&m_sXSelNum[2],
    (gslc_Rect){130,120,120,50},E_FONT_BTN);    
    
  return true;
}


int main( int argc, char* args[] )
{
  bool              bOk = true;
  char              acTxt[MAX_STR];

  // -----------------------------------
  // Initialize

  gslc_InitEnv(&m_gui);
  if (!gslc_Init(&m_gui,m_asPage,MAX_PAGE,m_asFont,MAX_FONT,NULL,0)) { exit(1); }

  gslc_InitTs(&m_gui,"/dev/input/touchscreen");

  // Load Fonts
  // - In this example, we are loading the same font but at
  //   different point sizes. We could also refer to other
  //   font files as well.
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TITLE,FONT_DROID_SANS,32);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }


  // -----------------------------------
  // Create page elements
  // -----------------------------------
  InitOverlays(dirname(args[0])); // Pass executable path to find resource files


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

    gslc_ElemXGaugeUpdate(pElemProgress,((m_nCount/200)%100));

    
    // Periodically call GUIslice update function    
    gslc_Update(&m_gui);
    
  } // bQuit


  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}

