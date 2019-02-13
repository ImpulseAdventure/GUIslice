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
package builder.parser;

import java.lang.NumberFormatException;

import builder.common.MetaIds;

/**
 * ApiNode base class for all GUIslice calls translated 
 * to UI Widgets that the GUIsliceBuilder supports.
 * This class is used to support importing a file into the Builder.
 * The objects are created by the Parser class and sent to the 
 * Import class for final processing.
 *
 * @author Paul Conti
 * 
 */
public class ApiNode
{
  
  /** The Constant PAGE. */
  public static final int PAGE        = 1;
  
  /** The Constant BOX. */
  public static final int BOX         = 2;
  
  /** The Constant CHECKBOX. */
  public static final int CHECKBOX    = 3;
  
  /** The Constant GRAPH. */
  public static final int GRAPH       = 4;
  
  /** The Constant IMAGE. */
  public static final int IMAGE       = 5;
  
  /** The Constant IMGBUTTON. */
  public static final int IMGBUTTON   = 6;
  
  /** The Constant PROGRESSBAR. */
  public static final int PROGRESSBAR = 7;
  
  /** The Constant RADIOBUTTON. */
  public static final int RADIOBUTTON = 8;
  
  /** The Constant SLIDER. */
  public static final int SLIDER      = 9;
  
  /** The Constant TEXT. */
  public static final int TEXT        = 10;
  
  /** The Constant TEXTBOX. */
  public static final int TEXTBOX     = 11;
  
  /** The Constant TXTBUTTON. */
  public static final int TXTBUTTON   = 12;
  
  /** The Constant PROP_VAL_ID. */
  static public final int PROP_VAL_ID=0;
  
  /** The Constant PROP_VAL_NAME. */
  static public final int PROP_VAL_NAME=1;
  
  /** The Constant PROP_VAL_VALUE. */
  static public final int PROP_VAL_VALUE=2;
  
  /** The Constant PROP_ENUM. */
  static public final int PROP_ENUM           = 0;
  
  /** The Constant PROP_X. */
  static public final int PROP_X              = 1;
  
  /** The Constant PROP_Y. */
  static public final int PROP_Y              = 2;
  
  /** The Constant PROP_WIDTH. */
  static public final int PROP_WIDTH          = 3;
  
  /** The Constant PROP_HEIGHT. */
  static public final int PROP_HEIGHT         = 4;

  /** The data is made up of 3 columns.
   *  Column 1 has the Meta Property ID. 
   *  Column 2 is the Property Name exposed to users (more for debugging)
   *  Column 3 is the cells Property value. 
   */
  Object[][] data;

  /** the page enum the UI Widget is on. */
  private String pageEnum;

  /** the type of the UI Widget. */
  int apiType;

  /** the string type of the UI Widget. */
  String widgetType;

  /**
   * initProp - helper method for loading a set of property attributes
   ^          - NOTE: all values are set to null so that the importer 
   *                  can skip uninitialized properties.
   * @param row the cell's row within our data table
   * @param id  the meta property id - must not change over the life of the builder
   * @param name the property name
   */
  public void initProp(int row, String id, String name) {
    data[row][PROP_VAL_ID]=id;
    data[row][PROP_VAL_NAME]=name;
    data[row][PROP_VAL_VALUE]=null;
  }
  
  /**
   * initCommonProps - Initialize the set of common property attributes.
   */
  public void initCommonProps() {
    initProp(PROP_ENUM, "COM-002", "ENUM");
    initProp(PROP_X,"COM-003","X");
    initProp(PROP_Y,"COM-004","Y");
    initProp(PROP_WIDTH,"COM-005","Width");
    initProp(PROP_HEIGHT,"COM-006","Height");
  }
  
  /**
   * getPageEnum.
   *
   * @return the page enum
   * @returns the page enum this UI widget is on
   */
  public String getPageEnum() { 
    return pageEnum;
  }
  
  /**
   * set the PageEnum.
   *
   * @param s
   *          the page enum
   */
  public void setPageEnum(String s) {
    pageEnum = s;
  }
   
  /**
   * getRowCount.
   *
   * @return the row count
   * @returns the number of rows in our node
   */
  public int getRowCount() {
    return data.length;  
  }

  /**
   * Returns the api id of the node.
   *
   * @return the type
   */
  public int getType()
  {
    return apiType;
  }

  /**
   * Sets the api id of the node.
   *
   * @param id
   *          the new type
   */
  public void setType(int id)
  {
    apiType = id;
  }

  /**
   * Returns the WidgetType as a String.
   *
   * @return the widget
   */
  public String getWidgetType()
  {
    return widgetType;
  }

  /**
   * sets the widget type.
   *
   * @param s
   *          the new widget type
   */
  public void setWidgetType(String s)
  {
    widgetType = s;
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
   * getValue() is used to return values from our Node.
   *
   * @param row
   *          in table to change
   * @return the value
   */
  public Object getValue(int row) {
    return data[row][PROP_VAL_VALUE];
  }
  
  /**
   * setValue() is a method used by our setXXX() methods for changing a value
   * inside our Node.
   *
   * @param value
   *          - new object value
   * @param row
   *          - row in table to change
   */
  public void setValue(Object value, int row) {
    data[row][PROP_VAL_VALUE] = value;
  }

  /**
   * getEnum.
   *
   * @return the enum
   * @returns the enum of UI widget
   */
  public String getEnum() { 
    return (String)getValue(PROP_ENUM);
  }
  
  /**
   * set the Enum.
   *
   * @param s
   *          the page enum
   */
  public void setEnum(String s) {
    setValue(s, PROP_ENUM); 
  }
   
  /**
   * getMetaId
   * @param row of the property
   * @returns the id of UI widget
   */
  public String getMetaId(int row) { 
     return (String)data[row][PROP_VAL_ID];
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
   * @throws NumberFormatException
   *           the number format exception
   */
  public void setX(String x) throws NumberFormatException {
    setValue(Integer.valueOf(x), PROP_X);
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
   * @throws NumberFormatException
   *           the number format exception
   */
  public void setY(String y)  throws NumberFormatException {
    setValue(Integer.valueOf(y), PROP_Y);
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
   * @throws NumberFormatException
   *           the number format exception
   */
  public void setWidth(String w) throws NumberFormatException {
    setValue(Integer.valueOf(w), PROP_WIDTH);
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
   * @throws NumberFormatException
   *           the number format exception
   */
  public void setHeight(String h) throws NumberFormatException {
    setValue(Integer.valueOf(h), PROP_HEIGHT);
  }

  /**
   * toString.
   *
   * @return the <code>string</code> object
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    String result = new String(pageEnum + " widget: " + getWidgetType() + "\n");
    int rows = getRowCount();
    for (int i = 0; i < rows; i++) {
      if (data[i][PROP_VAL_VALUE] != null) {
//        result = result + (data[i][PROP_VAL_ID]).toString() + " = ";
        result = result + MetaIds.getInstance().getName((String)data[i][PROP_VAL_ID]) + " = ";
        result = result + (data[i][PROP_VAL_VALUE]) + "\n";
      }
    }
    return result;
  }
}
