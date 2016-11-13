//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 07: Sliders with dynamic color control
//

#include "microsdl.h"
#include "microsdl_ex.h"

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
#define MAX_FONT      10
microSDL_tsGui        m_gui;
microSDL_tsFont       m_asFont[MAX_FONT];
microSDL_tsXSlider    m_sXSlider_R,m_sXSlider_G,m_sXSlider_B;

#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    30
microSDL_tsPage             m_asPage[MAX_PAGE];
microSDL_tsElem             m_asPageElem[MAX_ELEM_PG_MAIN];


// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,microSDL_teTouch eTouch,int nX,int nY)
{
  if (eTouch == MSDL_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Create page elements
bool InitOverlays()
{
  microSDL_tsElem*  pElem = NULL;

  microSDL_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);
  
  // Background flat color
  microSDL_SetBkgndColor(&m_gui,MSDL_COL_GRAY_DK);

  // Create Title with offset shadow
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){2,2,320,50},
    "Home Automation",E_FONT_TITLE);
  microSDL_ElemSetTxtCol(pElem,(SDL_Color){32,32,60});
  microSDL_ElemSetTxtAlign(pElem,MSDL_ALIGN_MID_MID);
  microSDL_ElemSetFillEn(pElem,false);
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){0,0,320,50},
    "Home Automation",E_FONT_TITLE);
  microSDL_ElemSetTxtCol(pElem,(SDL_Color){128,128,240});
  microSDL_ElemSetTxtAlign(pElem,MSDL_ALIGN_MID_MID);
  microSDL_ElemSetFillEn(pElem,false);
  
  // Create background box
  pElem = microSDL_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,180});
  microSDL_ElemSetCol(pElem,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);
  
  // Create dividers
  pElem = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,100,280,1});
  microSDL_ElemSetCol(pElem,MSDL_COL_GRAY_DK,MSDL_COL_BLACK,MSDL_COL_BLACK);  
  
  pElem = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){235,60,1,35});
  microSDL_ElemSetCol(pElem,MSDL_COL_GRAY_DK,MSDL_COL_BLACK,MSDL_COL_BLACK);  
  
  
  // Create color box
  pElem = microSDL_ElemCreateBox(&m_gui,E_ELEM_COLOR,E_PG_MAIN,(SDL_Rect){20,90+30,130,100});
  microSDL_ElemSetCol(pElem,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);
  
  // Create Quit button with text label
  pElem = microSDL_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (SDL_Rect){250,60,50,30},"SAVE",E_FONT_BTN,&CbBtnQuit);
  microSDL_ElemSetCol(pElem,(SDL_Color){0,0,192},(SDL_Color){0,0,128},(SDL_Color){0,0,224});    
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_WHITE);
  
  // Create dummy selector
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,65,100,20},
    "Selected Room:",E_FONT_HEAD);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_GRAY_LT);

  pElem = microSDL_ElemCreateBtnTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,
    (SDL_Rect){140,65,80,20},"Kitchen...",E_FONT_BTN,NULL);
  microSDL_ElemSetCol(pElem,(SDL_Color){64,64,64},(SDL_Color){32,32,32},(SDL_Color){0,0,224});    
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_WHITE);
  
  // Create sliders
  // - Define element arrangement
  int nCtrlY    = 115;
  int nCtrlGap  = 30;
  int nSlideW   = 80;
  int nSlideH   = 20;
  int nLabelX   = 160;
  int nLabelW   = 30;
  int nLabelH   = 20;
  int nSlideX = nLabelX + nLabelW + 20;
  
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){nLabelX,nCtrlY,120,20},
    "Set LED RGB:",E_FONT_HEAD);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_WHITE);
  nCtrlY += 25;
  
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){nLabelX,nCtrlY,nLabelW,nLabelH},
    "Red:",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_GRAY_LT);  
  pElem = microSDL_ElemXSliderCreate(&m_gui,E_SLIDER_R,E_PG_MAIN,&m_sXSlider_R,
          (SDL_Rect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,255,5,false);
  microSDL_ElemSetCol(pElem,(SDL_Color){255,0,0},MSDL_COL_BLACK,MSDL_COL_WHITE);          
  microSDL_ElemXSliderSetStyle(pElem,true,(SDL_Color){128,0,0},10,
          5,(SDL_Color){64,64,64});
  nCtrlY += nCtrlGap;
  
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){nLabelX,nCtrlY,nLabelW,nLabelH},
    "Green:",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_GRAY_LT);  
  pElem = microSDL_ElemXSliderCreate(&m_gui,E_SLIDER_G,E_PG_MAIN,&m_sXSlider_G,
          (SDL_Rect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,128,5,false);
  microSDL_ElemSetCol(pElem,(SDL_Color){0,255,0},MSDL_COL_BLACK,MSDL_COL_WHITE);
  microSDL_ElemXSliderSetStyle(pElem,true,(SDL_Color){0,128,0},10,
          5,(SDL_Color){64,64,64});
  nCtrlY += nCtrlGap;
  
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){nLabelX,nCtrlY,nLabelW,nLabelH},
    "Blue:",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_GRAY_LT);  
  pElem = microSDL_ElemXSliderCreate(&m_gui,E_SLIDER_B,E_PG_MAIN,&m_sXSlider_B,
          (SDL_Rect){nSlideX,nCtrlY,nSlideW,nSlideH},0,255,0,5,false);
  microSDL_ElemSetCol(pElem,(SDL_Color){0,0,255},MSDL_COL_BLACK,MSDL_COL_WHITE);          
  microSDL_ElemXSliderSetStyle(pElem,true,(SDL_Color){0,0,128},10,
          5,(SDL_Color){64,64,64});
  nCtrlY += nCtrlGap;

  
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){250,230,60,10},
    "microSDL GUI Example",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_BLACK); 
  microSDL_ElemSetTxtAlign(pElem,MSDL_ALIGN_MID_RIGHT); 
  microSDL_ElemSetFillEn(pElem,false); 
  
  return true;
}


int main( int argc, char* args[] )
{
  bool                bOk = true;

  // -----------------------------------
  // Initialize

  microSDL_InitEnv(&m_gui);

  if (!microSDL_Init(&m_gui,m_asPage,MAX_PAGE,m_asFont,MAX_FONT,NULL,0)) { exit(1); }  

  microSDL_InitTs(&m_gui,"/dev/input/touchscreen");

  // Load Fonts
  // - Normally we would select a number of different fonts
  bOk = microSDL_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,14);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_TXT,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_HEAD,FONT_DROID_SANS,14);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_TITLE,FONT_DROID_SANS,36);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }

  // -----------------------------------
  // Start display
  InitOverlays();

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);

  // Save some element references for quick access  
  microSDL_tsElem*  pElemSliderR  = microSDL_PageFindElemById(&m_gui,E_PG_MAIN,E_SLIDER_R);
  microSDL_tsElem*  pElemSliderG  = microSDL_PageFindElemById(&m_gui,E_PG_MAIN,E_SLIDER_G);
  microSDL_tsElem*  pElemSliderB  = microSDL_PageFindElemById(&m_gui,E_PG_MAIN,E_SLIDER_B);
  microSDL_tsElem*  pElemColor    = microSDL_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_COLOR);
  
  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {

    // General counter
    m_nCount++;

    // -----------------------------------

    // Update elements on active page
    // - TODO: Replace with Slider callback
    int nPosR = microSDL_ElemXSliderGetPos(pElemSliderR);
    int nPosG = microSDL_ElemXSliderGetPos(pElemSliderG);
    int nPosB = microSDL_ElemXSliderGetPos(pElemSliderB);
    SDL_Color colRGB = (SDL_Color){nPosR,nPosG,nPosB};
    microSDL_ElemSetCol(pElemColor,MSDL_COL_WHITE,colRGB,MSDL_COL_WHITE);
    
    // Periodically call microSDL update function
    microSDL_Update(&m_gui);
    
  } // bQuit

  // Read slider state:
  // int nPosR = microSDL_ElemXSliderGetPos(pElemSliderR);

  // -----------------------------------
  // Close down display

  microSDL_Quit(&m_gui);

  return 0;
}

