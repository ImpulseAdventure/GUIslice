//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 09 (LINUX):
//     Demonstrate radial and ramp controls
//     NOTE: The ramp control is still beta and subject to change
//     NOTE: The radial control is disabled by default, but can
//           be enabled by GSLC_FEATURE_XGAUGE_RADIAL
//
#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include "unistd.h"   // For usleep() //xxx

// Defines for resources
#define FONT1 "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_COLOR,
      E_RADIAL,E_RAMP,E_SLIDER,E_ELEM_TXT_COUNT};
enum {E_FONT_BTN,E_FONT_TXT,E_FONT_TITLE};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_FONT            3
#define MAX_ELEM_PG_MAIN    8

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

gslc_tsXGauge               m_sXRadial,m_sXRamp;
gslc_tsXSlider              m_sXSlider;


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

// Quit button callback
bool CbBtnQuit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}


bool CbSlideRadial(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  char    acTxt[8];
  int16_t nVal;

  // Fetch the new RGB component from the slider
  switch (pElem->nId) {
    case E_SLIDER:
      nVal = gslc_ElemXSliderGetPos(pGui,pElemRef);

      // Link slider to the radial control
      gslc_tsElemRef* pElemRad = gslc_PageFindElemById(pGui,E_PG_MAIN,E_RADIAL);
      gslc_ElemXGaugeUpdate(pGui,pElemRad,nVal);

      // Link slider to the ramp control
      gslc_tsElemRef* pElemRamp = gslc_PageFindElemById(pGui,E_PG_MAIN,E_RAMP);
      gslc_ElemXGaugeUpdate(pGui,pElemRamp,nVal);

      // Link slider to the numerical display
      snprintf(acTxt,8,"%u",nVal);
      gslc_tsElemRef* pElemCnt = gslc_PageFindElemById(pGui,E_PG_MAIN,E_ELEM_TXT_COUNT);
      gslc_ElemSetTxtStr(pGui,pElemCnt,acTxt);
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
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create Title with offset shadow
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){2,2,320,50},
    "Directional",0,E_FONT_TITLE);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,(gslc_tsColor){32,32,60});
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){0,0,320,50},
    "Directional",0,E_FONT_TITLE);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,(gslc_tsColor){128,128,240});
  gslc_ElemSetTxtAlign(&m_gui,pElemRef,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(&m_gui,pElemRef,false);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,300,180});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_RADIAL,E_PG_MAIN,&m_sXRadial,
          (gslc_tsRect){210,140,80,80},0,100,0,GSLC_COL_YELLOW,false);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXGaugeSetStyle(&m_gui,pElemRef,GSLCX_GAUGE_STYLE_RADIAL);
  gslc_ElemXGaugeSetIndicator(&m_gui,pElemRef,GSLC_COL_YELLOW,30,3,true);
  gslc_ElemXGaugeSetTicks(&m_gui,pElemRef,GSLC_COL_GRAY_LT1,8,5);

  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_RAMP,E_PG_MAIN,&m_sXRamp,
          (gslc_tsRect){80,140,100,80},0,100,50,GSLC_COL_YELLOW,false);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXGaugeSetStyle(&m_gui,pElemRef,GSLCX_GAUGE_STYLE_RAMP);

  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_SLIDER,E_PG_MAIN,&m_sXSlider,
          (gslc_tsRect){20,60,140,20},0,100,50,5,false);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GREEN,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,GSLC_COL_GREEN_DK4,10,5,GSLC_COL_GRAY_DK2);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlideRadial);

  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){180,60,40,20},
    "",0,E_FONT_TXT);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){250,60,50,30},"QUIT",0,E_FONT_BTN,&CbBtnQuit);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);


  return true;
}


int main( int argc, char* args[] )
{
  bool                bOk = true;

  // -----------------------------------
  // Initialize
  gslc_InitDebug(&DebugOut);
  UserInitEnv();
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  // - Normally we would select a number of different fonts
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_FNAME,FONT1,14);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_FNAME,FONT1,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TITLE,GSLC_FONTREF_FNAME,FONT1,36);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }

  // -----------------------------------
  // Start display
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // -----------------------------------
  // Main event loop
  while (!m_bQuit) {
    gslc_Update(&m_gui);
  }

  // -----------------------------------
  // Close down display
  gslc_Quit(&m_gui);

  return 0;
}
