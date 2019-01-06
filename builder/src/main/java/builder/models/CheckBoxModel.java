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
 * The Class CheckBoxModel implements the model for the Checkbox widget.
 * 
 * @author Paul Conti
 * 
 */
public class CheckBoxModel extends WidgetModel { 
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant PROP_CHECKED. */
  static private final int PROP_CHECKED        = 6;
  
  /** The Constant PROP_MARK_COLOR. */
  static private final int PROP_MARK_COLOR     = 7;
  
  /** The Constant PROP_DEFAULT_COLORS. */
  static private final int PROP_DEFAULT_COLORS = 8;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR    = 9;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR     = 10;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR = 11;
  
  /** The Constant PROP_GROUP. */
  static public  final int PROP_GROUP          = 12;

  static private final int DEF_WIDTH = 20;
  static private final int DEF_HEIGHT= 20;

  /**
   * Instantiates a new check box model.
   */
  public CheckBoxModel() {
    cf = ColorFactory.getInstance();
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.CHECKBOX;
    data = new Object[13][5];
    
    initCommonProps(DEF_WIDTH, DEF_HEIGHT);
    
    initProp(PROP_CHECKED, Boolean.class, "CBOX-100", Boolean.FALSE,"Checked?",Boolean.FALSE);
    initProp(PROP_MARK_COLOR, Color.class, "COL-305", Boolean.FALSE,"Check Mark Color",Color.ORANGE);

    initProp(PROP_DEFAULT_COLORS, Boolean.class, "COL-300", Boolean.FALSE,"Use Default Colors?",Boolean.TRUE);
    initProp(PROP_FRAME_COLOR, Color.class, "COL-302", Boolean.TRUE,"Frame Color",cf.getDefFrameCol());
    initProp(PROP_FILL_COLOR, Color.class, "COL-303", Boolean.TRUE,"Fill Color",cf.getDefFillCol());
    initProp(PROP_SELECTED_COLOR, Color.class, "COL-304", Boolean.TRUE,"Selected Color",cf.getDefGlowCol());

    initProp(PROP_GROUP, String.class, "CBOX-101", Boolean.TRUE,"Group ID","");

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
   * Gets the mark color.
   *
   * @return the mark color
   */
  public Color getMarkColor() {
    return (((Color) data[PROP_MARK_COLOR][PROP_VAL_VALUE]));
  }

  /**
   * Checks if is checked.
   *
   * @return true, if is checked
   */
  public boolean isChecked() {
    return ((Boolean) data[PROP_CHECKED][PROP_VAL_VALUE]).booleanValue();
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
   * Gets the group id.
   *
   * @return the group id
   */
  public String getGroupId() {
    return ((String) data[PROP_GROUP][PROP_VAL_VALUE]);
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
   * @see builder.mementos.PositionMemento
   * @see builder.mementos.WidgetMemento
   * @see builder.views.PagePane
   * @see builder.widgets.Widget
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
  }

}
