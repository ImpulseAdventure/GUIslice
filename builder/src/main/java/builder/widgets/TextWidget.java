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

import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;

import builder.common.CommonUtil;
import builder.common.FontFactory;
import builder.models.TextModel;
import builder.prefs.TextEditor;

/**
 * The Class TextWidget simulates GUIslice API gslc_ElemCreateTxt() call.
 * 
 * @author Paul Conti
 * 
 */
public class TextWidget extends Widget {
  
  /** The text model. */
  TextModel m = null;
  
  /**
   * Instantiates a new text widget.
   *
   * @param x
   *          the x coordinate position
   * @param y
   *          the y coordinate position
   */
  public TextWidget(int x, int y) {
    u = CommonUtil.getInstance();
    ff = FontFactory.getInstance();
    m = new TextModel();
    model = m;
    Point p = CommonUtil.getInstance().fitToGrid(x, y, model.getWidth(), model.getHeight());
    p = CommonUtil.getInstance().snapToGrid(p.x, p.y);
    model.setX(p.x);
    model.setY(p.y);
    setUserPrefs(TextEditor.getInstance().getModel());
  }

  /**
   * draw
   *
   * @see builder.widgets.Widget#draw(java.awt.Graphics2D)
   */
  public void draw(Graphics2D g2d) {
    Font font = ff.getFont(m.getFontDisplayName());
    Rectangle b = getWinBounded();
    g2d.setColor(m.getFillColor());
    g2d.fillRect(b.x, b.y, b.width, b.height);
    if (m.isFrameEnabled()) {
      g2d.setColor(m.getFrameColor());
      g2d.drawRect(b.x, b.y, b.width, b.height);
    }
    g2d.setColor(m.getTextColor());
    ff.alignString(g2d, m.getAlignment(), b, m.getText(), font);
    super.drawSelRect(g2d, b);
  }

  /**
   * getWinBounded
   *
   * @see builder.widgets.Widget#getWinBounded()
   */
  @Override
  public Rectangle getWinBounded() {
    int dx = u.toWinX(m.getX());
    int dy = u.toWinY(m.getY());
    Rectangle b = new Rectangle();
    b.x = dx;
    b.y = dy;
    b.width = m.getWidth();
    b.height = m.getHeight();
    return b;
  }

  /**
   * contains
   *
   * @see builder.widgets.Widget#contains(java.awt.Point)
   */
  @Override
  /**
   * Return true if this node contains p.
   */

  public boolean contains(Point p) {
    Rectangle b = new Rectangle();
    b.x = model.getX()-2;
    b.y = model.getY()-2;
    b.width = m.getWidth()+4;
    b.height = m.getHeight()+4;
    return b.contains(p);
  }

}
