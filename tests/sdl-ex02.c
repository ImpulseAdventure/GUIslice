//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 02: Accept touch input
//

#include "microsdl.h"


// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT};
enum {E_FONT_BTN};

bool                  m_bQuit = false;

// Instantiate the GUI
#define MAX_ELEM      30
#define MAX_FONT      10
microSDL_tsGui        m_gui;
microSDL_tsElem       m_asElem[MAX_ELEM];
microSDL_tsFont       m_asFont[MAX_FONT];

// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,microSDL_teTouch eTouch,int nX,int nY)
{
  if (eTouch == MSDL_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

int main( int argc, char* args[] )
{
  bool              bOk = true;
  microSDL_tsElem*  pElem = NULL;

  // -----------------------------------
  // Initialize

  microSDL_InitEnv(&m_gui);
  if (!microSDL_Init(&m_gui,m_asElem,MAX_ELEM,m_asFont,MAX_FONT,NULL,0)) { exit(1); }

  microSDL_InitTs(&m_gui,"/dev/input/touchscreen");

  // Load Fonts
  bOk = microSDL_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { printf("ERROR: microSDL_FontAdd() failed\n"); exit(1); }


  // -----------------------------------
  // Create page elements

  // Background flat color
  microSDL_SetBkgndColor(&m_gui,MSDL_COL_GRAY_DK);

  // Create background box
  pElem = microSDL_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetCol(pElem,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Create Quit button with text label
  pElem = microSDL_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (SDL_Rect){120,100,80,40},"Quit",E_FONT_BTN,&CbBtnQuit);

  // -----------------------------------
  // Start display

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);

  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {
  
    // Periodically call microSDL update function    
    microSDL_Update(&m_gui);   

  } // bQuit


  // -----------------------------------
  // Close down display

  microSDL_Quit(&m_gui);

  return 0;
}

