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
unsigned m_nCount;


// Create the default elements on each page
bool InitOverlays()
{
  microSDL_tsElem   sElem;

  // -----------------------------------
  // Background
  //microSDL_SetBkgndColor(m_colGrayDk);
  //microSDL_SetBkgndImage((char*)"./res/grad_320x240.bmp");
  microSDL_SetBkgndImage((char*)IMG_BKGND);


  // -----------------------------------
  // PAGE: MAIN

  // Create background box
  sElem = microSDL_ElemCreateBox(MSDL_ID_ANON,E_PG_MAIN,(SDL_Rect){20,50,280,150});
  microSDL_ElemSetStyleMain(&sElem,m_colWhite,m_colBlack,m_colBlack);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create title
  sElem = microSDL_ElemCreateTxt(MSDL_ID_ANON,E_PG_MAIN,(SDL_Rect){10,10,300,40},
    "microSDL Demo",E_FONT_TITLE);
  sElem.eTxtAlignH = MSDL_ALIGN_H_MID;
  sElem.bFillEn = false;
  sElem.colElemText = m_colWhite;
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create Quit button with text label
  sElem = microSDL_ElemCreateBtnTxt(E_ELEM_BTN_QUIT,E_PG_MAIN,
  (SDL_Rect){100,140,50,20},"Quit",E_FONT_BTN);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create Extra button with text label
  sElem = microSDL_ElemCreateBtnTxt(E_ELEM_BTN_EXTRA,E_PG_MAIN,
  (SDL_Rect){170,140,50,20},"Extra",E_FONT_BTN);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create counter
  sElem = microSDL_ElemCreateTxt(MSDL_ID_ANON,E_PG_MAIN,(SDL_Rect){40,60,50,10},
    "Count:",E_FONT_TXT);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }
  sElem = microSDL_ElemCreateTxt(E_ELEM_TXT_COUNT,E_PG_MAIN,(SDL_Rect){100,60,50,10},
    "",E_FONT_TXT);
  sElem.colElemText = m_colYellow;
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create progress bar
  sElem = microSDL_ElemCreateTxt(MSDL_ID_ANON,E_PG_MAIN,(SDL_Rect){40,80,50,10},
    "Progress:",E_FONT_TXT);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }
  sElem = microSDL_ElemCreateGauge(E_ELEM_PROGRESS,E_PG_MAIN,(SDL_Rect){100,80,50,10},
    0,100,0,m_colGreenDk,false);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // -----------------------------------
  // PAGE: EXTRA

  // Create background box
  sElem = microSDL_ElemCreateBox(MSDL_ID_ANON,E_PG_EXTRA,(SDL_Rect){40,40,240,160});
  microSDL_ElemSetStyleMain(&sElem,m_colWhite,m_colBlack,m_colBlack);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create Back button with text label
  sElem = microSDL_ElemCreateBtnTxt(E_ELEM_BTN_BACK,E_PG_EXTRA,
  (SDL_Rect){135,120,50,20},"Back",E_FONT_BTN);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create a few labels
  unsigned nPosY = 50;
  unsigned nSpaceY = 20;
  sElem = microSDL_ElemCreateTxt(MSDL_ID_ANON,E_PG_EXTRA,(SDL_Rect){60,nPosY,50,10},
    "Data 1",E_FONT_TXT); nPosY += nSpaceY;
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }
  sElem = microSDL_ElemCreateTxt(MSDL_ID_ANON,E_PG_EXTRA,(SDL_Rect){60,nPosY,50,10},
    "Data 2",E_FONT_TXT); nPosY += nSpaceY;
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }
  sElem = microSDL_ElemCreateTxt(MSDL_ID_ANON,E_PG_EXTRA,(SDL_Rect){60,nPosY,50,10},
    "Data 3",E_FONT_TXT); nPosY += nSpaceY;
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }
  
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

  microSDL_InitEnv();
  microSDL_Init();

  microSDL_InitFont();
  microSDL_InitTs("/dev/input/touchscreen");

  // Load Fonts
  bOk = microSDL_FontAdd(E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); return false; }
  bOk = microSDL_FontAdd(E_FONT_TXT,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); return false; }
  bOk = microSDL_FontAdd(E_FONT_TITLE,FONT_DROID_SANS,32);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); return false; }


  // -----------------------------------
  // Create page elements
  // -----------------------------------
  InitOverlays();


  // -----------------------------------
  // Start display

  // Start up display on main page
  microSDL_SetPageCur(E_PG_MAIN);
  microSDL_ElemDrawPageCur();

  // -----------------------------------
  // Main event loop

  bQuit = false;
  while (!bQuit) {

    m_nCount++;

    unsigned nPageCur = microSDL_GetPageCur();

    // Immediate update of element on active page
    // - Note that these immediate updates can
    //   rely on an initial full-page draw when
    //   the page was first entered.
    if (nPageCur == E_PG_MAIN) {
      sprintf(acTxt,"%u",m_nCount);
      microSDL_ElemUpdateTxt(E_ELEM_TXT_COUNT,acTxt);
      microSDL_ElemDraw(E_ELEM_TXT_COUNT);

      microSDL_ElemUpdateGauge(E_ELEM_PROGRESS,((m_nCount/200)%100));
      microSDL_ElemDraw(E_ELEM_PROGRESS); 
    }
  
    // Poll for touchscreen presses
    if (microSDL_GetTsClick(nClickX,nClickY,nClickPress)) {
 
      // Track the touch event and find any associated object
      microSDL_TrackClick(nClickX,nClickY,nClickPress);
      nTrackElemClicked = microSDL_GetTrackElemClicked();

      // Any selectable object clicked?
      if (nTrackElemClicked != MSDL_ID_NONE) {

        // Convert element index to element ID
        nElemId = microSDL_ElemGetId(nTrackElemClicked);
        if (nElemId == E_ELEM_BTN_QUIT) {
          // Quit button pressed
          bQuit = true;
        } else if (nElemId == E_ELEM_BTN_EXTRA) {
          microSDL_SetPageCur(E_PG_EXTRA);
          microSDL_ElemDrawPageCur();
        } else if (nElemId == E_ELEM_BTN_BACK) {
          microSDL_SetPageCur(E_PG_MAIN);
          microSDL_ElemDrawPageCur();
        } // nElemId
  
        // Clear click event
        microSDL_ClearTrackElemClicked();
  
      } // Object clicked
    } // Touchscreen press
  } // bQuit


  // -----------------------------------
  // Close down display

  microSDL_FontCloseAll();
  microSDL_Quit();
}

