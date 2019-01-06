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

import java.awt.Color;

/**
 * The Class ColorItem is a representation of a single GUIslice Library color.
 * 
 * @author Paul Conti
 * 
 */
public class ColorItem {
  
  /** The java color object. */
  private Color  color;
  
  /** The red value. */
  private int red;
  
  /** The green value. */
  private int green;
  
  /** The blue value. */
  private int blue;
  
  /** The GUIslice Library name. */
  private String displayName;
  
  /** The index position of this color within our colorList. */
  private int index;
  
  /**
   * Instantiates a new color item.
   *
   * @param index
   *          the index
   * @param displayName
   *          the display name
   */
  public ColorItem(int index, String displayName) {
    this.color = null;
    this.displayName = displayName;
    this.red = 0;
    this.green = 0;
    this.blue = 0;
    this.index = index;
  }

  /**
   * Instantiates a new color item.
   *
   * @param index
   *          the index
   * @param displayName
   *          the display name
   * @param red
   *          the red
   * @param green
   *          the green
   * @param blue
   *          the blue
   */
  public ColorItem(int index, String displayName, int red, int green, int blue) {
    this.color = new Color(red,green,blue);
    this.displayName = displayName;
    this.red = red;
    this.green = green;
    this.blue = blue;
    this.index = index;
  }
  
  /**
   * Gets the color.
   *
   * @return the <code>Color</code> object
   */
  public Color getColor() {
    return color;
  }
  
  /**
   * Gets the red.
   *
   * @return the red value
   */
  public int getRed() {
    return red;
  }
  
  /**
   * Gets the green.
   *
   * @return the green value
   */
  public int getGreen() {
    return green;
  }
  
  /**
   * Gets the blue.
   *
   * @return the blue value
   */
  public int getBlue() {
    return blue;
  }
  
  /**
   * Gets the display name.
   *
   * @return the display name
   */
  public String getDisplayName() {
    return displayName;
  }
  
  /**
   * Gets the index.
   *
   * @return the index position
   */
  public int getIndex() {
    return index;
  }
  
}
