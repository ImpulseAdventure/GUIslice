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
package builder.common;

import java.awt.Color;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * A factory for creating and Managing GUIslice Library Color objects
 * and mapping them to and from something Java can understand.
 * 
 * @author Paul Conti
 * 
 */
public class ColorFactory {
  
  /** The Constant DEFAULT_COLORS_FILE. */
  public  static final String DEFAULT_COLORS_FILE = "default_colors.csv";
  
  /** The instance. */
  private static ColorFactory instance = null;
  
  /** The Constant DEFAULT_FRAME_COLOR. */
  private static final int DEFAULT_FRAME_COLOR = 0;
  
  /** The Constant DEFAULT_FILL_COLOR. */
  private static final int DEFAULT_FILL_COLOR  = 1;
  
  /** The Constant DEFAULT_GLOW_COLOR. */
  private static final int DEFAULT_GLOW_COLOR  = 2;
  
  /** The Constant DEFAULT_TEXT_COLOR. */
  private static final int DEFAULT_TEXT_COLOR  = 3;
  
  /** The Constant BUTTON_FRAME_COLOR. */
  private static final int BUTTON_FRAME_COLOR  = 4;
  
  /** The Constant BUTTON_FILL_COLOR. */
  private static final int BUTTON_FILL_COLOR   = 5;
  
  /** The Constant BUTTON_GLOW_COLOR. */
  private static final int BUTTON_GLOW_COLOR   = 6;
  
  /** The Constant BUTTON_TEXT_COLOR. */
  private static final int BUTTON_TEXT_COLOR   = 7;
  
  /** The default colors. */
  private static List<Color> defaultColors = new ArrayList<Color>();
  
  /** The colors list. */
  private static List<ColorItem> colorsList = new ArrayList<ColorItem>();
  
  /**
   * Gets the single instance of ColorFactory.
   *
   * @return single instance of ColorFactory
   */
  public static synchronized ColorFactory getInstance() {
    if (instance == null) {
      instance = new ColorFactory();
      String fullPath;
      String strUserDir = System.getProperty("user.dir");
      int n = strUserDir.indexOf("bin");
      if (n > 0) {
        strUserDir = strUserDir.substring(0,n-1);  // remove "/bin"
      }
      fullPath = strUserDir + System.getProperty("file.separator"); 
      String csvFile = fullPath + "templates" + System.getProperty("file.separator") 
          + DEFAULT_COLORS_FILE;
      instance.readDefaultColors(csvFile);
    }
    return instance;
  }

  /**
   * Instantiates a new color factory.
   */
  public ColorFactory()  {
  }

  /**
   * Gets the color list.
   *
   * @return the color list
   */
  public List<ColorItem> getColorList() {
    return colorsList;
  }
  
  /**
   * Gets the default frame color.
   *
   * @return the Color
   */
  public Color getDefFrameCol() {
    return defaultColors.get(DEFAULT_FRAME_COLOR);
  }
   
  /**
   * Gets the default fill color.
   *
   * @return the Color
   */
  public Color getDefFillCol() {
    return defaultColors.get(DEFAULT_FILL_COLOR);
  }
   
  /**
   * Gets the default glow color.
   *
   * @return the Color
   */
  public Color getDefGlowCol() {
    return defaultColors.get(DEFAULT_GLOW_COLOR);
  }
   
  /**
   * Gets the default text color.
   *
   * @return the Color
   */
  public Color getDefTextCol() {
    return defaultColors.get(DEFAULT_TEXT_COLOR);
  }
   
  /**
   * Gets the default button frame color.
   *
   * @return Color
   */
  public Color getBtnFrameCol() {
    return defaultColors.get(BUTTON_FRAME_COLOR);
  }
   
  /**
   * Gets the default button fill color.
   *
   * @return the Color
   */
  public Color getBtnFillCol() {
    return defaultColors.get(BUTTON_FILL_COLOR);
  }
   
  /**
   * Gets the default button glow color.
   *
   * @return the Color
   */
  public Color getBtnGlowCol() {
    return defaultColors.get(BUTTON_GLOW_COLOR);
  }
   
  /**
   * Gets the default button text color.
   *
   * @return the Color
   */
  public Color getBtnTextCol() {
    return defaultColors.get(BUTTON_TEXT_COLOR);
  }
   
  /**
   * Find color item.
   *
   * @param index
   *          the index
   * @return the <code>ColorItem</code> object
   */
  public ColorItem findColorItem(int index) { 
    return colorsList.get(index);
  }

  /**
   * Find color item.
   *
   * @param color
   *          the color
   * @return the <code>ColorItem</code> object
   */
  public ColorItem findColorItem(Color color) { 
    for (int i=1; i<colorsList.size(); i++) {
      ColorItem item = colorsList.get(i);
      if (item.getColor().equals(color)) {
         return item;
      }
    }
    return colorsList.get(0);
  }

  /**
   * colorAsString() - convert java Color object to a string GUIslice API can use
   * Example: We will return either a #define like "Color.BLACK" or if no matching
   * #define can be found we will return the red, green, blue as
   * "(gslc_tsColor){999,999,999}"
   *
   * @param color
   *          the color
   * @return GUIslice Library string representing the color
   */
  public String colorAsString(Color color) {
    String strColor = "";
    for (int i=1; i<colorsList.size(); i++) {
      ColorItem item = colorsList.get(i);
      if (item.getColor().equals(color)) {
         return item.getDisplayName();
      }
    }
    strColor = String.format("(gslc_tsColor){%d,%d,%d}", color.getRed(), color.getGreen(), color.getBlue());
    return strColor;
  }
  
  /**
   * Read default colors.
   *
   * @param csvFile
   *          the csv file
   */
  private void readDefaultColors(String csvFile) {
    String line = "";
    String cvsSplitBy = ",";
    BufferedReader br = null;
    try {
      br = new BufferedReader(new FileReader(csvFile));
      int i =0;
      int j = 1;
      ColorItem item = new ColorItem(0, "CUSTOM COLOR");
      colorsList.add(item);
      while ((line = br.readLine()) != null) {
        // Need to skip comment lines 
        if (!line.startsWith("#")) {
          String[] f = line.split(cvsSplitBy);
          Color c = new Color(Integer.parseInt(f[1]), Integer.parseInt(f[2]), Integer.parseInt(f[3]));
          if (i<=7) {
            // use comma as separator
            defaultColors.add(c);
          } else {
            item = new ColorItem(j, f[0], Integer.parseInt(f[1]), Integer.parseInt(f[2]), Integer.parseInt(f[3]));
            colorsList.add(item);
            j++;
          }
          i++;
          
        }
      }
      br.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

}
