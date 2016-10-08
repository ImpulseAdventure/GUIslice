//
// microSDL GUI Library Examples
// - Calvin Hass
// - http://www.impulseadventure.com/elec/microsdl-sdl-gui.html
// - Example 06: Example of viewport and image draw
//

#include "microsdl.h"

#include "math.h"

// Defines for resources
#define FONT_DROID_SANS "/usr/share/fonts/truetype/droid/DroidSans.ttf"
#define IMG_LOGO        "./res/logo1-200x40.bmp"

// Enumerations for pages, elements, fonts, images
enum {E_PG_MAIN};
enum {E_ELEM_BTN_QUIT,E_ELEM_TXT_COUNT,E_ELEM_PROGRESS,
      E_ELEM_DATAX,E_ELEM_DATAY,E_ELEM_DATAZ,E_ELEM_VIEW};
enum {E_FONT_BTN,E_FONT_TXT};
enum {E_VIEW};

// Free-running counter for display
unsigned  m_nCount = 0;

float     m_fCoordX = 0;
float     m_fCoordY = 0;
float     m_fCoordZ = 0;

// Instantiate the GUI
#define MAX_ELEM  30
#define MAX_FONT  10
#define MAX_VIEW  5
microSDL_tsGui  m_gui;
microSDL_tsElem m_asElem[MAX_ELEM];
microSDL_tsFont m_asFont[MAX_FONT];
microSDL_tsView m_asView[MAX_VIEW];


// Create page elements
bool InitOverlays()
{
  int   nElemId;

  // Background flat color
  microSDL_SetBkgndColor(&m_gui,MSDL_COL_BLACK);

  // Graphic logo
  nElemId = microSDL_ElemCreateImg(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){160-100,5,200,40},
    IMG_LOGO);

  // Create background box
  nElemId = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){10,50,300,150});
  microSDL_ElemSetCol(&m_gui,nElemId,MSDL_COL_WHITE,MSDL_COL_BLACK,MSDL_COL_BLACK);

  // Create Quit button with text label
  nElemId = microSDL_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (SDL_Rect){40,210,50,20},"QUIT",E_FONT_BTN);

  // Create counter
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,60,50,10},
    "Searches:",E_FONT_TXT);
  nElemId = microSDL_ElemCreateTxt(&m_gui,E_ELEM_TXT_COUNT,E_PG_MAIN,(SDL_Rect){80,60,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_GRAY_LT);

  // Create progress bar
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){20,80,50,10},
    "Progress:",E_FONT_TXT);
  nElemId = microSDL_ElemCreateGauge(&m_gui,E_ELEM_PROGRESS,E_PG_MAIN,(SDL_Rect){80,80,50,10},
    0,100,0,MSDL_COL_GREEN_DK,false);

  // Create other labels
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){40,100,50,10},
    "Coord X:",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_WHITE);
  nElemId = microSDL_ElemCreateTxt(&m_gui,E_ELEM_DATAX,E_PG_MAIN,(SDL_Rect){100,100,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_GRAY_LT);

  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){40,120,50,10},
    "Coord Y:",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_WHITE);
  nElemId = microSDL_ElemCreateTxt(&m_gui,E_ELEM_DATAY,E_PG_MAIN,(SDL_Rect){100,120,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_GRAY_LT);

  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){40,140,50,10},
    "Coord Z:",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_WHITE);
  nElemId = microSDL_ElemCreateTxt(&m_gui,E_ELEM_DATAZ,E_PG_MAIN,(SDL_Rect){100,140,50,10},
    "",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_GRAY_LT);

  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){120,210,170,20},
    "Example of microSDL GUI C library",E_FONT_BTN);
  microSDL_ElemSetTxtAlign(&m_gui,nElemId,MSDL_ALIGN_MID_LEFT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_RED_LT);

  // Create viewport
  nElemId = microSDL_ElemCreateBox(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){190-1-2,75-1-12,100+2+4,100+2+10+4});
  microSDL_ElemSetCol(&m_gui,nElemId,MSDL_COL_BLUE_LT,MSDL_COL_BLACK,MSDL_COL_BLACK);
  nElemId = microSDL_ElemCreateTxt(&m_gui,MSDL_ID_AUTO,E_PG_MAIN,(SDL_Rect){190,75-11,100,10},
    "SCANNER",E_FONT_TXT);
  microSDL_ElemSetTxtCol(&m_gui,nElemId,MSDL_COL_BLUE_DK);
  microSDL_ElemSetTxtAlign(&m_gui,nElemId,MSDL_ALIGN_MID_MID);
  nElemId = microSDL_ElemCreateBox(&m_gui,E_ELEM_VIEW,E_PG_MAIN,(SDL_Rect){190-1,75-1,100+2,100+2});
  microSDL_ElemSetCol(&m_gui,nElemId,MSDL_COL_BLUE_LT,MSDL_COL_BLACK,MSDL_COL_BLACK);
  nElemId = microSDL_ViewCreate(&m_gui,E_VIEW,(SDL_Rect){190,75,100,100},50,50);

  return true;
}

// This is an immediate-draw routine that updates
// the viewport element. The viewport provides a
// local coordinate system and clipping.
void DrawViewport()
{
  // Temp redraw view
  // Draw the viewport background
  microSDL_ElemDraw(&m_gui,E_ELEM_VIEW);

  // Switch to drawing within the viewport
  // - Until the next ViewSet() command, all drawing
  //   will be done with local coordinates that
  //   are remapped and clipped to match the viewport.
  microSDL_ViewSet(&m_gui,E_VIEW);

  // Perform the drawing of example graphic primitives
  microSDL_Line(&m_gui,0,-200,0,+200,MSDL_COL_GRAY_DK);
  microSDL_Line(&m_gui,-200,0,+200,0,MSDL_COL_GRAY_DK);

  microSDL_FrameRect(&m_gui,(SDL_Rect){-30,-20,60,40},MSDL_COL_BLUE_DK);
  for (int nInd=-5;nInd<=5;nInd++) {
    microSDL_Line(&m_gui,0,0,0+nInd*20,100,MSDL_COL_PURPLE);
  }

  microSDL_FillRect(&m_gui,(SDL_Rect){1,1,10,10},MSDL_COL_RED_DK);
  microSDL_FillRect(&m_gui,(SDL_Rect){1,-10,10,10},MSDL_COL_GREEN_DK);
  microSDL_FillRect(&m_gui,(SDL_Rect){-10,1,10,10},MSDL_COL_BLUE_DK);
  microSDL_FillRect(&m_gui,(SDL_Rect){-10,-10,10,10},MSDL_COL_YELLOW);

  // Restore the drawing to the screen
  microSDL_ViewSet(&m_gui,MSDL_VIEW_ID_SCREEN);

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
  microSDL_Init(&m_gui,m_asElem,MAX_ELEM,m_asFont,MAX_FONT,m_asView,MAX_VIEW);

  microSDL_InitTs(&m_gui,"/dev/input/touchscreen");

  // Load Fonts
  bOk = microSDL_FontAdd(&m_gui,E_FONT_BTN,FONT_DROID_SANS,12);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); return false; }
  bOk = microSDL_FontAdd(&m_gui,E_FONT_TXT,FONT_DROID_SANS,10);
  if (!bOk) { fprintf(stderr,"ERROR: FontAdd failed\n"); return false; }


  // -----------------------------------
  // Create the graphic elements
  InitOverlays();

  // Start up display on main page
  microSDL_SetPageCur(&m_gui,E_PG_MAIN);
  microSDL_ElemDrawPageCur(&m_gui);

  // Initial draw of viewport
  DrawViewport();
  // Any time that we have performed manual draw
  // commands, we need to finalize with a Flip()
  microSDL_Flip(&m_gui);


  // -----------------------------------
  // Main event loop

  bQuit = false;
  while (!bQuit) {

    // Update the data display values
    m_nCount++;
    m_fCoordX = 50+25.0*(sin(m_nCount/250.0));
    m_fCoordY = 50+15.0*(cos(m_nCount/175.0));
    m_fCoordZ = 13.02;

    // Adjust the viewport origin for fun
    Sint16 nOriginX = (Sint16)m_fCoordX;
    Sint16 nOriginY = (Sint16)m_fCoordY;
    microSDL_ViewSetOrigin(&m_gui,E_VIEW,nOriginX,nOriginY);

    // -----------------------------------------------

    // Perform any immediate updates on active page
    sprintf(acTxt,"%u",m_nCount);
    microSDL_ElemSetTxtStr(&m_gui,E_ELEM_TXT_COUNT,acTxt);
    microSDL_ElemDraw(&m_gui,E_ELEM_TXT_COUNT);

    sprintf(acTxt,"%4.2f",m_fCoordX-50);
    microSDL_ElemSetTxtStr(&m_gui,E_ELEM_DATAX,acTxt);
    microSDL_ElemDraw(&m_gui,E_ELEM_DATAX);
    sprintf(acTxt,"%4.2f",m_fCoordY-50);
    microSDL_ElemSetTxtStr(&m_gui,E_ELEM_DATAY,acTxt);
    microSDL_ElemDraw(&m_gui,E_ELEM_DATAY);
    sprintf(acTxt,"%4.2f",m_fCoordZ);
    microSDL_ElemSetTxtStr(&m_gui,E_ELEM_DATAZ,acTxt);
    microSDL_ElemSetTxtCol(&m_gui,E_ELEM_DATAZ,(m_fCoordY>50)?MSDL_COL_GREEN_LT:MSDL_COL_RED_DK);
    microSDL_ElemDraw(&m_gui,E_ELEM_DATAZ);

    microSDL_ElemUpdateGauge(&m_gui,E_ELEM_PROGRESS,50+50*sin(m_nCount/500.0));
    microSDL_ElemDraw(&m_gui,E_ELEM_PROGRESS); 

    // -----------------------------------------------


    // Perform immediate graphics updates to viewport
    DrawViewport();

    // Finalize the immediate mode drawing
    microSDL_Flip(&m_gui);
  
    // -----------------------------------------------

    // Poll for touchscreen presses
    if (microSDL_GetTsClick(&m_gui,nClickX,nClickY,nClickPress)) {
 
      // Track the touch event and find any associated object
      microSDL_TrackClick(&m_gui,nClickX,nClickY,nClickPress);
      nTrackElemClicked = microSDL_GetTrackElemClicked(&m_gui);

      // Any selectable object clicked? (MSDL_ID_NONE if no)
      if (nTrackElemClicked == E_ELEM_BTN_QUIT) {
        // Quit button pressed
        bQuit = true;
      }

      // Clear click event
      microSDL_ClearTrackElemClicked(&m_gui);
  
    } // Touchscreen press

  } // bQuit


  // -----------------------------------
  // Close down display

  microSDL_Quit(&m_gui);
}

