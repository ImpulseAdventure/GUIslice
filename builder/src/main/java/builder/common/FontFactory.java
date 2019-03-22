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

import java.awt.Canvas;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.font.FontRenderContext;
import java.awt.geom.Rectangle2D;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import builder.Builder;
import builder.controller.CodeGenerator;
import builder.models.GeneralModel;
import builder.prefs.GeneralEditor;

/**
 * A factory for creating and managing GUIslice Library Font objects
 * and mapping them to and from something Java can understand.
 * 
 * @author Paul Conti
 * 
 */
public class FontFactory {
  
  /** The instance. */
  private static FontFactory instance = null;
  
  /** The arduino fonts. */
  private static List<FontItem> arduinoFonts = new ArrayList<FontItem>();
  
  /** The arduino map. */
  private static HashMap<String, Integer> arduinoMap = new HashMap<String, Integer>();
  
  /** The linux fonts. */
  private static List<FontItem> linuxFonts = new ArrayList<FontItem>();
  
  /** The linux map. */
  private static HashMap<String, Integer> linuxMap = new HashMap<String, Integer>();
  
  /** The general model. */
  private static GeneralModel generalModel;
  
  /** The arduino CVS. */
  private static String arduinoCVS;
  
  /** The linux CVS. */
  private static String linuxCVS;
  
  /** Free Mono Font Sizes */
  private static Dimension[] MonoPlainSizes = new Dimension[4];
  private static Dimension[] MonoBoldSizes = new Dimension[4];
  private static Dimension[] MonoItalicSizes = new Dimension[4];
  private static Dimension[] MonoBoldItalicSizes = new Dimension[4];
  
  /**
   * Gets the single instance of FontFactory.
   *
   * @return single instance of FontFactory
   */
  public static synchronized FontFactory getInstance() {
    if (instance == null) {
      instance = new FontFactory();
      generalModel = (GeneralModel) GeneralEditor.getInstance().getModel();
      String fullPath = CommonUtil.getInstance().getWorkingDir();
      arduinoCVS = fullPath + "templates" + System.getProperty("file.separator") 
          + CodeGenerator.ARDUINO_FONT_TEMPLATE;
      arduinoMap = new HashMap<String, Integer>(128);
      instance.readFonts(arduinoCVS,arduinoFonts,arduinoMap);
      linuxCVS = fullPath + "templates"  + System.getProperty("file.separator") 
          + CodeGenerator.LINUX_FONT_TEMPLATE;
      linuxMap = new HashMap<String, Integer>(128);
      instance.readFonts(linuxCVS,linuxFonts,linuxMap);
      
      // setup mono font tables
      MonoPlainSizes[0] = new Dimension(11,14);
      MonoPlainSizes[1] = new Dimension(14,19);
      MonoPlainSizes[2] = new Dimension(21,30);
      MonoPlainSizes[3] = new Dimension(28,39);
      MonoBoldSizes[0] = new Dimension(11,16);
      MonoBoldSizes[1] = new Dimension(14,22);
      MonoBoldSizes[2] = new Dimension(21,31);
      MonoBoldSizes[3] = new Dimension(28,41);
      MonoItalicSizes[0] = new Dimension(11,14);
      MonoItalicSizes[1] = new Dimension(14,20);
      MonoItalicSizes[2] = new Dimension(21,30);
      MonoItalicSizes[3] = new Dimension(28,39);
      MonoBoldItalicSizes[0] = new Dimension(11,16);
      MonoBoldItalicSizes[1] = new Dimension(14,22);
      MonoBoldItalicSizes[2] = new Dimension(21,31);
      MonoBoldItalicSizes[3] = new Dimension(28,41);
    }
    return instance;
  }

  /**
   * Instantiates a new font factory.
   */
  public FontFactory() {
  }

  /**
   * This method creates a <code>Font</code> using the String values displayed to
   * users of GUIsliceBuider by our various Widget Models.
   *
   * @param fontName
   *          - is the GUIslice font name not the real java font name.
   * @param fontSize
   *          - is the point size of our font as a String value.
   * @param fontStyle
   *          - is the font style "PLAIN", "BOLD", "ITALIC", or "BOLD+ITALIC".
   * @return font The java font we can use to display text
   * @see java.awt.Font
   * @see java.lang.String
   */
  static public Font createFont(String fontName, String fontSize, String fontStyle) {
    Font font;
    int style;
    switch (fontStyle) {
    case "BOLD":
      style = Font.BOLD;
      break;
    case "ITALIC":
      style = Font.ITALIC;
      break;
    case "BOLD+ITALIC":
      style = Font.BOLD + Font.ITALIC;
      break;
    default:
      style = Font.PLAIN;
      break;
    }
    font = new Font(fontName, style, Integer.parseInt(fontSize));
    return font;
  }
  
  /**
   * reloadFonts - called whenever user changes DPI.
   */
  public void reloadFonts() {
    arduinoMap.clear();
    linuxMap.clear();
    readFonts(arduinoCVS,arduinoFonts,arduinoMap);
    readFonts(linuxCVS,linuxFonts,linuxMap);
  }
  
  /**
   * Gets the font.
   *
   * @param key
   *          the key
   * @return the font
   */
  public Font getFont(String key) {
    Integer idx = Integer.valueOf(0);  // always return something...
    String target = Builder.testPlatform;
    if (target == null) {
      target = generalModel.getTarget();
    }
    if (target.equals("linux")) {
      if (linuxMap.containsKey(key)) 
        idx = linuxMap.get(key);
      return linuxFonts.get(idx.intValue()).getFont();
    } else {
      if (arduinoMap.containsKey(key)) 
        idx = arduinoMap.get(key);
      return arduinoFonts.get(idx.intValue()).getFont();
    }
  }
  
  /**
   * Gets the name of the default font for the target platform.
   *
   * @return the font name
   */
  public String getDefFontName() {
    FontItem item = null;
    String target = Builder.testPlatform;
    if (target == null) {
      target = generalModel.getTarget();
    }
    if (target.equals("linux")) {
      item = linuxFonts.get(0);
    } else {
      item = arduinoFonts.get(0);
    }
    return item.getDisplayName();
  }
  
  /**
   * Gets the name of the default font enum for the target platform.
   *
   * @return the font enum
   */
  public String getDefFontEnum() {
    FontItem item = null;
    String target = Builder.testPlatform;
    if (target == null) {
      target = generalModel.getTarget();
    }
    if (target.equals("linux")) {
      item = linuxFonts.get(0);
    } else {
      item = arduinoFonts.get(0);
    }
    return item.getFontId();
  }
  
  /**
   * Gets the font list.
   *
   * @return the font list
   */
  public List<FontItem> getFontList() {
    String target = Builder.testPlatform;
    if (target == null) {
      target = generalModel.getTarget();
    }
    if (target.equals("linux")) {
      return linuxFonts;
    } else {
      return arduinoFonts;
    }
  }
  
  /**
   * Gets the font enum.
   *
   * @param key
   *          the key (display name)
   * @return the font enum
   */
  public String getFontEnum(String key) {
    FontItem item = null;
    Integer idx = Integer.valueOf(0);  // always return something...
    String target = Builder.testPlatform;
    if (target == null) {
      target = generalModel.getTarget();
    }
    if (target.equals("linux")) {
      if (linuxMap.containsKey(key)) {
        idx = linuxMap.get(key);
      }
      item = linuxFonts.get(idx.intValue());
    } else {
      if (arduinoMap.containsKey(key)) {
        idx = arduinoMap.get(key);
      }
      item = arduinoFonts.get(idx.intValue());
    }
    return item.getFontId();
  }
  
  /**
   * Gets the font display name.
   *
   * @param key
   *          the key (font enum)
   * @return the font display name or null on failure
   */
  public String getFontDisplayName(String key) {
    String name = null;
    List<FontItem> list = getFontList();
    // this isn't called often enough to warrant anything but brute force search
    for (FontItem item : list) {
      if (item.getFontId().equals(key)) {
        name = item.getDisplayName();
        break;
      }
    }
    return name;
  }
  
  /**
   * Gets the font item.
   *
   * @param key
   *          the key
   * @return the font item
   */
  public FontItem getFontItem(String key) {
    Integer idx = Integer.valueOf(0);  // always return something...
    String target = Builder.testPlatform;
    if (target == null) {
      target = generalModel.getTarget();
    }
    if (target.equals("linux")) {
      if (linuxMap.containsKey(key)) {
        idx = linuxMap.get(key);
      }
      return linuxFonts.get(idx.intValue());
    } else {
      if (arduinoMap.containsKey(key)) {
        idx = arduinoMap.get(key);
      }
      return arduinoFonts.get(idx.intValue());
    }
  }
  
  /**
   * measureChar() - Give back the size of a character adjusted for target
   * platform.
   *
   * @param fontName
   *          the font name
   * @return the <code>dimension</code> object
   */
  public Dimension measureChar(String fontName) {
    FontItem item = getFontItem(fontName);
    Dimension nChSz = new Dimension();
    if (fontName.startsWith("BuiltIn")) {
      int size = Integer.parseInt(item.getFontSz());
      nChSz.width = (6 * size);
      nChSz.height = 8 * size;
    } else {
      int idx = -1;
      switch (item.getLogicalSize()) {
        case "9":
          idx = 0;
          break;
        case "12":
          idx = 1;
          break;
        case "18":
          idx = 2;
          break;
        case "24":
          idx = 3;
          break;
        default:
          break;
      }
      if (idx >= 0) {
        switch (item.getLogicalStyle()) {
        case "BOLD":
          nChSz = MonoBoldSizes[idx];
          break;
        case "ITALIC":
          nChSz = MonoItalicSizes[idx];
          break;
        case "BOLD+ITALIC":
          nChSz = MonoBoldItalicSizes[idx];
          break;
        default:
          nChSz = MonoPlainSizes[idx];
          break;
        }
      } else {
        String acHeight = "p$";
        String acWidth  = "W";
        Font tmpFont = createFont(item.getLogicalName(), item.getLogicalSize(), item.getLogicalStyle());
        Dimension txtHeight = measureText(acHeight, tmpFont);
        Dimension txtWidth = measureText(acWidth, tmpFont);
        nChSz.width = txtWidth.width-4;
        nChSz.height = txtHeight.height;
      }
    }
    return nChSz;
  }
  
  /**
   * Measure adafruit text.
   *
   * @param s
   *          the s
   * @param fontName
   *          the font name
   * @return the <code>dimension</code> object
   */
  public Dimension measureAdafruitText(String s, String fontName) {
    FontItem item = getFontItem(fontName);
    Dimension nChSz = new Dimension();
    int size = Integer.parseInt(item.getFontSz());
    nChSz.width = (6 * size) * s.length()+2;
    nChSz.height = (8 * size) + 4;
    return nChSz;
  }
  
  /**
   * measureText() - Give back the size of our text.
   *
   * @param s
   *          the s
   * @param font
   *          the font
   * @return the <code>dimension</code> object
   */
  public Dimension measureText(String s, Font font) {
    Canvas c = new Canvas();
    // get metrics from the Canvas
    FontMetrics metrics = c.getFontMetrics(font);
    // get the height of a line of text in this
    // font and render context
    int hgt = metrics.getHeight();
    // get the advance of my text in this font
    // and render context
    int adv = metrics.stringWidth(s);
    // calculate the size of a box to hold the
    // text with some padding.
    return new Dimension(adv+5, hgt);
  }
  
  /**
   * alignString().
   *
   * @param g
   *          the g
   * @param align
   *          - String "Left", "Right", or "Center"
   * @param r
   *          the r
   * @param s
   *          the s
   * @param font
   *          the font
   */
  public void alignString(Graphics g, String align, Rectangle r, String s, Font font) {
    FontRenderContext frc = new FontRenderContext(null, true, true);
    Rectangle2D r2D = font.getStringBounds(s, frc);
    int rHeight = (int) Math.round(r2D.getHeight());
    int rY = (int) Math.round(r2D.getY());
    int b = (r.height / 2) - (rHeight / 2) - rY;
    Canvas c = new Canvas();
    FontMetrics metrics = c.getFontMetrics(font);
    int adv = metrics.stringWidth(s);
    g.setFont(font);
    switch (align)
    {
    case "Left":
        g.drawString(s, r.x, r.y + b);
        break;
      case "Center":
        centerString(g, r, s, font);
        break;
      case "Right":
        g.drawString(s, r.x + (r.width - adv), r.y + b);
        break;
    }  
  }
  
  /**
   * This method centers a <code>String</code> in a bounding
   * <code>Rectangle</code>.
   * 
   * @param g - The <code>Graphics</code> instance.
   * @param r - The bounding <code>Rectangle</code>.
   * @param s - The <code>String</code> to center in the bounding rectangle.
   * @param font - The display font of the <code>String</code>
   * 
   * @see java.awt.Graphics
   * @see java.awt.Rectangle
   * @see java.lang.String
   */
  public void centerString(Graphics g, Rectangle r, String s, Font font) {
    FontRenderContext frc = new FontRenderContext(null, true, true);
    Rectangle2D r2D = font.getStringBounds(s, frc);
    
    int rWidth = (int) Math.round(r2D.getWidth());
    int rHeight = (int) Math.round(r2D.getHeight());
    int rY = (int)r2D.getY();
    
    int a = (r.width - rWidth) / 2;
    int b = (r.height / 2) - (rHeight / 2) - rY;

    g.setFont(font);
    g.drawString(s, r.x + a, r.y + b);
  }

  /**
   * Read fonts.
   *
   * @param csvFile
   *          the csv file
   * @param list
   *          the list
   * @param map
   *          the map
   */
  public void readFonts(String csvFile, List<FontItem> list, HashMap<String, Integer>map) {
    String line = "";
    String cvsSplitBy = ",";
    FontItem item = null;
    BufferedReader br = null;
    try {
      br = new BufferedReader(new FileReader(csvFile));
      while ((line = br.readLine()) != null) {
        if (!line.startsWith("#")) {
          // use comma as separator
          String[] f = line.split(cvsSplitBy);
          item = new FontItem(generalModel.getDPI(),
              f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8], f[9]);
          list.add(item);
        }
      }
      br.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
    // now load our font map - given the small number of fonts this may be over-kill.
    // However, users can add as many fonts as they want so...
    for (int i=0; i<list.size(); i++) {
      item = list.get(i);
      map.put(item.getDisplayName(), Integer.valueOf(i));
    }
  }
  
}
