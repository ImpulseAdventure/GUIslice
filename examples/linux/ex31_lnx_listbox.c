//
// GUIslice Library Examples
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 31 (LINUX):
//     Demonstrate listbox control with scrollbar
//
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
#include "elem/XListbox.h"
#include "elem/XSlider.h"


#include "unistd.h"   // For usleep()

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
#define FONT1 "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"
// NOTE: The "NotoMono-Regular" font was selected because it is
//       generally available in most systems. Change the following
//       to match your preferred font selection.
//       Available fonts can usually be reported by the "fc-list" command.
#define FONT2 "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"
#define FONT3 "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
enum {E_PG_MAIN};
enum {E_BTN_QUIT,E_SCROLL,E_LISTBOX};
enum {E_FONT_TITLE,E_FONT_LISTBOX,E_FONT_TXT,MAX_FONT};


// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------
#define MAX_PAGE                1
#define MAX_ELEM_PG_MAIN        10




// GUI Elements
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];


gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];
gslc_tsXListbox             m_sListbox;
gslc_tsXSlider              m_sXSlider[1];

#define COUNTRY_CNT  12
#define COUNTRY_LEN  15
// ------------------------------------------------
// Save some element references for update loop access
// ------------------------------------------------
gslc_tsElemRef*  m_pElemSel     = NULL;
gslc_tsElemRef*  m_pElemListbox = NULL;

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
bool      m_bQuit = false;

char m_astrCountryCodes[COUNTRY_CNT][3] = {
  "US",
  "BR",
  "CA",
  "DK",
  "DE",
  "FR",
  "IN",
  "JP",
  "MX",
  "PE",
  "UK",
  "VN"
};

char m_astrCountries[COUNTRY_CNT][COUNTRY_LEN+1] = {
  "USA",
  "Brazil",
  "Canada",
  "Denmark",
  "Germany",
  "France",
  "India",
  "Japan",
  "Mexico",
  "Peru",
  "England",
  "Vietnam"
};


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

// ------------------------------------------------
// Utility Methods
// ------------------------------------------------
void ListboxLoad(gslc_tsElemRef* pElemRef) 
{
  for (int i=0; i<COUNTRY_CNT; i++) {
    gslc_ElemXListboxAddItem(&m_gui,m_pElemListbox,m_astrCountries[i]);
  }
}

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
      case E_BTN_QUIT:
        m_bQuit = true;
        break;
      default:
        break;
    }
  }
  return true;
}

bool CbListBox(void* pvGui, void* pvElemRef, int16_t nSelId)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  char acTxt[5];
  
  if (pElemRef == NULL) {
    return false;
  }

  // Update the status message with the selection
  if (nSelId == XLISTBOX_SEL_NONE) {
    strncpy(acTxt, "NONE", 5);
  } else {
    strncpy(acTxt, m_astrCountryCodes[nSelId], 5);
  }
  gslc_ElemSetTxtStr(&m_gui, m_pElemSel, acTxt);

  return true;
}

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;
  int16_t         nVal;
  gslc_tsElemRef* pElemRefTmp = NULL;

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
    case E_SCROLL:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,pElemRef);
      // Update the textbox scroll position
      pElemRefTmp = gslc_PageFindElemById(pGui,E_PG_MAIN,E_LISTBOX);
      gslc_ElemXListboxSetScrollPos(pGui,pElemRefTmp,nVal);
      break;
    default:
      break;
  }

  return true;
}


// Create page elements
bool InitOverlays()
{
  gslc_tsElemRef*  pElemRef = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK3);

  // -----------------------------------
  // PAGE: E_PG_MAIN
   
  // Create wrapping box for textbox E_LISTBOX and scrollbar
  gslc_tsRect rList = (gslc_tsRect) { 17, 75, 200, 100 };
  pElemRef = gslc_ElemCreateBox(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, rList);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create listbox
  // - Establish storage for listbox
  // - Note that XLISTBOX_BUF_OH_R is extra required per item to
  //   accommodate GUI metadata
  static uint8_t m_pXListboxBuf[COUNTRY_CNT*(COUNTRY_LEN+XLISTBOX_BUF_OH_R)];
  pElemRef = gslc_ElemXListboxCreate(&m_gui, E_LISTBOX, E_PG_MAIN, &m_sListbox,
    (gslc_tsRect){rList.x+2,rList.y+4,rList.w-23,rList.h-7}, E_FONT_LISTBOX,
    m_pXListboxBuf, sizeof(m_pXListboxBuf), 0);
  gslc_ElemXListboxItemsSetSize(&m_gui, pElemRef, XLISTBOX_SIZE_AUTO, XLISTBOX_SIZE_AUTO);
  gslc_ElemXListboxSetSize(&m_gui,pElemRef,5,1); // 5 rows, 1 column
  gslc_ElemXListboxSetSelFunc(&m_gui, pElemRef, &CbListBox);  
  gslc_ElemSetFrameEn(&m_gui, pElemRef, true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLUE_DK3);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_BLUE_LT3);
  gslc_ElemSetTxtMarginXY(&m_gui, pElemRef,5, 0); // Adjust horizontal offset
  gslc_ElemSetGlowCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_BLUE_DK3, GSLC_COL_WHITE);
  m_pElemListbox = pElemRef;

  // Create vertical scrollbar for textbox
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SCROLL,E_PG_MAIN,&m_sXSlider[0],
    (gslc_tsRect){rList.x+rList.w-21,rList.y+4,20,rList.h-8},0,COUNTRY_CNT-1,0,5,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_LT1,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_BLUE_DK1,0,0,GSLC_COL_BLACK);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){15,45,100,25},
    (char*)"Country Code:",0,E_FONT_TXT);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK);

  static char mstr2[20] = "US"; // FIXME
  pElemRef = gslc_ElemCreateTxt(&m_gui, GSLC_ID_AUTO, E_PG_MAIN, (gslc_tsRect) {125,45,55,25},
    mstr2, sizeof(mstr2), E_FONT_TXT);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_ORANGE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK);
  m_pElemSel = pElemRef; // Save for quick access

  // Create GSLC_ID_AUTO text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){10,20,200,25},
    (char*)"Country Chooser",0,E_FONT_TITLE);
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK);

  // Create E_BTN_QUIT button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){217-60,190,60,30},(char*)"OK",0,E_FONT_TXT,&CbBtnCommon);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GREEN_DK2,GSLC_COL_GREEN_DK4,GSLC_COL_GREEN_DK1);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  
  return true;
}


int main( int argc, char* args[] )
{
  // -----------------------------------
  // Initialize
  gslc_InitDebug(&DebugOut);
  UserInitEnv();
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
  if (!gslc_FontSet(&m_gui,E_FONT_LISTBOX,GSLC_FONTREF_FNAME,FONT1,14)) { return 0; }
  if (!gslc_FontSet(&m_gui,E_FONT_TITLE,GSLC_FONTREF_FNAME,FONT2,22)) { return 0; }
  if (!gslc_FontSet(&m_gui,E_FONT_TXT,GSLC_FONTREF_FNAME,FONT3,14)) { return 0; }

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitOverlays();

  // ------------------------------------------------
  // Populate our Listbox with data
  // ------------------------------------------------
  ListboxLoad(m_pElemListbox);

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);


  // -----------------------------------
  // Main event loop
  while (!m_bQuit) {
    gslc_Update(&m_gui);

    usleep(50);
  }

  // -----------------------------------
  // Close down display
  gslc_Quit(&m_gui);

  return 0;
}
