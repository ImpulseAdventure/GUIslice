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
package builder.common;

import java.awt.Dimension;
import java.awt.Point;
import java.awt.geom.Point2D;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

import javax.imageio.ImageIO;

import builder.prefs.GeneralEditor;
import builder.prefs.GridEditor;

import java.util.Base64;

/**
 * The Class CommonUtil is a catch all for useful routines that don't seem to fit
 * in any other classes.
 * 
 * @author Paul Conti
 * 
 */
public class CommonUtil {
  
  /** The instance. */
  private static CommonUtil instance = null;
  
  /** The offset x. */
  private static int offset_x; // Window offset of coordinate system along x axis
  
  /** The offset y. */
  private static int offset_y; // Window offset of coordinate system along y axis
  
  /**
   * getInstance() - get our Singleton Object.
   *
   * @return instance
   */
  public static synchronized CommonUtil getInstance() {
    if (instance == null) {
      instance = new CommonUtil();
    }
    return instance;
  }

  /**
   * empty constructor.
   */
  public CommonUtil() {
  }

  /**
   * setWinOffsets() - sets offset_x and offset_y for future calls.
   *
   * @param panel
   *          the panel dimensions
   * @param canvasWidth
   *          the grid width
   * @param canvasHeight
   *          the grid height
   */
  public void setWinOffsets(Dimension panel, int canvasWidth, int canvasHeight) {
    offset_x = (panel.width - canvasWidth) / 2;
    offset_y = (panel.height - canvasHeight) / 2;
  }
  
  /**
   * toWinX() maps absolute x coordinate to window x coordinate. Needed because
   * our design canvas doesn't sit at x=0 and y=0 instead its centered in our
   * panel.
   *
   * @param x
   *          the x
   * @return x + offset_x
   */
  public int toWinX(int x) {
    return x + offset_x;
  }
  
  /**
   * toWinY() maps absolute y coordinate to window y coordinate. Needed because
   * our design canvas doesn't sit at x=0 and y=0 instead its centered in our
   * panel.
   *
   * @param y
   *          the y
   * @return y + offset_y
   */
  public int toWinY(int y) {
    return y + offset_y;
  }
  
  /**
   * toWinPoint() does in one call what calling both toWinX() and toWinY()
   * achieves.
   *
   * @param p
   *          the p
   * @return the <code>point</code> object
   */
  public Point toWinPoint(Point p) {
    return new Point(p.x+offset_x, p.y+offset_y);
  }
  
  /**
   * fromWinX() - reverse mapping from window x coordinate to absolute x
   * coordinate.
   *
   * @param x
   *          the x
   * @return x - offset_x
   */
  public int fromWinX(int x) {
    return x - offset_x;
  }
  
  /**
   * fromWinY() - reverse mapping from window y coordinate to absolute y
   * coordinate.
   *
   * @param y
   *          the y
   * @return y - offset_y
   */
  public int fromWinY(int y) {
    return y - offset_y;
  }
  
  /**
   * fromWinPoint() - reverse mapping from window x+y coordinates to absolute x+y
   * coordinates.
   *
   * @param p
   *          the point to be mapped into user space
   * @return p remapped
   */
  public Point2D.Double fromWinPoint(Point2D.Double p) {
    return new Point2D.Double(p.x-(double)offset_x, p.y-(double)offset_y);
  }
 
  /**
   * fitToGrid() - Remaps coordinates to fit on our design canvas and stay inside
   * the margins. Doesn't handle widgets if they are too large to fit.
   *
   * @param x
   *          the x
   * @param y
   *          the y
   * @param widgetWidth
   *          the widget width
   * @param widgetHeight
   *          the widget height
   * @return p
   */
  public Point fitToGrid(int x, int y, int widgetWidth, int widgetHeight) {
    GeneralEditor ed = GeneralEditor.getInstance();
    int margins = ed.getMargins();
    int canvas_width = ed.getWidth() - margins;
    int canvas_height = ed.getHeight() - margins;
    // force the new Coordinates fit on our canvas and inside the margins
    if ((x + widgetWidth) > canvas_width)
      x = canvas_width - widgetWidth;
    if (x < margins)
      x = margins;
    if ((y + widgetHeight) > canvas_height)
      y = canvas_height - widgetHeight;
    if (y < margins)
      y = margins;
    Point p = new Point(x, y);
    return p;
  }
  
  /**
   * snapToGrid() - Remaps coordinates to our nearest grid line.
   *
   * @param x
   *          the x
   * @param y
   *          the y
   * @return p
   */
  public Point snapToGrid(int x, int y) {
    GridEditor ed = GridEditor.getInstance();
    // check for snap to grid
    if (ed.getGridSnapTo()) {
      x = (x / ed.getGridMinorWidth()) * ed.getGridMinorWidth();
      y = (y / ed.getGridMinorHeight()) * ed.getGridMinorHeight();
    }
    Point p = new Point(x, y);
    return p;
  }
  
  /**
   * encodeToString() - .
   *
   * @param image
   *          the image
   * @return imageString
   */
  public String encodeToString(BufferedImage image) {
    String imageString = null;
    ByteArrayOutputStream bos = new ByteArrayOutputStream();

    try {
      ImageIO.write(image, "bmp", bos);
      byte[] imageBytes = bos.toByteArray();

      imageString = Base64.getEncoder().encodeToString(imageBytes);

      bos.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
    return imageString;
  }

  /**
   * decodeToImage() - .
   *
   * @param imageString
   *          the image string
   * @return image
   */
  public BufferedImage decodeToImage(String imageString) {

    BufferedImage image = null;
    byte[] imageByte;
    try {
      imageByte = Base64.getDecoder().decode(imageString);
      ByteArrayInputStream bis = new ByteArrayInputStream(imageByte);
      image = ImageIO.read(bis);
      bis.close();
    } catch (Exception e) {
      e.printStackTrace();
    }
    return image;
  }

  /**
   * getWorkingDir - attempts to find the directory where our executable is
   * running.
   *
   * @return workingDir - our working directory
   */
  public String getWorkingDir() {
    // The code checking for "bin" is to take care of the case 
    // where we are running not inside eclipse IDE
    String workingDir;
    String strUserDir = System.getProperty("user.dir");
    int n = strUserDir.indexOf("bin");
    if (n > 0) {
      strUserDir = strUserDir.substring(0,n-1);  // remove "/bin"
    }
    workingDir = strUserDir + System.getProperty("file.separator"); 
    return workingDir;
  }
}
