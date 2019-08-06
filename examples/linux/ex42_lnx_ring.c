//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 42 (LINUX):
//   - Demonstrate ring gauge, controlled by slider
//
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Include any extended elements
#include "elem/XSlider.h"
#include "elem/XRingGauge.h"


// Defines for resources
#define FONT_FNAME_BTN "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"
#define FONT_FNAME_DIAL "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"

// Enumerations for pages, elements, fonts, images
enum { E_PG_MAIN };
enum { E_ELEM_BOX, E_ELEM_BTN_QUIT, E_ELEM_XRING, E_ELEM_SLIDER };
enum { E_FONT_BTN, E_FONT_DIAL, MAX_FONT };

bool    m_bQuit = false;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    4

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

gslc_tsXSlider              m_sXSlider;
gslc_tsXRingGauge           m_sXRingGauge;

// Save some element references for quick access
gslc_tsElemRef*  m_pElemSlider = NULL;
gslc_tsElemRef*  m_pElemXRingGauge = NULL;

// Program global variables
int16_t m_nSliderPos = 0;


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
bool CbBtnQuit(void* pvGui, void *pvElemRef, gslc_teTouch eTouch, int16_t nX, int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}


// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui, void* pvElemRef, int16_t nPos)
{
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);
  //gslc_tsXSlider* pSlider = (gslc_tsXSlider*)(pElem->pXData);

  switch (pElem->nId) {
  case E_ELEM_SLIDER:
    m_nSliderPos = gslc_ElemXSliderGetPos(pGui, pElemRef);
    break;
  default:
    break;
  }
  return true;
}

// Create page elements
bool InitOverlays()
{
  gslc_tsElemRef* pElemRef = NULL;
  
  // -----------------------------------
  // Create page elements
  gslc_PageAdd(&m_gui, E_PG_MAIN, m_asPageElem, MAX_ELEM_PG_MAIN, m_asPageElemRef, MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui, GSLC_COL_GRAY_DK2);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui, E_ELEM_BOX, E_PG_MAIN, (gslc_tsRect) { 10, 50, 300, 150 });
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_WHITE, GSLC_COL_BLACK, GSLC_COL_BLACK);

  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui, E_ELEM_BTN_QUIT, E_PG_MAIN,
    (gslc_tsRect) { 235, 5, 80, 30 }, (char*)"Quit", 0, E_FONT_BTN, &CbBtnQuit);

  // Create a RingGauge
  static char m_str10[10] = "";
  pElemRef = gslc_ElemXRingGaugeCreate(&m_gui, E_ELEM_XRING, E_PG_MAIN, &m_sXRingGauge,
    (gslc_tsRect) { 80, 80, 100, 100 }, m_str10, 10, E_FONT_DIAL);
  gslc_ElemXRingGaugeSetValRange(&m_gui, pElemRef, 0, 100);
  gslc_ElemXRingGaugeSetVal(&m_gui, pElemRef, 60); // Set initial value
  // The following are some additional config options available
  //gslc_ElemXRingGaugeSetAngleRange(&m_gui, pElemRef, 0, 360, true);    // Full circle 
  //gslc_ElemXRingGaugeSetAngleRange(&m_gui, pElemRef, -90, 180, true);  // Upper Semi-circle
  //gslc_ElemXRingGaugeSetAngleRange(&m_gui, pElemRef, 0, 90, true);     // Top-Right Quarter-circle
  //gslc_ElemXRingGaugeSetAngleRange(&m_gui, pElemRef, -135, 270, true); // Three-Quarter circle
  //gslc_ElemXRingGaugeSetThickness(&m_gui,pElemRef, 8);
  //gslc_ElemXRingGaugeSetQuality(&m_gui,pElemRef, 72);
  //gslc_ElemXRingGaugeSetColorInactive(&m_gui,pElemRef, GSLC_COL_GRAY_DK3);
  //gslc_ElemXRingGaugeSetColorActiveFlat(&m_gui,pElemRef, GSLC_COL_ORANGE);
  //gslc_ElemXRingGaugeSetColorActiveGradient(&m_gui, pElemRef, GSLC_COL_BLUE_LT4, GSLC_COL_RED);
  m_pElemXRingGauge = pElemRef; // Save for quick access

   // Create slider
  pElemRef = gslc_ElemXSliderCreate(&m_gui, E_ELEM_SLIDER, E_PG_MAIN, &m_sXSlider,
    (gslc_tsRect) { 200, 80, 100, 20 }, 0, 100, 60, 5, false);
  gslc_ElemXSliderSetStyle(&m_gui, pElemRef, true, (gslc_tsColor) { 0, 0, 128 }, 10,
    5, (gslc_tsColor) { 64, 64, 64 });
  gslc_ElemXSliderSetPosFunc(&m_gui, pElemRef, &CbSlidePos);
  m_pElemSlider = pElemRef; // Save for quick access
  
  return true;
}


int main( int argc, char* args[] )
{
  char   acStr[10];  

  // -----------------------------------
  // Initialize
  gslc_InitDebug(&DebugOut);
  UserInitEnv();
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  // - Normally we would select a number of different fonts 
  if (!gslc_FontSet(&m_gui, E_FONT_BTN, GSLC_FONTREF_FNAME, FONT_FNAME_BTN, 10)) { return 0; }
  if (!gslc_FontSet(&m_gui, E_FONT_DIAL, GSLC_FONTREF_FNAME, FONT_FNAME_DIAL, 18)) { return 0; }

  // -----------------------------------
  // Start display
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  m_bQuit = false;

  // Initialize startup state
  m_nSliderPos = gslc_ElemXSliderGetPos(&m_gui, m_pElemSlider);

  // -----------------------------------
  // Main event loop
  while (!m_bQuit) {
  
    // Update the XRingGauge position with the slider position
    gslc_ElemXRingGaugeSetVal(&m_gui, m_pElemXRingGauge, m_nSliderPos);

    // Update the XRingGauge text string with a percentage
    snprintf(acStr, 10, "%d%%", m_nSliderPos);
    gslc_ElemSetTxtStr(&m_gui, m_pElemXRingGauge, acStr);  
  
    gslc_Update(&m_gui);
  }

  // -----------------------------------
  // Close down display
  gslc_Quit(&m_gui);

  return 0;
}
