//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 05: Multiple page handling
//

#include "microsdl.h"


// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"
#define IMG_BKGND       "./res/bkgnd1_320x240.bmp"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN,E_PG_EXTRA};
enum {E_ELEM_BTN_QUIT,E_ELEM_BTN_EXTRA,E_ELEM_BTN_BACK,
      E_ELEM_TXT_COUNT,E_ELEM_PROGRESS};
enum {E_FONT_BTN,E_FONT_TXT,E_FONT_TITLE};

// Free-running counter for display
unsigned m_nCount = 0;

microSDL_tsGui  m_gui;

// Create the default elements on each page
bool InitOverlays()
{
  int               nElemId;

  // -----------------------------------
  // Background
  microSDL_SetBkgndImage(&m_gui,(char*)IMG_BKGND);


  // -----------------------------------
  // PAGE: MAIN

  // Create background box
  nElemId = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,50,280,150});
  microSDL_ElemSetCol(&m_gui,nElemId,m_colWhite,m_colBlack,m_colBlack);

  // Create title
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){10,10,310,40},
    "microSDL Demo",E_FONT_TITLE);
  microSDL_ElemSetTxtAlign(&m_gui,nElemId,MSDL_ALIGN_MID_MID);
  microSDL_ElemSetFillEn(&m_gui,nElemId,false);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,m_colWhite);

  // Create Quit button with text label
  nElemId = microSDL_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (SDL_Rect){100,140,50,20},"Quit",E_FONT_BTN);

  // Create Extra button with text label
  nElemId = microSDL_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_EXTRA,E_PG_MAIN,
    (SDL_Rect){170,140,50,20},"Extra",E_FONT_BTN);

  // Create counter
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){40,60,50,10},
    "Count:",E_FONT_TXT);
  nElemId = microSDL_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(SDL_Rect){100,60,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,m_colYellow);

  // Create progress bar
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){40,80,50,10},
    "Progress:",E_FONT_TXT);
  nElemId = microSDL_ElemCreateGauge(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,(SDL_Rect){100,80,50,10},
    0,100,0,m_colGreenDk,false);

  // -----------------------------------
  // PAGE: EXTRA

  // Create background box
  nElemId = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_EXTRA,(SDL_Rect){40,40,240,160});
  microSDL_ElemSetCol(&m_gui,nElemId,m_colWhite,m_colBlack,m_colBlack);

  // Create Back button with text label
  nElemId = microSDL_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_BACK,E_PG_EXTRA,
    (SDL_Rect){135,120,50,20},"Back",E_FONT_BTN);

  // Create a few labels
  unsigned nPosY = 50;
  unsigned nSpaceY = 20;
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_EXTRA,(SDL_Rect){60,nPosY,50,10},
    "Data 1",E_FONT_TXT); nPosY += nSpaceY;
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_EXTRA,(SDL_Rect){60,nPosY,50,10},
    "Data 2",E_FONT_TXT); nPosY += nSpaceY;
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_EXTRA,(SDL_Rect){60,nPosY,50,10},
    "Data 3",E_FONT_TXT); nPosY += nSpaceY;
  
  return true;
}


int main( int argc, char* args[] )
{
  bool              bOk = true;
  bool              bQuit = false;  
  int               nRet;
  int               nClickX,nClickY;
  unsigned          nClickPress;
  int               nElemId;
  int               nTrackElemClicked;

  char              acTxt[100];

  // -----------------------------------
  // Initialize

  microSDL_InitEnv(&m_gui);
  microSDL_Init(&m_gui);

  microSDL_InitFont(&m_gui);
  microSDL_InitTs(&m_gui,"/dev/input/touchscreen");

  // Load Fonts
  // - In this example, we are loading the same font but at
  //   different point sizes. We could also refer to other
  //   font files as well.
  bOk = microSDL_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); return false; }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_TXT,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); return false; }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_TITLE,FONT_DROID_SANS,32);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); return false; }


  // -----------------------------------
  // Create page elements
  // -----------------------------------
  InitOverlays();


  // -----------------------------------
  // Start display

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);
  microSDL_ElemDrawPageCur(&m_gui);
  // -----------------------------------
  // Main event loop

  bQuit = false;
  while (!bQuit) {

    m_nCount++;

    unsigned nPageCur = microSDL_GetPageCur(&m_gui);

    // Immediate update of element on active page
    // - Note that these immediate updates can
    //   rely on an initial full-page draw when
    //   the page was first entered.
    if (nPageCur == E_PG_MAIN) {
      sprintf(acTxt,"%u",m_nCount);
      microSDL_ElemSetTxtStr(&m_gui,E_ELEM_TXT_COUNT,acTxt);
      microSDL_ElemDraw(&m_gui,E_ELEM_TXT_COUNT);

      microSDL_ElemUpdateGauge(&m_gui,E_ELEM_PROGRESS,((m_nCount/200)%100));
      microSDL_ElemDraw(&m_gui,E_ELEM_PROGRESS); 
    }
  
    // Poll for touchscreen presses
    if (microSDL_GetTsClick(&m_gui,nClickX,nClickY,nClickPress)) {
 
      // Track the touch event and find any associated object
      microSDL_TrackClick(&m_gui,nClickX,nClickY,nClickPress);
      nTrackElemClicked = microSDL_GetTrackElemClicked(&m_gui);

      // Any selectable object clicked?
      if (nTrackElemClicked == E_ELEM_BTN_QUIT) {
        // Quit button pressed
        bQuit = true;
      } else if (nTrackElemClicked == E_ELEM_BTN_EXTRA) {
        microSDL_SetPageCur(&m_gui,E_PG_EXTRA);
        microSDL_ElemDrawPageCur(&m_gui);
      } else if (nTrackElemClicked == E_ELEM_BTN_BACK) {
        microSDL_SetPageCur(&m_gui,E_PG_MAIN);
        microSDL_ElemDrawPageCur(&m_gui);
      } // nTrackElemClicked
  
      // Clear click event
      microSDL_ClearTrackElemClicked(&m_gui);

    } // Touchscreen press
  } // bQuit


  // -----------------------------------
  // Close down display

  microSDL_FontCloseAll(&m_gui);
  microSDL_Quit(&m_gui);
}

