//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 05: Multiple page handling
//

#include "microsdl.h"
#include "microsdl_ex.h"

#include <libgen.h>       // For path parsing

// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"
#define IMG_BKGND       "/res/bkgnd1_320x240.bmp"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN,E_PG_EXTRA};
enum {E_ELEM_BTN_QUIT,E_ELEM_BTN_EXTRA,E_ELEM_BTN_BACK,
      E_ELEM_TXT_COUNT,E_ELEM_PROGRESS};
enum {E_FONT_BTN,E_FONT_TXT,E_FONT_TITLE};

// Free-running counter for display
unsigned m_nCount = 0;

// Instantiate the GUI
#define MAX_ELEM    30
#define MAX_FONT    10
microSDL_tsGui      m_gui;
microSDL_tsElem     m_asElem[MAX_ELEM];
microSDL_tsFont     m_asFont[MAX_FONT];
microSDL_tsXGauge   m_sXGauge;

// Create the default elements on each page
// - strPath: Path to executable passed in to locate resource files
bool InitOverlays(char *strPath)
{
  int               nElemId;

  // -----------------------------------
  // Background
  char* strImgBkgndPath = (char*)malloc(strlen(strPath)+strlen(IMG_BKGND)+1);
  strcpy(strImgBkgndPath, strPath);
  strcat(strImgBkgndPath, IMG_BKGND);
  microSDL_SetBkgndImage(&m_gui,strImgBkgndPath);
  free(strImgBkgndPath);
  
  // -----------------------------------
  // PAGE: MAIN

  // Create background box
  nElemId = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,50,280,150});
  microSDL_ElemSetCol(&m_gui,nElemId,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Create title
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){10,10,310,40},
    "microSDL Demo",E_FONT_TITLE);
  microSDL_ElemSetTxtAlign(&m_gui,nElemId,MSDL_ALIGN_MID_MID);
  microSDL_ElemSetFillEn(&m_gui,nElemId,false);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_WHITE);

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
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_YELLOW);

  // Create progress bar
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){40,80,50,10},
    "Progress:",E_FONT_TXT);
  nElemId = microSDL_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(SDL_Rect){100,80,50,10},
    0,100,0,MSDL_COL_GREEN_DK,false);

  // -----------------------------------
  // PAGE: EXTRA

  // Create background box
  nElemId = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_EXTRA,(SDL_Rect){40,40,240,160});
  microSDL_ElemSetCol(&m_gui,nElemId,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Create Back button with text label
  nElemId = microSDL_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_BACK,E_PG_EXTRA,
    (SDL_Rect){135,120,50,20},"Back",E_FONT_BTN);

  // Create a few labels
  Sint16    nPosY = 50;
  Sint16    nSpaceY = 20;
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
  int               nClickX,nClickY;
  unsigned          nClickPress;
  int               nTrackElemClicked;

  char              acTxt[100];

  // -----------------------------------
  // Initialize

  microSDL_InitEnv(&m_gui);
  if (!microSDL_Init(&m_gui,m_asElem,MAX_ELEM,m_asFont,MAX_FONT,NULL,0)) { exit(1); }

  microSDL_InitTs(&m_gui,"/dev/input/touchscreen");

  // Load Fonts
  // - In this example, we are loading the same font but at
  //   different point sizes. We could also refer to other
  //   font files as well.
  bOk = microSDL_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_TXT,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_TITLE,FONT_DROID_SANS,32);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }


  // -----------------------------------
  // Create page elements
  // -----------------------------------
  InitOverlays(dirname(args[0])); // Pass executable path to find resource files


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

    // -----------------------------------
    // Perform some drawing updates depending on the active page

    unsigned nPageCur = microSDL_GetPageCur(&m_gui);

    // Immediate update of element on active page
    // - Note that these immediate updates can
    //   rely on an initial full-page draw when
    //   the page was first entered.
    if (nPageCur == E_PG_MAIN) {
      sprintf(acTxt,"%u",m_nCount);
      microSDL_ElemSetTxtStr(&m_gui,E_ELEM_TXT_COUNT,acTxt);
      microSDL_ElemDraw(&m_gui,E_ELEM_TXT_COUNT);

      microSDL_ElemXGaugeUpdate(&m_gui,E_ELEM_PROGRESS,((m_nCount/200)%100));
      microSDL_ElemDraw(&m_gui,E_ELEM_PROGRESS); 
    }

    // Call Flip() to finalize any drawing updates
    microSDL_Flip(&m_gui);

    // -----------------------------------
  
    // Poll for touchscreen presses
    if (microSDL_GetTsClick(&m_gui,&nClickX,&nClickY,&nClickPress)) {
 
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

  microSDL_Quit(&m_gui);

  return 0;
}

