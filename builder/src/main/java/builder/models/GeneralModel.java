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
import java.util.ArrayList;
import java.util.List;

import javax.swing.DefaultCellEditor;
import javax.swing.JComboBox;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;
import javax.swing.table.TableCellEditor;

import builder.Builder;
import builder.common.EnumFactory;

/**
 * The Class GeneralModel implements the model for the builder.
 * 
 * @author Paul Conti
 *  
 */
public class GeneralModel extends WidgetModel {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID  = 1L;
  
  /** The Constant GENERAL_THEME. */
  public static final int GENERAL_THEME       = 1;
  
  /** The Constant GENERAL_TARGET. */
  public static final int GENERAL_TARGET      = 2;
  
  /** The Constant DISPLAY_WIDTH. */
  public static final int DISPLAY_WIDTH       = 3;
  
  /** The Constant DISPLAY_HEIGHT. */
  public static final int DISPLAY_HEIGHT      = 4;
  
  /** The Constant DISPLAY_DPI. */
  public static final int DISPLAY_DPI         = 5;
  
  /** The Constant GENERAL_PROJECT_DIR. */
  public static final int GENERAL_PROJECT_DIR = 6;
  
  /** The Constant GENERAL_IMAGE_DIR. */
  public static final int GENERAL_IMAGE_DIR   = 7;
  
  /** The Constant GENERAL_BACKGROUND. */
  public static final int GENERAL_BACKGROUND  = 8;
  
  /** The Constant GENERAL_MARGINS. */
  public static final int GENERAL_MARGINS     = 9;
  
  /** The Constant GENERAL_HSPACING. */
  public static final int GENERAL_HSPACING    = 10;
  
  /** The Constant GENERAL_VSPACING. */
  public static final int GENERAL_VSPACING    = 11;
  
  /** The Constant GENERAL_MAX_STRING. */
  public static final int GENERAL_MAX_STRING  = 12;
 
  /** The Constant GENERAL_MAX_STRING. */
  public static final int GENERAL_RECENT_COLORS = 13;
 
  /** The themes. */
  List<String> themes;
  
  /** The theme class names. */
  List<String> themeClassNames;
  
  /** The idx theme. */
  int idxTheme;
  
  /** The cb themes. */
  JComboBox<String> cbThemes;
  
  /** The theme cell editor. */
  DefaultCellEditor  themeCellEditor;

  /** The cb target. */
  JComboBox<String> cbTarget;
  
  /** The target cell editor. */
  DefaultCellEditor targetCellEditor;
  
  /** The default theme name */
  String defThemeName;

  /** The default theme class */
  String defThemeClass;

  /**
   * Instantiates a new general model.
   */
  public GeneralModel() {
    initThemes();
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.GENERAL;
    data = new Object[14][5];

    initProp(PROP_KEY, String.class, "COM-001", Boolean.TRUE,"Key",widgetType);
    initProp(GENERAL_THEME, String.class, "GEN-100", Boolean.FALSE,"Theme",defThemeName);
    if (Builder.isMAC) 
      data[GENERAL_THEME][PROP_VAL_READONLY]= Boolean.TRUE;
    initProp(GENERAL_TARGET, String.class, "GEN-101", Boolean.FALSE,"Target Platform","arduino");

    initProp(DISPLAY_WIDTH, Integer.class, "GEN-102", Boolean.FALSE,"TFT Screen Width",Integer.valueOf(320));
    initProp(DISPLAY_HEIGHT, Integer.class, "GEN-103", Boolean.FALSE,"TFT Screen Height",Integer.valueOf(240));
    initProp(DISPLAY_DPI, Integer.class, "GEN-104", Boolean.FALSE,"TFT Screen DPI",Integer.valueOf(144));

    initProp(GENERAL_PROJECT_DIR, String.class, "GEN-105", Boolean.FALSE,"Project Directory","projects");
    initProp(GENERAL_IMAGE_DIR, String.class, "GEN-106", Boolean.FALSE,"Target's Image Directory","");

    initProp(GENERAL_BACKGROUND, Color.class, "COL-310", Boolean.FALSE,"Background Color",Color.BLACK);

    initProp(GENERAL_MARGINS, Integer.class, "GEN-107", Boolean.FALSE,"Screen Margins",Integer.valueOf(10));
    initProp(GENERAL_HSPACING, Integer.class, "GEN-108", Boolean.FALSE,"Horizontal Spacing between widgets",Integer.valueOf(20));
    initProp(GENERAL_VSPACING, Integer.class, "GEN-109", Boolean.FALSE,"Vertical Spacing between widgets",Integer.valueOf(20));
    initProp(GENERAL_MAX_STRING, Integer.class, "GEN-110", Boolean.FALSE,"MAX_STR",Integer.valueOf(100));
    initProp(GENERAL_RECENT_COLORS, String.class, "GEN-111", Boolean.TRUE,"Recent Colors","");
  }
  
  /**
   * Initializes the themes.
   */
  protected void initThemes()
  {
    themes = new ArrayList<String>();
    themeClassNames = new ArrayList<String>();
    idxTheme = 0;
    int i = 0;
    defThemeClass = UIManager.getSystemLookAndFeelClassName();
    if (!Builder.isMAC) {
      if (defThemeClass.equals("com.sun.java.swing.plaf.windows.WindowsLookAndFeel")) {
        defThemeName = "Windows";
      } else {
        defThemeName = "Metal";
      }
      for (LookAndFeelInfo look_and_feel : UIManager.getInstalledLookAndFeels()) {
        themes.add(look_and_feel.getName());
        themeClassNames.add(look_and_feel.getClassName());
        if (defThemeClass.equals(look_and_feel.getClassName()))
          idxTheme = i;
        i++;
      }
      cbThemes = new JComboBox<String>();
      for(String t : themes) {
        cbThemes.addItem(t);
      }
      cbThemes.setSelectedIndex(idxTheme);
      themeCellEditor =  new DefaultCellEditor(cbThemes);
    } else {
      defThemeName = "Aqua";
    }
    cbTarget = new JComboBox<String>();
    cbTarget.addItem("arduino");
    cbTarget.addItem("linux");
    cbTarget.addItem("arduino_min");
    targetCellEditor = new DefaultCellEditor(cbTarget);
  }
  
  /**
   * getRowCount gives back the number of user visible properties
   * its 1 less than the data[][] table size because we hide 
   * recent colors from the user and only show them inside
   * our color chooser.
   * @return the row count
   * @see javax.swing.table.TableModel#getRowCount()
   */
  @Override
  public int getRowCount() {
    return data.length-1;  
  }

  /**
   * Gets the theme class name.
   *
   * @return the theme class name
   */
  public String getThemeClassName() {
    // look-and-feel user setting
    if (Builder.isMAC) {
      return defThemeClass;
    }
    String currentTheme = (String) data[GENERAL_THEME][PROP_VAL_VALUE];
    for (int j = 0; j < themes.size(); j++) {
      if (currentTheme.equals(themes.get(j)))
        idxTheme = j;
    }
    return themeClassNames.get(idxTheme);
  }
  
  /**
   * Gets the target.
   *
   * @return the target
   */
  public String getTarget() {
    return (String) data[GENERAL_TARGET][PROP_VAL_VALUE];
  }

  /**
   * setTarget sets the target platform
   * @param s
   */
  public void setTarget(String s) { 
    shortcutValue(s, GENERAL_TARGET);
  }

  /**
   * Gets the recent colors.
   *
   * @return the recent colors
   */
  public String getRecentColors() {
    return (String) data[GENERAL_RECENT_COLORS][PROP_VAL_VALUE];
  }

  /**
   * setRecentColors sets the recent colors
   * called by our color chooser.
   * @param s
   */
  public void setRecentColors(String s) { 
    shortcutValue(s, GENERAL_RECENT_COLORS);
  }

  /**
   * Gets the project dir.
   *
   * @return the project dir
   */
  public String getProjectDir() {
    return (String) data[GENERAL_PROJECT_DIR][PROP_VAL_VALUE];
  }

  /**
   * Gets the image dir.
   *
   * @return the image dir
   */
  public String getImageDir() {
    return (String) data[GENERAL_IMAGE_DIR][PROP_VAL_VALUE];
  }

 /**
  * getWidth
  *
  * @see builder.models.WidgetModel#getWidth()
  */
 @Override
  public int getWidth() {
    // return Integer.parseInt((String) data[GENERAL_WIDTH][PROP_VAL_VALUE]);
    return (((Integer) (data[DISPLAY_WIDTH][PROP_VAL_VALUE])).intValue());
  }

  /**
   * setWidth
   *
   * @see builder.models.WidgetModel#setWidth(int)
   */
  @Override
  public void setWidth(int w) {
    shortcutValue(Integer.valueOf(w), DISPLAY_WIDTH);
  }

  /**
   * getHeight
   *
   * @see builder.models.WidgetModel#getHeight()
   */
  @Override
  public int getHeight() {
  //  return Integer.parseInt((String) data[GENERAL_HEIGHT][PROP_VAL_VALUE]);
    return (((Integer) (data[DISPLAY_HEIGHT][PROP_VAL_VALUE])).intValue());
  }

  /**
   * setHeight
   *
   * @see builder.models.WidgetModel#setHeight(int)
   */
  @Override
  public void setHeight(int h) {
    shortcutValue(Integer.valueOf(h), DISPLAY_HEIGHT);
  }

  /**
   * Gets the dpi.
   *
   * @return the dpi
   */
  public int getDPI() {
    return (((Integer) (data[DISPLAY_DPI][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the fill color.
   *
   * @return the fill color
   */
  public Color getFillColor() {
    return (((Color) data[GENERAL_BACKGROUND][PROP_VAL_VALUE]));
  }

  /**
   * Gets the margins.
   *
   * @return the margins
   */
  public int getMargins() {
    return (((Integer) (data[GENERAL_MARGINS][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the h spacing.
   *
   * @return the h spacing
   */
  public int getHSpacing() {
    return (((Integer) (data[GENERAL_HSPACING][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the v spacing.
   *
   * @return the v spacing
   */
  public int getVSpacing() {
    return (((Integer) (data[GENERAL_VSPACING][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the max str.
   *
   * @return the max str
   */
  public int getMaxStr() {
    return (((Integer) (data[GENERAL_MAX_STRING][PROP_VAL_VALUE])).intValue());
  }

  /**
   * getEditorAt
   *
   * @see builder.models.WidgetModel#getEditorAt(int)
   */
  @Override
  public TableCellEditor getEditorAt(int rowIndex) {
    if (rowIndex == GENERAL_THEME)
      return themeCellEditor;
    else if (rowIndex == GENERAL_TARGET)
      return targetCellEditor;
    return null;
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
    if (row == GENERAL_TARGET) {
      if (getTarget().equals("linux")) {
        data[GENERAL_IMAGE_DIR][PROP_VAL_VALUE]="/res/";
      } else {
        data[GENERAL_IMAGE_DIR][PROP_VAL_VALUE]="";
      }
      fireTableCellUpdated(GENERAL_IMAGE_DIR, COLUMN_VALUE);
    }
  }

}
