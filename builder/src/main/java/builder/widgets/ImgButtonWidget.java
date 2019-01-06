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
import builder.models.ImgButtonModel;

/**
 * The Class ImgButtonWidget simulates GUIslice API gslc_ElemCreateBtnImg() call.
 * 
 * @author Paul Conti
 * 
 */
public class ImgButtonWidget extends Widget {

  /**
   * Instantiates a new img button widget.
   */
  public ImgButtonWidget() {
    u = CommonUtil.getInstance();
    model = new ImgButtonModel();
  }

  /**
   * Sets the image.
   *
   * @param file
   *          the file containing image to use when button is not selected.
   * @param x
   *          the x coordinate position
   * @param y
   *          the y coordinate position
   */
  public void setImage(File file, int x, int y) {
    ((ImgButtonModel)model).setImage(file, x, y);
    Point p = CommonUtil.getInstance().fitToGrid(x, y, model.getWidth(), model.getHeight());
    p = CommonUtil.getInstance().snapToGrid(p.x, p.y);
    model.setX(p.x);
    model.setY(p.y);
  }
  
  /**
   * Sets the image selected.
   *
   * @param file
   *          the file containing image to use when button is selected.
   */
  public void setImageSelected(File file) {
    ((ImgButtonModel)model).setImageSelected(file);
  }
  
  /**
   * draw
   *
   * @see builder.widgets.Widget#draw(java.awt.Graphics2D)
   */
  public void draw(Graphics2D g2d) {
    Rectangle b = getWinBounded();
    if (bSelected) {
      g2d.drawImage(((ImgButtonModel)model).getImageSelected(), b.x, b.y, null);
    } else {
      g2d.drawImage(((ImgButtonModel)model).getImage(), b.x, b.y, null);
    }
    super.drawSelRect(g2d, b);
  }

}
