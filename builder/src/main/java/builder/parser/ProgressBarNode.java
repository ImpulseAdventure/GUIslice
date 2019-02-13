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
 * The Class ProgressBarNode implements the parser node for the Progress Bar widget.
 * 
 * @author Paul Conti
 * 
 */
public class ProgressBarNode extends ApiNode { 
  
  /** The Constant PROP_ELEMENTREF. */
  static private final int PROP_ELEMENTREF        =5;
  
  /** The Constant PROP_VERTICAL. */
  static private final int PROP_VERTICAL          =6;
  
  /** The Constant PROP_STYLE. */
  static private final int PROP_STYLE             =7;
  
  /** The Constant PROP_MIN. */
  static private final int PROP_MIN               =8;
  
  /** The Constant PROP_MAX. */
  static private final int PROP_MAX               =9;
  
  /** The Constant PROP_VALUE. */
  static private final int PROP_CURVALUE          =10;
  
  /** The Constant PROP_DIVISIONS. */
  static private final int PROP_DIVISIONS         =11;
  
  /** The Constant PROP_TICKSZ. */
  static private final int PROP_TICKSZ            =12;
  
  /** The Constant PROP_TICK_COLOR. */
  static private final int PROP_TICK_COLOR        =13;
  
  /** The Constant PROP_INDICATOR_SIZE. */
  static private final int PROP_INDICATOR_SZ      =14;
  
  /** The Constant PROP_INDICATOR_SIZE. */
  static private final int PROP_INDICATOR_TIP_SZ  =15;

  /** The Constant PROP_INDICATOR_FILL. */
  static private final int PROP_INDICATOR_FILL    =16;
  
  /** The Constant PROP_GAUGE_COLOR. */
  static private final int PROP_GAUGE_COLOR       =17;
  
  /** The Constant PROP_DEFAULT_COLORS. */
  static private final int PROP_DEFAULT_COLORS    =18;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR       =19;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR        =20;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR    =21;
    
  /**
   * Instantiates a new progress bar node.
   */
  public ProgressBarNode() {
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    apiType = ApiNode.PROGRESSBAR;
    widgetType = "ProgressBar";
    data = new Object[22][3];
    
    initCommonProps();
    
    initProp(PROP_ELEMENTREF, "TXT-206","ElementRef");
    initProp(PROP_VERTICAL, "BAR-100","Vertical?");
    initProp(PROP_STYLE, "BAR-105", "Graph Style");

    initProp(PROP_MIN, "BAR-102","Minimum Value");
    initProp(PROP_MAX, "BAR-103","Maximum Value");
    initProp(PROP_CURVALUE, "BAR-104", "Starting Value");

    initProp(PROP_DIVISIONS, "BAR-106", "Tick Divisions");
    initProp(PROP_TICKSZ, "BAR-107", "Tick Size");
    initProp(PROP_TICK_COLOR, "BAR-108","Tick Color");
    initProp(PROP_INDICATOR_SZ, "BAR-109", "Indicator Length");
    initProp(PROP_INDICATOR_TIP_SZ, "BAR-110", "Indicator Tip Size");
    initProp(PROP_INDICATOR_FILL, "BAR-111", "Indicator Fill?");

    initProp(PROP_GAUGE_COLOR, "COL_308", "Gauge Indicator Color");
    initProp(PROP_DEFAULT_COLORS, "COL-300", "Use Default Colors?");
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
   * sets the divisions.
   *
   * @param the divisions
   */
  public void setDivisions(String string) {
    setValue(Integer.valueOf(string),PROP_DIVISIONS);
  }
  
  /**
   * sets the tick size.
   *
   * @param the tick size
   */
  public void setTickSize(String v) {
    setValue(Integer.valueOf(v),PROP_TICKSZ);
  }

  /**
   * sets the tick color.
   *
   * @return the tick color
   */
  public void setTickColor(Color c) {
    setValue(c, PROP_TICK_COLOR);
  }

  /**
   * sets the indicator size.
   *
   * @param the indicator size
   */
  public void setIndicatorSize(String v) {
    setValue(Integer.valueOf(v),PROP_INDICATOR_SZ);
  }

  /**
   * sets the indicator tip size.
   *
   * @param the indicator tip size
   */
  public void setIndicatorTipSize(String v) {
    setValue(Integer.valueOf(v),PROP_INDICATOR_TIP_SZ);
  }

  /**
   * sets if is indicator fill.
   *
   * @param true, if is indicator fill
   */
  public void setIndicatorFill(String s) {
    setValue(Boolean.valueOf(s), PROP_INDICATOR_FILL);
  }

  /**
   * sets the indicator color.
   *
   * @param the indicator color
   */
  public void setIndicatorColor(Color c) {
    setValue(c, PROP_GAUGE_COLOR);
  }

  /**
  * sets the progress bar vertical
  *
  * @param true, if vertical, false otherwise
  */
  public void setVertical(String value) 
  {
    setValue(Boolean.valueOf(value), PROP_VERTICAL);
  }
  
  /**
   * sets the Gauge style.
   *
   * @param the Gauge style
   */
  public void setGaugeStyle(String s) {
    setValue(s, PROP_STYLE);
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
