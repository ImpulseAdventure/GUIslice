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
 * The Class TxtButtonNode implements the parser node for the Text Button widget.
 * 
 * @author Paul Conti
 * 
 */
public class TxtButtonNode extends ApiNode { 
  
  /** The Constant PROP_FONT. */
  static private final int PROP_FONT              = 5;
  
  /** The Constant PROP_FONT_ENUM. */
  static private final int PROP_FONT_ENUM         = 6;
  
  /** The Constant PROP_TEXT. */
  static private final int PROP_TEXT              = 7;
  
  /** The Constant PROP_UTF8. */
  static private final int PROP_UTF8              = 8;
  
  /** The Constant PROP_CHANGE_PAGE. */
  static private final int PROP_CHANGE_PAGE       = 9;
  
  /** The Constant PROP_PAGE. */
  static private final int PROP_PAGE              = 10;
  
  /** The Constant PROP_DEFAULT_COLORS. */
  static private final int PROP_DEFAULT_COLORS    = 11;
  
  /** The Constant PROP_TEXT_COLOR. */
  static private final int PROP_TEXT_COLOR        = 12;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR       = 13;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR        = 14;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR    = 15;
  
  /**
   * Instantiates a new txt button node.
   */
  public TxtButtonNode() {
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    apiType = ApiNode.TXTBUTTON;
    widgetType = "TextButton";
    
    data = new Object[16][3];
    
    initCommonProps();
    
    initProp(PROP_FONT, "TXT-200","Font");
    initProp(PROP_FONT_ENUM, "TXT-211", "Font Enum");
    initProp(PROP_TEXT, "TXT-202","Label");
    initProp(PROP_UTF8, "TXT-203","UTF-8?");
    
    initProp(PROP_CHANGE_PAGE, "TBTN-100","Change Page Funct?");
    initProp(PROP_PAGE, "TBNT-101","Jump to Page Enum");

    initProp(PROP_DEFAULT_COLORS, "COL-300","Use Default Colors?");
    initProp(PROP_TEXT_COLOR, "COL-301","Text Color");
    initProp(PROP_FRAME_COLOR, "COL-302","Frame Color");
    initProp(PROP_FILL_COLOR, "COL-303","Fill Color");
    initProp(PROP_SELECTED_COLOR, "COL-304","Selected Color");

  }

  /**
   * Set utf8.
   *
   * @param true, if is utf8
   */
  public void setUTF8(String b) {
    setValue(Boolean.valueOf(b), PROP_UTF8);
  }

  /**
   * Sets change page funct.
   *
   * @param true, if is change page funct
   */
  public void setChangePageFunct(String b) {
    setValue(Boolean.valueOf(b), PROP_CHANGE_PAGE);
  }

  /**
   * Sets the change page enum.
   *
   * @param the change page enum
   */
  public void getChangePageEnum(String s) {
    setValue(s, PROP_PAGE);
  }

  /**
   * Gets the font display name.
   *
   * @return the font display name
   */
  public String getFontDisplayName() {
    return (String) getValue(PROP_FONT);
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
   * Sets the text.
   *
   * @param the text
   */
  public void setText(String s) {
    setValue(s, PROP_TEXT);
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
   * Sets the text color.
   *
   * @param the text color
   */
  public void setTextColor(Color c) {
    setValue(c, PROP_TEXT_COLOR);
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
