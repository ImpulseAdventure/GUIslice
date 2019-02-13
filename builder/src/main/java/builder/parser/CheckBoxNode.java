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
 * The Class GaugeNode implements the parser node for either the Checkbox or RadioButton widget.
 * 
 * @author Paul Conti
 * 
 */
public class CheckBoxNode extends ApiNode { 
  
  /** The Constant PROP_CHECKED. */
  static private final int PROP_CHECKED        = 5;
  
  /** The Constant PROP_MARK_COLOR. */
  static private final int PROP_MARK_COLOR     = 6;
  
  /** The Constant PROP_DEFAULT_COLORS. */
  static private final int PROP_DEFAULT_COLORS = 7;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR    = 8;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR     = 9;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR = 10;
  
  /** The Constant PROP_GROUP. */
  static public  final int PROP_GROUP          = 11;

  /**
   * Instantiates a new check box node.
   */
  public CheckBoxNode() {
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    data = new Object[12][3];
    
    initCommonProps();
    
    initProp(PROP_CHECKED, "CBOX-100", "Checked?");
    initProp(PROP_MARK_COLOR, "COL-305", "Check Mark Color");

    initProp(PROP_DEFAULT_COLORS, "COL-300", "Use Default Colors?");
    initProp(PROP_FRAME_COLOR, "COL-302", "Frame Color");
    initProp(PROP_FILL_COLOR, "COL-303", "Fill Color");
    initProp(PROP_SELECTED_COLOR, "COL-304", "Selected Color");

    initProp(PROP_GROUP, "CBOX-101", "Group ID");

  }

  /**
  * sets the check mark on/off.
  *
  * @param true for on, false off
  */
  public void setChecked(String value) 
  {
    setValue(Boolean.valueOf(value), PROP_CHECKED);
  }
 
  /**
  * sets the mark color.
  *
  * @param the mark color
  */
  public void setMarkColor(Color c) {
    setValue(c, PROP_MARK_COLOR);
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

  /**
   * Sets the group id.
   *
   * @param g
   *          the group id
   */
  public void setGroup(int g) {
    setValue(Integer.valueOf(g), PROP_GROUP);
  }
  
}
