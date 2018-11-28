//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 18 (LINUX):
//     Multiple page handling
//     Background image
//     Compound elements
// - NOTE: The XSelNum compound element requires GSLC_FEATURE_COMPOUND enabled
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <libgen.h>       // For path parsing


// Defines for resources
#define MAX_PATH  255
#define FONT1 "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"
#define IMG_BKGND       "/res/bkgnd1_320x240.bmp"
char  m_strImgBkgnd[MAX_PATH];

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
#define MAX_PAGE            2
#define MAX_FONT            5
#define MAX_ELEM_PG_MAIN    10  // Max # elements on main page
#define MAX_ELEM_PG_EXTRA   10  // Max # elements on second page

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asMainElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asMainElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                 m_asExtraElem[MAX_ELEM_PG_EXTRA];
gslc_tsElemRef              m_asExtraElemRef[MAX_ELEM_PG_EXTRA];

gslc_tsXGauge               m_sXGauge;
gslc_tsXSelNum              m_sXSelNum[3];


#define MAX_STR             100

// Configure environment variables suitable for display
// - These may need modification to match your system
//   environment and display type
// - Defaults for GSLC_DEV_FB and GSLC_DEV_TOUCH are in GUIslice_config.h
// - Note that the environment variable settings can
//   also be set directly within the shell via export
//   (or init script).
//   - eg. export TSLIB_FBDEVICE=/dev/fb1
void UserInitEnv()
{
#if defined(DRV_DISP_SDL1) || defined(DRV_DISP_SDL2)
  setenv((char*)"FRAMEBUFFER",GSLC_DEV_FB,1);
  setenv((char*)"SDL_FBDEV",GSLC_DEV_FB,1);
  setenv((char*)"SDL_VIDEODRIVER",GSLC_DEV_VID_DRV,1);
#endif

#if defined(DRV_TOUCH_TSLIB)
  setenv((char*)"TSLIB_FBDEVICE",GSLC_DEV_FB,1);
  setenv((char*)"TSLIB_TSDEVICE",GSLC_DEV_TOUCH,1);
  setenv((char*)"TSLIB_CALIBFILE",(char*)"/etc/pointercal",1);
  setenv((char*)"TSLIB_CONFFILE",(char*)"/etc/ts.conf",1);
  setenv((char*)"TSLIB_PLUGINDIR",(char*)"/usr/local/lib/ts",1);
#endif
}

// Define debug message function
static int16_t DebugOut(char ch) { fputc(ch,stderr); return 0; }

// Button callbacks
// - Show example of common callback function
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);
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
// - strPath: Path to executable passed in to locate resource files
bool InitOverlays(char *strPath)
{
  gslc_tsElemRef*  pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asMainElem,MAX_ELEM_PG_MAIN,m_asMainElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_EXTRA,m_asExtraElem,MAX_ELEM_PG_EXTRA,m_asExtraElemRef,MAX_ELEM_PG_EXTRA);

  // -----------------------------------
  // Background
  strncpy(m_strImgBkgnd,strPath,MAX_PATH);
  strncat(m_strImgBkgnd,IMG_BKGND,MAX_PATH);
  gslc_SetBkgndImage(&m_gui,gslc_GetImageFromFile(m_strImgBkgnd,GSLC_IMGREF_FMT_BMP16));

  // -----------------------------------
  // PAGE: MAIN

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,50,280,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create title
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){10,10,310,40},
    "GUIslice Demo",0,E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){100,140,50,20},"Quit",0,E_FONT_BTN,&CbBtnCommon);

  // Create Extra button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA,E_PG_MAIN,
    (gslc_tsRect){170,140,50,20},"Extra",0,E_FONT_BTN,&CbBtnCommon);

  // Create counter
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,60,50,10},
    "Count:",0,E_FONT_TXT);
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){100,60,50,10},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);

  // Create progress bar
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){40,80,50,10},
    "Progress:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_tsRect){100,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);

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
    (gslc_tsRect){50,170,50,20},"Back",0,E_FONT_BTN,&CbBtnCommon);

  // Create a few labels
  int16_t    nPosY = 50;
  int16_t    nSpaceY = 20;
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){60,nPosY,50,10},
    "Data 1",0,E_FONT_TXT); nPosY += nSpaceY;
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){60,nPosY,50,10},
    "Data 2",0,E_FONT_TXT); nPosY += nSpaceY;
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_EXTRA,(gslc_tsRect){60,nPosY,50,10},
    "Data 3",0,E_FONT_TXT); nPosY += nSpaceY;

  // Add compound element
  pElemRef = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP2,E_PG_EXTRA,&m_sXSelNum[1],
    (gslc_tsRect){130,60,120,50},E_FONT_BTN);

  pElemRef = gslc_ElemXSelNumCreate(&m_gui,E_ELEM_COMP3,E_PG_EXTRA,&m_sXSelNum[2],
    (gslc_tsRect){130,120,120,50},E_FONT_BTN);

  return true;
}


int main( int argc, char* args[] )
{
  bool              bOk = true;
  char              acTxt[MAX_STR];

  // -----------------------------------
  // Initialize
  gslc_InitDebug(&DebugOut);
  UserInitEnv();
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  // - In this example, we are loading the same font but at
  //   different point sizes. We could also refer to other
  //   font files as well.
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_FNAME,FONT1,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_FNAME,FONT1,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TITLE,GSLC_FONTREF_FNAME,FONT1,32);
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
  gslc_tsElemRef*  pElemCnt        = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_tsElemRef*  pElemProgress   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);

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
    gslc_ElemSetTxtStr(&m_gui,pElemCnt,acTxt);

    gslc_ElemXGaugeUpdate(&m_gui,pElemProgress,((m_nCount/200)%100));


    // Periodically call GUIslice update function
    gslc_Update(&m_gui);

  } // bQuit


  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}

