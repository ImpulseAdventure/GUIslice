#include <SDL2/SDL.h>


//
// GUIslice sample SDL test for SDL2.0
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
//
// This program is a simple test to ensure that the SDL
// library is installed correctly. If everything is
// working then you should see the external screen
// (such as PiTFT) display a red-to-blue gradient and
// a green box.
//


// Main entrypoint
SDL_Window*     pWind = NULL;
SDL_Renderer*   pRender = NULL;

int main(int argc,char* args[]) {

  // --------------------------------------
  // Initialization
  // --------------------------------------

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr,"ERROR: SDL_Init(): %s\n",SDL_GetError());
    return 0;
  }

  // Now that we have successfully initialized SDL
  // we need to register an exit handler so that SDL_Quit()
  // gets called at program termination. If we don't do this
  // then some types of errors/aborts will result in
  // the SDL environment being left in a bad state that
  // affects the next SDL program execution.
  atexit(SDL_Quit);

  // Default to using OpenGL rendering engine and full-screen
  // When using SDL_WINDOW_FULLSCREEN, the width & height dimensions are ignored
  pWind = SDL_CreateWindow("GUIslice",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
          0,0,SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
  if (!pWind) {
    fprintf(stderr,"ERROR: SDL_CreateWindow(): %s\n",SDL_GetError());
    return 0;
  }

  // Save a copy of the display dimensions from the created fullscreen window
  int nResX,nResY;
  SDL_GetWindowSize(pWind,&nResX,&nResY);

  // Create renderer
  pRender = SDL_CreateRenderer(pWind,-1,SDL_RENDERER_ACCELERATED);
  if (!pRender) {
    fprintf(stderr,"ERROR: SDL_CreateRenderer(): %s\n",SDL_GetError());
    return 0;
  }

  // --------------------------------------
  // Perform some simple drawing primitives
  // --------------------------------------

  // Clear the drawing before any new drawing occurs
  SDL_SetRenderDrawColor(pRender,0,0,0,255);
  SDL_RenderClear(pRender);

  // Draw a gradient from red to blue
  SDL_Rect  rectTmp;
  Uint16    nPosX;
  for (nPosX = 0; nPosX < nResX; nPosX++) {
    rectTmp = (SDL_Rect){nPosX,nResY/2,1,nResY/2};
    SDL_SetRenderDrawColor(pRender,nPosX % 256, 0, 255 - (nPosX % 256), 255);
    SDL_RenderFillRect(pRender,&rectTmp);
  }

  // Draw a green box
  SDL_Rect rectBox = {0,0,(Uint16)(nResX),(Uint16)(nResY/2)};
  SDL_SetRenderDrawColor(pRender,0,255,0,255);
  SDL_RenderFillRect(pRender,&rectBox);

  // Now that we've completed drawing, update the main display
  SDL_RenderPresent(pRender);

  // Wait for a short delay
  SDL_Delay(3000);

  // Close down SDL
  SDL_Quit();

  return 1;
}
