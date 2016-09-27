//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 04: Dynamic content
//

#include "microsdl.h"


// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS};
enum {E_FONT_BTN,E_FONT_TXT};

// Free-running counter for display
unsigned m_nCount;

// Create page elements
bool InitOverlays()
{
  microSDL_tsElem   sElem;

  // Background flat color
  microSDL_SetBkgndColor(m_colGrayDk);

  // Create background box
  sElem = microSDL_ElemCreateBox(E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetStyleMain(&sElem,m_colWhite,m_colBlack,m_colBlack);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create Quit button with text label
  sElem = microSDL_ElemCreateBtnTxt(E_ELEM_BTN_QUIT,E_PG_MAIN,
  (SDL_Rect){120,100,80,40},"Quit",E_FONT_BTN);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create counter
  sElem = microSDL_ElemCreateTxt(MSDL_ID_ANON,E_PG_MAIN,(SDL_Rect){20,60,50,10},
    "Count:",E_FONT_TXT);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }
  sElem = microSDL_ElemCreateTxt(E_ELEM_TXT_COUNT,E_PG_MAIN,(SDL_Rect){80,60,50,10},
    "",E_FONT_TXT);
  sElem.colElemText = m_colYellow;
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }

  // Create progress bar
  sElem = microSDL_ElemCreateTxt(MSDL_ID_ANON,E_PG_MAIN,(SDL_Rect){20,80,50,10},
    "Progress:",E_FONT_TXT);
  if (!microSDL_ElemAdd(sElem)) { fprintf(stderr,"ERROR: ElemAdd failed\n"); return false; }
  sElem = microSDL_ElemCreateGauge(E_ELEM_PROGRESS,E_PG_MAIN,(SDL_Rect){80,80,50,10},
    0,100,0,m_colGreenDk,false);
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
  bOk = microSDL_FontAdd(E_FONT_TXT,FONT_DROID_SANS,8);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); return false; }


  // -----------------------------------
  // Start display
  InitOverlays();

  // Start up display on main page
  microSDL_SetPageCur(E_PG_MAIN);
  microSDL_ElemDrawPageCur();

  // -----------------------------------
  // Main event loop

  bQuit = false;
  while (!bQuit) {

    m_nCount++;

    // Immediate update of element on active page
    sprintf(acTxt,"%u",m_nCount);
    microSDL_ElemUpdateTxt(E_ELEM_TXT_COUNT,acTxt);
    microSDL_ElemDraw(E_ELEM_TXT_COUNT);

    microSDL_ElemUpdateGauge(E_ELEM_PROGRESS,((m_nCount/200)%100));
    microSDL_ElemDraw(E_ELEM_PROGRESS); 
  
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
        }
  
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

