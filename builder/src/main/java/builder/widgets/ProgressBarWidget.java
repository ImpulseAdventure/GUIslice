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
import builder.models.ProgressBarModel;

/**
 * The Class ProgressBarWidget simulates GUIslice API gslc_ElemXGaugeCreate() call.
 * 
 * @author Paul Conti
 * 
 */
public class ProgressBarWidget extends Widget {

  /**
   * Instantiates a new progress bar widget.
   *
   * @param x
   *          the x coordinate position
   * @param y
   *          the y coordinate position
   */
  public ProgressBarWidget(int x, int y) {
    u = CommonUtil.getInstance();
    model = new ProgressBarModel();
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
    if (((ProgressBarModel) model).getGaugeStyle().equals("Radial")) {
      int nElemRad = (b.width>=b.height)? b.height/2 : b.width/2;
      int nElemMidX = b.x + (nElemRad/2);
      int nElemMidY = b.y + (nElemRad/2);
      g2d.setColor(((ProgressBarModel) model).getFillColor());
      g2d.fillOval(nElemMidX,nElemMidY,nElemRad,nElemRad);
      g2d.setColor(((ProgressBarModel) model).getFrameColor());
      g2d.drawOval(nElemMidX,nElemMidY,nElemRad,nElemRad);
      g2d.setColor(((ProgressBarModel) model).getIndicatorColor());
      int ind_len = ((ProgressBarModel) model).getIndicatorSize();
      int dx = b.x + (b.width/2);
      int dy = b.y + (b.height/2);
      g2d.drawLine(dx, dy, dx-ind_len, dy-ind_len);
    } else {
      if (((ProgressBarModel) model).isVertical()) {
        int ind_height = b.height/3;  
        g2d.setColor(((ProgressBarModel) model).getIndicatorColor());
        g2d.fillRect(b.x, b.y+( b.height-ind_height), b.width, ind_height);
        g2d.setColor(((ProgressBarModel) model).getFillColor());
        g2d.fillRect(b.x, b.y, b.width, b.height-ind_height);
        if (((ProgressBarModel) model).getGaugeStyle().equals("Ramp")) {
          int y, dy;
          dy = b.height / 10;
          y = b.y + b.height-dy;
          for (y=(b.y+b.height-dy); y>(b.y+(b.height-ind_height)); y-=dy) {
            g2d.drawLine(b.x, y, b.x+b.width, y);
          }
        }
      } else {
        int ind_width = b.width/3;
        g2d.setColor(((ProgressBarModel) model).getIndicatorColor());
        g2d.fillRect(b.x, b.y, ind_width, b.height);
        g2d.setColor(((ProgressBarModel) model).getFillColor());
        g2d.fillRect(b.x+ind_width, b.y, b.width-ind_width, b.height);
        if (((ProgressBarModel) model).getGaugeStyle().equals("Ramp")) {
          int x, dx;
          dx = b.width / 10;
          for (x=b.x+dx; x<(b.x+ind_width); x+=dx) {
            g2d.drawLine(x, b.y, x, b.y+b.height);
          }
        }
      }
      g2d.setColor(((ProgressBarModel) model).getFrameColor());
      g2d.drawRect(b.x, b.y, b.width, b.height);
    }
    super.drawSelRect(g2d, b);
  }

}
