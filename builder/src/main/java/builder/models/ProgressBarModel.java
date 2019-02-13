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

import javax.swing.DefaultCellEditor;
import javax.swing.JComboBox;
import javax.swing.table.TableCellEditor;

import builder.common.ColorFactory;
import builder.common.EnumFactory;
import builder.events.MsgBoard;
import builder.events.MsgEvent;

// TODO: Auto-generated Javadoc
/**
 * The Class ProgressBarModel implements the model for the Progress Bar widget.
 * 
 * @author Paul Conti
 * 
 */
public class ProgressBarModel extends WidgetModel { 
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant for gslc_tsElemRef* m_pElementRef name. */
  public static final String ELEMENTREF_NAME = "m_pElemProgress";
  
  /** The Constant PROP_ELEMENTREF. */
  static private final int PROP_ELEMENTREF        = 6;
  
  /** The Constant PROP_VERTICAL. */
  static private final int PROP_VERTICAL          =7;
  
  /** The Constant PROP_STYLE. */
  static private final int PROP_STYLE             =8;
  
  /** The Constant PROP_MIN. */
  static private final int PROP_MIN               =9;
  
  /** The Constant PROP_MAX. */
  static private final int PROP_MAX               =10;
  
  /** The Constant PROP_CURVALUE. */
  static private final int PROP_CURVALUE          =11;
  
  /** The Constant PROP_DIVISIONS. */
  static private final int PROP_DIVISIONS         =12;
  
  /** The Constant PROP_TICKSZ. */
  static private final int PROP_TICKSZ            =13;
  
  /** The Constant PROP_TICK_COLOR. */
  static private final int PROP_TICK_COLOR        =14;
  
  /** The Constant PROP_INDICATOR_SIZE. */
  static private final int PROP_INDICATOR_SZ      =15;
  
  /** The Constant PROP_INDICATOR_SIZE. */
  static private final int PROP_INDICATOR_TIP_SZ  =16;

  /** The Constant PROP_INDICATOR_FILL. */
  static private final int PROP_INDICATOR_FILL    =17;
  
  /** The Constant PROP_GAUGE_COLOR. */
  static private final int PROP_GAUGE_COLOR       =18;
  
  /** The Constant PROP_DEFAULT_COLORS. */
  static private final int PROP_DEFAULT_COLORS    =19;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR       =20;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR        =21;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR    =22;
    
  /** The cb style. */
  JComboBox<String> cbStyle;
  
  /** The style cell editor. */
  DefaultCellEditor styleCellEditor;

  /** The Constant DEF_WIDTH. */
  static private final int DEF_WIDTH = 50;
  
  /** The Constant DEF_HEIGHT. */
  static private final int DEF_HEIGHT= 10;
  
  /**
   * Instantiates a new progress bar model.
   */
  public ProgressBarModel() {
    cf = ColorFactory.getInstance();
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.PROGRESSBAR;
    data = new Object[23][5];
    
    initCommonProps(DEF_WIDTH, DEF_HEIGHT);
    
    initProp(PROP_ELEMENTREF, String.class, "TXT-206", Boolean.FALSE,"ElementRef","");
    initProp(PROP_VERTICAL, Boolean.class, "BAR-100", Boolean.FALSE,"Vertical?",Boolean.FALSE);
    initProp(PROP_STYLE, String.class, "BAR-105", Boolean.FALSE,"Graph Style","Bar");

    initProp(PROP_MIN, Integer.class, "BAR-102", Boolean.FALSE,"Minimum Value",Integer.valueOf(0));
    initProp(PROP_MAX, Integer.class, "BAR-103", Boolean.FALSE,"Maximum Value",Integer.valueOf(100));
    initProp(PROP_CURVALUE, Integer.class, "BAR-104", Boolean.FALSE,"Starting Value",Integer.valueOf(0));

    initProp(PROP_DIVISIONS, Integer.class, "BAR-106", Boolean.TRUE,"Tick Divisions",Integer.valueOf(8));
    initProp(PROP_TICKSZ, Integer.class, "BAR-107", Boolean.TRUE,"Tick Size",Integer.valueOf(5));
    initProp(PROP_TICK_COLOR, Color.class, "BAR-108", Boolean.TRUE,"Tick Color",Color.GRAY);
    initProp(PROP_INDICATOR_SZ, Integer.class, "BAR-109", Boolean.TRUE,"Indicator Length",Integer.valueOf(10));
    initProp(PROP_INDICATOR_TIP_SZ, Integer.class, "BAR-110", Boolean.TRUE,"Indicator Tip Size",Integer.valueOf(3));
    initProp(PROP_INDICATOR_FILL, Boolean.class, "BAR-111", Boolean.TRUE,"Indicator Fill?",Boolean.FALSE);

    initProp(PROP_GAUGE_COLOR, Color.class, "COL-308", Boolean.FALSE,"Gauge Indicator Color",Color.GREEN);
    initProp(PROP_DEFAULT_COLORS, Boolean.class, "COL-300", Boolean.FALSE,"Use Default Colors?",Boolean.TRUE);
    initProp(PROP_FRAME_COLOR, Color.class, "COL-302", Boolean.TRUE,"Frame Color",cf.getDefFrameCol());
    initProp(PROP_FILL_COLOR, Color.class, "COL-303", Boolean.TRUE,"Fill Color",cf.getDefFillCol());
    initProp(PROP_SELECTED_COLOR, Color.class, "COL-304", Boolean.TRUE,"Selected Color",cf.getDefGlowCol());

    cbStyle = new JComboBox<String>();
    cbStyle.addItem("Bar");
    cbStyle.addItem("Radial");
    cbStyle.addItem("Ramp");
    styleCellEditor = new DefaultCellEditor(cbStyle);

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
   * Gets the divisions.
   *
   * @return the divisions
   */
  public int getDivisions() {
    return (((Integer) (data[PROP_DIVISIONS][PROP_VAL_VALUE])).intValue());
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
   * Gets the tick color.
   *
   * @return the tick color
   */
  public Color getTickColor() {
    return (((Color) data[PROP_TICK_COLOR][PROP_VAL_VALUE]));
  }

  /**
   * Gets the indicator size.
   *
   * @return the indicator size
   */
  public int getIndicatorSize() {
    return (((Integer) (data[PROP_INDICATOR_SZ][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the indicator tip size.
   *
   * @return the indicator tip size
   */
  public int getIndicatorTipSize() {
    return (((Integer) (data[PROP_INDICATOR_TIP_SZ][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Checks if is indicator fill.
   *
   * @return true, if is indicator fill
   */
  public boolean isIndicatorFill() {
    return ((Boolean) data[PROP_INDICATOR_FILL][PROP_VAL_VALUE]).booleanValue();
  }

  /**
   * Gets the indicator color.
   *
   * @return the indicator color
   */
  public Color getIndicatorColor() {
    return (((Color) data[PROP_GAUGE_COLOR][PROP_VAL_VALUE]));
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
   * Gets the Gauge style.
   *
   * @return the Gauge style
   */
  public String getGaugeStyle() {
    return (String) data[PROP_STYLE][PROP_VAL_VALUE];
  }
  
  /**
   * getEditorAt
   *
   * @see builder.models.WidgetModel#getEditorAt(int)
   */
  @Override
  public TableCellEditor getEditorAt(int rowIndex) {
    if (rowIndex == PROP_STYLE)
      return styleCellEditor;
    return null;
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
   * Use default colors.
   *
   * @return <code>true</code>, if successful
   */
  public boolean useDefaultColors() {
    return ((Boolean) data[PROP_DEFAULT_COLORS][PROP_VAL_VALUE]).booleanValue();
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
   * changeValueAt.
   *
   * @param value
   *          the value
   * @param row
   *          the row
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
    if (row == PROP_STYLE) {
      if (getGaugeStyle().equals("Radial")) {
        data[PROP_DIVISIONS][PROP_VAL_READONLY]=Boolean.FALSE; 
        data[PROP_TICKSZ][PROP_VAL_READONLY]=Boolean.FALSE;
        data[PROP_TICK_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
        data[PROP_INDICATOR_SZ][PROP_VAL_READONLY]=Boolean.FALSE; 
        data[PROP_INDICATOR_TIP_SZ][PROP_VAL_READONLY]=Boolean.FALSE;
        data[PROP_INDICATOR_FILL][PROP_VAL_READONLY]=Boolean.FALSE; 
        data[PROP_VERTICAL][PROP_VAL_READONLY]=Boolean.TRUE; 
      } else {
        data[PROP_DIVISIONS][PROP_VAL_READONLY]=Boolean.TRUE; 
        data[PROP_TICKSZ][PROP_VAL_READONLY]=Boolean.TRUE;
        data[PROP_TICK_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
        data[PROP_INDICATOR_SZ][PROP_VAL_READONLY]=Boolean.TRUE; 
        data[PROP_INDICATOR_TIP_SZ][PROP_VAL_READONLY]=Boolean.TRUE;
        data[PROP_INDICATOR_FILL][PROP_VAL_READONLY]=Boolean.TRUE; 
        data[PROP_VERTICAL][PROP_VAL_READONLY]=Boolean.FALSE; 
      }
      fireTableCellUpdated(PROP_DIVISIONS, COLUMN_VALUE);
      fireTableCellUpdated(PROP_TICKSZ, COLUMN_VALUE);
      fireTableCellUpdated(PROP_TICK_COLOR, COLUMN_VALUE);
      fireTableCellUpdated(PROP_INDICATOR_SZ, COLUMN_VALUE);
      fireTableCellUpdated(PROP_INDICATOR_TIP_SZ, COLUMN_VALUE);
      fireTableCellUpdated(PROP_INDICATOR_FILL, COLUMN_VALUE);
      fireTableCellUpdated(PROP_VERTICAL, COLUMN_VALUE);
    }
    if (row == PROP_DEFAULT_COLORS) {
      // check for switching back and forth
      if (useDefaultColors()) {
        data[PROP_FRAME_COLOR][PROP_VAL_VALUE]=cf.getDefFrameCol(); 
        data[PROP_FILL_COLOR][PROP_VAL_VALUE]=cf.getDefFillCol();
        data[PROP_SELECTED_COLOR][PROP_VAL_VALUE]=cf.getDefGlowCol(); 
        data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
        data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.TRUE;
        data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
      } else {
        data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
        data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.FALSE;
        data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
      }
      fireTableCellUpdated(PROP_FRAME_COLOR, COLUMN_VALUE);
      fireTableCellUpdated(PROP_FILL_COLOR, COLUMN_VALUE);
      fireTableCellUpdated(PROP_SELECTED_COLOR, COLUMN_VALUE);
    }     
    if (bSendEvents) {
      event = new MsgEvent();
      event.code = MsgEvent.WIDGET_REPAINT;
      event.message = getKey();
      MsgBoard.getInstance().publish(event);
    }
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
//  System.out.println("WM readModel() " + getKey());
    if (widgetType != null)
      this.widgetType = widgetType;
    bSendEvents = in.readBoolean();
//  System.out.println("bSendEvents: " + bSendEvents);
    int rows = in.readInt();
    String metaID = null;
    Object objectData = null;
    int row;
//  System.out.println("WM rows: " + rows);
    boolean bNeedFix = false;
    for (int i=0; i<rows; i++) {
      metaID = (String)in.readObject();
      objectData = in.readObject();
      // work-around fix for bug in beta release where metaID's BAR-100, BAR-101 were duplicated
      // and BAR-102 was miss-assigned
      if (metaID.equals("BAR-102") && bNeedFix) {
        metaID = "BAR-104";
        bNeedFix = false;
      }
      if (metaID.equals("BAR-100") && objectData instanceof Integer) {
        metaID = "BAR-102";
        bNeedFix = true;
      }
      if (metaID.equals("BAR-101") && objectData instanceof Integer) {
        metaID = "BAR-103";
      }
      // now that we remapped them we need to do another converting isRamp to our cbStyle combo
      if (metaID.equals("BAR-101") && objectData instanceof Boolean) {
        if (((Boolean)objectData).booleanValue()) {
          // convert isRamp to "Ramp"
          metaID = "BAR-105";
          objectData = (String)"Ramp";
        }
          
      }
      row = super.mapMetaIDtoProperty(metaID);
      if (row >= 0) {
        data[row][PROP_VAL_VALUE] = objectData;
/*
   System.out.println(data[row][PROP_VAL_NAME].toString() + ": " + data[row][PROP_VAL_VALUE].toString()
         + " mapped to row " + row);
*/   
      }
    }
    if (getGaugeStyle().equals("Radial")) {
      data[PROP_DIVISIONS][PROP_VAL_READONLY]=Boolean.FALSE; 
      data[PROP_TICKSZ][PROP_VAL_READONLY]=Boolean.FALSE;
      data[PROP_TICK_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
      data[PROP_INDICATOR_SZ][PROP_VAL_READONLY]=Boolean.FALSE; 
      data[PROP_INDICATOR_TIP_SZ][PROP_VAL_READONLY]=Boolean.FALSE;
      data[PROP_INDICATOR_FILL][PROP_VAL_READONLY]=Boolean.FALSE; 
      data[PROP_VERTICAL][PROP_VAL_READONLY]=Boolean.TRUE; 
    } else {
      data[PROP_DIVISIONS][PROP_VAL_READONLY]=Boolean.TRUE; 
      data[PROP_TICKSZ][PROP_VAL_READONLY]=Boolean.TRUE;
      data[PROP_TICK_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
      data[PROP_INDICATOR_SZ][PROP_VAL_READONLY]=Boolean.TRUE; 
      data[PROP_INDICATOR_TIP_SZ][PROP_VAL_READONLY]=Boolean.TRUE;
      data[PROP_INDICATOR_FILL][PROP_VAL_READONLY]=Boolean.TRUE; 
      data[PROP_VERTICAL][PROP_VAL_READONLY]=Boolean.FALSE; 
    }
    if (useDefaultColors()) {
      data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
      data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.TRUE;
      data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
    } else {
      data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
      data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.FALSE;
      data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
    }
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
