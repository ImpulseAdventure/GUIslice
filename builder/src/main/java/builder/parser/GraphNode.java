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
 * The Class GraphNode implements the parser node for the Graph widget.
 * 
 * @author Paul Conti
 * 
 */
public class GraphNode extends ApiNode { 
  
  /** The Constant for gslc_tsElemRef* m_pElementRef name */
  public static final String ELEMENTREF_NAME = "m_pElemGraph";
  
  /** The Constant PROP_ELEMENTREF. */
  static private final int PROP_ELEMENTREF        = 5;
  
  /** The Constant PROP_FONT. */
  static private final int PROP_FONT              = 6;
  
  /** The Constant PROP_ROWS. */
  static private final int PROP_ROWS              = 7;
  
  /** The Constant PROP_STYLE. */
  static private final int PROP_STYLE             = 8;
  
  /** The Constant PROP_GRAPH_COLOR. */
  static private final int PROP_GRAPH_COLOR       = 9;
  
  /** The Constant PROP_DEFAULT_COLORS. */
  static private final int PROP_DEFAULT_COLORS    = 10;
  
  /** The Constant PROP_FRAME_COLOR. */
  static private final int PROP_FRAME_COLOR       = 11;
  
  /** The Constant PROP_FILL_COLOR. */
  static private final int PROP_FILL_COLOR        = 12;
  
  /** The Constant PROP_SELECTED_COLOR. */
  static private final int PROP_SELECTED_COLOR    = 13;

  /**
   * Instantiates a new graph node.
   */
  public GraphNode() {
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    apiType = ApiNode.GRAPH;
    widgetType = "Graph";
    data = new Object[14][3];
    
    initCommonProps();

    initProp(PROP_ELEMENTREF, "TXT-206","ElementRef");
    initProp(PROP_FONT, "TXT-200","Font");

    initProp(PROP_ROWS, "GRPH-100","Maximum Points");
    initProp(PROP_STYLE, "GRPH-101","Graph Style");

    initProp(PROP_GRAPH_COLOR, "COL-309","Color of Graph");
    initProp(PROP_DEFAULT_COLORS, "COL-300","Use Default Colors?");
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
   * Sets the font display name.
   *
   * @param the font display name
   */
  public void setFontDisplayName(String s) {
    setValue(s, PROP_FONT);
  }
  
  /**
   * Sets the graph color.
   *
   * @param the graph color
   */
  public void setGraphColor(Color c) {
    setValue(c, PROP_GRAPH_COLOR);
  }
  
  /**
   * Sets the num rows.
   *
   * @param the num rows
   */
  public void setNumRows(String n) {
    setValue(Integer.valueOf(n), PROP_ROWS);
  }

  /**
   * Sets the graph style.
   *
   * @param the graph style
   */
  public void setGraphStyle(String s) {
    setValue(s, PROP_STYLE);
  }
  
  /**
  * sets the default color.
  *
  * @param true uses default colors, false otherwise
  */
  public void setDefaultColors(boolean value) 
  {
    setValue(Boolean.valueOf(value), PROP_DEFAULT_COLORS);
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
