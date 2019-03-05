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
import java.awt.Dimension;
import java.io.IOException;
import java.io.ObjectInputStream;

import javax.swing.JTextField;

import builder.common.ColorFactory;
import builder.common.EnumFactory;
import builder.common.FontFactory;
import builder.common.FontItem;
import builder.events.MsgBoard;
import builder.events.MsgEvent;
import builder.prefs.GeneralEditor;

/**
 * The Class TextBoxModel implements the model for the Text Box widget.
 * 
 * @author Paul Conti
 * 
 */
public class TextBoxModel extends WidgetModel { 
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant for gslc_tsElemRef* m_pElementRef name */
  public static final String ELEMENTREF_NAME = "m_pElemTextbox";
  
  /** The Constant PROP_ELEMENTREF. */
  static private final int PROP_ELEMENTREF        = 6;
  
  /** The Constant PROP_FONT. */
  static private final int PROP_FONT              = 7;
  
  /** The Constant PROP_FONT_ENUM. */
  static private final int PROP_FONT_ENUM         = 8;
  
  /** The Constant PROP_WRAP. */
  static private final int PROP_WRAP              = 9;
  
  /** The Constant PROP_ROWS. */
  static private final int PROP_ROWS              = 10;
  
  /** The Constant PROP_COLS. */
  static private final int PROP_COLS              = 11;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR       = 12;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR        = 13;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR    = 14;
  
  /** The gm. */
  GeneralModel gm = null;
  
  /** The ff. */
  FontFactory  ff = null;
  
  static private final int DEF_WIDTH = 203;
  static private final int DEF_HEIGHT= 68;

  /**
   * Instantiates a new text box model.
   */
  public TextBoxModel() {
    gm = (GeneralModel) GeneralEditor.getInstance().getModel();
    ff = FontFactory.getInstance();
    cf = ColorFactory.getInstance();
    initProperties();
    calcSizes();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.TEXTBOX;
    data = new Object[15][5];
    
    initCommonProps(DEF_WIDTH, DEF_HEIGHT);
    
    initProp(PROP_ELEMENTREF, String.class, "TXT-206", Boolean.FALSE,"ElementRef","");
    initProp(PROP_FONT, JTextField.class, "TXT-200", Boolean.FALSE,"Font",ff.getDefFontName());
    initProp(PROP_FONT_ENUM, String.class, "TXT-211", Boolean.FALSE,"Font Enum",ff.getDefFontEnum());
    initProp(PROP_WRAP, Boolean.class, "TXT-208", Boolean.FALSE,"Wrap Text",Boolean.TRUE);
    initProp(PROP_ROWS, Integer.class, "TXT-209", Boolean.FALSE,"Text Rows",Integer.valueOf(0));
    initProp(PROP_COLS, Integer.class, "TXT-210", Boolean.FALSE,"Text Columns",Integer.valueOf(0));

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
    boolean bChangeFontEnum = false;
    // The test for Integer supports copy and paste from clipboard.
    // Otherwise we get a can't cast class String to Integer fault
    if ( (getClassAt(row) == Integer.class) && (value instanceof String)) {
        data[row][PROP_VAL_VALUE] = Integer.valueOf(Integer.parseInt((String)value));
    } else {
      if (row == PROP_FONT) {
        // check to see if user defined this font enum, if so we won't change it
        if (!ff.getFontEnum((String)value).equals(getFontEnum())) bChangeFontEnum=true;
      }
      data[row][PROP_VAL_VALUE] = value;
    }
    fireTableCellUpdated(row, COLUMN_VALUE);
    if (row == PROP_FONT_ENUM) {
      String strName = ff.getFontDisplayName(getFontEnum());
      if (strName != null && !strName.equals(getFontDisplayName())) {
        data[PROP_FONT][PROP_VAL_VALUE]=strName;
        fireTableCellUpdated(PROP_FONT, COLUMN_VALUE);
      }
    }
    if (row == PROP_WIDTH  ||
        row == PROP_HEIGHT ||
        row == PROP_FONT ) {
      if (bChangeFontEnum) {
        setFontEnum(ff.getFontEnum(getFontDisplayName()));
        fireTableCellUpdated(PROP_FONT_ENUM, COLUMN_VALUE);
      }
      // re-calc number of text rows and columns
      calcSizes();
      fireTableCellUpdated(PROP_ROWS, COLUMN_VALUE);
      fireTableCellUpdated(PROP_COLS, COLUMN_VALUE);
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
   * Gets the font enum.
   *
   * @return the font enum
   */
  public String getFontEnum() {
    return (String) ((String)data[PROP_FONT_ENUM][PROP_VAL_VALUE]);
  }
  
  /**
   * Sets the font enum.
   *
   * @param s
   *          the new font enum.
   */
  public void setFontEnum(String s) { 
    shortcutValue(s, PROP_FONT_ENUM);
  }
  
  /**
   * Wrap text.
   *
   * @return <code>true</code>, if successful
   */
  public boolean wrapText() {
    return ((Boolean) data[PROP_WRAP][PROP_VAL_VALUE]).booleanValue();
  }

  /**
   * Gets the num text rows.
   *
   * @return the num text rows
   */
  public int getNumTextRows() {
    return (((Integer) (data[PROP_ROWS][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the num text columns.
   *
   * @return the num text columns
   */
  public int getNumTextColumns() {
    return (((Integer) (data[PROP_COLS][PROP_VAL_VALUE])).intValue());
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
   * Calc sizes.
   */
  private void calcSizes() {
    int nCols;
    int nRows;
    String name = getFontDisplayName();
    FontItem item = ff.getFontItem(name);
    if (!item.getDisplayName().equals(name)) {
      data[PROP_FONT][PROP_VAL_VALUE] = item.getDisplayName();
      data[PROP_FONT_ENUM][PROP_VAL_VALUE] = item.getFontId();
    }
    Dimension nChSz = ff.measureChar(getFontDisplayName());
    nCols = (getWidth() - 33) / nChSz.width;
    nRows = (getHeight() - 15) / nChSz.height; 
    data[PROP_ROWS][PROP_VAL_VALUE]= Integer.valueOf(nRows);
    data[PROP_COLS][PROP_VAL_VALUE]=Integer.valueOf(nCols);
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
    if (getElementRef().equals("")) {
      String strKey = "";
      int n = 0;
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
