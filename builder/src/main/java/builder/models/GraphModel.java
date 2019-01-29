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
import javax.swing.JTextField;
import javax.swing.table.TableCellEditor;

import builder.common.ColorFactory;
import builder.common.EnumFactory;
import builder.events.MsgBoard;
import builder.events.MsgEvent;

/**
 * The Class GraphModel implements the model for the Graph widget.
 * 
 * @author Paul Conti
 * 
 */
public class GraphModel extends WidgetModel { 
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant for gslc_tsElemRef* m_pElementRef name */
  public static final String ELEMENTREF_NAME = "m_pElemGraph";
  
  /** The Constant PROP_ELEMENTREF. */
  static private final int PROP_ELEMENTREF        = 6;
  
  /** The Constant PROP_FONT. */
  static private final int PROP_FONT              = 7;
  
  /** The Constant PROP_ROWS. */
  static private final int PROP_ROWS              = 8;
  
  /** The Constant PROP_STYLE. */
  static private final int PROP_STYLE             = 9;
  
  /** The Constant PROP_GRAPH_COLOR. */
  static private final int PROP_GRAPH_COLOR       = 10;
  
  /** The Constant PROP_DEFAULT_COLORS. */
  static private final int PROP_DEFAULT_COLORS    = 11;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR       = 12;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR        = 13;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR    = 14;

  /** The cb style. */
  JComboBox<String> cbStyle;
  
  /** The style cell editor. */
  DefaultCellEditor styleCellEditor;

  static private final int DEF_WIDTH = 180;
  static private final int DEF_HEIGHT= 120;
  
  /**
   * Instantiates a new graph model.
   */
  public GraphModel() {
    cf = ColorFactory.getInstance();
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.GRAPH;
    data = new Object[15][5];
    
    initCommonProps(DEF_WIDTH, DEF_HEIGHT);

    initProp(PROP_ELEMENTREF, String.class, "TXT-206", Boolean.FALSE,"ElementRef","");
    initProp(PROP_FONT, JTextField.class, "TXT-200", Boolean.FALSE,"Font","BuiltIn->5x8pt7b");

    initProp(PROP_ROWS, Integer.class, "GRPH-100", Boolean.FALSE,"Maximum Points",Integer.valueOf(0));
    initProp(PROP_STYLE, String.class, "GRPH-101", Boolean.FALSE,"Graph Style","Dot");

    initProp(PROP_GRAPH_COLOR, Color.class, "COL-309", Boolean.FALSE,"Color of Graph",Color.ORANGE);
    initProp(PROP_DEFAULT_COLORS, Boolean.class, "COL-300", Boolean.FALSE,"Use Default Colors?",Boolean.TRUE);
    initProp(PROP_FRAME_COLOR, Color.class, "COL-302", Boolean.TRUE,"Frame Color",cf.getDefFrameCol());
    initProp(PROP_FILL_COLOR, Color.class, "COL-303", Boolean.TRUE,"Fill Color",cf.getDefFillCol());
    initProp(PROP_SELECTED_COLOR, Color.class, "COL-304", Boolean.TRUE,"Selected Color",cf.getDefGlowCol());

    cbStyle = new JComboBox<String>();
    cbStyle.addItem("Dot");
    cbStyle.addItem("Fill");
    cbStyle.addItem("Line");
    styleCellEditor = new DefaultCellEditor(cbStyle);
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
   * Gets the font display name.
   *
   * @return the font display name
   */
  public String getFontDisplayName() {
    return (String) ((String)data[PROP_FONT][PROP_VAL_VALUE]);
  }
  
  /**
   * Gets the graph color.
   *
   * @return the graph color
   */
  public Color getGraphColor() {
    return (((Color) data[PROP_GRAPH_COLOR][PROP_VAL_VALUE]));
  }
  
  /**
   * Gets the num rows.
   *
   * @return the num rows
   */
  public int getNumRows() {
    return (((Integer) (data[PROP_ROWS][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the graph style.
   *
   * @return the graph style
   */
  public String getGraphStyle() {
    return (String) data[PROP_STYLE][PROP_VAL_VALUE];
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
    if (useDefaultColors()) {
      data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
      data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.TRUE;
      data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
    } else {
      data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
      data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.FALSE;
      data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
    }   
    String strKey = "";
    int n = 0;
    String strCount = ""; 
    String ref = ""; 
    if (getElementRef().equals("")) {
      ref = ELEMENTREF_NAME;
      strKey = getKey();
      n = strKey.indexOf("$");
      strCount = strKey.substring(n+1, strKey.length());
      ref = ref + strCount;
      setElementRef(ref);
    }
  }

}
