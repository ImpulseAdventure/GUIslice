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
import java.lang.NumberFormatException;

/**
 * The Class SliderNode implements the parser node for the Slider widget.
 * 
 * @author Paul Conti
 * 
 */
public class SliderNode extends ApiNode { 
  
  /** The Constant PROP_ELEMENTREF. */
  static private final int PROP_ELEMENTREF     = 5;
  
  /** The Constant PROP_MIN. */
  static private final int PROP_MIN            =6;
  
  /** The Constant PROP_MAX. */
  static private final int PROP_MAX            =7;
  
  /** The Constant PROP_VALUE. */
  static private final int PROP_CURVALUE       =8;
  
  /** The Constant PROP_THUMBSZ. */
  static private final int PROP_THUMBSZ        =9;
  
  /** The Constant PROP_VERTICAL. */
  static private final int PROP_VERTICAL       =10;
  
  /** The Constant PROP_DIVISIONS. */
  static private final int PROP_DIVISIONS      =11;
  
  /** The Constant PROP_TICKSZ. */
  static private final int PROP_TICKSZ         =12;
  
  /** The Constant PROP_TICK_COLOR. */
  static private final int PROP_TICK_COLOR     =13;
  
  /** The Constant PROP_TRIM. */
  static private final int PROP_TRIM           =14;
  
  /** The Constant PROP_TRIM_COLOR. */
  static private final int PROP_TRIM_COLOR     =15;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR    =16;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR     =17;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR =18;
    
  /**
   * Instantiates a new slider node.
   */
  public SliderNode() {
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    apiType = ApiNode.SLIDER;
    widgetType = "Slider";
    data = new Object[19][3];
    
    initCommonProps();
    
    initProp(PROP_ELEMENTREF, "TXT-206", "ElementRef");
    initProp(PROP_MIN, "SLD-100", "Minimum Value");
    initProp(PROP_MAX, "SLD-101", "Maximum Value");
    initProp(PROP_CURVALUE, "SLD-102", "Starting Value");
    initProp(PROP_THUMBSZ, "SLD-103", "Thumb Size");
    initProp(PROP_VERTICAL, "SLD-104", "Vertical?");
    initProp(PROP_DIVISIONS, "SLD-105", "Tick Divisions");
    initProp(PROP_TICKSZ, "SLD-106", "Tick Size");
    initProp(PROP_TRIM, "SLD-107", "Trim Style?");

    initProp(PROP_TICK_COLOR, "COL-306", "Tick Color");
    initProp(PROP_TRIM_COLOR, "COL_307", "Trim Color");
    initProp(PROP_FRAME_COLOR, "COL-302", "Frame Color");
    initProp(PROP_FILL_COLOR, "COL-303", "Fill Color");
    initProp(PROP_SELECTED_COLOR, "COL-304", "Selected Color");
    
  }
  
  /**
   * Sets the element ref.
   *
   * @param s
   *          the new element ref
   */
  public void setElementRef(String s) { 
    setValue(s, PROP_ELEMENTREF);
  }
  
  /**
   * sets the min.
   *
   * @param the min
   */
  public void setMin(String v) throws NumberFormatException {
    setValue(Integer.valueOf(v), PROP_MIN);
  }
  
  /**
   * sets the max.
   *
   * @param the max
   */
  public void setMax(String v) throws NumberFormatException {
    setValue(Integer.valueOf(v), PROP_MAX);
  }
  
  /**
   * sets the value.
   *
   * @param the value
   */
  public void setCurValue(String v) throws NumberFormatException {
    setValue(Integer.valueOf(v), PROP_CURVALUE);
  }

  /**
   * sets the tick color.
   *
   * @param the tick color
   */
  public void setTickColor(Color c) {
    setValue(c, PROP_TICK_COLOR);
  }

  /**
   * sets the trim color.
   *
   * @param the trim color
   */
  public void setTrimColor(Color c) {
    setValue(c, PROP_TRIM_COLOR);
  }

  /**
  * sets the slider vertical
  *
  * @param true, if vertical, false otherwise
  */
  public void setVertical(String value) 
  {
    setValue(Boolean.valueOf(value), PROP_VERTICAL);
  }
  
  /**
  * sets the trim style.
  *
  * @param true, if use trim style, false otherwise
  */
  public void setTrimStyle(String value) 
  {
    setValue(Boolean.valueOf(value), PROP_TRIM);
  }
  
  /**
   * sets the thumb size.
   *
   * @param the thumb size
   */
  public void setThumbSize(String v) throws NumberFormatException {
    setValue(Integer.valueOf(v), PROP_THUMBSZ);
  }

  /**
   * sets the tick size.
   *
   * @param the tick size
   */
  public void setTickSize(String v) throws NumberFormatException {
    setValue(Integer.valueOf(v), PROP_TICKSZ);
  }

  /**
   * sets the divisions.
   *
   * @param the divisions
   */
  public void setDivisions(String v) throws NumberFormatException {
    setValue(Integer.valueOf(v), PROP_DIVISIONS);
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
