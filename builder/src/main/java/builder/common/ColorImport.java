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
package builder.common;

import java.awt.Color;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * A mapping class for importing GUIslice Library Color objects.
 * It's bascally the inverse of our ColorFactory class.
 * 
 * @author Paul Conti
 * 
 */
public class ColorImport {
  
  /** The Constant DEFAULT_COLORS_FILE. */
  public  static final String DEFAULT_COLORS_FILE = "default_colors.csv";
  
  /** The instance. */
  private static ColorImport instance = null;
  
  /** The colors list. */
  private static List<ColorItem> colorsList = new ArrayList<ColorItem>();
  
  /**
   * Gets the single instance of ColorImport.
   *
   * @return single instance of ColorImport
   */
  public static synchronized ColorImport getInstance() {
    if (instance == null) {
      instance = new ColorImport();
      String fullPath;
      String strUserDir = System.getProperty("user.dir");
      int n = strUserDir.indexOf("bin");
      if (n > 0) {
        strUserDir = strUserDir.substring(0,n-1);  // remove "/bin"
      }
      fullPath = strUserDir + System.getProperty("file.separator") + "templates" 
        + System.getProperty("file.separator");
      String csvFile = fullPath + DEFAULT_COLORS_FILE;
      instance.readDefaultColors(csvFile);
    }
    return instance;
  }

  /**
   * Instantiates a new color import instance.
   */
  public ColorImport()  {
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
   * Read GUIslice api colors.
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
      while ((line = br.readLine()) != null) {
        // Need to skip comment lines 
        if (!line.startsWith("#")) {
          String[] f = line.split(cvsSplitBy);
          // the first seven non-comment line are the default colors and are not needed for imports
          if (i>6) {
            ColorItem item = new ColorItem(i-7, f[0], Integer.parseInt(f[1]), Integer.parseInt(f[2]), Integer.parseInt(f[3]));
            colorsList.add(item);
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
