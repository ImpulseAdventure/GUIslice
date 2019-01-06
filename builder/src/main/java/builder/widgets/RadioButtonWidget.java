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
import builder.models.RadioButtonModel;
import builder.prefs.RadioButtonEditor;

/**
 * <p>
 * The Class RadioButtonWidget simulates GUIslice API gslc_ElemXCheckboxCreate() call
 * with the GSLCX_CHECKBOX_STYLE_ROUND.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class RadioButtonWidget extends Widget {

  /**
   * Instantiates a new radio button widget.
   *
   * @param x
   *          the x coordinate position
   * @param y
   *          the y coordinate position
   */
  public RadioButtonWidget(int x, int y) {
    u = CommonUtil.getInstance();
    model = new RadioButtonModel();
    Point p = CommonUtil.getInstance().fitToGrid(x, y, model.getWidth(), model.getHeight());
    p = CommonUtil.getInstance().snapToGrid(p.x, p.y);
    model.setX(p.x);
    model.setY(p.y);
    setUserPrefs(RadioButtonEditor.getInstance().getModel());
  }

  /**
   * draw
   *
   * @see builder.widgets.Widget#draw(java.awt.Graphics2D)
   */
  public void draw(Graphics2D g2d) {
    Rectangle b = getWinBounded();
    int cx = b.x + (b.width/2);
    int cy = b.y + (b.height/2);
    int radius = b.width/2;
    if (bSelected) {
      g2d.setColor(((RadioButtonModel) model).getSelectedColor());
      fillCircle(g2d,cx, cy, radius);
    } else {
      g2d.setColor(((RadioButtonModel) model).getFillColor());
      fillCircle(g2d,cx, cy, radius);
    }
    g2d.setColor(((RadioButtonModel) model).getFrameColor());
    drawCircle(g2d, cx, cy, radius);
    if (((RadioButtonModel) model).isChecked()) {
      // Draw an circle in center if checked
      radius = b.width/3;
      g2d.setColor(((RadioButtonModel) model).getMarkColor());
      fillCircle(g2d,cx, cy, radius);
    }
    super.drawSelRect(g2d, b);
  }

  /**
   * Draw circle.
   * Convenience method to draw a circle from center with radius
   *
   * @param g2d
   *          the graphics object
   * @param xCenter
   *          the x center
   * @param yCenter
   *          the y center
   * @param r
   *          the r
   */
  public void drawCircle(Graphics2D g2d, int xCenter, int yCenter, int r) {
    g2d.drawOval(xCenter-r, yCenter-r, 2*r, 2*r);
  }

  /**
   * Fill circle.
   * Convenience method to fill circle from center with radius
   *
   * @param g2d
   *          the graphics
   * @param xCenter
   *          the x center
   * @param yCenter
   *          the y center
   * @param r
   *          the r
   */
  public void fillCircle(Graphics2D g2d, int xCenter, int yCenter, int r) {
    g2d.fillOval(xCenter-r, yCenter-r, 2*r, 2*r);
  }

}
