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

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * A mapping class for importing GUIslice Library Font objects
 * 
 * @author Paul Conti
 * 
 */
public class FontImport {
  
  /** The Constant ARDUINO_FONT_TEMPLATE. */
  public  final static String ARDUINO_FONT_TEMPLATE  = "arduinofonts.csv";
  
  /** The Constant LINUX_FONT_TEMPLATE. */
  public  final static String LINUX_FONT_TEMPLATE    = "linuxfonts.csv";
  
  /** The instance. */
  private static FontImport instance = null;
  
  /** The arduino fonts. */
  private static List<String> arduinoFonts = new ArrayList<String>();
  
  /** The arduino map. */
  private static HashMap<String, Integer> arduinoMap = new HashMap<String, Integer>();
  
  /** The linux fonts. */
  private static List<String> linuxFonts = new ArrayList<String>();
  
  /** the sizes for linux fonts */
  private static List<Integer> linuxFontSz = new ArrayList<Integer>();
  
  /** The arduino CVS. */
  private static String arduinoCVS;
  
  /** The linux CVS. */
  private static String linuxCVS;
  
  /**
   * Gets the single instance of FontImport.
   *
   * @return single instance of FontImport
   */
  public static synchronized FontImport getInstance() {
    if (instance == null) {
      instance = new FontImport();
      String fullPath;
      String strUserDir = System.getProperty("user.dir");
      int n = strUserDir.indexOf("bin");
      if (n > 0) {
        strUserDir = strUserDir.substring(0,n-1);  // remove "/bin"
      }
      fullPath = strUserDir + System.getProperty("file.separator") + "templates"
        + System.getProperty("file.separator");
      arduinoCVS = fullPath + FontImport.ARDUINO_FONT_TEMPLATE;
      arduinoMap = new HashMap<String, Integer>(128);
      instance.readArduinoFonts();
      linuxCVS = fullPath + FontImport.LINUX_FONT_TEMPLATE;
      instance.readLinuxFonts();

    }
    return instance;
  }

  /**
   * Instantiates a new font import object.
   */
  public FontImport() {
  }

  /**
   * Gets the name of the default font for the target platform.
   *
   * @return true if name match found in Arduino List
   */
  public boolean nameMatch(String name) {
      return arduinoMap.containsKey(name);
  }
  
  /**
   * Gets the arduino font name by size
   *
   * @param size
   *          the key
   * @return the font name
   */

  public String getArduinoFontName(String size) {
    int idx = Integer.valueOf(size);  
    return arduinoFonts.get(idx-1);
  }

  /**
   * Gets the linux font name by size
   *
   * @param size
   *          the key
   * @return the font name
   */

  public String getLinuxFontName(String strSize) {
    int idx = 0;
    int i = 0;
    Integer size = Integer.valueOf(strSize);  
    for (Integer fontSize : linuxFontSz) {
      if (fontSize.intValue() == size.intValue()) {
        idx = i;
        break;
      }
      if (fontSize.intValue() > size.intValue()) {
        if (i > 0)
          idx = i-1;
        break;
      }
      i++;
    }
    return linuxFonts.get(idx);
  }

  /**
   * Read Arduino fonts.
   *
   */
  public void readArduinoFonts() {
    String line = "";
    String cvsSplitBy = ",";
    String name = null;
    BufferedReader br = null;
    try {
      br = new BufferedReader(new FileReader(arduinoCVS));
      while ((line = br.readLine()) != null) {
        if (!line.startsWith("#")) {
          // use comma as separator
          String[] f = line.split(cvsSplitBy);
          name = f[0];  // display name
          arduinoFonts.add(name);
        }
      }
      br.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
    // now load our font map - given the small number of fonts this may be over-kill.
    // However, users can add as many fonts as they want so...
    for (int i=0; i<arduinoFonts.size(); i++) {
      name = arduinoFonts.get(i);
      arduinoMap.put(name, Integer.valueOf(i));
    }
  }
  
  /**
   * Read Linux fonts.
   *
   */
  public void readLinuxFonts() {
    String line = "";
    String cvsSplitBy = ",";
    String name = null;
    String size = null;
    BufferedReader br = null;
    int nPrevSize = 0;
    int nCurSize = 0;
    try {
      br = new BufferedReader(new FileReader(linuxCVS));
      while ((line = br.readLine()) != null) {
        if (!line.startsWith("#")) {
          // use comma as separator
          String[] f = line.split(cvsSplitBy);
          name = f[0];  // display name
          size = f[8];  // LogicalFontSize
          try {
            nCurSize = Integer.valueOf(size);
          } catch (NumberFormatException e) {
            nCurSize = 0;
          }
          // we only need a sizes 10 to 24 or so for import since we have no way to match names
          if (nCurSize < nPrevSize) break;
          linuxFonts.add(name);
          linuxFontSz.add(nCurSize);
        }
      }
      br.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
  
}
