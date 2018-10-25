//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 04 (LINUX): Dynamic content
//     Demonstrates push buttons, checkboxes and slider controls
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <time.h> // For clock() (frame rate reporting)

// Defines for resources
#define FONT1 "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,E_ELEM_PROGRESS1,
      E_ELEM_CHECK1,E_ELEM_RADIO1,E_ELEM_RADIO2,E_ELEM_SLIDER,E_ELEM_TXT_SLIDER};
enum {E_FONT_BTN,E_FONT_TXT};
enum {E_GROUP1};

bool        m_bQuit = false;

// Free-running counter for display
unsigned    m_nCount = 0;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_FONT            5
#define MAX_ELEM_PG_MAIN    21

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

gslc_tsXGauge               m_sXGauge,m_sXGauge1;
gslc_tsXCheckbox            m_asXCheck[3];
gslc_tsXSlider              m_sXSlider;

#define MAX_STR             100

// Enable frame/update rate reporting? (1 to enable, 0 to disable)
#define TEST_UPDATE_RATE     0

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
bool CbBtnQuit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}


// Create page elements
bool InitOverlays()
{
  gslc_tsElemRef*  pElemRef;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);


  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);


  // Create Quit button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){160,80,80,40},"Quit",0,E_FONT_BTN,&CbBtnQuit);

  // Create counter
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,60,50,10},
    "Count:",0,E_FONT_TXT);
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_tsRect){80,60,50,10},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_YELLOW);

  // Create progress bar (horizontal)
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,80,50,10},
    "Progress:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,
    (gslc_tsRect){80,80,50,10},0,100,0,GSLC_COL_GREEN,false);

  // Second progress bar (vertical)
  // - Demonstration of vertical bar with offset zero-pt showing both positive and negative range
  pElemRef = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS1,E_PG_MAIN,&m_sXGauge1,
    (gslc_tsRect){280,80,10,100},-25,75,-15,GSLC_COL_RED,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create checkbox 1
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,100,20,20},
    "Check1:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_CHECK1,E_PG_MAIN,&m_asXCheck[0],
    (gslc_tsRect){80,100,20,20},false,GSLCX_CHECKBOX_STYLE_X,GSLC_COL_BLUE_LT2,false);

  // Create radio 1
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,135,20,20},
    "Radio1:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO1,E_PG_MAIN,&m_asXCheck[1],
    (gslc_tsRect){80,135,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);
  gslc_ElemSetGroup(&m_gui,pElemRef,E_GROUP1);

  // Create radio 2
  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){20,160,20,20},
    "Radio2:",0,E_FONT_TXT);
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_RADIO2,E_PG_MAIN,&m_asXCheck[2],
    (gslc_tsRect){80,160,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_ORANGE,false);
  gslc_ElemSetGroup(&m_gui,pElemRef,E_GROUP1);

  // Create slider
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_ELEM_SLIDER,E_PG_MAIN,&m_sXSlider,
    (gslc_tsRect){160,140,100,20},0,100,60,5,false);
  gslc_ElemXSliderSetStyle(&m_gui,pElemRef,true,(gslc_tsColor){0,0,128},10,
          5,(gslc_tsColor){64,64,64});
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_SLIDER,E_PG_MAIN,(gslc_tsRect){160,160,80,20},
    "Slider: ???",0,E_FONT_TXT);

  return true;
}


int main( int argc, char* args[] )
{
  char                acTxt[100];

  // -----------------------------------
  // Initialize
  gslc_InitDebug(&DebugOut);
  UserInitEnv();
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  gslc_FontAdd(&m_gui,E_FONT_BTN,GSLC_FONTREF_FNAME,FONT1,12);
  gslc_FontAdd(&m_gui,E_FONT_TXT,GSLC_FONTREF_FNAME,FONT1,10);


  // -----------------------------------
  // Start display
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // Save some element references for quick access
  gslc_tsElemRef*  pElemCnt        = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_tsElemRef*  pElemProgress   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);
  gslc_tsElemRef*  pElemProgress1  = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS1);
  gslc_tsElemRef*  pElemSlider     = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_SLIDER);
  gslc_tsElemRef*  pElemSliderTxt  = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_SLIDER);

  // -----------------------------------
  // Main event loop

  #if (TEST_UPDATE_RATE)
  uint32_t  nNumUpdates = 0;
  clock_t   sClkStart,sClkEnd;
  sClkStart = clock();
  #endif

  m_bQuit = false;
  while (!m_bQuit) {

    // General counter
    m_nCount++;

    // -----------------------------------

    // Update elements on active page
    snprintf(acTxt,MAX_STR,"%u",m_nCount);
    gslc_ElemSetTxtStr(&m_gui,pElemCnt,acTxt);

    gslc_ElemXGaugeUpdate(&m_gui,pElemProgress,((m_nCount/200)%100));

    // NOTE: A more efficient method is to move the following
    //       code into the slider position callback function.
    //       Please see example 07.
    int nPos = gslc_ElemXSliderGetPos(&m_gui,pElemSlider);
    snprintf(acTxt,MAX_STR,"Slider: %u",nPos);
    gslc_ElemSetTxtStr(&m_gui,pElemSliderTxt,acTxt);

    gslc_ElemXGaugeUpdate(&m_gui,pElemProgress1,(nPos*80.0/100.0)-15);

    // Periodically call GUIslice update function
    gslc_Update(&m_gui);

    // Simple update rate reporting
    #if (TEST_UPDATE_RATE)
    nNumUpdates++;
    sClkEnd = clock();
    if ((sClkEnd - sClkStart) > 10*1000000) {
      // Reached end of interval, report average
      printf("DBG: Update rate = [%6u per 10 sec]\n",nNumUpdates);
      // Reset interval
      nNumUpdates = 0;
      sClkStart = sClkEnd;
    }
    #endif

  } // bQuit

  // Read checkbox state
  // - Either read individual checkboxes
  //   bool bCheck = gslc_ElemXCheckboxGetState(&m_gui,E_ELEM_CHECK1);
  // - Or find one in the group that was checked (eg. for radio buttons)
  //   gslc_tsElem* pElem = gslc_ElemXCheckboxFindChecked(&m_gui,GSLC_GROUP_ID_NONE);

  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}

