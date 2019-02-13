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
 * The Class TextBoxNode implements the the parser node for the Text Box widget.
 * 
 * @author Paul Conti
 * 
 */
public class TextBoxNode extends ApiNode { 
  
  /** The Constant PROP_ELEMENTREF. */
  static private final int PROP_ELEMENTREF        = 5;
  
  /** The Constant PROP_FONT. */
  static private final int PROP_FONT              = 6;
  
  /** The Constant PROP_FONT_ENUM. */
  static private final int PROP_FONT_ENUM         = 7;
  
  /** The Constant PROP_WRAP. */
  static private final int PROP_WRAP              = 8;
  
  /** The Constant PROP_ROWS. */
  static private final int PROP_ROWS              = 9;
  
  /** The Constant PROP_COLS. */
  static private final int PROP_COLS              = 10;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR       = 11;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR        = 12;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR    = 13;
  
  /**
   * Instantiates a new text box model.
   */
  public TextBoxNode() {
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    apiType = ApiNode.BOX;
    widgetType = "TextBox";
    data = new Object[14][3];

    initCommonProps();

    initProp(PROP_ELEMENTREF, "TXT-206","ElementRef");
    initProp(PROP_FONT, "TXT-200","Font");
    initProp(PROP_FONT_ENUM, "TXT-211","Font Enum");
    initProp(PROP_WRAP, "TXT-208","Wrap Text");
    initProp(PROP_ROWS, "TXT-209","Text Rows");
    initProp(PROP_COLS, "TXT-210","Text Columns");

    initProp(PROP_FRAME_COLOR, "COL-302","Frame Color");
    initProp(PROP_FILL_COLOR, "COL-303","Fill Color");
    initProp(PROP_SELECTED_COLOR, "COL-304","Selected Color");

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
   * Gets the font display name.
   *
   * @return the font display name
   */
  public String getFontDisplayName() {
    return (String) ((String)data[PROP_FONT][PROP_VAL_VALUE]);
  }
  
  /**
   * Gets the font enum.
   *
   * @return the font enum
   */
  public String getFontEnum() {
    return (String) ((String)data[PROP_FONT_ENUM][PROP_VAL_VALUE]);
  }
  
  /**
   * Sets the font display name.
   *
   * @param the font display name
   */
  public void setFontDisplayName(String s) {
    setValue(s, PROP_FONT);
  }
 
  /**
   * Sets the font enum.
   *
   * @param the font enum
   */
  public void setFontEnum(String s) {
    setValue(s, PROP_FONT_ENUM);
  }
 
  /**
   * sets Wrap text.
   *
   * @param <code>true</code>, if successful
   */
  public void setWrapText(String s) {
    setValue(Boolean.valueOf(s), PROP_WRAP);
  }

  /**
   * sets the num text rows.
   *
   * @param the num text rows
   */
  public void setNumTextRows(String v) {
    setValue(Integer.valueOf(v),PROP_ROWS);
  }

  /**
   * Sets the num text columns.
   *
   * @param the num text columns
   */
  public void setNumTextColumns(String v) {
    setValue(Integer.valueOf(v),PROP_COLS);
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
