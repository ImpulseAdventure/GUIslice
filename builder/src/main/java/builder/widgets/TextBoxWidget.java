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
import builder.models.TextBoxModel;

/**
 * The Class TextBoxWidget simulates GUIslice API gslc_ElemXTextboxCreate() call.
 * 
 * @author Paul Conti
 * 
 */
public class TextBoxWidget extends Widget {

  /**
   * Instantiates a new text box widget.
   *
   * @param x
   *          the x coordinate position
   * @param y
   *          the y coordinate position
   */
  public TextBoxWidget(int x, int y) {
    u = CommonUtil.getInstance();
    model = new TextBoxModel();
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
    TextBoxModel m = ((TextBoxModel) model);
    g2d.setColor(m.getFillColor());
    g2d.fillRect(b.x, b.y, b.width, b.height);
    g2d.setColor(m.getFrameColor());
    g2d.drawRect(b.x, b.y, b.width, b.height);
    g2d.drawRect(b.x+2, b.y+4, b.width-23, b.height-8);
    drawScrollBar(g2d, m, b);
    super.drawSelRect(g2d, b);
  }

  
  /**
   * Draw scroll bar.
   *
   * @param g2d
   *          the g 2 d
   * @param m
   *          the m
   * @param r
   *          the r
   */
  private void drawScrollBar(Graphics2D g2d, TextBoxModel m, Rectangle r) {
    // Most of this code was shamelessly ripped from GUIslice_ex.c->gslc_ElemXSliderDraw()
    int nX0,nY0,nX1,nY1,nXMid,width,height;
    nX0 = r.x + r.width - 21;
    nY0 = r.y + 4;
    width = 20;
    height = r.height - 8;
    nX1 = nX0 + width - 1;
    nY1 = nY0 + height - 1;
    nXMid = (nX0+nX1)/2;
    int nPosOffset;
    // Provide some margin so thumb doesn't exceed control bounds
    int nThumbSz = 5;
    int nMargin  = nThumbSz;
    nPosOffset = (height/2);
    // Draw the track
    g2d.setColor(m.getFrameColor());
    g2d.drawLine(nXMid,nY0+nMargin,nXMid,nY1-nMargin);
    // Draw the thumb control
    int nCtrlX0,nCtrlY0;
    nCtrlX0   = nXMid-nThumbSz;
    nCtrlY0   = nY0+nPosOffset-nThumbSz;
    g2d.setColor(m.getFrameColor());
    g2d.drawRect(nCtrlX0, nCtrlY0, 2*nThumbSz, 2*nThumbSz);
  }
}
