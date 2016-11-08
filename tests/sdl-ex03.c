//
// MicroSDL GUI Library Examples
// - Calvin Hass ( http://www.impulseadventure.com/elec/ )
// - Example 02: Accept touch input
//

#include "microsdl.h"

#include <libgen.h>       // For path parsing


// Defines for resources
#define IMG_BTN_QUIT      "/res/btn-exit32x32.bmp"
#define IMG_BTN_QUIT_SEL  "/res/btn-exit_sel32x32.bmp"
char*   strImgQuit;
char*   strImgQuitSel;

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT};

bool                  m_bQuit = false;

// Instantiate the GUI
#define MAX_ELEM      30
microSDL_tsGui        m_gui;
microSDL_tsElem       m_asElem[MAX_ELEM];


// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,microSDL_teTouch eTouch,int nX,int nY)
{
  if (eTouch == MSDL_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// - strPath: Path to executable passed in to locate resource files
bool InitOverlays(char *strPath)
{
  int   nElemId;
  
  // Background flat color
  microSDL_SetBkgndColor(&m_gui,MSDL_COL_GRAY_DK);

  // Create background box
  nElemId = microSDL_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetCol(&m_gui,nElemId,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Create Quit button with image label
  // - Extra code to demonstrate path generation based on location of executable
  char* strImgQuit    = (char*)malloc(strlen(strPath)+strlen(IMG_BTN_QUIT)+1);
  char* strImgQuitSel = (char*)malloc(strlen(strPath)+strlen(IMG_BTN_QUIT_SEL)+1);  
  strcpy(strImgQuit, strPath);
  strcat(strImgQuit, IMG_BTN_QUIT);  
  strcpy(strImgQuitSel, strPath);
  strcat(strImgQuitSel, IMG_BTN_QUIT_SEL);     
  nElemId = microSDL_ElemCreateBtnImg(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
          (SDL_Rect){258,70,32,32},strImgQuit,strImgQuitSel,&CbBtnQuit);
  free(strImgQuit);
  free(strImgQuitSel);

  return true;
}

int main( int argc, char* args[] )
{

  // -----------------------------------
  // Initialize

  microSDL_InitEnv(&m_gui);
  if (!microSDL_Init(&m_gui,m_asElem,MAX_ELEM,NULL,0,NULL,0)) { exit(1); }

  microSDL_InitTs(&m_gui,"/dev/input/touchscreen");


  // -----------------------------------
  // Create the graphic elements
  InitOverlays(dirname(args[0])); // Pass executable path to find resource files

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

