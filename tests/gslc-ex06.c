//
// GUIslice Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 06: Example of viewport and image draw
//

#include "GUIslice.h"
#include "GUIslice_ex.h"
#include "GUIslice_drv.h"

#include <math.h>
#include <libgen.h>       // For path parsing

// Define default device paths for framebuffer & touchscreen
#ifdef DRV_TYPE_SDL1
  #define GSLC_DEV_FB     "/dev/fb1"
#elif DRV_TYPE_SDL2
  #define GSLC_DEV_FB     "/dev/fb0"
#endif
#define GSLC_DEV_TOUCH  "/dev/input/touchscreen"

// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"
#define IMG_LOGO        "/res/logo1-200x40.bmp"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,
      E_ELEM_DATAX,E_ELEM_DATAY,E_ELEM_DATAZ,E_ELEM_SCAN};
enum {E_FONT_BTN,E_FONT_TXT};
enum {E_VIEW};

bool      m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

float     m_fCoordX = 0;
float     m_fCoordY = 0;
float     m_fCoordZ = 0;

// Instantiate the GUI
#define MAX_FONT            10
#define MAX_VIEW            5
gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsView                 m_asView[MAX_VIEW];
gslc_tsXGauge               m_sXGauge;

#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    30
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];

#define MAX_STR             100


// Scanner drawing callback function
// - This is called when E_ELEM_SCAN is being rendered
// - The scanner implements a custom element that replaces
//   the Box element type with a custom rendering function.
// - It uses a viewport (E_VIEW) to allow for a local
//   coordinate system and clipping.
bool CbDrawScanner(void* pvGui,void* pvElem)
{
  int nInd;

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*   pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElem*  pElem = (gslc_tsElem*)(pvElem);
  
  // Draw the background
  gslc_DrawFillRect(pGui,pElem->rElem,pElem->colElemFill);
  
  // Draw the frame
  gslc_DrawFrameRect(pGui,pElem->rElem,pElem->colElemFrame);
  

  // Switch to drawing coordinate space within the viewport
  // - Until the next ViewSet() command, all drawing
  //   will be done with local coordinates that
  //   are remapped and clipped to match the viewport.
  gslc_ViewSet(&m_gui,E_VIEW);

  // Perform the drawing of example graphic primitives
  gslc_DrawLine(&m_gui,0,-200,0,+200,GSLC_COL_GRAY_DK2);
  gslc_DrawLine(&m_gui,-200,0,+200,0,GSLC_COL_GRAY_DK2);

  gslc_DrawFrameRect(&m_gui,(gslc_Rect){-30,-20,60,40},GSLC_COL_BLUE_DK2);
  for (nInd=-5;nInd<=5;nInd++) {
    gslc_DrawLine(&m_gui,0,0,0+nInd*20,100,GSLC_COL_PURPLE);
  }

  gslc_DrawFillRect(&m_gui,(gslc_Rect){1,1,10,10},GSLC_COL_RED_DK2);
  gslc_DrawFillRect(&m_gui,(gslc_Rect){1,-10,10,10},GSLC_COL_GREEN_DK2);
  gslc_DrawFillRect(&m_gui,(gslc_Rect){-10,1,10,10},GSLC_COL_BLUE_DK2);
  gslc_DrawFillRect(&m_gui,(gslc_Rect){-10,-10,10,10},GSLC_COL_YELLOW);

  // Restore the drawing coordinate space to the screen
  gslc_ViewSet(&m_gui,GSLC_VIEW_ID_SCREEN);
  
  // Clear the redraw flag
  gslc_ElemSetRedraw(pElem,false);
  
  return true;
}

// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int nX,int nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Create the default elements on each page
// - strPath: Path to executable passed in to locate resource files
bool InitOverlays(char *strPath)
{
  gslc_tsElem*  pElem = NULL;

  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // Graphic logo
  // - Extra code to demonstrate path generation based on location of executable  
  char* strImgLogoPath = (char*)malloc(strlen(strPath)+strlen(IMG_LOGO)+1);
  strcpy(strImgLogoPath, strPath);
  strcat(strImgLogoPath, IMG_LOGO);  
  pElem = gslc_ElemCreateImg(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){160-100,5,200,40},
    strImgLogoPath);

  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){10,50,300,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with text label
  pElem = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_Rect){40,210,50,20},"QUIT",E_FONT_BTN,&CbBtnQuit); 

  // Create counter
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){20,60,50,10},
    "Searches:",E_FONT_TXT);
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(gslc_Rect){80,60,50,10},
    "",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT2);

  // Create progress bar
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){20,80,50,10},
    "Progress:",E_FONT_TXT);
  pElem = gslc_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(gslc_Rect){80,80,50,10},
    0,100,0,GSLC_COL_GREEN,false);

  
  // Create other labels
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){40,100,50,10},
    "Coord X:",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAX,E_PG_MAIN,(gslc_Rect){100,100,50,10},
    "",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT2);

  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){40,120,50,10},
    "Coord Y:",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAY,E_PG_MAIN,(gslc_Rect){100,120,50,10},
    "",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT2);

  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){40,140,50,10},
    "Coord Z:",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_WHITE);
  pElem = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATAZ,E_PG_MAIN,(gslc_Rect){100,140,50,10},
    "",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_GRAY_LT2);

  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){120,210,170,20},
    "Example of GUIslice C library",E_FONT_BTN);
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_LEFT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_RED_LT2);

  // --------------------------------------------------------------------------
  // Create scanner with viewport
  pElem = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){190-1-2,75-1-12,100+2+4,100+2+10+4});
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  pElem = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_Rect){190,75-11,100,10},
    "SCANNER",E_FONT_TXT);
  gslc_ElemSetTxtCol(pElem,GSLC_COL_BLUE_DK2);
  gslc_ElemSetTxtAlign(pElem,GSLC_ALIGN_MID_MID);
  
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_SCAN,E_PG_MAIN,(gslc_Rect){190-1,75-1,100+2,100+2});
  gslc_ElemSetCol(pElem,GSLC_COL_BLUE_LT2,GSLC_COL_BLACK,GSLC_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  gslc_ElemSetDrawFunc(pElem,&CbDrawScanner);
  
  gslc_ViewCreate(&m_gui,E_VIEW,(gslc_Rect){190,75,100,100},50,50);
  // --------------------------------------------------------------------------

  return true;
}

int main( int argc, char* args[] )
{
  bool              bOk = true;
  char              acTxt[100];

  // -----------------------------------
  // Initialize

  gslc_InitEnv(GSLC_DEV_FB,GSLC_DEV_TOUCH);
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT,m_asView,MAX_VIEW)) { exit(1); }

  gslc_InitTs(&m_gui,GSLC_DEV_TOUCH);

  // Load Fonts
  bOk = gslc_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = gslc_FontAdd(&m_gui,E_FONT_TXT,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }


  // -----------------------------------
  // Create the graphic elements
  InitOverlays(dirname(args[0])); // Pass executable path to find resource files

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // Save some element references for quick access
  gslc_tsElem*  pElemScan       = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_SCAN);
  gslc_tsElem*  pElemCount      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
  gslc_tsElem*  pElemDataX      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAX);
  gslc_tsElem*  pElemDataY      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAY);
  gslc_tsElem*  pElemDataZ      = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATAZ);
  gslc_tsElem*  pElemProgress   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_PROGRESS);
  
  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {
    
    // Update the data display values
    m_nCount++;
    m_fCoordX = 50+25.0*(sin(m_nCount/250.0));
    m_fCoordY = 50+15.0*(cos(m_nCount/175.0));
    m_fCoordZ = 13.02;

    // Adjust the scanner's viewport origin for fun
    int16_t nOriginX = (int16_t)m_fCoordX;
    int16_t nOriginY = (int16_t)m_fCoordY;
    gslc_ViewSetOrigin(&m_gui,E_VIEW,nOriginX,nOriginY);
    // Manually mark the scanner view as needing redraw
    // since it depends on E_VIEW
    gslc_ElemSetRedraw(pElemScan,true);

    // -----------------------------------------------

    // Perform any immediate updates on active page
    snprintf(acTxt,MAX_STR,"%u",m_nCount);
    gslc_ElemSetTxtStr(pElemCount,acTxt);

    snprintf(acTxt,MAX_STR,"%4.2f",m_fCoordX-50);
    gslc_ElemSetTxtStr(pElemDataX,acTxt);
    snprintf(acTxt,MAX_STR,"%4.2f",m_fCoordY-50);
    gslc_ElemSetTxtStr(pElemDataY,acTxt);
    snprintf(acTxt,MAX_STR,"%4.2f",m_fCoordZ);
    gslc_ElemSetTxtStr(pElemDataZ,acTxt);
    gslc_ElemSetTxtCol(pElemDataZ,(m_fCoordY>50)?GSLC_COL_GREEN_LT2:GSLC_COL_RED_DK2);

    gslc_ElemXGaugeUpdate(pElemProgress,50+50*sin(m_nCount/500.0));

    // -----------------------------------------------

    // Periodically call GUIslice update function    
    gslc_Update(&m_gui);

  } // bQuit


  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}

