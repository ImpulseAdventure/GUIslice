//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 07: Sliders with dynamic color control
//

#include "GUIslice.h"
#include "GUIslice_ex.h"

// Define default device paths for SDL framebuffer & touchscreen
#define GSLC_SDL_DEV_FB     "/dev/fb1"
#define GSLC_SDL_DEV_TOUCH  "/dev/input/touchscreen"

// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_COLOR,
      E_SLIDER_R,E_SLIDER_G,E_SLIDER_B};
enum {E_FONT_BTN,E_FONT_TXT,E_FONT_HEAD,E_FONT_TITLE};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

// Instantiate the GUI
#define MAX_FONT            10
gslc_tsGui                  m_gui;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsXSlider              m_sXSlider_R,m_sXSlider_G,m_sXSlider_B;

#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    30
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];


// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int nX,int nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Create page elements
bool InitOverlays()
{
  gslc_tsElem*  pElem = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create Title with offset shadow
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){2,2,320,50},
    "Home Automation",E_FONT_TITLE);
  gslc_ElemSetTxtCol(pElem,(gslc_Color){32,32,60});
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pElem,false);
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){0,0,320,50},
    "Home Automation",E_FONT_TITLE);
  gslc_ElemSetTxtCol(pElem,(gslc_Color){128,128,240});
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_MID);
  gslc_ElemSetFillEn(pElem,false);
  
  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_Rect){10,50,300,180});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create dividers
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){20,100,280,1});
  gslc_ElemSetCol(pElem,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);  
  
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){235,60,1,35});
  gslc_ElemSetCol(pElem,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);  
  
  
  // Create color box
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_COLOR,E_PG_MAIN,(gslc_Rect){20,90+30,130,100});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create Quit button with text label
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_Rect){250,60,50,30},"SAVE",E_FONT_BTN,&CbBtnQuit);
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_DK2,GSLC_COL_BLUE_DK4,GSLC_COL_BLUE_DK1);    
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  
  // Create dummy selector
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){20,65,100,20},
    "Selected Room:",E_FONT_HEAD);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT2);

  pElem = gslc_ElemCreateBtnTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,
    (gslc_Rect){140,65,80,20},"Kitchen...",E_FONT_BTN,NULL);
  gslc_ElemSetCol(pElem,GSLC_COL_GRAY_DK2,GSLC_COL_GRAY_DK3,GSLC_COL_BLUE_DK1);    
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  
  // Create sliders
  // - Define element arrangement
  int nCtrlY    = 115;
  int nCtrlGap  = 30;
  int nSlideW   = 80;
  int nSlideH   = 20;
  int nLabelX   = 160;
  int nLabelW   = 30;
  int nLabelH   = 20;
  int nSlideX   = nLabelX + nLabelW + 20;
  
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){nLabelX,nCtrlY,120,20},
    "Set LED RGB:",E_FONT_HEAD);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  nCtrlY += 25;
  
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){nLabelX,nCtrlY,nLabelW,nLabelH},
    "Red:",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT3);  
  pElem = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_R,E_PG_MAIN,&m_sXSlider_R,
          (gslc_Rect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,255,5,false);
  gslc_ElemSetCol(pElem,GSLC_COL_RED,GSLC_COL_BLACK,GSLC_COL_BLACK);          
  gslc_ElemXSliderSetStyle(pElem,true,GSLC_COL_RED_DK4,10,5,GSLC_COL_GRAY_DK2);
  nCtrlY += nCtrlGap;
  
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){nLabelX,nCtrlY,nLabelW,nLabelH},
    "Green:",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT3);  
  pElem = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_G,E_PG_MAIN,&m_sXSlider_G,
          (gslc_Rect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,128,5,false);
  gslc_ElemSetCol(pElem,GSLC_COL_GREEN,GSLC_COL_BLACK,GSLC_COL_BLACK);
  gslc_ElemXSliderSetStyle(pElem,true,GSLC_COL_GREEN_DK4,10,5,GSLC_COL_GRAY_DK2);
  nCtrlY += nCtrlGap;
  
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){nLabelX,nCtrlY,nLabelW,nLabelH},
    "Blue:",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT3);  
  pElem = gslc_ElemXSliderCreate(&m_gui,E_SLIDER_B,E_PG_MAIN,&m_sXSlider_B,
          (gslc_Rect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,0,5,false);
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_BLACK);          
  gslc_ElemXSliderSetStyle(pElem,true,GSLC_COL_BLUE_DK4,10,5,GSLC_COL_GRAY_DK2);
  nCtrlY += nCtrlGap;

  
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){250,230,60,10},
    "GUIslice Example",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_BLACK); 
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_RIGHT); 
  gslc_ElemSetFillEn(pElem,false); 
  
  return true;
}


int main( int argc, char* args[] )
{
  bool                bOk = true;

  // -----------------------------------
  // Initialize

  gslc_InitEnv(GSLC_SDL_DEV_FB,GSLC_SDL_DEV_TOUCH);

  if (!gslc_Init(&m_gui,m_asPage,MAX_PAGE,m_asFont,MAX_FONT,NULL,0)) { exit(1); }  

  gslc_InitTs(&m_gui,GSLC_SDL_DEV_TOUCH);

  // Load Fonts
  // - Normally we would select a number of different fonts
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,14);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_HEAD,FONT_DROID_SANS,14);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TITLE,FONT_DROID_SANS,36);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }

  // -----------------------------------
  // Start display
  InitOverlays();

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // Save some element references for quick access  
  gslc_tsElem*  pElemSliderR  = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_SLIDER_R);
  gslc_tsElem*  pElemSliderG  = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_SLIDER_G);
  gslc_tsElem*  pElemSliderB  = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_SLIDER_B);
  gslc_tsElem*  pElemColor    = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_COLOR);
  
  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {

    // General counter
    m_nCount++;

    // -----------------------------------

    // Update elements on active page
    // - TODO: Replace with Slider callback
    int nPosR = gslc_ElemXSliderGetPos(pElemSliderR);
    int nPosG = gslc_ElemXSliderGetPos(pElemSliderG);
    int nPosB = gslc_ElemXSliderGetPos(pElemSliderB);
    gslc_Color colRGB = (gslc_Color){nPosR,nPosG,nPosB};
    gslc_ElemSetCol(pElemColor,GSLC_COL_WHITE,colRGB,GSLC_COL_WHITE);
    
    // Periodically call GUIslice update function
    gslc_Update(&m_gui);
    
  } // bQuit

  // Read slider state:
  // int nPosR = gslc_ElemXSliderGetPos(pElemSliderR);

  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}

