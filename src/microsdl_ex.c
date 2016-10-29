// =======================================================================
// microSDL library (extensions)
// - Calvin Hass
// - http:/www.impulseadventure.com/elec/microsdl-sdl-gui.html
//
// - Version 0.2.3    (2016/10/29)
// =======================================================================


// MicroSDL library
#include "microsdl.h"
#include "microsdl_ex.h"

#include <stdio.h>

#include "SDL/SDL.h"
#include "SDL/SDL_getenv.h"
#include "SDL/SDL_ttf.h"

// Extended element definitions
// - This file extends the core microSDL functionality with
//   additional widget types


int microSDL_ElemXGaugeCreate(microSDL_tsGui* pGui,int nElemId,int nPage,
  microSDL_tsXGauge* pXData,SDL_Rect rElem,
  int nMin,int nMax,int nVal,SDL_Color colGauge,bool bVert)
{
  microSDL_tsElem sElem;
  if (pXData == NULL) { return MSDL_ID_NONE; }
  sElem = microSDL_ElemCreate(pGui,nElemId,nPage,MSDL_TYPEX_GAUGE,rElem,NULL,MSDL_FONT_NONE);
  sElem.bFrameEn        = true;
  sElem.bFillEn         = true;
  sElem.bClickEn        = false;
  pXData->nGaugeMin     = nMin;
  pXData->nGaugeMax     = nMax;
  pXData->nGaugeVal     = nVal;
  pXData->bGaugeVert    = bVert;
  pXData->colGauge      = colGauge;
  sElem.pXData          = (void*)(pXData);
  sElem.pfuncXDraw      = &microSDL_ElemXGaugeDraw;
  sElem.pfuncXTouch     = NULL;
  sElem.colElemFrame    = MSDL_COL_GRAY;
  sElem.colElemFill     = MSDL_COL_BLACK;
  bool bOk = microSDL_ElemAdd(pGui,sElem);
  return (bOk)? sElem.nId : MSDL_ID_NONE;
}


void microSDL_ElemXGaugeUpdate(microSDL_tsGui* pGui,int nElemId,int nVal)
{
  int nElemInd = microSDL_ElemFindIndFromId(pGui,nElemId);
  if (!microSDL_ElemIndValid(pGui,nElemInd)) {
    fprintf(stderr,"ERROR: microSDL_ElemXGaugeUpdate() invalid ID=%d\n",nElemInd);
    return;
  }
  microSDL_tsElem* pElem = &pGui->psElem[nElemInd];
  microSDL_tsXGauge*  pGauge = (microSDL_tsXGauge*)(pElem->pXData);
  pGauge->nGaugeVal = nVal;
}


bool microSDL_ElemXGaugeDraw(void* pvGui,void* pvElem)
{
  microSDL_tsGui*   pGui  = (microSDL_tsGui*)(pvGui);
  microSDL_tsElem*  pElem = (microSDL_tsElem*)(pvElem);
  
  SDL_Rect    rGauge;
  int         nElemX,nElemY;
  unsigned    nElemW,nElemH;

  nElemX = pElem->rElem.x;
  nElemY = pElem->rElem.y;
  nElemW = pElem->rElem.w;
  nElemH = pElem->rElem.h;

  rGauge.x = nElemX;
  rGauge.y = nElemY;

  microSDL_tsXGauge* pXGauge;
  pXGauge = (microSDL_tsXGauge*)(pElem->pXData);
  if (pXGauge == NULL) {
    fprintf(stderr,"ERROR: microSDL_ElemXGaugeDraw() pXData is NULL\n");
    return false;
  }
  bool  bVert = pXGauge->bGaugeVert;
  int   nMax = pXGauge->nGaugeMax;
  int   nMin = pXGauge->nGaugeMin;
  int   nRng = pXGauge->nGaugeMax - pXGauge->nGaugeMin;

  if (nRng == 0) {
    fprintf(stderr,"ERROR: microSDL_ElemXGaugeDraw() Zero gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }
  float   fScl;
  if (bVert) {
    fScl = (float)nElemH/(float)nRng;
  } else {
    fScl = (float)nElemW/(float)nRng;
  }

  int nGaugeMid;
  if ((nMin == 0) && (nMax > 0)) {
    nGaugeMid = 0;
  } else if ((nMin < 0) && (nMax > 0)) {
    nGaugeMid = -nMin*fScl;
  } else if ((nMin < 0) && (nMax == 0)) {
    nGaugeMid = -nMin*fScl;
  } else {
    fprintf(stderr,"ERROR: microSDL_ElemXGaugeDraw() Unsupported gauge range [%d,%d]\n",nMin,nMax);
    return false;
  }

  // Calculate the length of the bar
  int nLen = pXGauge->nGaugeVal * fScl;

  // Handle negative ranges
  if (nLen >= 0) {
    if (bVert) {
      rGauge.h = nLen;
      rGauge.y = nElemY + nGaugeMid;
    } else {
      rGauge.w = nLen;
      rGauge.x = nElemX + nGaugeMid;
    }
  } else {
    if (bVert) {
      rGauge.y = nElemY + nGaugeMid+nLen;
      rGauge.h = -nLen;
    } else {
      rGauge.x = nElemX + nGaugeMid+nLen;
      rGauge.w = -nLen;
    }
  }

  if (bVert) {
    rGauge.w = nElemW;
    rGauge.x = nElemX;
  } else {
    rGauge.h = nElemH;
    rGauge.y = nElemY;
  }

  // Now clip the region
  // TODO: Determine if we need to adjust by -1
  int nRectClipX1 = rGauge.x;
  int nRectClipX2 = rGauge.x+rGauge.w;
  int nRectClipY1 = rGauge.y;
  int nRectClipY2 = rGauge.y+rGauge.h;
  if (nRectClipX1 < nElemX)             { nRectClipX1 = nElemX;         }
  if (nRectClipX2 > nElemX+(int)nElemW) { nRectClipX2 = nElemX+nElemW;  }
  if (nRectClipY1 < nElemY)             { nRectClipY1 = nElemY;         }
  if (nRectClipY2 > nElemY+(int)nElemH) { nRectClipY2 = nElemY+nElemH;  }
  rGauge.x = nRectClipX1;
  rGauge.y = nRectClipY1;
  rGauge.w = nRectClipX2-nRectClipX1;
  rGauge.h = nRectClipY2-nRectClipY1;

  #ifdef DBG_LOG
  printf("Gauge: nMin=%4d nMax=%4d nRng=%d nVal=%4d fScl=%6.3f nGaugeMid=%4d RectX=%4d RectW=%4d\n",
    nMin,nMax,nRng,pXGauge->nGaugeVal,fScl,nGaugeMid,rGauge.x,rGauge.w);
  #endif

  microSDL_FillRect(pGui,rGauge,pXGauge->colGauge);

  // Now draw the midpoint line
  SDL_Rect    rMidLine;
  if (bVert) {
    rMidLine.x = nElemX;
    rMidLine.y = nElemY+nGaugeMid;
    rMidLine.w = nElemW;
    rMidLine.h = 1;
  } else {
    rMidLine.x = nElemX+nGaugeMid;
    rMidLine.y = nElemY;
    rMidLine.w = 1;
    rMidLine.h = nElemH;
  }
  microSDL_FillRect(pGui,rMidLine,pElem->colElemFrame);

  return true;
}