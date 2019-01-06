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
import java.io.File;

import builder.common.CommonUtil;
import builder.models.ImageModel;

/**
 * The Class ImageWidget simulates GUIslice API gslc_ElemCreateImg() call.
 * 
 * @author Paul Conti
 * 
 */
public class ImageWidget extends Widget {
  
  /**
   * Instantiates a new image widget.
   */
  public ImageWidget() {
    u = CommonUtil.getInstance();
    model = new ImageModel();
  }

  /**
   * Sets the image.
   *
   * @param file
   *          the file containing the image
   * @param x
   *          the x coordinate position
   * @param y
   *          the y coordinate position
   */
  public void setImage(File file, int x, int y) {
    ((ImageModel)model).setImage(file, x, y);
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
    g2d.drawImage(((ImageModel)model).getImage(), b.x, b.y, null);
    super.drawSelRect(g2d, b);
  }

}
