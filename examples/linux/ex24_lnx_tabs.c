//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 24 (LINUX):
//   - Multiple page handling, tab dialog, global (base layer) elements
//     and popup dialog
//

#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
#include "elem/XCheckbox.h"
#include "elem/XProgress.h"

#include <libgen.h>       // For path parsing


// Defines for resources
#define MAX_PATH  255
#define FONT1 "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"

// Enumerations for pages, elements, fonts, images
enum { E_PG_BASE, E_PG_MAIN, E_PG_CONFIG, E_PG_ALERT  };
enum {
  E_ELEM_BTN_QUIT, E_ELEM_TAB_MAIN, E_ELEM_TAB_CONFIG, E_ELEM_BTN_ALERT,
  E_ELEM_TXT_COUNT, E_ELEM_PROGRESS,
  E_ELEM_CHECK1, E_ELEM_CHECK2, E_ELEM_CHECK3,
  // E_PG_ALERT
  E_ELEM_ALERT_OK, E_ELEM_ALERT_CANCEL,
};
enum { E_FONT_BTN, E_FONT_TXT, E_FONT_TITLE, MAX_FONT };

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Instantiate the GUI
#define MAX_PAGE            4
#define MAX_ELEM_PG_BASE    8   // # Elems total on Global page
#define MAX_ELEM_PG_MAIN    3   // # Elems total on Main page
#define MAX_ELEM_PG_CONFIG  6   // # Elems total on Extra page
#define MAX_ELEM_PG_ALERT   4   // # Elems total on Alert popup page

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asGlbElem[MAX_ELEM_PG_BASE];
gslc_tsElemRef              m_asGlbElemRef[MAX_ELEM_PG_BASE];
gslc_tsElem                 m_asMainElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asMainElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                 m_asConfigElem[MAX_ELEM_PG_CONFIG];
gslc_tsElemRef              m_asConfigElemRef[MAX_ELEM_PG_CONFIG];
gslc_tsElem                 m_asAlertElem[MAX_ELEM_PG_ALERT];
gslc_tsElemRef              m_asAlertElemRef[MAX_ELEM_PG_ALERT];

gslc_tsXProgress            m_sXProgress;
gslc_tsXCheckbox            m_asXCheck[3];


#define MAX_STR             100

// Save some element pointers for quick access
gslc_tsElemRef*  m_pElemCnt = NULL;
gslc_tsElemRef*  m_pElemProgress = NULL;
gslc_tsElemRef*  m_pElemTabMain = NULL;
gslc_tsElemRef*  m_pElemTabConfig = NULL;
gslc_tsElemRef*  m_pElemAlertMsg = NULL;


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

// Update the tab dialog button highlights
void SetTabHighlight(int16_t nTabSel)
{
  gslc_ElemSetCol(&m_gui, m_pElemTabMain, (nTabSel == E_ELEM_TAB_MAIN) ? GSLC_COL_WHITE : GSLC_COL_BLUE_DK2,
    GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
  gslc_ElemSetCol(&m_gui, m_pElemTabConfig, (nTabSel == E_ELEM_TAB_CONFIG) ? GSLC_COL_WHITE : GSLC_COL_BLUE_DK2,
    GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);
}
 
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
    }
    else if (nElemId == E_ELEM_TAB_CONFIG) {
      gslc_SetPageCur(&m_gui, E_PG_CONFIG);
      SetTabHighlight(E_ELEM_TAB_CONFIG);
    }
    else if (nElemId == E_ELEM_TAB_MAIN) {
      gslc_SetPageCur(&m_gui, E_PG_MAIN);
      SetTabHighlight(E_ELEM_TAB_MAIN);
    }
    else if (nElemId == E_ELEM_BTN_ALERT) {
      // Show alert popup, modal
      gslc_ElemSetTxtStr(&m_gui, m_pElemAlertMsg, "Alert Message!");
      gslc_PopupShow(&m_gui,E_PG_ALERT,true); // Use false for modeless
    }
    else if (nElemId == E_ELEM_ALERT_OK) {
      GSLC_DEBUG_PRINT("INFO: Alert popup selected OK\n", "");
      // Dispose of alert
      gslc_PopupHide(&m_gui);
    }
    else if (nElemId == E_ELEM_ALERT_CANCEL) {
      GSLC_DEBUG_PRINT("INFO: Alert popup selected Cancel\n", "");
      // Dispose of alert
      gslc_PopupHide(&m_gui);
    }
  }
  return true;
}


// Create the default elements on each page
bool InitOverlays()
{
  gslc_tsElemRef*  pElemRef = NULL;

  gslc_PageAdd(&m_gui, E_PG_BASE, m_asGlbElem, MAX_ELEM_PG_BASE, m_asGlbElemRef, MAX_ELEM_PG_BASE);
  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asMainElem, MAX_ELEM_PG_MAIN, m_asMainElemRef, MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui, E_PG_CONFIG, m_asConfigElem, MAX_ELEM_PG_CONFIG, m_asConfigElemRef, MAX_ELEM_PG_CONFIG);
  gslc_PageAdd(&m_gui, E_PG_ALERT, m_asAlertElem, MAX_ELEM_PG_ALERT, m_asAlertElemRef, MAX_ELEM_PG_ALERT);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // Note that the current page defaults to the first page added, which in the
  // above sequence is E_PG_BASE. Therefore, we should explicitly ensure
  // that the main page is the current page.
  gslc_SetPageCur(&m_gui, E_PG_MAIN);
  // Now mark the E_PG_BASE as a "base" page which means that it's elements
  // are always visible. This is useful for common page elements.
  gslc_SetPageBase(&m_gui, E_PG_BASE);

  // -----------------------------------
  // PAGE: BASE
  // Create title
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_BASE, (gslc_tsRect) { 10, 10, 100, 20 },
    (char*)"GUIslice Demo", 0, E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui, pElemRef, GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetFillEn(&m_gui, pElemRef, false);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_WHITE);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_QUIT, E_PG_BASE,
    (gslc_tsRect) { 240, 5, 50, 25 }, (char*)"Quit", 0, E_FONT_BTN, &CbBtnCommon);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_RED_DK2, GSLC_COL_RED_DK4, GSLC_COL_RED_DK1);

  pElemRef = gslc_ElemCreateLine(&m_gui, GSLC_ID_AUTO, E_PG_BASE, 0, 35, 320, 35);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLACK, GSLC_COL_GRAY, GSLC_COL_GRAY);


  // Create counter
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_BASE, (gslc_tsRect) { 110, 10, 50, 20 },
    (char*)"Count:", 0, E_FONT_TXT);
  static char mstr1[8] = "";
  pElemRef = gslc_ElemCreateTxt(&m_gui, E_ELEM_TXT_COUNT, E_PG_BASE, (gslc_tsRect) { 170, 10, 50, 20 },
    mstr1, sizeof(mstr1), E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_PURPLE);
  m_pElemCnt = pElemRef; // Save for quick access


  // Create Tab label buttons, start with main with framed highlight
  m_pElemTabMain = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_TAB_MAIN, E_PG_BASE,
    (gslc_tsRect) { 30, 50, 50, 20 }, (char*)"Main", 0, E_FONT_BTN, &CbBtnCommon);
  m_pElemTabConfig = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_TAB_CONFIG, E_PG_BASE,
    (gslc_tsRect) { 90, 50, 50, 20 }, (char*)"Extra", 0, E_FONT_BTN, &CbBtnCommon);
  SetTabHighlight(E_ELEM_TAB_MAIN);

  // Create tab box
  pElemRef = gslc_ElemCreateBox(&m_gui, GSLC_ID_AUTO, E_PG_BASE, (gslc_tsRect) { 20, 70, 200, 150 });
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: MAIN

  // Create progress bar
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) { 40, 120, 50, 10 },
    (char*)"Progress:", 0, E_FONT_TXT);
  pElemRef = gslc_ElemXProgressCreate(&m_gui, E_ELEM_PROGRESS, E_PG_MAIN, &m_sXProgress, (gslc_tsRect) { 100, 120, 50, 10 },
    0, 100, 0, GSLC_COL_GREEN, false);
  m_pElemProgress = pElemRef; // Save for quick access


  // Checkbox element
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui, E_ELEM_CHECK1, E_PG_MAIN, &m_asXCheck[0],
    (gslc_tsRect) { 100, 160, 30, 30 }, false, GSLCX_CHECKBOX_STYLE_X, GSLC_COL_BLUE_LT2, false);

  // -----------------------------------
  // PAGE: CONFIG

  // Create Dummy button
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_ALERT, E_PG_CONFIG,
    (gslc_tsRect) { 60, 170, 50, 20 }, (char*)"Alert", 0, E_FONT_BTN, &CbBtnCommon);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GREEN_DK2, GSLC_COL_GREEN_DK4, GSLC_COL_GREEN_DK1);

  // Create a few labels & checkboxes
  int16_t    nPosY = 80;
  int16_t    nSpaceY = 30;

  pElemRef = gslc_ElemXCheckboxCreate(&m_gui, E_ELEM_CHECK2, E_PG_CONFIG, &m_asXCheck[1],
    (gslc_tsRect) { 60, nPosY, 20, 20 }, false, GSLCX_CHECKBOX_STYLE_X, GSLC_COL_RED_LT2, false);
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_CONFIG, (gslc_tsRect) { 100, nPosY, 50, 10 },
    (char*)"Data 1", 0, E_FONT_TXT);
  nPosY += nSpaceY;

  pElemRef = gslc_ElemXCheckboxCreate(&m_gui, E_ELEM_CHECK3, E_PG_CONFIG, &m_asXCheck[2],
    (gslc_tsRect) { 60, nPosY, 20, 20 }, false, GSLCX_CHECKBOX_STYLE_X, GSLC_COL_RED_LT2, false);
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_CONFIG, (gslc_tsRect) { 100, nPosY, 50, 10 },
    (char*)"Data 2", 0, E_FONT_TXT);
  nPosY += nSpaceY;

  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_CONFIG, (gslc_tsRect) { 100, nPosY, 50, 10 },
    (char*)"Data 3", 0, E_FONT_TXT);
  nPosY += nSpaceY;

  // -----------------------------------
  // PAGE: POPUP Confirm

  // Create alert box
  pElemRef = gslc_ElemCreateBox(&m_gui, GSLC_ID_AUTO, E_PG_ALERT, (gslc_tsRect) { 160 - 90, 120 - 30, 2 * 90, 90 });
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_LT2, GSLC_COL_GRAY_DK3, GSLC_COL_GRAY_DK3);

  // Alert message
  static char m_strAlertMsg[20] = "alert";
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_ALERT, (gslc_tsRect) { 160 - 80, 120 - 20, 2 * 80, 40 },
    m_strAlertMsg, sizeof(m_strAlertMsg), E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_RED_LT1);
  m_pElemAlertMsg = pElemRef; // Save for quick access

  // Create OK button
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_ALERT_OK, E_PG_ALERT,
    (gslc_tsRect) { 160 - 40 - 30, 120 + 30, 2 * 30, 2 * 10 }, (char*)"OK", 0, E_FONT_BTN, &CbBtnCommon);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);

  // Create Cancel button
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_ALERT_CANCEL, E_PG_ALERT,
    (gslc_tsRect) { 160 + 40 - 30, 120 + 30, 2 * 30, 2 * 10 }, (char*)"Cancel", 0, E_FONT_BTN, &CbBtnCommon);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_BLUE_DK2, GSLC_COL_BLUE_DK4, GSLC_COL_BLUE_DK1);



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
  bOk = gslc_FontSet(&m_gui,E_FONT_BTN,GSLC_FONTREF_FNAME,FONT1,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontSet failed\n"); exit(1); }
  bOk = gslc_FontSet(&m_gui,E_FONT_TXT,GSLC_FONTREF_FNAME,FONT1,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontSet failed\n"); exit(1); }
  bOk = gslc_FontSet(&m_gui,E_FONT_TITLE,GSLC_FONTREF_FNAME,FONT1,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontSet failed\n"); exit(1); }


  // -----------------------------------
  // Create page elements
  // -----------------------------------
  InitOverlays(dirname(args[0])); // Pass executable path to find resource files


  // -----------------------------------
  // Start display

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);


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
    gslc_ElemSetTxtStr(&m_gui,m_pElemCnt,acTxt);

    gslc_ElemXProgressSetVal(&m_gui,m_pElemProgress,((m_nCount/200)%100));

    // We can change or disable the global page as needed:
    //   gslc_SetPageBase(&m_gui, E_PG_BASE);    // Set to E_PG_BASE
    //   gslc_SetPageBase(&m_gui, GSLC_PAGE_NONE); // Disable

    // Periodically call GUIslice update function
    gslc_Update(&m_gui);

  } // bQuit


  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}

