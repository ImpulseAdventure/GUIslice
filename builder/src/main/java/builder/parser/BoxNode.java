/**
 *
 * The MIT License
 *
 * Copyright 2019 Paul Conti
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
package builder.parser;

import java.awt.Color;

/**
 * The Class BoxNode implements the parser node for the Box widget.
 * 
 * @author Paul Conti
 * 
 */
public class BoxNode extends ApiNode { 
  
  /** The Constant PROP_DRAW. */
  static private final int PROP_DRAW              = 5;
  
  /** The Constant PROP_TICKCB. */
  static private final int PROP_TICKCB            = 6;
  
  /** The Constant PROP_DEFAULT_COLORS. */
  static private final int PROP_DEFAULT_COLORS    = 7;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR       = 8;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR        = 9;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR    = 10;
  
  /**
   * Instantiates a new box node.
   */
  public BoxNode() {
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    apiType = ApiNode.BOX;
    widgetType = "Box";
    data = new Object[11][3];
    
    initCommonProps();

    initProp(PROP_DRAW, "BOX-100","Draw Function");
    initProp(PROP_TICKCB, "BOX-101","Tick Function");

    initProp(PROP_DEFAULT_COLORS, "COL-300", "Use Default Colors?");
    initProp(PROP_FRAME_COLOR, "COL-302", "Frame Color");
    initProp(PROP_FILL_COLOR, "COL-303", "Fill Color");
    initProp(PROP_SELECTED_COLOR, "COL-304", "Selected Color");

  }

  /**
  * sets the useage of draw callback.
  *
  * @param true uses draw callback, false otherwise
  */
  public void setDrawFunct(String value) 
  {
    setValue(Boolean.valueOf(value), PROP_DRAW);
  }

  /**
  * sets the usage of tick callback.
  *
  * @param true uses tick callback, false otherwise
  */
  public void setTickFunct(String value) 
  {
    setValue(Boolean.valueOf(value), PROP_TICKCB);
  }
 
  /**
  * sets the default color.
  *
  * @param true uses default colors, false otherwise
  */
  public void setDefaultColors(String value) 
  {
    setValue(Boolean.valueOf(value), PROP_DEFAULT_COLORS);
  }
  
  /**
  * sets the fill color.
  *
  * @param the fill color
  */
  public void setFillColor(Color c) {
    setValue(c, PROP_FILL_COLOR);
  }

  /**
  * sets the frame color.
  *
  * @param the frame color
  */
  public void setFrameColor(Color c) {
    setValue(c, PROP_FRAME_COLOR);
  }

  /**
  * sets the selected color.
  *
  * @param the selected color
  */
  public void setSelectedColor(Color c) {
    setValue(c, PROP_SELECTED_COLOR);
  }

}
