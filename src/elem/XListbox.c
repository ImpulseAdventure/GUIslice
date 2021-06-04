// =======================================================================
// GUIslice library (extensions)
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// =======================================================================
//
// The MIT License
//
// Copyright 2016-2020 Calvin Hass
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =======================================================================
/// \file XListbox.c



// GUIslice library
#include "GUIslice.h"
#include "GUIslice_drv.h"

#include "elem/XListbox.h"

#include <stdio.h>

#if (GSLC_USE_PROGMEM)
  #if defined(__AVR__)
    #include <avr/pgmspace.h>
  #else
    #include <pgmspace.h>
  #endif
#endif

// ----------------------------------------------------------------------------
// Error Messages
// ----------------------------------------------------------------------------

extern const char GSLC_PMEM ERRSTR_NULL[];
extern const char GSLC_PMEM ERRSTR_PXD_NULL[];


// ----------------------------------------------------------------------------
// Extended element definitions
// ----------------------------------------------------------------------------
//
// - This file extends the core GUIslice functionality with
//   additional widget types

// ----------------------------------------------------------------------------

// TODO: Combine with GUIslice MAX_STR
// Defines the maximum length of a listbox item
#define XLISTBOX_MAX_STR        20

// ============================================================================
// Extended Element: Listbox
// - A Listbox control
// ============================================================================


// Basic debug functionality
/*
void debug_ElemXListboxDump(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef)
{
  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui, pElemRef);
  gslc_tsXListbox*  pListbox = (gslc_tsXListbox*)(pElem->pXData);

  // Prevent runaway looping due to damaged buffer
  // Don't ask why I know to do this - PaulC

  int16_t    nSafety=0; 
  char acTxt[50 + 1];
  GSLC_DEBUG2_PRINT("Xlistbox:Dump (nBufPos=%d nItemCnt=%d)\n", pListbox->nBufItemsPos,pListbox->nItemCnt);
  for (unsigned nInd = 0; nInd < pListbox->nBufItemsPos; nInd++) {
    nSafety++;
    if (nSafety > pListbox->nBufItemsMax) break;
    char ch = pListbox->pBufItems[nInd];
    if (ch == 0) {
      snprintf(acTxt, 40,"0x%04x: [%2u] = %02X = [NULL]", (char*)&pListbox->pBufItems[nInd],nInd, ch);
    }
    else {
      snprintf(acTxt, 40,"0x%04x: [%2u] = %02X = [%c]", (char*)&pListbox->pBufItems[nInd],nInd, ch, ch);
    }
    GSLC_DEBUG2_PRINT("XListbox:Dump: %s\n", acTxt);
  }
}
*/

char* gslc_ElemXListboxGetItemAddr(gslc_tsXListbox* pListbox, int16_t nItemCurSel)
{
  char*      pBuf = NULL;
  uint16_t   nBufPos = 0;
  int16_t    nItemInd = 0;
  bool       bFound = false;
  while (1) {
    if (nItemInd == nItemCurSel) {
      bFound = true;
      break;
    }
    if (nBufPos >= pListbox->nBufItemsMax) {
      break;
    }
    if (pListbox->pBufItems[nBufPos] == 0) {
      nItemInd++;
    }
    nBufPos++;
  }
  if (bFound) {
    pBuf = (char*)&(pListbox->pBufItems[nBufPos]);
    return pBuf;
  } else {
    return NULL;
  }
}

// Recalculate listbox item sizing if enabled
bool gslc_ElemXListboxRecalcSize(gslc_tsXListbox* pListbox,gslc_tsRect rElem)
{
  int16_t nElem;
  int16_t nElemInner;
  int16_t nItemOuter;
  int16_t nItemWOld;
  bool bNeedRedraw = false;

  // If number of rows was auto-sized based on content, then calculate now
  if (pListbox->nRows == XLISTBOX_SIZE_AUTO) {
    if (pListbox->nItemCnt == 0) {
      pListbox->nRows = 1; // Force at least one row
    } else {
      pListbox->nRows = ((pListbox->nItemCnt + 1) / pListbox->nCols);
    }
  }

  // NOTE: In the nElemInner calculation, we add nItemGap to account
  // for the fact that the last column does not include a "gap" after it.
  if (pListbox->bItemAutoSizeW) {
    nItemWOld = pListbox->nItemW;
    nElem = rElem.w;
    nElemInner = nElem - (2 * pListbox->nMarginW) + pListbox->nItemGap;
    nItemOuter = nElemInner / pListbox->nCols;
    pListbox->nItemW = nItemOuter - pListbox->nItemGap;
    if (pListbox->nItemW != nItemWOld) {
      bNeedRedraw = true;
    }
  }
  if (pListbox->bItemAutoSizeH) {
    nItemWOld = pListbox->nItemH;
    nElem = rElem.h;
    nElemInner = nElem - (2 * pListbox->nMarginH) + pListbox->nItemGap;
    nItemOuter = nElemInner / pListbox->nRows;
    pListbox->nItemH = nItemOuter - pListbox->nItemGap;
    if (pListbox->nItemH != nItemWOld) {
      bNeedRedraw = true;
    }
  }

  // Clear Need Recalc flag
  pListbox->bNeedRecalc = false;

  return bNeedRedraw;
}

void gslc_ElemXListboxSetSize(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nRows, int8_t nCols)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return;

  pListbox->nRows = nRows;
  pListbox->nCols = nCols;
  pListbox->bNeedRecalc = true;
  // Mark as needing full redraw
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);
}



void gslc_ElemXListboxSetMargin(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nMarginW, int8_t nMarginH)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return;

  pListbox->nMarginW = nMarginW;
  pListbox->nMarginH = nMarginH;
  pListbox->bNeedRecalc = true;
  // Mark as needing full redraw
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);
}


void gslc_ElemXListboxItemsSetSize(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nItemW, int16_t nItemH)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return;

  pListbox->nItemW = nItemW;
  pListbox->nItemH = nItemH;
  // Determine if auto-sizing requested
  pListbox->bItemAutoSizeW = (nItemW == XLISTBOX_SIZE_AUTO) ? true : false;
  pListbox->bItemAutoSizeH = (nItemH == XLISTBOX_SIZE_AUTO) ? true : false;

  pListbox->bNeedRecalc = true;
  // Mark as needing full redraw
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);
}

void gslc_ElemXListboxItemsSetGap(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int8_t nGap, gslc_tsColor colGap)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return;

  pListbox->nItemGap = nGap;
  pListbox->colGap = colGap;
  pListbox->bNeedRecalc = true;
  // Mark as needing full redraw
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);
}


void gslc_ElemXListboxReset(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return;

  pListbox->nBufItemsPos = 0;
  pListbox->nItemCnt = 0;
  pListbox->nItemCurSel = XLISTBOX_SEL_NONE;
  pListbox->bNeedRecalc = true;
  // Mark as needing full redraw
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);
}


bool gslc_ElemXListboxAddItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, const char* pStrItem)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return false;

  int8_t      nStrItemLen;
  char*       pBuf = NULL;
  uint16_t    nBufItemsPos = pListbox->nBufItemsPos;
  uint16_t    nBufItemsMax = pListbox->nBufItemsMax;

  nStrItemLen = strlen(pStrItem);

  if (nStrItemLen == 0) {
    // Nothing to add
    return false;
  }

  // Ensure we won't overrun the buffer, including the terminator
  if (nBufItemsPos + nStrItemLen + 1 > nBufItemsMax) {
    // Proceed with truncation
    nStrItemLen = nBufItemsMax - pListbox->nBufItemsPos - 1;
    GSLC_DEBUG2_PRINT("ERROR: ElemXListboxAddItem() buffer too small\n", "");
  }

  // If the truncation left nothing to add, skip out now
  if (nStrItemLen <= 0) {
    return false;
  }

  // Treat this section of the buffer as [char]
  pBuf = (char*)pListbox->pBufItems + nBufItemsPos;
  gslc_StrCopy(pBuf, pStrItem, nStrItemLen+1);
  pListbox->nBufItemsPos += (nStrItemLen+1); // +1 for terminator

  pListbox->nItemCnt++;

  //GSLC_DEBUG2_PRINT("Xlistbox:Add\n", "");
  //debug_ElemXListboxDump(pGui, pElemRef);

  // Inidicate sizing may need update
  pListbox->bNeedRecalc = true;

  // Mark as needing full redraw
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);
  return true;
}

bool gslc_ElemXListboxInsertItemAt(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nInsertPos, 
  const char* pStrItem)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return false;

  int8_t      nStrItemLen;
  char*       pBuf = NULL;
  uint16_t    nBufItemsPos = pListbox->nBufItemsPos;
  uint16_t    nBufItemsMax = pListbox->nBufItemsMax;

  if ((int16_t)nInsertPos > pListbox->nItemCnt) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXListboxInsertItemAt() Current Count: %d Invalid Position %d\n", 
      pListbox->nItemCnt,nInsertPos);
    return false;
  }
  nStrItemLen = strlen(pStrItem);

  if (nStrItemLen == 0) {
    // Nothing to add
    return false;
  }

  // Ensure we won't overrun the buffer, including the terminator
  if (nBufItemsPos + nStrItemLen + 1 > nBufItemsMax) {
    GSLC_DEBUG2_PRINT("ERROR: ElemXListboxInsertItemAt() buffer too small\n", "");
    return false;
  }

  // If the truncation left nothing to add, skip out now
  if (nStrItemLen <= 0) {
    return false;
  }

//  GSLC_DEBUG2_PRINT("Xlistbox:InsertAt: %d\n", nInsertPos);
//  debug_ElemXListboxDump(pGui, pElemRef);

  pBuf = gslc_ElemXListboxGetItemAddr(pListbox, nInsertPos);
  // If position is incorrect, bail out...
  if (pBuf == NULL) {
    return false;
  }
 
  // Make a hole in the buffer to slot in the new item
  char* pSrc = (char*)pListbox->pBufItems+nBufItemsPos-1;
  char* pDest = pSrc+nStrItemLen+1;  
  uint16_t nMoveLen = (int16_t)(pSrc - pBuf)+1;
  uint8_t ch;
  for (uint16_t nInd = 0; nInd < nMoveLen; nInd++) {
    ch = *pSrc;
    *pDest = ch;
    pDest--;
    pSrc--;
  }
  
  // Now slot in the new item
  memcpy(pBuf, pStrItem, nStrItemLen+1);

  // update our buffer information
  pListbox->nBufItemsPos += nStrItemLen+1;
  pListbox->nItemCnt++;

//  GSLC_DEBUG2_PRINT("Xlistbox:After InsertAt %d\n", nInsertPos);
//  debug_ElemXListboxDump(pGui, pElemRef);

  // Indicate sizing may need update
  pListbox->bNeedRecalc = true;

  // Mark as needing full redraw
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);

  return true;
}

bool gslc_ElemXListboxDeleteItemAt(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nDeletePos)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return false;

  int8_t      nStrItemLen;
  char*       pBuf = NULL;
  uint16_t    nBufItemsPos = pListbox->nBufItemsPos;

//  GSLC_DEBUG2_PRINT("Xlistbox:DeleteAt: %d\n", nDeletePos);
//  debug_ElemXListboxDump(pGui, pElemRef);

  pBuf = gslc_ElemXListboxGetItemAddr(pListbox, nDeletePos);
  // If position is incorrect, bail out...
  if (pBuf == NULL) {
    return false;
  }
  nStrItemLen = strlen(pBuf);
  
  // Pull items after this delete position up to delete this item
  char* pSrc  = (char*)pBuf+nStrItemLen+1;
  char* pDest = (char*)pBuf;
  char* pEndOfBuf = (char*)(pListbox->pBufItems+nBufItemsPos);
  uint16_t nMoveLen = (int16_t)(pEndOfBuf-pSrc)+1;
  uint8_t ch;
  if (nMoveLen > 1) {
    for (uint16_t nInd = 0; nInd < nMoveLen; nInd++) {
      ch = *pSrc;
      *pDest = ch;
      pDest++;
      pSrc++;
    }
  }
  
  // update our buffer information
  pListbox->nBufItemsPos -= nStrItemLen+1;
  pListbox->nItemCnt--;
  
  // unselect item
  gslc_ElemXListboxSetSel(pGui, pElemRef, XLISTBOX_SEL_NONE);

//  GSLC_DEBUG2_PRINT("Xlistbox:After DeleteAt %d\n", nDeletePos);
//  debug_ElemXListboxDump(pGui, pElemRef);

  // Indicate sizing may need update
  pListbox->bNeedRecalc = true;

  // Mark as needing full redraw
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);

  return true;
}

bool gslc_ElemXListboxGetItem(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, int16_t nItemCurSel, char* pStrItem, 
  uint8_t nStrItemLen)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return false;

  // Ensure user provided valid string
  if ((pStrItem == NULL) || (nStrItemLen == 0)) {
    // ERROR
    return false;
  }
  uint16_t   nBufPos = 0;
  int16_t   nItemInd = 0;
  uint8_t*   pBuf = NULL;
  bool       bFound = false;
  while (1) {
    if (nItemInd == nItemCurSel) {
      bFound = true;
      break;
    }
    if (nBufPos >= pListbox->nBufItemsMax) {
      break;
    }
    if (pListbox->pBufItems[nBufPos] == 0) {
      nItemInd++;
    }
    nBufPos++;
  }
  if (bFound) {
    pBuf = &(pListbox->pBufItems[nBufPos]);
    gslc_StrCopy(pStrItem, (char*)pBuf, nStrItemLen);
    return true;
  } else {
    // If no item was found, return an empty string (NULL)
    pStrItem[0] = 0;
    return false;
  }
}


int16_t gslc_ElemXListboxGetItemCnt(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return 0;

  return pListbox->nItemCnt;
}


// Create a Listbox element and add it to the GUI element list
// - Defines default styling for the element
// - Defines callback for redraw and touch
gslc_tsElemRef* gslc_ElemXListboxCreate(gslc_tsGui* pGui,int16_t nElemId,int16_t nPage,
  gslc_tsXListbox* pXData,gslc_tsRect rElem,int16_t nFontId,uint8_t* pBufItems,uint16_t nBufItemsMax,int16_t nItemDefault)
{
  if ((pGui == NULL) || (pXData == NULL)) {
    static const char GSLC_PMEM FUNCSTR[] = "ElemXListboxCreate";
    GSLC_DEBUG2_PRINT_CONST(ERRSTR_NULL,FUNCSTR);
    return NULL;
  }
  gslc_tsElem     sElem;
  gslc_tsElemRef* pElemRef = NULL;
  sElem = gslc_ElemCreate(pGui,nElemId,nPage,GSLC_TYPEX_LISTBOX,rElem,NULL,0,nFontId);
  sElem.nFeatures        &= ~GSLC_ELEM_FEA_FRAME_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FILL_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_CLICK_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_GLOW_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_EDIT_EN;
  sElem.nFeatures        |= GSLC_ELEM_FEA_FOCUS_EN;

  sElem.nGroup            = GSLC_GROUP_ID_NONE;
  pXData->pBufItems       = pBufItems;
  pXData->nBufItemsMax    = nBufItemsMax;
  pXData->nBufItemsPos    = 0;
  pXData->nItemCnt        = 0;
  pXData->nItemCurSel     = nItemDefault;
  pXData->nItemCurSelLast = XLISTBOX_SEL_NONE;
  pXData->nItemSavedSel   = XLISTBOX_SEL_NONE;
  pXData->nItemTop        = 0;
  pXData->pfuncXSel       = NULL;
  pXData->nCols           = 1;
  pXData->nRows           = XLISTBOX_SIZE_AUTO;     // Auto-calculated from content
  pXData->bNeedRecalc     = true;                   // Force auto-sizing
  pXData->nMarginW        = 5;
  pXData->nMarginH        = 5;
  pXData->bItemAutoSizeW  = true;                   // Force auto-sizing
  pXData->bItemAutoSizeH  = false;
  pXData->nItemW          = XLISTBOX_SIZE_AUTO;     // Auto-sized
  pXData->nItemH          = 30;
  pXData->nItemGap        = 2;
  pXData->colGap          = GSLC_COL_BLACK;
  pXData->nItemCurSelLast = XLISTBOX_SEL_NONE;
  pXData->bGlowLast       = false;
  pXData->bFocusLast      = false;
  sElem.pXData            = (void*)(pXData);
  // Specify the custom drawing callback
  sElem.pfuncXDraw        = &gslc_ElemXListboxDraw;
  // Specify the custom touch tracking callback
  sElem.pfuncXTouch       = &gslc_ElemXListboxTouch;

  sElem.colElemFill       = GSLC_COL_BLACK;
  sElem.colElemFillGlow   = GSLC_COL_BLACK;
  sElem.colElemFrame      = GSLC_COL_GRAY;
  sElem.colElemFrameGlow  = GSLC_COL_WHITE;

  // Set default text alignment:
  // - Vertical center, left justify
  sElem.eTxtAlign        = GSLC_ALIGN_MID_LEFT;

  if (nPage != GSLC_PAGE_NONE) {
    pElemRef = gslc_ElemAdd(pGui,nPage,&sElem,GSLC_ELEMREF_DEFAULT);
    return pElemRef;
#if (GSLC_FEATURE_COMPOUND)
  } else {
    // Save as temporary element
    pGui->sElemTmp = sElem;
    pGui->sElemRefTmp.pElem = &(pGui->sElemTmp);
    pGui->sElemRefTmp.eElemFlags = GSLC_ELEMREF_DEFAULT | GSLC_ELEMREF_REDRAW_FULL;
    return &(pGui->sElemRefTmp);
#endif
  }
  return NULL;
}


// Redraw the Listbox
// - Note that this redraw is for the entire element rect region
// - The Draw function parameters use void pointers to allow for
//   simpler callback function definition & scalability.
bool gslc_ElemXListboxDraw(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*       pGui  = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef*   pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return false;

  gslc_tsElem*      pElem = gslc_GetElemFromRef(pGui,pElemRef);

  bool            bFrameEn     = (pElem->nFeatures & GSLC_ELEM_FEA_FRAME_EN);
  bool            bGlow        = (pElem->nFeatures & GSLC_ELEM_FEA_GLOW_EN) && gslc_ElemGetGlow(pGui,pElemRef);
  bool            bGlowLast    = pListbox->bGlowLast;  
  bool            bFocus       = (pElem->nFeatures & GSLC_ELEM_FEA_FOCUS_EN) && gslc_ElemGetFocus(pGui,pElemRef);
  bool            bFocusLast   = pListbox->bFocusLast;
  int16_t         nItemCurSel  = pListbox->nItemCurSel;

  // Determine the regions and colors based on element state
  gslc_tsRectState sState;
  gslc_ElemCalcRectState(pGui,pElemRef,&sState);

  if (bFrameEn) {
    bool bDrawFrame = false;
    if (eRedraw == GSLC_REDRAW_FULL) { bDrawFrame = true; }
    if (bGlowLast != bGlow) { bDrawFrame = true; }
    if (bFocusLast != bFocus) { bDrawFrame = true; }
    if (bDrawFrame) {
      gslc_DrawFrameRect(pGui, sState.rFull, sState.colFrm);
    }
  }

  // Update last state
  pListbox->bGlowLast = bGlow;
  pListbox->bFocusLast = bFocus;

  // If full redraw and gap is enabled:
  // - Clear background with gap color, as list items
  //   will be overdrawn in colBg color
  if (eRedraw == GSLC_REDRAW_FULL) {
    if (pListbox->nItemGap > 0) {
      gslc_DrawFillRect(pGui, sState.rInner, pListbox->colGap);
    }
  }

  // Determine if we need to recalculate the item sizing
  if (pListbox->bNeedRecalc) {
    gslc_ElemXListboxRecalcSize(pListbox, sState.rInner);
  }

  int16_t nX0 = sState.rInner.x;
  int16_t nY0 = sState.rInner.y;

  int8_t        nRows = pListbox->nRows;
  int8_t        nCols = pListbox->nCols;
  gslc_tsRect   rItemRect;
  int16_t       nItemBaseX, nItemBaseY;
  int16_t       nItemX, nItemY;
  int16_t       nItemW, nItemH;
  bool          bItemSel;
  gslc_tsColor  colFill, colTxt;

  // Error check
  if (nCols == 0) {
    return false;
  }

  // Determine top-left coordinate of list matrix
  nItemBaseX = nX0 + pListbox->nMarginW;
  nItemBaseY = nY0 + pListbox->nMarginH;
  char acStr[XLISTBOX_MAX_STR+1] = "";


  // Loop through the items in the list
  int16_t nItemTop = pListbox->nItemTop;
  int16_t nItemCnt = pListbox->nItemCnt;
  int16_t nItemInd;

  // Note that nItemTop is always pointing to an
  // item index at the start of a row

  // Determine the list indices to display in the visible window due to scrolling
  int16_t nDispIndMax = (nRows * nCols);

  for (int16_t nDispInd = 0; nDispInd < nDispIndMax; nDispInd++) {

    // Calculate the item index based on the display index
    nItemInd = nItemTop + nDispInd;

    // Did we go past the end of our list?
    if (nItemInd >= nItemCnt) {
      break;
    }

    // Fetch the list item
    bool bOk = gslc_ElemXListboxGetItem(pGui, pElemRef, nItemInd, acStr, XLISTBOX_MAX_STR);
    if (!bOk) {
      // TODO: Erorr handling
      break;
    }

    int16_t   nItemIndX, nItemIndY;
    int16_t   nItemOuterW, nItemOuterH;

    // Convert linear count into row & column
    nItemIndY = nDispInd / nCols; // Round down
    nItemIndX = nDispInd % nCols;

    // Calculate total spacing between items (including gap)
    nItemOuterW = pListbox->nItemW + pListbox->nItemGap;
    nItemOuterH = pListbox->nItemH + pListbox->nItemGap;

    // Determine top-left corner of each item
    nItemW = pListbox->nItemW;
    nItemH = pListbox->nItemH;
    nItemY = nItemBaseY + (nItemIndY * nItemOuterH);
    nItemX = nItemBaseX + (nItemIndX * nItemOuterW);

    // Create rect for item
    rItemRect = (gslc_tsRect) { nItemX, nItemY, nItemW, nItemH };

    // Is the item selected?
    bItemSel = (nItemInd == nItemCurSel) ? true : false;

    // Determine the color based on state
    colFill = (bItemSel) ? pElem->colElemFillGlow : pElem->colElemFill;
    colTxt = (bItemSel) ? pElem->colElemTextGlow : pElem->colElemText;

    bool bDoRedraw = false;
    if (eRedraw == GSLC_REDRAW_FULL) {
      bDoRedraw = true;
    } else if (eRedraw == GSLC_REDRAW_INC) {
      // Redraw both the old selected item (ie. to unselect it)
      // and the current selected item (ie. to select it)
      if (nItemInd == pListbox->nItemCurSelLast) {
        bDoRedraw = true;
      } else if (nItemInd == nItemCurSel) {
        bDoRedraw = true;
      }
    }

    // Draw the list item
    if (bDoRedraw) {
      gslc_DrawFillRect(pGui, rItemRect, colFill);

      // Set the text flags to indicate that the user has separately
      // allocated memory for the text strings.
      gslc_teTxtFlags eTxtFlags = GSLC_TXT_MEM_RAM | GSLC_TXT_ALLOC_EXT;

      // Draw the aligned text string (by default it is GSLC_ALIGN_MID_LEFT)
      gslc_DrawTxtBase(pGui, acStr, rItemRect, pElem->pTxtFont, eTxtFlags,
        pElem->eTxtAlign, colTxt, colFill, pElem->nTxtMarginX, pElem->nTxtMarginY);
    }

  }

  // Save the last selected item during redraw
  pListbox->nItemCurSelLast = nItemCurSel;

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  // Mark page as needing flip
  gslc_PageFlipSet(pGui,true);

  return true;
}

bool gslc_ElemXListboxTouch(void* pvGui, void* pvElemRef, gslc_teTouch eTouch, int16_t nRelX, int16_t nRelY)
{
  #if defined(DRV_TOUCH_NONE)
  return false;
  #else
  gslc_tsGui*           pGui = NULL;
  gslc_tsElemRef*       pElemRef = NULL;

  // Typecast the parameters to match the GUI
  pGui = (gslc_tsGui*)(pvGui);
  pElemRef = (gslc_tsElemRef*)(pvElemRef);

  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return false;

  //bool    bGlowingOld = gslc_ElemGetGlow(pGui, pElemRef);
  bool    bEditingOld = gslc_ElemGetEdit(pGui, pElemRef);
  bool    bIndexed = false;

  int16_t nItemSavedSel = pListbox->nItemSavedSel;
  int16_t nItemCurSelOld = pListbox->nItemCurSel;
  int16_t nItemCurSel = XLISTBOX_SEL_NONE;
  bool bSelTrack = false;
  bool bSelSave = false;
  bool bSelInItem = true;

  switch (eTouch) {

  case GSLC_TOUCH_DOWN_IN:
    // Start glowing as must be over it
    gslc_ElemSetGlow(pGui, pElemRef, true);
    // User pressed inside elem: start selection
    bSelTrack = true;
    break;

  case GSLC_TOUCH_MOVE_IN:
    gslc_ElemSetGlow(pGui, pElemRef, true);
    // Track changes in selection
    bSelTrack = true;
    break;
  case GSLC_TOUCH_MOVE_OUT:
    gslc_ElemSetGlow(pGui, pElemRef, false);
    // User has dragged to outside elem: deselect
    bSelTrack = true;
    break;

  case GSLC_TOUCH_UP_IN:
    // End glow
    gslc_ElemSetGlow(pGui, pElemRef, false);
    // User released inside elem.
    // Save selection.
    // If selection is same as previous: toggle it
    bSelTrack = true;
    bSelSave = true;
    break;
  case GSLC_TOUCH_UP_OUT:
    // End glow
    gslc_ElemSetGlow(pGui, pElemRef, false);
    // User released outside elem: leave selection as-is
    bSelTrack = true;
    bSelSave = true; // Save SEL_NONE
    break;

  case GSLC_TOUCH_FOCUS_SELECT:
    gslc_ElemSetEdit(pGui,pElemRef,!bEditingOld);
    break;

  case GSLC_TOUCH_SET_REL:
  case GSLC_TOUCH_SET_ABS:
    bIndexed = true;
    // Keyboard / pin control
    bSelTrack = true;
    bSelSave = true;
    break;

  default:
    return false;
    break;
  }

  int8_t nCols = pListbox->nCols;
  int8_t nRows = pListbox->nRows;

  // If we need to update the Listbox selection, calculate the value
  // and perform the update
  if (bSelTrack) {

    if (bIndexed) {
      // The selection is changed by direct control (eg. keyboard)
      // instead of touch coordinates

      if (eTouch == GSLC_TOUCH_SET_REL) {
        // If nothing has been selected, force to select first entry
        if (nItemCurSelOld == XLISTBOX_SEL_NONE) {
          nItemCurSel = 0;
        } else {
          // Overload the "nRelY" parameter as an increment value
          nItemCurSel = nItemCurSelOld + nRelY;
        }
      }
      else if (eTouch == GSLC_TOUCH_SET_ABS) {
        // Overload the "nRelY" parameter as an absolute value
        nItemCurSel = nRelY;
      }

      gslc_ElemXListboxSetSel(pGui, pElemRef, nItemCurSel);

      // If any selection callback is defined, call it now
      // - However, we need to read-back the value to account
      //   for any validation (limit checking) performed
      //   on the above increment/absolute value.
      nItemCurSel = gslc_ElemXListboxGetSel(pGui,pElemRef);

      // If any selection callback is defined, call it now
      if (pListbox->pfuncXSel != NULL) {
        (*pListbox->pfuncXSel)((void*)(pGui), (void*)(pElemRef), nItemCurSel);
      }
    } else {
      // Determine which item we are tracking
      int16_t nItemOuterW, nItemOuterH;
      int16_t nDispR, nDispC;
      int16_t nItemR, nItemC;

      // Get position relative to top-left list matrix cell
      nRelX -= pListbox->nMarginW;
      nRelY -= pListbox->nMarginH;

      // Determine spacing between matrix cells
      nItemOuterW = pListbox->nItemW + pListbox->nItemGap;
      nItemOuterH = pListbox->nItemH + pListbox->nItemGap;

      // Determine which matrix cell we are in
      nDispC = nRelX / nItemOuterW;
      nDispR = nRelY / nItemOuterH;

      if ((nRelX < 0) || (nRelY < 0)) {
        bSelInItem = false;
      }

      // Determine if the selection was inside the range of displayed items
      if ((nDispR < 0) || (nDispR >= nRows) || (nDispC < 0) || (nDispC >= nCols)) {
        bSelInItem = false;
      }
      if (bSelInItem) {

        // We have confirmed that the selected cell is
        // within the visible display range. Now translate
        // the display cell index to the absolute list cell index
        // by taking into account the scroll position.
        // - Note that nItemTop is always pointing to an
        //   item index at the start of a row
        nItemC = nDispC;
        nItemR = pListbox->nItemTop + nDispR;

        // Now we have identified the cell within the list matrix
        nItemCurSel = nItemR * nCols + nItemC;

        // Confirm we haven't selected out of range
        if (nItemCurSel >= pListbox->nItemCnt) {
          bSelInItem = false;
        }
      }

      if (bSelInItem) {
        // Now check for touch in gap region
        // - First find offset from top-left of matrix cell
        nRelX = nRelX % nItemOuterW;
        nRelY = nRelY % nItemOuterH;
        // If we are in the gap region, disable
        if (nRelX > pListbox->nItemW) {
          bSelInItem = false;
        }
        if (nRelY > pListbox->nItemH) {
          bSelInItem = false;
        }
      }

      // If we determined that the coordinate was not inside an
      // element, then clear current selection
      if (!bSelInItem) {
        nItemCurSel = XLISTBOX_SEL_NONE;
      }

      // If we have committed a touch press within an item
      // that was already selected, then toggle (deselect it)
      if ((bSelSave) && (nItemCurSel == nItemSavedSel)) {
        nItemCurSel = XLISTBOX_SEL_NONE;
      }

      bool bDoRedraw = false;

      if (nItemCurSel != nItemCurSelOld) {
        // Selection changed, so we will redraw
        bDoRedraw = true;
      }

      if (bDoRedraw) {
        // Update the selection
        gslc_ElemXListboxSetSel(pGui, pElemRef, nItemCurSel);

        // If any selection callback is defined, call it now
        // - However, we need to read-back the value to account
        //   for any validation (limit checking) performed
        //   on the above increment/absolute value.
        nItemCurSel = gslc_ElemXListboxGetSel(pGui,pElemRef);
        if (pListbox->pfuncXSel != NULL) {
          (*pListbox->pfuncXSel)((void*)(pGui), (void*)(pElemRef), nItemCurSel);
        }
        // Redraw the element
        // - Note that ElemXListboxSetSel() above will also request redraw
        gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_INC);
      }

      // Update the saved selection
      if (bSelSave) {
        pListbox->nItemSavedSel = nItemCurSel;
      }

    } // bIndexed

  } // bSelTrack

  return true;
  
  #endif // DRV_TOUCH_NONE
}

int16_t gslc_ElemXListboxGetSel(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return XLISTBOX_SEL_NONE;

  return pListbox->nItemCurSel;
}

bool gslc_ElemXListboxSetSel(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef, int16_t nItemCurSel)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return false;

  bool bOk = false;
  if (nItemCurSel == XLISTBOX_SEL_NONE) { bOk = true; }
  if ((nItemCurSel >= 0) && (nItemCurSel < pListbox->nItemCnt)) { bOk = true; }
  if (bOk) {
    pListbox->nItemCurSel = nItemCurSel;
    gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_INC);

    // Now ensure that the selection is still visible,
    // otherwise force a scroll
    int16_t nItemTop = pListbox->nItemTop;
    int16_t nRange   = pListbox->nRows * pListbox->nCols;

    if (nItemCurSel == XLISTBOX_SEL_NONE) {
    } else if (nItemCurSel < nItemTop) {
      // Selected an element above the current view
      // - Therefore, scroll to place it at the top
      gslc_ElemXListboxSetScrollPos(pGui,pElemRef,nItemCurSel);

    } else if (nItemCurSel >= (nItemTop + nRange)) {
      // Selected an element below the current view
      // - Therefore, scroll to place it at the bottom
      gslc_ElemXListboxSetScrollPos(pGui,pElemRef,nItemCurSel+1-nRange);
    }

  }
  return bOk;
}

bool gslc_ElemXListboxSetScrollPos(gslc_tsGui* pGui, gslc_tsElemRef* pElemRef, uint16_t nScrollPos)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return false;

  bool bOk = false;
  if ((int16_t)nScrollPos < pListbox->nItemCnt) {
    bOk = true;
  }

  int16_t nCols = pListbox->nCols;
  int16_t nItemCnt = pListbox->nItemCnt;

  // Error handling: in case position out of bounds
  if ((int16_t)nScrollPos >= nItemCnt) {
    nScrollPos = (nItemCnt > 0) ? nItemCnt - 1 : 0;
  }

  // Adjust the top item index to the start of its row
  // - This is done because we may have multiple columns
  //   per row. This ensures nItemTop points to the list
  //   index at the start of a row.
  nScrollPos = (nScrollPos / nCols) * nCols;
  pListbox->nItemTop = nScrollPos;

  // Need to update all rows in display
  gslc_ElemSetRedraw(pGui, pElemRef, GSLC_REDRAW_FULL);
  return bOk;
}


// Assign the selection callback function for a Listbox
void gslc_ElemXListboxSetSelFunc(gslc_tsGui* pGui,gslc_tsElemRef* pElemRef,GSLC_CB_XLISTBOX_SEL funcCb)
{
  gslc_tsXListbox* pListbox = (gslc_tsXListbox*)gslc_GetXDataFromRef(pGui, pElemRef, GSLC_TYPEX_LISTBOX, __LINE__);
  if (!pListbox) return;

  pListbox->pfuncXSel = funcCb;
}

// ============================================================================
