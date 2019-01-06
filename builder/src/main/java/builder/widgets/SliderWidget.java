/**
 *
 * The MIT License
 *
 * Copyright 2018, 2019 Paul Conti
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
package builder.widgets;

import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;

import builder.common.CommonUtil;
import builder.models.SliderModel;

/**
 * The Class SliderWidget simulates GUIslice API gslc_ElemXSliderCreate() call.
 * 
 * @author Paul Conti
 * 
 */
public class SliderWidget extends Widget {

  /**
   * Instantiates a new slider widget.
   *
   * @param x
   *          the x coordinate position
   * @param y
   *          the y coordinate position
   */
  public SliderWidget(int x, int y) {
    u = CommonUtil.getInstance();
    model = new SliderModel();
    Point p = CommonUtil.getInstance().fitToGrid(x, y, model.getWidth(), model.getHeight());
    p = CommonUtil.getInstance().snapToGrid(p.x, p.y);
    model.setX(p.x);
    model.setY(p.y);
  }

  /**
   * draw
   *
   * @see builder.widgets.Widget#draw(java.awt.Graphics2D)
   */
  public void draw(Graphics2D g2d) {
    Rectangle b = getWinBounded();
    SliderModel m = ((SliderModel) model);
    // Most of this code was shamelessly ripped from GUIslice_ex.c->gslc_ElemXSliderDraw()
    int nX0,nY0,nX1,nY1,nXMid,nYMid;
    nX0 = b.x;
    nY0 = b.y;
    nX1 = b.x + b.width - 1;
    nY1 = b.y + b.height - 1;
    nXMid = (nX0+nX1)/2;
    nYMid = (nY0+nY1)/2;

    int nPosOffset;
    // Provide some margin so thumb doesn't exceed control bounds
    int nThumbSz = m.getThumbSize();
    int nMargin  = nThumbSz;
    int nCtrlRng;
    if (!m.isVertical()) {
      nCtrlRng = (nX1-nMargin)-(nX0+nMargin);
      nPosOffset = (b.width/2)-m.getMin();
    } else {
      nCtrlRng = (nY1-nMargin)-(nY0+nMargin);
      nPosOffset = (b.height/2)-m.getMin();
    }
    // Draw the background
    g2d.setColor(m.getFillColor());
    g2d.fillRect(b.x, b.y, b.width, b.height);
    // Draw any ticks
    int nTickDiv = m.getDivisions();
    // - Need at least one tick segment
    if (nTickDiv>=1) {
      int  nTickInd;
      int  nTickOffset;
      g2d.setColor(m.getTickColor());
      for (nTickInd=0;nTickInd<=nTickDiv;nTickInd++) {
        nTickOffset = nTickInd * nCtrlRng / nTickDiv;
        if (!m.isVertical()) {
          g2d.drawLine(nX0+nMargin+nTickOffset,nYMid,
                  nX0+nMargin+nTickOffset,nYMid+m.getTickSize());
        } else {
          g2d.drawLine(nXMid,nY0+nMargin+ nTickOffset,
                  nXMid+m.getTickSize(),nY0+nMargin + nTickOffset);
        }
      }
    }
    // Draw the track and optionally draw a trim line
     g2d.setColor(m.getFrameColor());
    if (!m.isVertical()) {
      g2d.drawLine(nX0+nMargin,nYMid,nX1-nMargin,nYMid);
      if (m.isTrimStyle()) {
        g2d.setColor(m.getTrimColor());
        g2d.drawLine(nX0+nMargin,nYMid+1,nX1-nMargin,nYMid+1);
      }
    } else {
      g2d.drawLine(nXMid,nY0+nMargin,nXMid,nY1-nMargin);
      if (m.isTrimStyle()) {
        g2d.setColor(m.getTrimColor());
        g2d.drawLine(nXMid+1,nY0+nMargin,nXMid+1,nY1-nMargin);
      }
    }
    // Draw the thumb control
    int nCtrlX0,nCtrlY0;
    if (!m.isVertical()) {
      nCtrlX0   = nX0+nPosOffset-nThumbSz;
      nCtrlY0   = nYMid-nThumbSz;
    } else {
      nCtrlX0   = nXMid-nThumbSz;
      nCtrlY0   = nY0+nPosOffset-nThumbSz;
    }
    g2d.setColor(m.getFillColor());
    g2d.fillRect(nCtrlX0, nCtrlY0, 2*nThumbSz, 2*nThumbSz);
    g2d.setColor(m.getFrameColor());
    g2d.drawRect(nCtrlX0, nCtrlY0, 2*nThumbSz, 2*nThumbSz);

    super.drawSelRect(g2d, b);
  }

}
