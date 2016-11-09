//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 06: Example of viewport and image draw
//

#include "microsdl.h"
#include "microsdl_ex.h"

#include <math.h>
#include <libgen.h>       // For path parsing

// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"
#define IMG_LOGO        "/res/logo1-200x40.bmp"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,
      E_ELEM_DATAX,E_ELEM_DATAY,E_ELEM_DATAZ,E_ELEM_SCAN};
enum {E_FONT_BTN,E_FONT_TXT};
enum {E_VIEW};

bool                  m_bQuit = false;

// Free-running counter for display
unsigned  m_nCount = 0;

float     m_fCoordX = 0;
float     m_fCoordY = 0;
float     m_fCoordZ = 0;

// Instantiate the GUI
#define MAX_ELEM    30
#define MAX_FONT    10
#define MAX_VIEW    5
microSDL_tsGui      m_gui;
microSDL_tsElem     m_asElem[MAX_ELEM];
microSDL_tsFont     m_asFont[MAX_FONT];
microSDL_tsView     m_asView[MAX_VIEW];
microSDL_tsXGauge   m_sXGauge;


// Scanner drawing callback function
// - This is called when E_ELEM_SCAN is being rendered
// - The scanner implements a custom element that replaces
//   the Box element type with a custom rendering function.
// - It uses a viewport (E_VIEW) to allow for a local
//   coordinate system and clipping.
bool DrawScannerCb(void* pvGui,void* pvElem)
{
  int nInd;

  // Typecast the parameters to match the GUI and element types
  microSDL_tsGui*   pGui  = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*  pElem = (microSDL_tsElem*)(pvElem);
  
  // Draw the background
  microSDL_FillRect(pGui,pElem->rElem,pElem->colElemFill);
  
  // Draw the frame
  microSDL_FrameRect(pGui,pElem->rElem,pElem->colElemFrame);
  

  // Switch to drawing coordinate space within the viewport
  // - Until the next ViewSet() command, all drawing
  //   will be done with local coordinates that
  //   are remapped and clipped to match the viewport.
  microSDL_ViewSet(&m_gui,E_VIEW);

  // Perform the drawing of example graphic primitives
  microSDL_Line(&m_gui,0,-200,0,+200,MSDL_COL_GRAY_DK);
  microSDL_Line(&m_gui,-200,0,+200,0,MSDL_COL_GRAY_DK);

  microSDL_FrameRect(&m_gui,(SDL_Rect){-30,-20,60,40},MSDL_COL_BLUE_DK);
  for (nInd=-5;nInd<=5;nInd++) {
    microSDL_Line(&m_gui,0,0,0+nInd*20,100,MSDL_COL_PURPLE);
  }

  microSDL_FillRect(&m_gui,(SDL_Rect){1,1,10,10},MSDL_COL_RED_DK);
  microSDL_FillRect(&m_gui,(SDL_Rect){1,-10,10,10},MSDL_COL_GREEN_DK);
  microSDL_FillRect(&m_gui,(SDL_Rect){-10,1,10,10},MSDL_COL_BLUE_DK);
  microSDL_FillRect(&m_gui,(SDL_Rect){-10,-10,10,10},MSDL_COL_YELLOW);

  // Restore the drawing coordinate space to the screen
  microSDL_ViewSet(&m_gui,MSDL_VIEW_ID_SCREEN);
  
  // Clear the redraw flag
  microSDL_ElemSetRedraw(pElem,false);
  
  return true;
}

// Button callbacks
bool CbBtnQuit(void* pvGui,void *pvElem,microSDL_teTouch eTouch,int nX,int nY)
{
  if (eTouch == MSDL_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

// Create the default elements on each page
// - strPath: Path to executable passed in to locate resource files
bool InitOverlays(char *strPath)
{
  microSDL_tsElem*  pElem = NULL;
  int               nViewId;

  // Background flat color
  microSDL_SetBkgndColor(&m_gui,MSDL_COL_BLACK);

  // Graphic logo
  // - Extra code to demonstrate path generation based on location of executable  
  char* strImgLogoPath = (char*)malloc(strlen(strPath)+strlen(IMG_LOGO)+1);
  strcpy(strImgLogoPath, strPath);
  strcat(strImgLogoPath, IMG_LOGO);  
  pElem = microSDL_ElemCreateImg(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){160-100,5,200,40},
    strImgLogoPath);

  // Create background box
  pElem = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetCol(pElem,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Create Quit button with text label
  pElem = microSDL_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (SDL_Rect){40,210,50,20},"QUIT",E_FONT_BTN,&CbBtnQuit);

  // Create counter
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,60,50,10},
    "Searches:",E_FONT_TXT);
  pElem = microSDL_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(SDL_Rect){80,60,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_GRAY_LT);

  // Create progress bar
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,80,50,10},
    "Progress:",E_FONT_TXT);
  pElem = microSDL_ElemXGaugeCreate(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,&m_sXGauge,(SDL_Rect){80,80,50,10},
    0,100,0,MSDL_COL_GREEN_DK,false);

  
  // Create other labels
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){40,100,50,10},
    "Coord X:",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_WHITE);
  pElem = microSDL_ElemCreateTxt(&m_gui,E_ELEM_DATAX,E_PG_MAIN,(SDL_Rect){100,100,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_GRAY_LT);

  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){40,120,50,10},
    "Coord Y:",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_WHITE);
  pElem = microSDL_ElemCreateTxt(&m_gui,E_ELEM_DATAY,E_PG_MAIN,(SDL_Rect){100,120,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_GRAY_LT);

  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){40,140,50,10},
    "Coord Z:",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_WHITE);
  pElem = microSDL_ElemCreateTxt(&m_gui,E_ELEM_DATAZ,E_PG_MAIN,(SDL_Rect){100,140,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_GRAY_LT);

  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){120,210,170,20},
    "Example of microSDL GUI C library",E_FONT_BTN);
  microSDL_ElemSetTxtAlign(pElem,MSDL_ALIGN_MID_LEFT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_RED_LT);

  // --------------------------------------------------------------------------
  // Create scanner with viewport
  pElem = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){190-1-2,75-1-12,100+2+4,100+2+10+4});
  microSDL_ElemSetCol(pElem,MSDL_COL_BLUE_LT,MSDL_COL_BLACK,MSDL_COL_BLACK);
  
  pElem = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){190,75-11,100,10},
    "SCANNER",E_FONT_TXT);
  microSDL_ElemSetTxtCol(pElem,MSDL_COL_BLUE_DK);
  microSDL_ElemSetTxtAlign(pElem,MSDL_ALIGN_MID_MID);
  
  pElem = microSDL_ElemCreateBox(&m_gui,E_ELEM_SCAN,E_PG_MAIN,(SDL_Rect){190-1,75-1,100+2,100+2});
  microSDL_ElemSetCol(pElem,MSDL_COL_BLUE_LT,MSDL_COL_BLACK,MSDL_COL_BLACK);
  // Set the callback function to handle all drawing for the element
  microSDL_ElemSetDrawFunc(pElem,&DrawScannerCb);
  
  nViewId = microSDL_ViewCreate(&m_gui,E_VIEW,(SDL_Rect){190,75,100,100},50,50);
  // --------------------------------------------------------------------------

  
  return true;
}

// This is an immediate-draw routine that updates
// the viewport element. The viewport provides a
// local coordinate system and clipping.
void DrawViewport()
{
  int nInd;
  // Temp redraw view
  // Draw the viewport background
  microSDL_ElemDraw(&m_gui,E_ELEM_SCAN);

  // Switch to drawing coordinate space within the viewport
  // - Until the next ViewSet() command, all drawing
  //   will be done with local coordinates that
  //   are remapped and clipped to match the viewport.
  microSDL_ViewSet(&m_gui,E_VIEW);

  // Perform the drawing of example graphic primitives
  microSDL_Line(&m_gui,0,-200,0,+200,MSDL_COL_GRAY_DK);
  microSDL_Line(&m_gui,-200,0,+200,0,MSDL_COL_GRAY_DK);

  microSDL_FrameRect(&m_gui,(SDL_Rect){-30,-20,60,40},MSDL_COL_BLUE_DK);
  for (nInd=-5;nInd<=5;nInd++) {
    microSDL_Line(&m_gui,0,0,0+nInd*20,100,MSDL_COL_PURPLE);
  }

  microSDL_FillRect(&m_gui,(SDL_Rect){1,1,10,10},MSDL_COL_RED_DK);
  microSDL_FillRect(&m_gui,(SDL_Rect){1,-10,10,10},MSDL_COL_GREEN_DK);
  microSDL_FillRect(&m_gui,(SDL_Rect){-10,1,10,10},MSDL_COL_BLUE_DK);
  microSDL_FillRect(&m_gui,(SDL_Rect){-10,-10,10,10},MSDL_COL_YELLOW);

  // Restore the drawing coordinate space to the screen
  microSDL_ViewSet(&m_gui,MSDL_VIEW_ID_SCREEN);

}


int main( int argc, char* args[] )
{
  bool              bOk = true;
  char              acTxt[100];

  // -----------------------------------
  // Initialize

  microSDL_InitEnv(&m_gui);
  if (!microSDL_Init(&m_gui,m_asElem,MAX_ELEM,m_asFont,MAX_FONT,m_asView,MAX_VIEW)) { exit(1); }

  microSDL_InitTs(&m_gui,"/dev/input/touchscreen");

  // Load Fonts
  bOk = microSDL_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_TXT,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); exit(1); }


  // -----------------------------------
  // Create the graphic elements
  InitOverlays(dirname(args[0])); // Pass executable path to find resource files

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);

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
    Sint16 nOriginX = (Sint16)m_fCoordX;
    Sint16 nOriginY = (Sint16)m_fCoordY;
    microSDL_ViewSetOrigin(&m_gui,E_VIEW,nOriginX,nOriginY);
    // Manually mark the scanner view as needing redraw
    // since it depends on E_VIEW
    microSDL_ElemSetRedraw(microSDL_ElemGet(&m_gui,E_ELEM_SCAN),true);

    // -----------------------------------------------

    // Perform any immediate updates on active page
    sprintf(acTxt,"%u",m_nCount);
    microSDL_ElemSetTxtStr(microSDL_ElemGet(&m_gui,E_ELEM_TXT_COUNT),acTxt);

    sprintf(acTxt,"%4.2f",m_fCoordX-50);
    microSDL_ElemSetTxtStr(microSDL_ElemGet(&m_gui,E_ELEM_DATAX),acTxt);
    sprintf(acTxt,"%4.2f",m_fCoordY-50);
    microSDL_ElemSetTxtStr(microSDL_ElemGet(&m_gui,E_ELEM_DATAY),acTxt);
    sprintf(acTxt,"%4.2f",m_fCoordZ);
    microSDL_ElemSetTxtStr(microSDL_ElemGet(&m_gui,E_ELEM_DATAZ),acTxt);
    microSDL_ElemSetTxtCol(microSDL_ElemGet(&m_gui,E_ELEM_DATAZ),
            (m_fCoordY>50)?MSDL_COL_GREEN_LT:MSDL_COL_RED_DK);

    microSDL_ElemXGaugeUpdate(microSDL_ElemGet(&m_gui,E_ELEM_PROGRESS),50+50*sin(m_nCount/500.0));

    // -----------------------------------------------

    // Periodically call microSDL update function    
    microSDL_Update(&m_gui);

  } // bQuit


  // -----------------------------------
  // Close down display

  microSDL_Quit(&m_gui);

  return 0;
}

