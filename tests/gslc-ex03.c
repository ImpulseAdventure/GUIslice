//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 03: Accept touch input, graphic button
//

#include "GUIslice.h"
#include "GUIslice_drv.h"

#include <libgen.h>       // For path parsing

// Define default device paths for framebuffer & touchscreen
#ifdef DRV_TYPE_SDL1
  #define GSLC_DEV_FB     "/dev/fb1"
#elif DRV_TYPE_SDL2
  #define GSLC_DEV_FB     "/dev/fb0"
#endif
#define GSLC_DEV_TOUCH  "/dev/input/touchscreen"

// Defines for resources
#define IMG_BTN_QUIT      "/res/btn-exit32x32.bmp"
#define IMG_BTN_QUIT_SEL  "/res/btn-exit_sel32x32.bmp"
char*   strImgQuit;
char*   strImgQuitSel;

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BOX,E_ELEM_BTN_QUIT};

bool        m_bQuit = false;

// Instantiate the GUI
gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;

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

// - strPath: Path to executable passed in to locate resource files
bool InitOverlays(char *strPath)
{
  gslc_tsElem*  pElem = NULL;
  
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);  
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_Rect){10,50,300,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with image label
  // - Extra code to demonstrate path generation based on location of executable
  char* strImgQuit    = (char*)malloc(strlen(strPath)+strlen(IMG_BTN_QUIT)+1);
  char* strImgQuitSel = (char*)malloc(strlen(strPath)+strlen(IMG_BTN_QUIT_SEL)+1);  
  strcpy(strImgQuit, strPath);
  strcat(strImgQuit, IMG_BTN_QUIT);  
  strcpy(strImgQuitSel, strPath);
  strcat(strImgQuitSel, IMG_BTN_QUIT_SEL);     
  pElem = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
          (gslc_Rect){258,70,32,32},strImgQuit,strImgQuitSel,&CbBtnQuit);
  free(strImgQuit);
  free(strImgQuitSel);

  return true;
}

int main( int argc, char* args[] )
{

  // -----------------------------------
  // Initialize

  gslc_InitEnv(GSLC_DEV_FB,GSLC_DEV_TOUCH);
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,NULL,0)) { exit(1); }  

  gslc_InitTs(&m_gui,GSLC_DEV_TOUCH);
  
  // -----------------------------------
  // Create the graphic elements
  InitOverlays(dirname(args[0])); // Pass executable path to find resource files

  // -----------------------------------
  // Start display

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {
    
    // Periodically call GUIslice update function
    gslc_Update(&m_gui);
    
  } // bQuit


  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}

