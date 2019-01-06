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
import java.awt.Stroke;

import builder.common.CommonUtil;
import builder.models.GraphModel;

/**
 * The Class GraphWidget simulates GUIslice API gslc_ElemXGraphCreate() call.
 * 
 * @author Paul Conti
 * 
 */
public class GraphWidget extends Widget {

  /**
   * Instantiates a new graph widget.
   *
   * @param x
   *          the x coordinate position
   * @param y
   *          the y coordinate position
   */
  public GraphWidget(int x, int y) {
    u = CommonUtil.getInstance();
    model = new GraphModel();
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
    GraphModel m = ((GraphModel) model);
    g2d.setColor(m.getFillColor());
    g2d.fillRect(b.x, b.y, b.width, b.height);
    g2d.setColor(m.getFrameColor());
    g2d.drawRect(b.x, b.y, b.width, b.height);
    if (m.getGraphStyle().equals("Dot")) {
      g2d.setColor(m.getGraphColor());
      Stroke defaultStroke = g2d.getStroke();
      g2d.setStroke(Widget.dotted);
      g2d.drawLine(b.x+5, 
          b.y+(b.height-(b.height/3)), 
          b.x+b.width-5, 
          b.y+(b.height-(b.height/3*2)));
      g2d.setStroke(defaultStroke);  
    } else {
      fillPolygon(g2d, m, b);
    }
    super.drawSelRect(g2d, b);
  }

  
  /**
   * Fill polygon.
   *
   * @param g2d
   *          the graphics object
   * @param m
   *          the m
   * @param r
   *          the r
   */
  private void fillPolygon(Graphics2D g2d, GraphModel m, Rectangle r) {
    int xpoints[] = new int[4];
    int ypoints[] = new int[4];

    xpoints[0] = r.x+5;
    ypoints[0] = r.y+r.height;
    xpoints[1] = r.x+5;
    ypoints[1] = r.y+(r.height-(r.height/3));
    xpoints[2] = r.x+r.width-5;
    ypoints[2] = r.y+(r.height-(r.height/3*2));
    xpoints[3] = r.x+r.width-5;
    ypoints[3] = r.y+r.height;
   
    g2d.drawPolygon(xpoints, ypoints, 4);
    g2d.setColor(m.getGraphColor());
    g2d.fillPolygon(xpoints, ypoints, 4);

  }
}
