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

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableCellEditor;

import builder.commands.Command;
import builder.commands.History;
import builder.commands.PropertyCommand;
import builder.common.ColorFactory;
import builder.events.MsgBoard;
import builder.events.MsgEvent;

/**
 * The Class WidgetModel is the base class for all of our models.
 * 
 * @author Paul Conti
 * 
 */
public class WidgetModel extends AbstractTableModel {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant PROP_VAL_CLASS. */
  static public final int PROP_VAL_CLASS=0;
  
  /** The Constant PROP_VAL_ID. */
  static public final int PROP_VAL_ID=1;
  
  /** The Constant PROP_VAL_READONLY. */
  static public final int PROP_VAL_READONLY=2;
  
  /** The Constant PROP_VAL_NAME. */
  static public final int PROP_VAL_NAME=3;
  
  /** The Constant PROP_VAL_VALUE. */
  static public final int PROP_VAL_VALUE=4;
  
  /** The Constant PROP_KEY. */
  static public final int PROP_KEY            = 0;  // key is our primary index
  
  /** The Constant PROP_ENUM. */
  static public final int PROP_ENUM           = 1;
  
  /** The Constant PROP_X. */
  static public final int PROP_X              = 2;
  
  /** The Constant PROP_Y. */
  static public final int PROP_Y              = 3;
  
  /** The Constant PROP_WIDTH. */
  static public final int PROP_WIDTH          = 4;
  
  /** The Constant PROP_HEIGHT. */
  static public final int PROP_HEIGHT         = 5;

  /** The data is made up of 5 columns, the first three are hidden from users view.
   *  Column 0 is the Class of the JTable cell contents, like String, Integer, Color, etc...
   *  Column 1 has the Meta Property ID. 
   *  Column 2 is a boolean indicating if this cell is read-only.
   *  Column 3 is the Property Name exposed to users.
   *  Column 4 is the cells Property value, also exposed to users. 
   */
  Object[][] data;
  
  /** The JTable column names. */
  String[] columnNames = {"Name", "Value"};
  
  /** The Constant COLUMN_NAME which is the index into our JTable the user sees. */
  static public final int COLUMN_NAME         =0;

  /** The Constant COLUMN_VALUE which is the index into our JTable the user sees. */
  static public final int COLUMN_VALUE        =1;
  
  /** The widget type. */
  String widgetType; // type is always hidden from user so keep it out of data array
  
  /** The color factory. */
  ColorFactory cf = null;
  
  /** The event. */
  MsgEvent event;
  
  /** The b send events. */
  boolean bSendEvents = true;
  
  /** The page enum. 
   * pageEnum is set and used by our code generator, as an optimization,
   * and only for a few widgets. Its unreliable for any other purpose.
   */
  String  pageEnum = "";  
  
  /**
   * initProp - helper method for loading a set of property attributes
   * @param row the cell's row within our data table
   * @param c   the class of this cell
   * @param id  the meta property id - must not change over the life of the builder
   * @param readOnly boolean indicating if the cell is currently read only
   * @param name the property name
   * @param value the default property value
   */
  public void initProp(int row, Class<?> c, String id, Boolean readOnly, String name, Object value) {
    data[row][PROP_VAL_CLASS]=c;
    data[row][PROP_VAL_ID]=id;
    data[row][PROP_VAL_READONLY]=readOnly;
    data[row][PROP_VAL_NAME]=name;
    data[row][PROP_VAL_VALUE]=value;
  }
  
  /**
   * initCommonProps - Initialize the set of common property attributes
   * @param width the widget's width
   * @param height the widget's height
   */
  public void initCommonProps(int width, int height) {
    initProp(PROP_KEY, String.class, "COM-001", Boolean.TRUE,"Key",widgetType);
    initProp(PROP_ENUM, String.class, "COM-002", Boolean.FALSE,"ENUM",widgetType);
    initProp(PROP_X, Integer.class, "COM-003", Boolean.FALSE,"X",Integer.valueOf(0));
    initProp(PROP_Y, Integer.class, "COM-004", Boolean.FALSE,"Y",Integer.valueOf(0));
    initProp(PROP_WIDTH, Integer.class, "COM-005", Boolean.FALSE,"Width",Integer.valueOf(width));
    initProp(PROP_HEIGHT, Integer.class, "COM-006", Boolean.FALSE,"Height",Integer.valueOf(height));
  }
  
  /**
   * Turn off events.
   */
  public void TurnOffEvents() {
    bSendEvents = false;
  }
  
  /**
   * Turn on events.
   */
  public void TurnOnEvents() {
    bSendEvents = true;
  }
  
  /**
   * Gets the page enum.
   *
   * @return the page enum
   */
  public String getPageEnum() {
    return pageEnum;
  }
  
  /**
   * Sets the page enum.
   *
   * @param pageEnum
   *          the new page enum
   */
  public void setPageEnum(String pageEnum) {
    this.pageEnum = pageEnum;
  }
  
  /**
   * getRowCount gives back the number of user visible properties
   * @return the row count
   * @see javax.swing.table.TableModel#getRowCount()
   */
  @Override
  public int getRowCount() {
    return data.length;  
  }

  /**
   * getPropertyCount gives the actual number of properties
   * while getRowCount gives back the number of user visible
   * properties allowing us to hide some at the end of data[][].
   *
   * @see javax.swing.table.TableModel#getRowCount()
   */
  public int getPropertyCount() {
    return data.length;  
  }

  /**
   * getColumnName
   *
   * @see javax.swing.table.AbstractTableModel#getColumnName(int)
   */
  @Override
  public String getColumnName(int index) {
      return columnNames[index];
  }
  
  /**
   * getColumnCount
   *
   * @see javax.swing.table.TableModel#getColumnCount()
   */
  @Override
  public int getColumnCount() {
    return columnNames.length;
  }

  /**
   * getValueAt
   * The first three fields of our data array is always hidden 
   * so we just add 3 to columnIndex when our property editor 
   * asks for a field.
   * This makes our five field table look a two field table.
   *
   * @see javax.swing.table.TableModel#getValueAt(int, int)
   */
  @Override
  public Object getValueAt(int rowIndex, int columnIndex) {
    return data[rowIndex][columnIndex+3];
  }
  
  /**
   * Gets the type.
   *
   * @return the type
   */
  public String getType() {
    return widgetType;
  }
  
  /**
   * Gets the key.
   * key property is used as an index key in various lists to identify specific
   * object instances.  It starts off as simply a copy of widgetType and gets
   * a serial number appended to make it unique.  
   * ie. WidgetType$1,..WidgetType$N.
   *
   * @return the key
   * @see builder.common.EnumFactory#createKey(java.lang.String)
   */
  public String getKey() {
    return (String) data[PROP_KEY][PROP_VAL_VALUE];
  }
  
  /**
   * Sets the key.
   *
   * @param key
   *          the new key
   */
  public void setKey(String key) { 
    shortcutValue(key, PROP_KEY);
  }
  
  /**
   * Gets the enum.
   * The enum is for the GUIslice API's use as an identifier of the widget.
   * @return the enum
   */
  public String getEnum() {
    return (String) data[PROP_ENUM][PROP_VAL_VALUE];
  }

  /**
   * Sets the enum.
   *
   * @param s
   *          the new enum
   */
  public void setEnum(String s) { 
    shortcutValue(s, PROP_ENUM);
  }
  
  /**
   * Gets the x coordinate.
   *
   * @return the x
   */
  public int getX() {
    return (((Integer) (data[PROP_X][PROP_VAL_VALUE])).intValue());
  }
  
  /**
   * Sets the x coordinate.
   *
   * @param x
   *          the new x
   */
  public void setX(int x) {
    shortcutValue(Integer.valueOf(x), PROP_X);
  }

  /**
   * Gets the y coordinate.
   *
   * @return the y
   */
  public int getY() {
    return (((Integer) (data[PROP_Y][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Sets the y coordinate.
   *
   * @param y
   *          the new y
   */
  public void setY(int y) {
    shortcutValue(Integer.valueOf(y), PROP_Y);
  }

  /**
   * Gets the width.
   *
   * @return the width
   */
  public int getWidth() {
    return (((Integer) (data[PROP_WIDTH][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Sets the width.
   *
   * @param w
   *          the new width
   */
  public void setWidth(int w) {
    shortcutValue(Integer.valueOf(w), PROP_WIDTH);
  }

  /**
   * Gets the height.
   *
   * @return the height
   */
  public int getHeight() {
    return (((Integer) (data[PROP_HEIGHT][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Sets the height.
   *
   * @param h
   *          the new height
   */
  public void setHeight(int h) {
    shortcutValue(Integer.valueOf(h), PROP_HEIGHT);
  }

  /**
   * Gets the class at.
   *
   * @param rowIndex
   *          the row index
   * @return the class at
   */
  public Class<?> getClassAt(int rowIndex) {
    return (Class<?>)data[rowIndex][PROP_VAL_CLASS];
  }

  /**
   * Gets the editor at.
   *
   * @param rowIndex
   *          the row index
   * @return the editor at
   */
  public TableCellEditor getEditorAt(int rowIndex) {
    return null;
  }
  
  /**
   * isCellReadOnly
   * @param row cell row number
   * @return true, if is cell readonly
   */
  public boolean isCellReadOnly(int row) {
    return ((Boolean) data[row][PROP_VAL_READONLY]).booleanValue();
  }
  
  /**
   * isCellEditable.
   *
   * @param row
   *          the JTable row in question
   * @param col
   *          the JTable col 
   * @return true, if is cell editable
   * @see javax.swing.table.AbstractTableModel#isCellEditable(int, int)
   */
  @Override
  public boolean isCellEditable(int row, int col) {
    if (col == 0) return false;
    return !isCellReadOnly(row);
  }

  /**
   * setValueAt
   *
   * @see javax.swing.table.AbstractTableModel#setValueAt(java.lang.Object, int, int)
   */
  @Override
  public void setValueAt(Object value, int row, int col) {
    if (col == COLUMN_VALUE) {
      // commands are used to support undo and redo actions.
      PropertyCommand c = new PropertyCommand(this, value, row);
      execute(c);
    }
  }

  /**
   * shortcutValue() is a method used by our setXXX() methods so
   * Widgets can sync models with User Preferences, and Pagepane
   * can drag widgets around without creating circular loops.
   * Note that column isn't required because changes are only
   * allowed for column 2.  Again remember our column 2 is
   * thought to be column 1 by our JTable.
   * 
   * @param value - new object value
   * @param row   - row in table to change
   */
  public void shortcutValue(Object value, int row) {
    data[row][PROP_VAL_VALUE] = value;
    fireTableCellUpdated(row, COLUMN_VALUE);
  }

  /**
   * execute() will indirectly call changeValueAT() to make
   * the actual changes to the table.  It will also record a 
   * historical record of the change for undo/redo.
   * 
   * @param c is the property change command to run.
   */
  public void execute(Command c) {
    History.getInstance().push(c);
    c.execute();
  }
  
  /**
   * changeValueAt is a method used by our command interface 
   * so we can support undo/redo commands.
   * setValue will create the command and execute will 
   * cause the command to call this routine.
   * It is also responsible for sending around repaint notices
   * to our observer views, like Pagepage.
   * 
   * @param value - new object value
   * @param row   - row in table to change
   */
  public void changeValueAt(Object value, int row) {
    // The test for Integer supports copy and paste from clipboard.
    // Otherwise we get a can't cast class String to Integer fault
    if ( (getClassAt(row) == Integer.class) && (value instanceof String)) {
        data[row][PROP_VAL_VALUE] = Integer.valueOf(Integer.parseInt((String)value));
    } else {
      data[row][PROP_VAL_VALUE] = value;
    }
    fireTableCellUpdated(row, COLUMN_VALUE);
    if (bSendEvents) {
      event = new MsgEvent();
      event.code = MsgEvent.WIDGET_REPAINT;
      event.message = getKey();
      MsgBoard.getInstance().publish(event);
    }
  }

  /**
   * Gets the data.
   *
   * @return the data
   */
  public Object[][] getData() {
    return data;
  }

  /**
   * Sets the data - Used for JUNIT testing someday
   *
   * @param data
   *          the new data
   */
  public void setData(Object[][] data) {
    this.data = data;
  }

  /**
   * backup() supports our undo command by making a copy of the table cell's value
   * before any changes have taken place.
   *
   * @param row
   *          the row
   * @return cell's current value.
   * @see builder.commands.PropertyCommand
   * @see builder.mementos.PropertyMemento
   * @see builder.commands.History
   */
  public Object backup(int row) {
    return data[row][PROP_VAL_VALUE];
  }

  /**
   * restore() supports our redo command by replacing the current value of the
   * cell with the backup copy made earlier.
   *
   * @param oldValue
   *          the old value
   * @param row
   *          the row
   * @see builder.commands.PropertyCommand
   * @see builder.mementos.PropertyMemento
   * @see builder.commands.History
   */
  public void restore(Object oldValue, int row) {
    data[row][PROP_VAL_VALUE] = oldValue;
    fireTableCellUpdated(row, COLUMN_VALUE);
  }

  /**
   * writeModel() will serialize our model's data to a string object for backup
   * and recovery.
   *
   * @param out
   *          is our ObjectOutputStream.
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   * @see builder.mementos.PositionMemento
   * @see builder.mementos.WidgetMemento
   * @see builder.views.PagePane
   * @see builder.widgets.Widget
   */
  public void writeModel(ObjectOutputStream out) 
      throws IOException {
//    System.out.println("WM writeModel(): " + getKey());
//    System.out.println("bSendEvents: " + bSendEvents);
    out.writeBoolean(bSendEvents);
    int rows = getRowCount();
//    System.out.println("WM rows: " + rows);
    out.writeInt(rows);
    for (int i=0; i<rows; i++) {
      out.writeObject(data[i][PROP_VAL_ID]);
      out.writeObject(data[i][PROP_VAL_VALUE]);
/*     System.out.println(data[i][PROP_VAL_ID] + ": "
 *       + data[i][PROP_VAL_VALUE].toString() + " = " 
 *       + data[i][PROP_VAL_VALUE].toString());
 */
    }
  }
  
  /**
   * readModel() will de-serialize our model's data from a string object for backup
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
   * @see builder.widgets.Widget
   */
  public void readModel(ObjectInputStream in, String widgetType) throws IOException, ClassNotFoundException {
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
    for (int i = 0; i < rows; i++) {
      metaID = (String) in.readObject();
      objectData = in.readObject();
      row = mapMetaIDtoProperty(metaID);
      if (row >= 0) {
        data[row][PROP_VAL_VALUE] = objectData;
/*         
  System.out.println(data[row][PROP_VAL_NAME].toString() + ": " +
           data[row][PROP_VAL_VALUE].toString() + " mapped to row " + row);
*/        
      }
    }
  }
  
  /**
   * mapMetaIDtoProperty
   * 
   * @param metaID the id assigned that must never change over the life time of the builder
   * @return row that matches metaID, otherwise a -1 on no matching ID
   */
  public int mapMetaIDtoProperty(String metaID) {
    for (int i=0; i<data.length; i++) {
      if (metaID.equals((String)data[i][PROP_VAL_ID])) {
        return i;
      }
    }
    return -1;
  }

}
