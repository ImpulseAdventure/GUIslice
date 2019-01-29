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
package builder.models;

import java.awt.Color;
import java.io.IOException;
import java.io.ObjectInputStream;

import builder.common.ColorFactory;
import builder.common.EnumFactory;
import builder.events.MsgBoard;
import builder.events.MsgEvent;

/**
 * The Class SliderModel implements the model for the Slider widget.
 * 
 * @author Paul Conti
 * 
 */
public class SliderModel extends WidgetModel { 
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant for gslc_tsElemRef* m_pElementRef name */
  public static final String ELEMENTREF_NAME = "m_pElemSlider";
  
  /** The Constant PROP_ELEMENTREF. */
  static private final int PROP_ELEMENTREF     = 6;
  
  /** The Constant PROP_MIN. */
  static private final int PROP_MIN            =7;
  
  /** The Constant PROP_MAX. */
  static private final int PROP_MAX            =8;
  
  /** The Constant PROP_CURVALUE. */
  static private final int PROP_CURVALUE          =9;
  
  /** The Constant PROP_THUMBSZ. */
  static private final int PROP_THUMBSZ        =10;
  
  /** The Constant PROP_VERTICAL. */
  static private final int PROP_VERTICAL       =11;
  
  /** The Constant PROP_DIVISIONS. */
  static private final int PROP_DIVISIONS       =12;
  
  /** The Constant PROP_TICKSZ. */
  static private final int PROP_TICKSZ         =13;
  
  /** The Constant PROP_TICK_COLOR. */
  static private final int PROP_TICK_COLOR     =14;
  
  /** The Constant PROP_TRIM. */
  static private final int PROP_TRIM           =15;
  
  /** The Constant PROP_TRIM_COLOR. */
  static private final int PROP_TRIM_COLOR     =16;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR    =17;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR     =18;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR =19;
    
  static private final int DEF_WIDTH = 80;
  static private final int DEF_HEIGHT= 20;

  /**
   * Instantiates a new slider model.
   */
  public SliderModel() {
    cf = ColorFactory.getInstance();
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.SLIDER;
    data = new Object[20][5];
    
    initCommonProps(DEF_WIDTH, DEF_HEIGHT);
    
    initProp(PROP_ELEMENTREF, String.class, "TXT-206", Boolean.FALSE,"ElementRef","");
    initProp(PROP_MIN, Integer.class, "SLD-100", Boolean.FALSE,"Minimum Value",Integer.valueOf(0));
    initProp(PROP_MAX, Integer.class, "SLD-101", Boolean.FALSE,"Maximum Value",Integer.valueOf(255));
    initProp(PROP_CURVALUE, Integer.class, "SLD-102", Boolean.FALSE,"Starting Value",Integer.valueOf(0));
    initProp(PROP_THUMBSZ, Integer.class, "SLD-103", Boolean.FALSE,"Thumb Size",Integer.valueOf(5));
    initProp(PROP_VERTICAL, Boolean.class, "SLD-104", Boolean.FALSE,"Vertical?",Boolean.FALSE);
    initProp(PROP_DIVISIONS, Integer.class, "SLD-105", Boolean.FALSE,"Tick Divisions",Integer.valueOf(10));
    initProp(PROP_TICKSZ, Integer.class, "SLD-106", Boolean.FALSE,"Tick Size",Integer.valueOf(5));
    initProp(PROP_TRIM, Boolean.class, "SLD-107", Boolean.FALSE,"Trim Style?",Boolean.FALSE);

    initProp(PROP_TICK_COLOR, Color.class, "COL-306", Boolean.FALSE,"Tick Color",Color.BLUE);
    initProp(PROP_TRIM_COLOR, Color.class, "COL_307", Boolean.FALSE,"Trim Color",Color.BLUE);
    initProp(PROP_FRAME_COLOR, Color.class, "COL-302", Boolean.FALSE,"Frame Color",cf.getDefFrameCol());
    initProp(PROP_FILL_COLOR, Color.class, "COL-303", Boolean.FALSE,"Fill Color",cf.getDefFillCol());
    initProp(PROP_SELECTED_COLOR, Color.class, "COL-304", Boolean.FALSE,"Selected Color",cf.getDefGlowCol());
    
  }
  
  /**
   * changeValueAt
   *
   * @see builder.models.WidgetModel#changeValueAt(java.lang.Object, int)
   */
  @Override
  public void changeValueAt(Object value, int row) {
    // The test for Integer supports copy and paste from clipboard.
    // Otherwise we get a can't cast class String to Integer fault
    if ( (getClassAt(row) == Integer.class) && (value instanceof String)) {
        data[row][PROP_VAL_VALUE] = Integer.valueOf(Integer.parseInt((String)value));
    } else {
      data[row][PROP_VAL_VALUE] = value;
    }
    fireTableCellUpdated(row, COLUMN_VALUE);
    if (row == PROP_VERTICAL) {
      // swap height and width
      int tmp = getWidth();
      setWidth(getHeight());
      setHeight(tmp);
      fireTableCellUpdated(PROP_WIDTH, COLUMN_VALUE);
      fireTableCellUpdated(PROP_HEIGHT, COLUMN_VALUE);
    }
    
    if (bSendEvents) {
      event = new MsgEvent();
      event.code = MsgEvent.WIDGET_REPAINT;
      event.message = getKey();
      MsgBoard.getInstance().publish(event);
    }
  }

  /**
   * Gets the element ref.
   *
   * @return the element ref
   */
  public String getElementRef() {
    return (String) data[PROP_ELEMENTREF][PROP_VAL_VALUE];
  }
  
  /**
   * Sets the element ref.
   *
   * @param s
   *          the new element ref
   */
  public void setElementRef(String s) { 
    shortcutValue(s, PROP_ELEMENTREF);
  }
  
  /**
   * Gets the tick color.
   *
   * @return the tick color
   */
  public Color getTickColor() {
    return (((Color) data[PROP_TICK_COLOR][PROP_VAL_VALUE]));
  }

  /**
   * Gets the trim color.
   *
   * @return the trim color
   */
  public Color getTrimColor() {
    return (((Color) data[PROP_TRIM_COLOR][PROP_VAL_VALUE]));
  }

  /**
   * Checks if is vertical.
   *
   * @return true, if is vertical
   */
  public boolean isVertical() {
    return ((Boolean) data[PROP_VERTICAL][PROP_VAL_VALUE]).booleanValue();
  }

  /**
   * Checks if is trim style.
   *
   * @return true, if is trim style
   */
  public boolean isTrimStyle() {
    return ((Boolean) data[PROP_TRIM][PROP_VAL_VALUE]).booleanValue();
  }

  /**
   * Gets the min.
   *
   * @return the min
   */
  public int getMin() {
    return (((Integer) (data[PROP_MIN][PROP_VAL_VALUE])).intValue());
  }
  
  /**
   * Gets the max.
   *
   * @return the max
   */
  public int getMax() {
    return (((Integer) (data[PROP_MAX][PROP_VAL_VALUE])).intValue());
  }
  
  /**
   * Gets the value.
   *
   * @return the value
   */
  public int getCurValue() {
    return (((Integer) (data[PROP_CURVALUE][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the thumb size.
   *
   * @return the thumb size
   */
  public int getThumbSize() {
    return (((Integer) (data[PROP_THUMBSZ][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the tick size.
   *
   * @return the tick size
   */
  public int getTickSize() {
    return (((Integer) (data[PROP_TICKSZ][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the divisions.
   *
   * @return the divisions
   */
  public int getDivisions() {
    return (((Integer) (data[PROP_DIVISIONS][PROP_VAL_VALUE])).intValue());
  }
  
  /**
   * Use default colors.
   *
   * @return <code>true</code>, if successful
   */
  public boolean useDefaultColors() {
    return false;
  }
  
  /**
   * Gets the fill color.
   *
   * @return the fill color
   */
  public Color getFillColor() {
    return (((Color) data[PROP_FILL_COLOR][PROP_VAL_VALUE]));
  }

  /**
   * Gets the frame color.
   *
   * @return the frame color
   */
  public Color getFrameColor() {
    return (((Color) data[PROP_FRAME_COLOR][PROP_VAL_VALUE]));
  }

  /**
   * Gets the selected color.
   *
   * @return the selected color
   */
  public Color getSelectedColor() {
    return (((Color) data[PROP_SELECTED_COLOR][PROP_VAL_VALUE]));
  }

  /**
   * readModel() will deserialize our model's data from a string object for backup
   * and recovery.
   *
   * @param in
   *          the in stream
   * @param widgetType
   *          the widget type
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   * @throws ClassNotFoundException
   *           the class not found exception
    * @see builder.models.WidgetModel#readModel(java.io.ObjectInputStream, java.lang.String)
   */
  @Override
  public void readModel(ObjectInputStream in, String widgetType) 
      throws IOException, ClassNotFoundException {
    super.readModel(in,  widgetType);
    // upgrade to beta release where previously ElementRef was empty
    if (getElementRef().equals("")) {
      int n = 0;
      String strKey = "";
      String strCount = ""; 
      String ref = ""; 
      ref = ELEMENTREF_NAME;
      strKey = getKey();
      n = strKey.indexOf("$");
      strCount = strKey.substring(n+1, strKey.length());
      ref = ref + strCount;
      setElementRef(ref);
    }
  }
}
