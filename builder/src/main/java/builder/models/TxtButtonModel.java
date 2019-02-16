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
import builder.common.FontFactory;
import builder.events.MsgBoard;
import builder.events.MsgEvent;

/**
 * The Class TxtButtonModel implements the model for the Text Button widget.
 * 
 * @author Paul Conti
 * 
 */
public class TxtButtonModel extends WidgetModel { 
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant PROP_FONT. */
  static private final int PROP_FONT              = 6;
  
  /** The Constant PROP_FONT_ENUM. */
  static private final int PROP_FONT_ENUM         = 7;
  
  /** The Constant PROP_TEXT. */
  static private final int PROP_TEXT              = 8;
  
  /** The Constant PROP_UTF8. */
  static private final int PROP_UTF8              = 9;
  
  /** The Constant PROP_FILL_EN. */
  static private final int PROP_FILL_EN           = 10;
  
  /** The Constant PROP_FRAME_EN. */
  static private final int PROP_FRAME_EN          = 11;
  
  /** The Constant PROP_TEXT_SZ. */
  static private final int PROP_TEXT_SZ           = 12;

  /** The Constant PROP_ELEMENTREF. */
  static private final int PROP_ELEMENTREF        = 13;
  
  /** The Constant PROP_TEXT_ALIGN. */
  static private final int PROP_TEXT_ALIGN        = 14;
  
  /** The Constant PROP_CHANGE_PAGE. */
  static private final int PROP_CHANGE_PAGE       = 15;
  
  /** The Constant PROP_PAGE. */
  static private final int PROP_PAGE              = 16;
  
  /** The Constant PROP_DEFAULT_COLORS. */
  static private final int PROP_DEFAULT_COLORS    = 17;
  
  /** The Constant PROP_TEXT_COLOR. */
  static private final int PROP_TEXT_COLOR        = 18;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR       = 19;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR        = 20;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR    = 21;
  
  static private final int DEF_WIDTH = 80;
  static private final int DEF_HEIGHT= 40;

  /** The ff. */
  private FontFactory ff = null;
  
  /** The cb align. */
  JComboBox<String> cbAlign;
  
  /** The align cell editor. */
  DefaultCellEditor alignCellEditor;

  /**
   * Instantiates a new txt button model.
   */
  public TxtButtonModel() {
    ff = FontFactory.getInstance();
    cf = ColorFactory.getInstance();
    initProperties();
    initAlignments();
  }
  
  /**
   * Initializes the alignments.
   */
  private void initAlignments()
  {
    cbAlign = new JComboBox<String>();
    cbAlign.addItem("Left");
    cbAlign.addItem("Center");
    cbAlign.addItem("Right");
    alignCellEditor = new DefaultCellEditor(cbAlign);
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.TEXTBUTTON;
    
    data = new Object[22][5];
    
    initCommonProps(DEF_WIDTH, DEF_HEIGHT);
    
    initProp(PROP_FONT, JTextField.class, "TXT-200", Boolean.FALSE,"Font",ff.getDefFontName());
    initProp(PROP_FONT_ENUM, String.class, "TXT-211", Boolean.FALSE,"Font Enum",ff.getDefFontEnum());
    initProp(PROP_TEXT, String.class, "TXT-202", Boolean.FALSE,"Label","Button");
    initProp(PROP_UTF8, Boolean.class, "TXT-203", Boolean.FALSE,"UTF-8?",Boolean.FALSE);

    initProp(PROP_FILL_EN, Boolean.class, "COM-011", Boolean.FALSE,"Fill Enabled?",Boolean.TRUE);
    initProp(PROP_FRAME_EN, Boolean.class, "COM-010", Boolean.FALSE,"Frame Enabled?",Boolean.TRUE);
    initProp(PROP_TEXT_SZ, Integer.class, "TXT-205", Boolean.FALSE,"External Storage Size",Integer.valueOf(0));
    initProp(PROP_ELEMENTREF, String.class, "TXT-206", Boolean.FALSE,"ElementRef","");
    initProp(PROP_TEXT_ALIGN, String.class, "TXT-207", Boolean.FALSE,"Text Alignment","Center");
    
    initProp(PROP_CHANGE_PAGE, Boolean.class, "TBTN-100", Boolean.FALSE,"Change Page Funct?",Boolean.FALSE);
    initProp(PROP_PAGE, String.class, "TBNT-101", Boolean.TRUE,"Jump to Page Enum","");

    initProp(PROP_DEFAULT_COLORS, Boolean.class, "COL-300", Boolean.FALSE,"Use Default Colors?",Boolean.TRUE);
    initProp(PROP_TEXT_COLOR, Color.class, "COL-301", Boolean.TRUE,"Text Color",cf.getBtnTextCol());
    initProp(PROP_FRAME_COLOR, Color.class, "COL-302", Boolean.TRUE,"Frame Color",cf.getBtnFrameCol());
    initProp(PROP_FILL_COLOR, Color.class, "COL-303", Boolean.TRUE,"Fill Color",cf.getBtnFillCol());
    initProp(PROP_SELECTED_COLOR, Color.class, "COL-304", Boolean.TRUE,"Selected Color",cf.getBtnGlowCol());

  }

  /**
   * getEditorAt
   *
   * @see builder.models.WidgetModel#getEditorAt(int)
   */
  @Override
  public TableCellEditor getEditorAt(int rowIndex) {
    if (rowIndex == PROP_TEXT_ALIGN)
      return alignCellEditor;
    return null;
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
    if (row == PROP_FONT && bChangeFontEnum) {
      setFontEnum(ff.getFontEnum(getFontDisplayName()));
      fireTableCellUpdated(PROP_FONT_ENUM, COLUMN_VALUE);
    }
    if (row == PROP_DEFAULT_COLORS) {
      // check for switching back and forth
      if (useDefaultColors()) {
        data[PROP_TEXT_COLOR][PROP_VAL_VALUE]=cf.getBtnTextCol(); 
        data[PROP_FRAME_COLOR][PROP_VAL_VALUE]=cf.getBtnFrameCol(); 
        data[PROP_FILL_COLOR][PROP_VAL_VALUE]=cf.getBtnFillCol();
        data[PROP_SELECTED_COLOR][PROP_VAL_VALUE]=cf.getBtnGlowCol(); 
        data[PROP_TEXT_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
        data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
        data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.TRUE;
        data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
      } else {
        data[PROP_TEXT_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
        data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
        data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.FALSE;
        data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
      }
      fireTableCellUpdated(PROP_TEXT_COLOR, COLUMN_VALUE);
      fireTableCellUpdated(PROP_FRAME_COLOR, COLUMN_VALUE);
      fireTableCellUpdated(PROP_FILL_COLOR, COLUMN_VALUE);
      fireTableCellUpdated(PROP_SELECTED_COLOR, COLUMN_VALUE);
    }     
    if (row == PROP_CHANGE_PAGE) {
      if (isChangePageFunct()) {
        data[PROP_PAGE][PROP_VAL_READONLY]=Boolean.FALSE;
      } else {
        data[PROP_PAGE][PROP_VAL_VALUE]="";
        data[PROP_PAGE][PROP_VAL_READONLY]=Boolean.TRUE;
      }
      fireTableCellUpdated(PROP_PAGE, COLUMN_VALUE);
    }
    if (row == PROP_TEXT_SZ) {
      if (getTextStorage() > 0) {
        String strKey = getKey();
        int n = strKey.indexOf("$");
        String strCount = strKey.substring(n + 1, strKey.length());
        if (getElementRef().length() == 0) {
          setElementRef(new String("m_pElemBtn" + strCount));
          fireTableCellUpdated(PROP_ELEMENTREF, COLUMN_VALUE);
        }
      }
    }
    if (bSendEvents) {
      event = new MsgEvent();
      event.code = MsgEvent.WIDGET_REPAINT;
      event.message = getKey();
      MsgBoard.getInstance().publish(event);
    }
  }

  /**
   * Checks if is utf8.
   *
   * @return true, if is utf8
   */
  public boolean isUTF8() {
    return ((Boolean) data[PROP_UTF8][PROP_VAL_VALUE]).booleanValue();
  }

  /**
   * Checks if is fill enabled.
   *
   * @return true, if is fill enabled
   */
  public boolean isFillEnabled() {
    return ((Boolean) data[PROP_FILL_EN][PROP_VAL_VALUE]).booleanValue();
  }

  /**
   * Checks if is frame enabled.
   *
   * @return true, if is frame enabled
   */
  public boolean isFrameEnabled() {
    return ((Boolean) data[PROP_FRAME_EN][PROP_VAL_VALUE]).booleanValue();
  }

  /**
   * Gets the alignment.
   *
   * @return the alignment
   */
  public String getAlignment() {
    return (String) data[PROP_TEXT_ALIGN][PROP_VAL_VALUE];
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
   * Gets the text storage.
   *
   * @return the text storage
   */
  public int getTextStorage() {
    return (((Integer) (data[PROP_TEXT_SZ][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Checks if is change page funct.
   *
   * @return true, if is change page funct
   */
  public boolean isChangePageFunct() {
    return ((Boolean) data[PROP_CHANGE_PAGE][PROP_VAL_VALUE]).booleanValue();
  }

  /**
   * Gets the change page enum.
   *
   * @return the change page enum
   */
  public String getChangePageEnum() {
    return ((String) data[PROP_PAGE][PROP_VAL_VALUE]);
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
   * Gets the text.
   *
   * @return the text
   */
  public String getText() {
    return ((String) data[PROP_TEXT][PROP_VAL_VALUE]);
  }

  /**
   * Gets the text color.
   *
   * @return the text color
   */
  public Color getTextColor() {
    return (((Color) data[PROP_TEXT_COLOR][PROP_VAL_VALUE]));
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
      data[PROP_TEXT_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
      data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
      data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.TRUE;
      data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.TRUE; 
    } else {
      data[PROP_TEXT_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
      data[PROP_FRAME_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
      data[PROP_FILL_COLOR][PROP_VAL_READONLY]=Boolean.FALSE;
      data[PROP_SELECTED_COLOR][PROP_VAL_READONLY]=Boolean.FALSE; 
    }
    if (isChangePageFunct()) {
      data[PROP_PAGE][PROP_VAL_READONLY]=Boolean.FALSE;
    } else {
      data[PROP_PAGE][PROP_VAL_READONLY]=Boolean.TRUE;
    }
  }     

}
