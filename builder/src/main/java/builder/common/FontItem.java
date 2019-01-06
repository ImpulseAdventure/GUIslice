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

import java.awt.Font;

/**
 * The Class FontItem is a representation of a single GUIslice Library font.
 * 
 * @author Paul Conti
 * 
 */
public class FontItem {
  
  /** The java font. */
  private Font   font;
  
  /** The display name. */
  private String displayName;
  
  /** The include file (ex: Fonts/FreeSans9pt7b.h). */
  private String includeFile;
  
  /** The define file (ex: /usr/share/fonts/truetype/droid/DroidSans.ttf). */
  private String defineFile;
  
  /** The nFontId parameter used by GUIslice API. */
  private String nFontId;
  
  /** The e eFontRefType parameter used by GUIslice API. */
  private String eFontRefType;
  
  /** The pvFontRef parameter used by GUIslice API. */
  private String pvFontRef;
  
  /** The nFontSz parameter used by GUIslice API. */
  private String nFontSz;
  
  /** The java font name. */
  private String logicalName;
  
  /** The java font size. */
  private String logicalSize;
  
  /** The java font style. */
  private String logicalStyle;
  
  /** The scaled size. */
  private String scaledSize;
  
  /**
   * Instantiates a new font item.
   *
   * @param dpi target display pixels per inch
   * @param displayName name we show to user
   * @param includeFile name of any include file
   * @param defineFile  name of define file
   * @param nFontId GUIslice font id
   * @param eFontRefType GUIslice font type parameter
   * @param pvFontRef GUIslice font ref parameter
   * @param nFontSz GUIslice font size parameter
   * @param logicalName java font name
   * @param logicalSize java font size
   * @param logicalStyle java font style
   */
  public FontItem(int dpi,
                  String displayName,
                  String includeFile,
                  String defineFile,
                  String nFontId,
                  String eFontRefType,
                  String pvFontRef,
                  String nFontSz,
                  String logicalName,
                  String logicalSize,
                  String logicalStyle) {
    this.font = null;
    this.displayName = displayName;
    this.includeFile = includeFile;
    this.defineFile = defineFile;
    this.nFontId = nFontId;
    this.eFontRefType = eFontRefType;
    this.pvFontRef = pvFontRef;
    this.nFontSz = nFontSz;
    this.logicalName = logicalName;
    this.logicalSize = logicalSize;
    this.logicalStyle = logicalStyle;
    // We also have to take into account the target display screen's DPI
    // Adafruits's 2.8 screen is about DPI of 141. Yours likely will be different.
    // Fonts are in Points with 72 points per inch so DPI / 72 is our scaling factor.
    double scaleFactor = (double)dpi / 72.0d;
    int size = (int) ((double)Integer.parseInt(logicalSize) * scaleFactor);
    scaledSize = String.valueOf(size);
//    if (displayName.startsWith("BuiltIn"))
//      System.out.println(displayName + " size:" + logicalSize + " scale factor: " + scale factor + " resize: " + scaledSize);
    this.font = FontFactory.createFont(logicalName, scaledSize, logicalStyle);
  }
  
  /**
   * Gets the font.
   *
   * @return the java <code>Font</code> object
   */
  public Font getFont() {
    return font;
  }
  
  /**
   * Sets the font.
   *
   * @param font
   *          the new java <code>Font</code>
   */
  public void setFont(Font font) {
    this.font = font;
  }
  
  /**
   * Gets the display name.
   *
   * @return the display name
   */
  public String getDisplayName() {
    return displayName;
  }
  
  /**
   * Gets the include file.
   *
   * @return the include file
   */
  public String getIncludeFile() {
    return includeFile;
  }
  
  /**
   * Gets the define file.
   *
   * @return the define file
   */
  public String getDefineFile() {
    return defineFile;
  }
  
  /**
   * Gets the font id.
   *
   * @return the nFontId
   */
  public String getFontId() {
    return nFontId;
  }
  
  /**
   * Gets the font ref type.
   *
   * @return the eFontRefType
   */
  public String getFontRefType() {
    return eFontRefType;
  }

  /**
   * Gets the font ref.
   *
   * @return the font ref
   */
  public String getFontRef() {
    return pvFontRef;
  }

  /**
   * Gets the font sz.
   *
   * @return the nFontSz
   */
  public String getFontSz() {
    return nFontSz;
  }
  
  /**
   * Gets the logical name.
   *
   * @return the logical name
   */
  public String getLogicalName() {
    return logicalName;
  }
  
  /**
   * Gets the logical size.
   *
   * @return the logical size
   */
  public String getLogicalSize() {
    return logicalSize;
  }
  
  /**
   * Gets the scaled size.
   *
   * @return the scaled size
   */
  public String getScaledSize() {
    return scaledSize;
  }
  
  /**
   * Gets the logical style.
   *
   * @return the logical style
   */
  public String getLogicalStyle() {
    return logicalStyle;
  }
  
  /**
   * toString
   *
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Font: %s size: %s style: %s", displayName, logicalSize, logicalStyle);
  }
}
