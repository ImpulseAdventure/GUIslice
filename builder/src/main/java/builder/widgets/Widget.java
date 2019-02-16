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

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Stroke;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import builder.common.CommonUtil;
import builder.common.FontFactory;
import builder.models.WidgetModel;

/**
 * The Class Widget is the base class for simulating GUIslice API calls.
 * 
 * @author Paul Conti
 * 
 */
public class Widget {
  
  /** The Constant dashed. */
  final static public  BasicStroke dashed = new BasicStroke(3.0f, 
      BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 5.0f, new float[]{5.0f}, 0);
  
  /** The Constant dotted. */
  final static public  BasicStroke dotted = new BasicStroke(1.0f, 
      BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL, 0, new float[] {1.0f,2.0f}, 0);

  /** The u. */
  CommonUtil u;
  
  /** The ff. */
  FontFactory ff;
  
  /** The model. */
  WidgetModel model;
  
  /** The b selected. */
  boolean bSelected = false;

  /**
   * Sets the user prefs.
   *
   * @param peModel
   *          the new user prefs
   */
  /*
   * setUserPrefs from PrefsEditor stored model
   */
  public void setUserPrefs(WidgetModel peModel) {
     // merge user preference into our model
     // avoid x,y
    model.TurnOffEvents();
    int rows = model.getRowCount();
    for (int r=WidgetModel.PROP_Y+1; r<rows; r++) {
      Object o = peModel.getValueAt(r, 1);
      model.changeValueAt(o, r);
    }
    model.TurnOnEvents();
  }

  /**
   * Gets the model.
   *
   * @return the model
   */
  public WidgetModel getModel() {
    return model;
  }
  
  /**
   * Gets the key.
   *
   * @return the key
   */
  public String getKey() {
    return model.getKey();
  }
  
  /**
   * Gets the enum.
   *
   * @return the enum
   */
  public String getEnum() {
    return model.getEnum();
  }
  
  /**
   * Gets the type.
   *
   * @return the type
   */
  public String getType() {
    return model.getType();
  }
  
  /**
   * Return this widget's location.
   *
   * @return the location <code>Point</code> object
   */
  public Point getLocation() {
    return new Point(model.getX(), model.getY());
  }
  
  /**
   * Gets the win bounded.
   *
   * @return the window bounded <code>Rectangle</code> object
   */
  public Rectangle getWinBounded() {
    int dx = u.toWinX(model.getX());
    int dy = u.toWinY(model.getY());
    Rectangle b = new Rectangle();
    b.x = dx;
    b.y = dy;
    b.width = model.getWidth();
    b.height = model.getHeight();
    return b;
  }
  
  /**
   * Gets the bounded rectangle
   *
   * @return the <code>Rectangle</code> object
   */
  public Rectangle getBounded() {
    Rectangle b = new Rectangle();
    b.x = model.getX();
    b.y = model.getY();
    b.width = model.getWidth();
    b.height = model.getHeight();
    return b;
  }
  
  /**
   * updateLocation() will adjust our location to make sure it fits to our grid
   * margins.
   *
   * @param d
   *          the d
   * @return the <code>point</code> object
   */
  public Point updateLocation(Point d) {
    int x = model.getX() + d.x;
    int y = model.getY() + d.y;
    Point p = CommonUtil.getInstance().fitToGrid(x, y, model.getWidth(), model.getHeight());
    model.setX(p.x);
    model.setY(p.y);
    return p;
  }
  
  /**
   * Move by.
   *
   * @param d
   *          the d
   */
  public void moveBy(Point d) {
    model.setX(d.x);
    model.setY(d.y);
//    System.out.println("moveBy: " + d.toString());
  }

  /**
   * drop() will adjust our drop point to make sure it snaps to our grid
   * if thats turned on.
   * @param d is our widget drop point on the screen.
   * @return a possibly different point that fits on our screen.
   */
  public Point drop(Point d) {
    d = CommonUtil.getInstance().snapToGrid(d.x, d.y);
    return d;
  }

  /**
   * Return true if this node contains p.
   *
   * @param p
   *          the <code>Point</code> object
   * @return <code>true</code>, if successful
   */
  public boolean contains(Point p) {
    Rectangle b = new Rectangle();
    b.x = model.getX()-2;
    b.y = model.getY()-2;
    b.width = model.getWidth()+4;
    b.height = model.getHeight()+4;
    return b.contains(p);
  }

  /**
   * Return true if this node contains p.
   *
   * @param p
   *          the <code>Point</code> object
   * @return <code>true</code>, if successful
   */
  public boolean contains(Point2D p) {
    Rectangle2D.Double b = new Rectangle2D.Double();
    b.x = model.getX()-2;
    b.y = model.getY()-2;
    b.width = model.getWidth()+4;
    b.height = model.getHeight()+4;
    return b.contains(p);
  }

  /**
   * Return true if this node is selected.
   *
   * @return true, if is selected
   */
  public boolean isSelected() {
    return bSelected;
  }

  /**
   * Mark this node as selected.
   */
  public void select() {
    this.bSelected = true;
  }

  /**
   * Mark this node as not selected.
   */
  public void unSelect() {
    this.bSelected = false;
  }

  /**
   * Draw.
   *
   * @param g2d
   *          the graphics object
   */
  public void draw(Graphics2D g2d) {
    
  }
  
  /**
   * Draw sel rect.
   *
   * @param g2d
   *          the graphics object
   * @param b
   *          the bounded <code>Rectangle</code> object
   */
  public void drawSelRect(Graphics2D g2d, Rectangle b) {
    if (bSelected) {
      Stroke defaultStroke = g2d.getStroke();
      g2d.setColor(Color.RED);
      g2d.setStroke(Widget.dashed);
      g2d.drawRect(b.x-2, b.y-2, b.width+4, b.height+4);
      g2d.setStroke(defaultStroke);  
    }
  }
  
  /**
   * Write object serializes this object
   *
   * @param out
   *          the out
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  public void writeObject(ObjectOutputStream out) 
      throws IOException {
//    System.out.println("W writeObject(): " + getType());
    out.writeObject(model.getType());
    out.writeBoolean(bSelected);
    model.writeModel(out);
  }

  /**
   * Read object deserializes this object
   *
   * @param in
   *          the in
   * @param widgetType
   *          the widget type
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   * @throws ClassNotFoundException
   *           the class not found exception
   */
  public void readObject(ObjectInputStream in, String widgetType) 
      throws IOException, ClassNotFoundException {
//    System.out.println("W readObject(): " + widgetType);
    bSelected = in.readBoolean();
    model.readModel(in, widgetType);
  }
}
