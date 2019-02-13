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
package builder.parser;

import java.awt.Color;
import java.io.FileReader;
import java.io.IOException;
import java.lang.NumberFormatException;
import java.util.List;
import java.util.ArrayList;

import builder.common.ColorItem;
import builder.common.FontImport;
import builder.common.ColorImport;
import builder.parser.ApiNode;
import builder.parser.ParserException;
import builder.parser.Token;
import builder.parser.Tokenizer;

/**
 * A parser for importing GUIslice API UI Widgets into the GUIsliceBuilder. The
 * parser class defines a method parse() which takes a file and returns a list
 * of APINodes that holds a representation of the UI widgets that can be used to
 * import them into the GUIsliceBuilder.
 * 
 * @author Paul Conti
 * 
 */
public class Parser {
  
  /** Token id for API call names. */
  public static final int KEYWORDS = 1;
  
  /** Token id for integers. */
  public static final int INTEGER = 2;
  
  /** Token id for floats. */
  public static final int FLOAT = 3;
  
  /** Token id for word. */
  public static final int WORD = 4;
  
  /** Token id for string (alphanumerics in quotes). */
  public static final int STRING = 5;
  
  /** Token id for opening brackets. */
  public static final int OPEN_PARENTHESIS = 6;
  
  /** Token id for closing brackets. */
  public static final int CLOSE_PARENTHESIS = 7;
  
  /** Token id for opening brace. */
  public static final int OPEN_BRACE = 8;
  
  /** Token id for closing brace. */
  public static final int CLOSE_BRACE = 9;
  
  /** Token id for opening square bracket. */
  public static final int OPEN_SQBRACKET = 10;
  
  /** Token id for closing square bracket. */
  public static final int CLOSE_SQBRACKET = 11;
  
  /** Token id for comma. */
  public static final int COMMA = 12;
  
  /** Token id for equals sign. */
  public static final int EQUALS = 13;
  
  /** Token id for semicolon. */
  public static final int SEMICOLON = 14;
  
  /** Token id for special characters. */
  public static final int SPECIALCHAR = 99;

  /** The instance. */
  private static Parser instance = null;

  /** The Tokenizer. */
  private Tokenizer tokenizer;

  /** The node list. */
  private List<ApiNode> nodeList = null;

  /** The group list. */
  private List<String> groupList = null;
  
  /** The color list. */
  private List<ColorItem> colorsList = null;

  /** The font enum list. */
  private List<String> fontEnumList = null;

  /** the current token. */
  Token token = null;
  
  /** text storage name used by text node. */
  String storageName;
  
  /** text storage value used by text node. */
  String storageValue;
  
  /** text storage size used by text node. */
  String storageSz;

  /**
   * Gets the single instance of Parser.
   *
   * @return single instance of Parser
   */
  public static synchronized Parser getInstance() {
    if (instance == null) {
      instance = new Parser();
    }
    return instance;
  }

  /**
   * Default constructor.
   */
  public Parser() {
    colorsList = ColorImport.getInstance().getColorList();
    tokenizer = new Tokenizer();
    // Create our tokenizer for GUIslice API parsing
    tokenizer.defineIgnored("\\s+"); // Ignore whitespace
    tokenizer.defineIgnored("^#.*$|^\\/\\/.*$"); // Ignore comments (which start with # or //)
    tokenizer.add("[a-zA-Z][a-zA-Z0-9_]*", Parser.WORD);
    tokenizer.add("(\\.[0-9]+)|([0-9]+\\.[0-9]*)", Parser.FLOAT);
    tokenizer.add("-?[0-9]+", Parser.INTEGER);
    tokenizer.add("(\".*?\")+", Parser.STRING);
    tokenizer.add("\\(", Parser.OPEN_PARENTHESIS);
    tokenizer.add("\\)", Parser.CLOSE_PARENTHESIS);
    tokenizer.add("\\{", Parser.OPEN_BRACE);
    tokenizer.add("\\}", Parser.CLOSE_BRACE);
    tokenizer.add("\\[", Parser.OPEN_SQBRACKET);
    tokenizer.add("\\]", Parser.CLOSE_SQBRACKET);
    tokenizer.add("\\,", Parser.COMMA);
    tokenizer.add("\\=", Parser.EQUALS);
    tokenizer.add("\\;", Parser.SEMICOLON);
    tokenizer.add(".", Parser.SPECIALCHAR);

    tokenizer.defineKeywords(Parser.KEYWORDS, "gslc_PageAdd", 
        "gslc_ElemCreateBtnTxt",
        "gslc_ElemCreateBtnTxt_P",
        "gslc_ElemCreateBox",
        "gslc_ElemCreateBox_P",        
        "gslc_ElemCreateTxt",
        "gslc_ElemCreateTxt_P",
        "gslc_ElemCreateTxt_P_R",        
        "gslc_ElemSetCol", 
        "gslc_ElemSetTxtCol", 
        "gslc_ElemXCheckboxCreate",
        "gslc_ElemXCheckboxCreate_P",
        "gslc_ElemSetGroup", 
        "gslc_ElemXGaugeCreate",
        "gslc_ElemXGaugeCreate_P",
        "gslc_ElemXGaugeSetStyle",
        "gslc_ElemXGaugeSetIndicator",
        "gslc_ElemXGaugeSetTicks",
        "gslc_ElemXGraphCreate",
        "gslc_ElemXGraphSetStyle",
        "gslc_ElemXSliderCreate",
        "gslc_ElemXSliderCreate_P",        
        "gslc_ElemXSliderSetStyle",
        "gslc_ElemXTextboxCreate",
        "gslc_ElemXTextboxWrapSet",
        "gslc_ElemSetTxtAlign",
        "gslc_ElemSetFillEn",
        "gslc_FontAdd",
        "gslc_PageFindElemById",
        "gslc_ElemSetFrameEn",
        "gslc_ElemSetGlowEn",
        "gslc_ElemSetGlowCol",
        "gslc_ElemSetTxtMargin",
        "static"
        );

  }

  /**
   * Parse a Arduino .ino file or Linux .c file for GUIslice API calls
   * 
   * This is a method that reads file and parses it for GUIslice API calls and
   * creates a list of UI Widgets [API nodes] that can be used for import into the
   * GUIsliceBuilder.
   *
   * @param fileName
   *          the string holding the input
   * @param errorList
   *          the error list
   * @return the internal representation of the GUIslice UI widgets in form of an
   *         APInode list.
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   * @throws ParserException
   *           on error conditions
   */
  public List<ApiNode> parse(String fileName, List<String> errorList) throws IOException {
    storageName = null;
    storageValue = null;
    storageSz = null; 
    nodeList = new ArrayList<ApiNode>();
    groupList = new ArrayList<String>();
    fontEnumList = new ArrayList<String>();
    FileReader fr = null;
    fr = new FileReader(fileName);
    tokenizer.setSource(fr);
    while ((token = tokenizer.nextToken()).getToken() != null) {
      if (token.getType() == Parser.KEYWORDS) 
        parse_keywords(errorList);
    }
    fr.close();
    return nodeList;
  }

  /**
   * Parses the keywords.
   *
   * @param errorList
   *          the error list
   */
  public void parse_keywords(List<String> errorList)  {
    String curKeyword=token.getToken();
    try {
      if (token.getToken().equals("gslc_PageAdd")) {
        page_parser();
      } else if (token.getToken().equals("gslc_ElemCreateTxt")) {
        text_parser();
      } else if (token.getToken().equals("gslc_ElemCreateTxt_P")) {
        text_p_parser();
      } else if (token.getToken().equals("gslc_ElemCreateTxt_P_R")) {
        text_p_r_parser();
      } else if (token.getToken().equals("gslc_ElemCreateBtnTxt")) {
        txtbutton_parser();
      } else if (token.getToken().equals("gslc_ElemCreateBtnTxt_P")) {
        txtbutton_p_parser();
      } else if (token.getToken().equals("gslc_ElemCreateBox")) {
        box_parser();
      } else if (token.getToken().equals("gslc_ElemCreateBox_P")) {
        box_p_parser();
      } else if (token.getToken().equals("gslc_ElemXCheckboxCreate")) {
        checkbox_parser();
      } else if (token.getToken().equals("gslc_ElemXCheckboxCreate_P")) {
        checkbox_p_parser();
      } else if (token.getToken().equals("gslc_ElemXGaugeCreate")) {
        progressbar_parser();
      } else if (token.getToken().equals("gslc_ElemXGaugeCreate_P")) {
        progressbar_p_parser();
      } else if (token.getToken().equals("gslc_ElemXGraphCreate")) {
        graph_parser();
      } else if (token.getToken().equals("gslc_ElemXSliderCreate")) {
        slider_parser();
      } else if (token.getToken().equals("gslc_ElemXTextboxCreate")) {
        textbox_parser();
      } else if (token.getToken().equals("gslc_ElemXSliderCreate_P")) {
        slider_p_parser();
      } else if (token.getToken().equals("gslc_FontAdd")) {
        font_parser();
      } else if (token.getType() == Parser.WORD) {
        find_elemid_parser();
      } else if (token.getToken().equals("static")) {
        static_char_parser();
      }
    } catch (ParserException e) {
      String strMsg1 = "Keyword: " + curKeyword + "->" + e.toString();
      errorList.add(strMsg1);
    }
  }

  /**
   * Page parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void page_parser() throws ParserException {
    PageNode node = new PageNode();
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    node.setEnum(token.getToken());
    node.setPageEnum(token.getToken()); // point page to itself
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
  }

  /**
   * Box parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void box_parser() throws ParserException {
    BoxNode node = new BoxNode();
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    String strEnum = token.getToken();
    if (token.getType() != Parser.WORD) {
      if (token.getType() != Parser.INTEGER) parseError("enum");
      strEnum = "E_" + strEnum;
    }
    node.setEnum(strEnum);
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    node.setPageEnum(token.getToken());
    // pull out location (x,y), width and height of widget
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    parse_tsRect(node);
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_box_extras(node)) { }
  }

  /**
   * Box p parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void box_p_parser() throws ParserException {
    BoxNode node = new BoxNode();
    // parse pGui,x,y,w,h,
    parse_ard_min(node);
    // frame color
    Color c = parse_color();
    node.setFrameColor(c);
    node.setDefaultColors("false");
    // fill color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setFillColor(c);
    // bFrameEn - ignored for the moment
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bFrameEn true|false");
    // bFillEn - ignored for the moment
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bFillEn true|false");
    // pfuncXDraw 
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() == Parser.SPECIALCHAR && token.getToken() == "&") {
      // skip pass callback
      token = tokenizer.nextToken();
      if (token.getType() == Parser.WORD)
        node.setDrawFunct("true");
    } else if (token.getType() != Parser.WORD) parseError("pfuncXDraw");
    // pfuncXTick
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() == Parser.SPECIALCHAR && token.getToken() == "&") {
      // skip pass callback
      token = tokenizer.nextToken();
      if (token.getType() == Parser.WORD)
        node.setTickFunct("true");
    } else if (token.getType() != Parser.WORD) parseError("pfuncXTick");
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_box_extras(node)) { }
  }
  
  /**
   * Checkbox parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void checkbox_parser() throws ParserException {
    CheckBoxNode node = new CheckBoxNode();
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    String strEnum = token.getToken();
    if (token.getType() != Parser.WORD) {
      if (token.getType() != Parser.INTEGER) parseError("enum");
      strEnum = "E_" + strEnum;
    }
    node.setEnum(strEnum);
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    node.setPageEnum(token.getToken());
    // skip over pointer to extended element data structure
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // pull out location (x,y), width and height of widget
    parse_tsRect(node);
    // bRadio button? ignored
    // this boolean makes no sense, it only works if we have a group id
    // in any case this token should be true or false
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bRadio true|false");
    // scan for style: radio button or check box
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("GSLCX_CHECKBOX_STYLE");
    if (token.getToken().equals("GSLCX_CHECKBOX_STYLE_X")) {
      node.setType(ApiNode.CHECKBOX);
      node.setWidgetType("CheckBox");
    } else if (token.getToken().equals("GSLCX_CHECKBOX_STYLE_ROUND")) {
      node.setType(ApiNode.RADIOBUTTON);
      node.setWidgetType("RadioButton");
    } 
    // scan for mark color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    Color c = parse_color();
    node.setMarkColor(c);
    // scan for mark setting
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bChecked true|false");
    node.setChecked(token.getToken());
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_checkbox_extras(node)) { }
  }

  /**
   * Checkbox p parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void checkbox_p_parser() throws ParserException {
    CheckBoxNode node = new CheckBoxNode();
    // parse pGui,x,y,w,h,
    parse_ard_min(node);
    // fill color
    Color c = parse_color();
    node.setFillColor(c);
    // skip over bFillEn which isn't currently supported by builder
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // grab group id
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("group id");
    if (!token.getToken().equals("GSLC_GROUP_ID_NONE")) {
      // now search for index inside our group list
      boolean bFound = false;
      int n=0;
      for(String search : groupList) {
        if (search.equals(token.getToken())) {
          node.setGroup(n+1);
          bFound=true;
          break;
        }
      }
      if(!bFound)
      {
        groupList.add(token.getToken());
        node.setGroup(groupList.size());
      }
    }        
    // bRadio button? ignored
    // this boolean makes no sense, it only works if we have a group id.
    // in any case this token should be true or false
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bRadio true|false");
    // scan for style: radio button or check box
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("GSLCX_CHECKBOX_STYLE");
    if (token.getToken().equals("GSLCX_CHECKBOX_STYLE_X")) {
      node.setType(ApiNode.CHECKBOX);
      node.setWidgetType("CheckBox");
    } else if (token.getToken().equals("GSLCX_CHECKBOX_STYLE_ROUND")) {
      node.setType(ApiNode.RADIOBUTTON);
      node.setWidgetType("RadioButton");
    } 
    // scan for mark color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setMarkColor(c);
    // scan for mark setting
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bChecked true|false");
    node.setChecked(token.getToken());
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_checkbox_extras(node)) { }
  }

  /**
   * Graph parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void graph_parser() throws ParserException {
    GraphNode node = new GraphNode();
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    String strEnum = token.getToken();
    if (token.getType() != Parser.WORD) {
      if (token.getType() != Parser.INTEGER) parseError("enum");
      strEnum = "E_" + strEnum;
    }
    node.setEnum(strEnum);
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    node.setPageEnum(token.getToken());
    // skip over pointer to extended element data structure
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // pull out location (x,y), width and height of widget
    parse_tsRect(node);
    // grab font enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("font enum");
    String strFontEnum = token.getToken();
    // skip over buffer pointer
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // grab number of rows
    token = tokenizer.nextToken();
    if (token.getType() != Parser.INTEGER) parseError("#rows");
    node.setNumRows(token.getToken());
    // parse graph color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    Color c = parse_color();
    node.setGraphColor(c);
    // node is complete
    nodeList.add(node);
    fontEnumList.add(strFontEnum);
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_graph_extras(node)) { }
  }

  /**
   * Progressbar parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void progressbar_parser() throws ParserException {
    ProgressBarNode node = new ProgressBarNode();
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    String strEnum = token.getToken();
    if (token.getType() != Parser.WORD) {
      if (token.getType() != Parser.INTEGER) parseError("enum");
      strEnum = "E_" + strEnum;
    }
    node.setEnum(strEnum);
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    node.setPageEnum(token.getToken());
    // skip over pointer to extended element data structure
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // pull out location (x,y), width and height of widget
    parse_tsRect(node);  
    // pull out min, max, and starting value
    try {
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("min value");
      node.setMin(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("max value");
      node.setMax(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("start value");
      node.setCurValue(token.getToken());
    } catch (NumberFormatException e) {
      throw new ParserException("NumberFormatException: " + e.toString());
    }
    // scan for color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    Color c = parse_color();
    node.setIndicatorColor(c);
    // scan for orientation of bar
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bVertical true|false");
    node.setVertical(token.getToken());
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_progressbar_extras(node)) { }
  }

  /**
   * Progressbar p parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void progressbar_p_parser() throws ParserException {
    ProgressBarNode node = new ProgressBarNode();
    // parse pGui,x,y,w,h,
    parse_ard_min(node);
    // pull out min, max, and starting value
    try {
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("min value");
      node.setMin(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("max value");
      node.setMax(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("start value");
      node.setCurValue(token.getToken());
    } catch (NumberFormatException e) {
      throw new ParserException("NumberFormatException: " + e.toString());
    }
    // look for frame color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    Color c = parse_color();
    node.setFrameColor(c);
    node.setDefaultColors("false");
    // now fill color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setFillColor(c);
    // scan for indicator color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setIndicatorColor(c);
    // scan for orientation of bar
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bVertical true|false");
    node.setVertical(token.getToken());
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_progressbar_extras(node)) { }
  }
  
  /**
   * Slider parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void slider_parser() throws ParserException {
    SliderNode node = new SliderNode();
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    String strEnum = token.getToken();
    if (token.getType() != Parser.WORD) {
      if (token.getType() != Parser.INTEGER) parseError("enum");
      strEnum = "E_" + strEnum;
    }
    node.setEnum(strEnum);
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    node.setPageEnum(token.getToken());
    // skip over pointer to extended element data structure
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // pull out location (x,y), width and height of widget
    parse_tsRect(node);
    // pull out min, max, starting value, and thumb size
    try {
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("min value");
      node.setMin(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("max value");
      node.setMax(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("start value");
      node.setCurValue(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("thumb size");
      node.setThumbSize(token.getToken());
    } catch (NumberFormatException e) {
      throw new ParserException("NumberFormatException: " + e.toString());
    }
    // scan for orientation of slider
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bVertical true|false");
    node.setVertical(token.getToken());
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_slider_extras(node)) { }
  }

  /**
   * Slider p parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void slider_p_parser() throws ParserException {
    SliderNode node = new SliderNode();
    // parse pGui,x,y,w,h,
    parse_ard_min(node);
    // pull out min, max, starting value, and thumb size
    try {
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("min value");
      node.setMin(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("max value");
      node.setMax(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("start value");
      node.setCurValue(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("thumb size");
      node.setThumbSize(token.getToken());
    } catch (NumberFormatException e) {
      throw new ParserException("NumberFormatException: " + e.toString());
    }
    // scan for orientation of slider
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bVertical true|false");
    node.setVertical(token.getToken());
    // look for frame color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    Color c = parse_color();
    node.setFrameColor(c);
    // now fill color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setFillColor(c);
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_slider_extras(node)) { }
  }
  
  /**
   * Text parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void text_parser() throws ParserException {
    TextNode node = new TextNode();
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    String strEnum = token.getToken();
    if (token.getType() != Parser.WORD) {
      if (token.getType() != Parser.INTEGER) parseError("enum");
      strEnum = "E_" + strEnum;
    }
    node.setEnum(strEnum);
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    node.setPageEnum(token.getToken());
    // pull out location (x,y), width and height of widget
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    parse_tsRect(node);
    // grab our text
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() == Parser.OPEN_PARENTHESIS) {
      // format must be (char*)"text",
      while ((token = tokenizer.nextToken()).getType() != Parser.CLOSE_PARENTHESIS) {}
      token = tokenizer.nextToken();
    }
    // NOTE: if the token is not a string its most likely storage mstr type
    if (token.getType() == Parser.STRING) {
      String noQuotes = token.getToken().replaceAll("\"", "");
      if (noQuotes.length() > 0)
        node.setText(noQuotes);
    }
    if (token.getType() == Parser.WORD) {
      if (storageName != null && storageName.equals(token.getToken())) {
        node.setTextStorage(storageSz);
        if (storageValue != null) 
          node.setText(storageValue);
      }
    }
    // go pass storage - most often 0
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // save font enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("font enum");
    fontEnumList.add(token.getToken());
    node.setFontEnum(token.getToken());
    // node is complete
    nodeList.add(node);
    storageName=null;
    storageValue=null;
    storageSz=null;
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_text_extras(node)) { }
  }

  /**
   * Text p parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void text_p_parser() throws ParserException {
    TextNode node = new TextNode();
    // parse pGui,x,y,w,h,
    parse_ard_min(node);
    // grab text
    token = tokenizer.nextToken();
    if (token.getType() == Parser.STRING) {
      // remove all quote marks and check for empty string
      String noQuotes = token.getToken().replaceAll("\"", "");
      if (noQuotes.length() > 0)
        node.setText(noQuotes);
    }
    // skip over Pointer to font resource since we have no semantic information to guide us
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // text color
    Color c = parse_color();
    node.setTextColor(c);
    node.setDefaultColors("false");
    // frame color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setFrameColor(c);
    // fill color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setFillColor(c);
    // text alignment
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("alignment keyword");
    if (token.getToken().equals("GSLC_ALIGN_MID_MID"))
      node.setAlignment("Center");
    else if (token.getToken().equals("GSLC_ALIGN_MID_RIGHT"))
      node.setAlignment("Right");
    else 
      node.setAlignment("Left");
    // bFrameEn - ignored for the moment
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bFrameEn true|false");
    // bFillEn
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bFrameEn true|false");
    node.setFillEnabled(token.getToken());
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    storageName=null;
    storageValue=null;
    storageSz=null;
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_text_extras(node)) { }
  }

  /**
   * Text p r parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void text_p_r_parser() throws ParserException {
    TextNode node = new TextNode();
    // parse pGui,x,y,w,h,
    parse_ard_min(node);
    // text here is actual storage like mstr
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("mstr?");
    if (storageName != null && storageName.equals(token.getToken())) {
      node.setTextStorage(storageSz);
      if (storageValue != null) 
        node.setText(storageValue);
    }
    // next should be storage size
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() == Parser.INTEGER) {
      node.setTextStorage(token.getToken());
    } else {
      // not integer so skip pass storage - might be sizeof(mstrxxx)
      while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    }
    // skip over Pointer to font resource since we have no semantic information to guide us
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // text color
    Color c = parse_color();
    node.setTextColor(c);
    node.setDefaultColors("false");
    // frame color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setFrameColor(c);
    // fill color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setFillColor(c);
    // text alignment
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("alignment keyword");
    if (token.getToken().equals("GSLC_ALIGN_MID_MID"))
      node.setAlignment("Center");
    else if (token.getToken().equals("GSLC_ALIGN_MID_RIGHT"))
      node.setAlignment("Right");
    else 
      node.setAlignment("Left");
    // bFrameEn - ignored for the moment
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bFrameEn true|false");
    // bFillEn
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("bFrameEn true|false");
    node.setFillEnabled(token.getToken());
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    storageName=null;
    storageValue=null;
    storageSz=null;
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_text_extras(node)) { }
  }

  /**
   * Txtbutton parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void txtbutton_parser() throws ParserException {
    TxtButtonNode node = new TxtButtonNode();
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    String strEnum = token.getToken();
    if (token.getType() != Parser.WORD) {
      if (token.getType() != Parser.INTEGER) parseError("enum");
      strEnum = "E_" + strEnum;
    }
    node.setEnum(strEnum);
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    node.setPageEnum(token.getToken());
    // pull out location (x,y), width and height of widget
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    parse_tsRect(node);
    // grab our button label
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() == Parser.OPEN_PARENTHESIS) {
      // format must be (char*)"text",
      while ((token = tokenizer.nextToken()).getType() != Parser.CLOSE_PARENTHESIS) {}
      token = tokenizer.nextToken();
    }
    if (token.getType() == Parser.STRING) {
      // remove all quote marks and check for empty string
      String noQuotes = token.getToken().replaceAll("\"", "");
      if (noQuotes.length() > 0)
        node.setText(noQuotes);
    }
    // go pass storage - most often 0
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // save font enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("font enum");
    fontEnumList.add(token.getToken());
    node.setFontEnum(token.getToken());
    // node is complete
    nodeList.add(node);
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_txtbutton_extras(node)) { }
  }

  /**
   * Txtbutton p parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void txtbutton_p_parser() throws ParserException {
    TxtButtonNode node = new TxtButtonNode();
    // parse pGui,x,y,w,h,
    parse_ard_min(node);
    // grab text
    token = tokenizer.nextToken();
    if (token.getType() == Parser.STRING) {
      // remove all quote marks and check for empty string
      String noQuotes = token.getToken().replaceAll("\"", "");
      if (noQuotes.length() > 0)
        node.setText(noQuotes);
    }
    // skip over Pointer to font resource since we have no semantic information to guide us
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // text color
    Color c = parse_color();
    node.setTextColor(c);
    node.setDefaultColors("false");
    // frame color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setFrameColor(c);
    // fill color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setFillColor(c);
    // frame when glowing color - ignored
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    // fill when glowing color
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    c = parse_color();
    node.setSelectedColor(c);
    // skip over over the rest
    // node is complete
    nodeList.add(node);
    fontEnumList.add("NONE");
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_txtbutton_extras(node)) { }
  }

  /**
   * Text Box parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void textbox_parser() throws ParserException {
    TextBoxNode node = new TextBoxNode();
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    String strEnum = token.getToken();
    if (token.getType() != Parser.WORD) {
      if (token.getType() != Parser.INTEGER) parseError("enum");
      strEnum = "E_" + strEnum;
    }
    node.setEnum(strEnum);
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    node.setPageEnum(token.getToken());
    // skip over pointer to extended element data structure
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // pull out location (x,y), width and height of widget
    parse_tsRect(node);
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    // save font enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("font enum");
    node.setFontEnum(token.getToken());
    // skip over storage since we can't map it
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    while ((token=tokenizer.nextToken()).getType() != Parser.COMMA) { }
    // pull out rows and columns
    try {
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("rows value");
      node.setNumTextRows(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("columns value");
      node.setNumTextColumns(token.getToken());
    } catch (NumberFormatException e) {
      throw new ParserException("NumberFormatException: " + e.toString());
    }
    // node is complete
    fontEnumList.add(node.getFontEnum());
    nodeList.add(node);
    // now loop until we find a ';'
    parse_EOS();
    // now parse any extra calls that might affect this node
    while (parse_textbox_extras(node)) { }
  }

  /**
   * Font parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void font_parser() throws ParserException {
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("font enum");
    String strFontEnum = token.getToken();
    // find eFontRefType
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("eFontRefType");
    String strFontRefType = token.getToken();
    // find pvFontRef
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() == Parser.SPECIALCHAR && token.getToken().equals("&"))
      token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("pvFontRef");
    String strFontRef = token.getToken();
    // find nFontSz
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.INTEGER) parseError("nFontSz");
    String strFontSz = token.getToken();
    // now that we have finished parsing place our font
    // into the matching nodes
    // but first determine if arduino or linux fonts
    if (strFontRefType.equals("GSLC_FONTREF_PTR")) {
      process_arduino_fonts(strFontEnum, strFontRef, strFontSz);
    } else if (strFontRefType.equals("GSLC_FONTREF_FNAME")) {
      process_linux_fonts(strFontEnum, strFontSz);
    } else
      throw new ParserException("Missing 'eFontRefType' " + token.getToken() +" line: " + token.getLineNumber());
    // now loop until we find a ';'
    parse_EOS();
  }
  
  /**
   * Find elemid parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void find_elemid_parser() throws ParserException {
    // here we parse something like:
    //  gslc_tsElemRef* pElemCnt = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
    //  or pElemCnt = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_TXT_COUNT);
    if (token.getToken().equals("gslc_tsElemRef")) {
      token = tokenizer.nextToken();
      // start with '*'?
      if (token.getType() != Parser.SPECIALCHAR) return;
      if (!token.getToken().equals("*")) return;
      token = tokenizer.nextToken();
    }
    if (token.getType() != Parser.WORD) return;
    String strElemRef = token.getToken();
    token = tokenizer.nextToken();
    if (token.getType() != Parser.EQUALS) return;
    token = tokenizer.nextToken();
    if (token.getType() != Parser.KEYWORDS) return;
    if (!token.getToken().equals("gslc_PageFindElemById")) {
      tokenizer.pushToken();
      return;
    }
    // skip over Pointer to GUI
    parse_pGUI();
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    // now grab our ENUM
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("enum");
    String strNodeEnum = token.getToken();
    // search our nodes for this enum and set the ElemRef if we find it
    for (ApiNode n : nodeList) {
      if (strNodeEnum.equals(n.getEnum())) {
        updateNode_ElemRef(n, strElemRef);
        break;
      }
    }
  }
  
  /**
   * Static char parser.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void static_char_parser() throws ParserException {
    // search for construct like:
    // static char mstr1[8] = "";
    // that would hold storage for a text field
    String tempName=null;
    storageName = null;
    storageValue = null;
    storageSz = null;
    // see if we have 'char'
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) return;
    if (!token.getToken().equals("char")) return;
    // look for mstrxx
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) return;
    tempName = token.getToken();
    // now we parse out [n] for the size
    token = tokenizer.nextToken();
    if (token.getType() != Parser.OPEN_SQBRACKET) return;
    token = tokenizer.nextToken();
    if (token.getType() != Parser.INTEGER) return;
    storageSz = token.getToken();
    storageName = tempName;
    token = tokenizer.nextToken();
    if (token.getType() != Parser.CLOSE_SQBRACKET) return;
    // scan for '= "some string"' 
    token = tokenizer.nextToken();
    if (token.getType() != Parser.EQUALS) return;
    token = tokenizer.nextToken();
    if (token.getType() != Parser.STRING) return;
    // remove all quote marks and check for empty string
    storageValue = token.getToken().replaceAll("\"", "");
    int count = Integer.parseInt(storageSz);
    if (storageValue.length() == 0) {
      // fill with '?'
      for(int i=0; i<count-1; i++) {
        storageValue = storageValue + "?";
      }
    } 
    // now loop until we find a ';'
    parse_EOS();
  }

  // -------------------------------------------------------------------------------------------------------
  // Parser methods to search for extra GUIslice api calls that might apply to a particular UI widget
  /**
   * Parses the box extras.
   *
   * @param node
   *          the node
   * @return <code>true</code>, if successful
   * @throws ParserException
   *           the parser exception
   */
  // -------------------------------------------------------------------------------------------------------
  public boolean parse_box_extras(BoxNode node) throws ParserException {
    Color c = null;
    token = tokenizer.nextToken();
    if (token.getToken().equals("gslc_ElemSetCol")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setDefaultColors("false");
      node.setFrameColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setFillColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setSelectedColor(c);
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetFrameEn")) {
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetGlowEn")) {
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetGlowCol")) {
      parse_EOS();
      return true;
    }
    tokenizer.pushToken();
    return false;
  }
  
  /**
   * Parses the checkbox extras.
   *
   * @param node
   *          the node
   * @return <code>true</code>, if successful
   * @throws ParserException
   *           the parser exception
   */
  public boolean parse_checkbox_extras(CheckBoxNode node) throws ParserException {
    Color c = null;
    token = tokenizer.nextToken();
    if (token.getToken().equals("gslc_ElemSetGroup")) {
      // skip over Pointer to GUI and pElemRef
      parse_pGUI_and_pElemRef();
      // grab group enum
      token = tokenizer.nextToken();
      if (token.getType() != Parser.WORD) parseError("group enum");
      // now search for index inside our group list
      boolean bFound = false;
      int n=0;
      for(String search : groupList) {
        if (search.equals(token.getToken())) {
          node.setGroup(n+1);
          bFound=true;
          break;
        }
      }
      if(!bFound)
      {
        groupList.add(token.getToken());
        node.setGroup(groupList.size());
      }
      // now loop until we find a ';'
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetCol")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setDefaultColors("false");
      node.setFrameColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setFillColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setSelectedColor(c);
      parse_EOS();
      return true;
    }
    tokenizer.pushToken();
    return false;
  }
  
  /**
   * Parses the graph extras.
   *
   * @param node
   *          the node
   * @return <code>true</code>, if successful
   * @throws ParserException
   *           the parser exception
   */
  public boolean parse_graph_extras(GraphNode node) throws ParserException {
    Color c = null;
    token = tokenizer.nextToken();
    if (token.getToken().equals("gslc_ElemSetCol")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setFrameColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setFillColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setSelectedColor(c);
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemXGraphSetStyle")) {
      parse_pGUI_and_pElemRef();
      token = tokenizer.nextToken();
      if (token.getType() != Parser.WORD) parseError("graph style");
      if (token.getToken().equals("GSLCX_GRAPH_STYLE_DOT")) {
        node.setGraphStyle("Dot");
      } else if (token.getToken().equals("GSLCX_GRAPH_STYLE_FILL")) {
        node.setGraphStyle("Fill");
      } else if (token.getToken().equals("GSLCX_GRAPH_STYLE_LINE")) {
        node.setGraphStyle("Line");  // currently unimplemented in GUIslice api
      } else parseError("graph style");
      parse_EOS();
      return true;
    } else if (token.getType() == Parser.WORD) {
      // scan for something like: m_pElemCnt = pElemRef;
      String strElemRef = token.getToken();
      token = tokenizer.nextToken();
      if (token.getType() == Parser.EQUALS) {
        token = tokenizer.nextToken();
        if (token.getType() == Parser.WORD && token.getToken().equals("pElemRef")) {
          node.setElementRef(strElemRef);
          parse_EOS();
          return true;
        }
      }
    }
    tokenizer.pushToken();
    return false;
  }
  
  /**
   * Parses the progress bar extras.
   *
   * @param node
   *          the node
   * @return <code>true</code>, if successful
   * @throws ParserException
   *           the parser exception
   */
  public boolean parse_progressbar_extras(ProgressBarNode node) throws ParserException {
    Color c = null;
    token = tokenizer.nextToken();
    if (token.getToken().equals("gslc_ElemXGaugeSetStyle")) {
      parse_pGUI_and_pElemRef(); 
      token = tokenizer.nextToken();
      if (token.getType() != Parser.WORD) parseError("gauge style"); 
      if (token.getToken().equals("GSLCX_GAUGE_STYLE_RAMP"))
        node.setGaugeStyle(token.getToken());
      else if (token.getToken().equals("GSLCX_GAUGE_STYLE_PROG_BAR"))
        node.setGaugeStyle(token.getToken());
      else if (token.getToken().equals("GSLCX_GAUGE_STYLE_RADIAL"))
        node.setGaugeStyle(token.getToken());
      else
        parseError("gauge style");
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetCol")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setDefaultColors("false");
      node.setFrameColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setFillColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setSelectedColor(c);
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemXGaugeSetIndicator")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setIndicatorColor(c);
      try {
        token = tokenizer.nextToken();
        if (token.getType() != Parser.COMMA) parseError(",");
        token = tokenizer.nextToken();
        if (token.getType() != Parser.INTEGER) parseError("divsions?");
        node.setIndicatorSize(token.getToken());
        token = tokenizer.nextToken();
        if (token.getType() != Parser.COMMA) parseError(",");
        token = tokenizer.nextToken();
        if (token.getType() != Parser.INTEGER) parseError("tick size?");
        node.setIndicatorTipSize(token.getToken());
      } catch (NumberFormatException e) {
        throw new ParserException("gslc_ElemXSliderSetStyle NumberFormatException: " + e.toString());
      }
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.WORD) parseError("indicator fill true|false");
      node.setIndicatorFill(token.getToken());
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemXGaugeSetTicks")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setTickColor(c);
      try {
        token = tokenizer.nextToken();
        if (token.getType() != Parser.COMMA) parseError(",");
        token = tokenizer.nextToken();
        if (token.getType() != Parser.INTEGER) parseError("divsions?");
        node.setDivisions (token.getToken());
        token = tokenizer.nextToken();
        if (token.getType() != Parser.COMMA) parseError(",");
        token = tokenizer.nextToken();
        if (token.getType() != Parser.INTEGER) parseError("tick size?");
        node.setTickSize(token.getToken());
      } catch (NumberFormatException e) {
        throw new ParserException("gslc_ElemXSliderSetStyle NumberFormatException: " + e.toString());
      }
      parse_EOS();
      return true;
    } else if (token.getType() == Parser.WORD) {
      // scan for something like: m_pElemProgress = pElemRef;
      String strElemRef = token.getToken();
      token = tokenizer.nextToken();
      if (token.getType() == Parser.EQUALS) {
        token = tokenizer.nextToken();
        if (token.getType() == Parser.WORD && token.getToken().equals("pElemRef")) {
          node.setElementRef(strElemRef);
          parse_EOS();
          return true;
        }
      }
    } 
    tokenizer.pushToken();
    return false;
  }
  
  /**
   * Parses the slider extras.
   *
   * @param node
   *          the node
   * @return <code>true</code>, if successful
   * @throws ParserException
   *           the parser exception
   */
  public boolean parse_slider_extras(SliderNode node) throws ParserException {
    Color c = null;
    token = tokenizer.nextToken();
    if (token.getToken().equals("gslc_ElemXSliderSetStyle")) {
      parse_pGUI_and_pElemRef(); 
      // look for trim style
      token = tokenizer.nextToken();
      if (token.getType() != Parser.WORD) parseError("trim style?");
      node.setTrimStyle(token.getToken());
      // grab trim color
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setTrimColor(c);
      try {
        token = tokenizer.nextToken();
        if (token.getType() != Parser.COMMA) parseError(",");
        token = tokenizer.nextToken();
        if (token.getType() != Parser.INTEGER) parseError("divsions?");
        node.setDivisions(token.getToken());
        token = tokenizer.nextToken();
        if (token.getType() != Parser.COMMA) parseError(",");
        token = tokenizer.nextToken();
        if (token.getType() != Parser.INTEGER) parseError("tick size?");
        node.setTickSize(token.getToken());
      } catch (NumberFormatException e) {
        throw new ParserException("gslc_ElemXSliderSetStyle NumberFormatException: " + e.toString());
      }
      // grab tick color
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setTickColor(c);
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetCol")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setFrameColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setFillColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setSelectedColor(c);
      parse_EOS();
      return true;
    } else if (token.getType() == Parser.WORD) {
      // scan for something like: m_pElemSlider = pElemRef;
      String strElemRef = token.getToken();
      token = tokenizer.nextToken();
      if (token.getType() == Parser.EQUALS) {
        token = tokenizer.nextToken();
        if (token.getType() == Parser.WORD && token.getToken().equals("pElemRef")) {
          node.setElementRef(strElemRef);
          parse_EOS();
          return true;
        }
      }
    }
    tokenizer.pushToken();
    return false;
  }
  
  /**
   * Parses the text extras.
   *
   * @param node
   *          the node
   * @return <code>true</code>, if successful
   * @throws ParserException
   *           the parser exception
   */
  public boolean parse_text_extras(TextNode node) throws ParserException {
    Color c = null;
    token = tokenizer.nextToken();
    if (token.getToken().equals("gslc_ElemSetTxtCol")) {
      parse_pGUI_and_pElemRef(); 
      // scan for color
      c = parse_color();
      node.setTextColor(c);
      node.setDefaultColors("false");
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetTxtAlign")) {
      parse_pGUI_and_pElemRef(); 
      token = tokenizer.nextToken();
      if (token.getType() != Parser.WORD) parseError("alignment keyword");
      if (token.getToken().equals("GSLC_ALIGN_MID_MID"))
        node.setAlignment("Center");
      else if (token.getToken().equals("GSLC_ALIGN_MID_RIGHT"))
        node.setAlignment("Right");
      else 
        node.setAlignment("Left");
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetFillEn")) {
      parse_pGUI_and_pElemRef();
      token = tokenizer.nextToken();
      if (token.getType() != Parser.WORD) parseError("bFillEn true|false");
      node.setFillEnabled(token.getToken());
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetCol")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setDefaultColors("false");
      node.setFrameColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setFillColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setSelectedColor(c);
      parse_EOS();
      return true;
    } else if (token.getType() == Parser.WORD) {
      // scan for something like: m_pElemCnt = pElemRef;
      String strElemRef = token.getToken();
      token = tokenizer.nextToken();
      if (token.getType() == Parser.EQUALS) {
        token = tokenizer.nextToken();
        if (token.getType() == Parser.WORD && token.getToken().equals("pElemRef")) {
          node.setElementRef(strElemRef);
          parse_EOS();
          return true;
        }
      }
    } else if (token.getToken().equals("gslc_ElemSetFrameEn")) {
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetGlowEn")) {
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetGlowCol")) {
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetTxtMargin")) {
      parse_EOS();
      return true;
    }
    tokenizer.pushToken();
    return false;
  }
  
  /**
   * Parses the txtbutton extras.
   *
   * @param node
   *          the node
   * @return <code>true</code>, if successful
   * @throws ParserException
   *           the parser exception
   */
  public boolean parse_txtbutton_extras(TxtButtonNode node) throws ParserException {
    Color c = null;
    token = tokenizer.nextToken();
    if (token.getToken().equals("gslc_ElemSetTxtCol")) {
      parse_pGUI_and_pElemRef(); 
      // scan for color
      c = parse_color();
      node.setTextColor(c);
      node.setDefaultColors("false");
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetCol")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setDefaultColors("false");
      node.setFrameColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setFillColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setSelectedColor(c);
      parse_EOS();
      return true;
    }
    tokenizer.pushToken();
    return false;
  }
  
  /**
   * Parses the text box extras.
   *
   * @param node
   *          the node
   * @return <code>true</code>, if successful
   * @throws ParserException
   *           the parser exception
   */
  public boolean parse_textbox_extras(TextBoxNode node) throws ParserException {
    Color c = null;
    token = tokenizer.nextToken();
    if (token.getToken().equals("gslc_ElemXTextboxWrapSet")) {
      parse_pGUI_and_pElemRef(); 
      // look for trim style
      token = tokenizer.nextToken();
      if (token.getType() != Parser.WORD) parseError("Enable line wrap?");
      node.setWrapText(token.getToken());
      parse_EOS();
      return true;
    } else if (token.getToken().equals("gslc_ElemSetCol")) {
      parse_pGUI_and_pElemRef();
      c = parse_color();
      node.setFrameColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setFillColor(c);
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      c = parse_color();
      node.setSelectedColor(c);
      parse_EOS();
      return true;
    } else if (token.getType() == Parser.WORD) {
      // scan for something like: m_pElemSlider = pElemRef;
      String strElemRef = token.getToken();
      token = tokenizer.nextToken();
      if (token.getType() == Parser.EQUALS) {
        token = tokenizer.nextToken();
        if (token.getType() == Parser.WORD && token.getToken().equals("pElemRef")) {
          node.setElementRef(strElemRef);
          parse_EOS();
          return true;
        }
      }
    }
    tokenizer.pushToken();
    return false;
  }
  
  // -------------------------------------------------------------------------------------------------------
  // Update node methods
  /**
   * Process arduino fonts.
   *
   * @param strFontEnum
   *          the str font enum
   * @param strFontRef
   *          the str font ref
   * @param strFontSz
   *          the str font sz
   */
  // -------------------------------------------------------------------------------------------------------
  public void process_arduino_fonts(String strFontEnum, String strFontRef, String strFontSz) {
    if (strFontRef.equals("NULL")) {
      String name = FontImport.getInstance().getArduinoFontName(strFontSz);
      update_fontName(strFontEnum, name);
    }
    // is strFontRef a pointer to in memory font structure?
    if (FontImport.getInstance().nameMatch(strFontRef)) {
      update_fontName(strFontEnum, strFontRef);
    } else {
      String name = FontImport.getInstance().getArduinoFontName(strFontSz);
      update_fontName(strFontEnum, name);
    }
  }
  
  /**
   * Process linux fonts.
   *
   * @param strFontEnum
   *          the str font enum
   * @param strFontSz
   *          the str font sz
   */
  public void process_linux_fonts(String strFontEnum, String strFontSz) {
    String name = FontImport.getInstance().getLinuxFontName(strFontSz);
    update_fontName(strFontEnum, name);
  }
  
  /**
   * Update font name.
   *
   * @param strFontEnum
   *          the str font enum
   * @param strFontRef
   *          the str font ref
   */
  public void update_fontName(String strFontEnum, String strFontRef) {
    int i=0;
    for (String s : fontEnumList) {
      if (s.equals(strFontEnum)) {
        updateNode_font(i, strFontRef);
      }
      i++;
    }
  }
  
  /**
   * Update node font.
   *
   * @param i
   *          the i
   * @param strFontRef
   *          the str font ref
   */
  public void updateNode_font(int i, String strFontRef) {
    ApiNode a = nodeList.get(i);
    if (a.getType() == ApiNode.TEXT) {
      if (((TextNode) a).getFontDisplayName() == null)
        ((TextNode) a).setFontDisplayName(strFontRef);
    } else if (a.getType() == ApiNode.TXTBUTTON) {
      if (((TxtButtonNode) a).getFontDisplayName() == null)
        ((TxtButtonNode) a).setFontDisplayName(strFontRef);
    } else if (a.getType() == ApiNode.TEXTBOX) {
      if (((TextBoxNode) a).getFontDisplayName() == null)
        ((TextBoxNode) a).setFontDisplayName(strFontRef);
    }
  }
  
  /**
   * Update node elem ref.
   *
   * @param a
   *          the a
   * @param strElemRef
   *          the str elem ref
   */
  public void updateNode_ElemRef(ApiNode a, String strElemRef) {
    if (a.getType() == ApiNode.TEXT) {
      ((TextNode) a).setElementRef(strElemRef);
    } else if (a.getType() == ApiNode.PROGRESSBAR) {
      ((ProgressBarNode) a).setElementRef(strElemRef);
    } else if (a.getType() == ApiNode.SLIDER) {
      ((SliderNode) a).setElementRef(strElemRef);
    } else if (a.getType() == ApiNode.GRAPH) {
      ((GraphNode) a).setElementRef(strElemRef);
    } else if (a.getType() == ApiNode.TEXTBOX) {
      ((TextBoxNode) a).setElementRef(strElemRef);
    }
  }
  
  // -------------------------------------------------------------------------------------------------------
  // Helper methods
  // -------------------------------------------------------------------------------------------------------

  /**
   * Parses the p GUI.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void parse_pGUI() throws ParserException {
    token = tokenizer.nextToken();
    if (token.getType() != Parser.OPEN_PARENTHESIS) parseError("(");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.SPECIALCHAR) parseError("&");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("m_gui");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
  }

  /**
   * Parses the p GU I and p elem ref.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void parse_pGUI_and_pElemRef() throws ParserException {
    token = tokenizer.nextToken();
    if (token.getType() != Parser.OPEN_PARENTHESIS) parseError("(");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.SPECIALCHAR) parseError("&");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("m_gui");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    // parse pElemRef
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("pElemRef");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
  }

  /**
   * Parses the ts rect.
   *
   * @param node
   *          the node
   * @throws ParserException
   *           the parser exception
   */
  public void parse_tsRect(ApiNode node) throws ParserException {
    token = tokenizer.nextToken();
    if (token.getType() != Parser.OPEN_PARENTHESIS) parseError("(");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD && !token.getToken().equals("gslc_tsRect"))
      parseError("gslc_tsRect");
    try {
      token = tokenizer.nextToken();
      if (token.getType() != Parser.CLOSE_PARENTHESIS) parseError(")"); 
      token = tokenizer.nextToken();
      if (token.getType() != Parser.OPEN_BRACE) parseError("{");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("x value");
      node.setX(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("y value");
      node.setY(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("width");
      node.setWidth(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("height");
      node.setHeight(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.CLOSE_BRACE) parseError("}");
    } catch (NumberFormatException e) {
      throw new ParserException("tsRect NumberFormatException: " + e.toString());
    }
  }

  /**
   * Parses the ard min.
   *
   * @param n
   *          the n
   * @throws ParserException
   *           the parser exception
   */
  public void parse_ard_min(ApiNode n)  throws ParserException {
    token = tokenizer.nextToken();
    if (token.getType() != Parser.OPEN_PARENTHESIS) parseError("(");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.SPECIALCHAR) parseError("&");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("m_gui");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    // now grab and set the ENUM
    token = tokenizer.nextToken();
    String strEnum = token.getToken();
    if (token.getType() != Parser.WORD) {
      if (token.getType() != Parser.INTEGER) parseError("enum");
      strEnum = "E_" + strEnum;
    }
    n.setEnum(strEnum);
    // now grab page enum
    token = tokenizer.nextToken();
    if (token.getType() != Parser.COMMA) parseError(",");
    token = tokenizer.nextToken();
    if (token.getType() != Parser.WORD) parseError("page enum");
    n.setPageEnum(token.getToken());
    // parse out x,y,w, and h
    try {
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("x value");
      n.setX(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("y value");
      n.setY(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("width");
      n.setWidth(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
      token = tokenizer.nextToken();
      if (token.getType() != Parser.INTEGER) parseError("height");
      n.setHeight(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != Parser.COMMA) parseError(",");
    } catch (NumberFormatException e) {
      throw new ParserException("tsRect NumberFormatException: " + e.toString());
    }
  }
  
  /**
   * Parses the EOS.
   *
   * @throws ParserException
   *           the parser exception
   */
  public void parse_EOS() throws ParserException {
    while ((token = tokenizer.nextToken()).getType() != Parser.SEMICOLON) {
    }
  }

  /**
   * parse_color() - convert string to a java Color object. Example: A string such
   * as GSLC_COL_RED will be converted to new Color(255,0,0) while a string like
   * (gslc_tsColor){255,0,0} will also be new Color(255,0,0)
   *
   * @return java Color object representing the color
   * @throws ParserException
   *           the parser exception
   */
  public Color parse_color() throws ParserException {
    int r,g,b;
    token = tokenizer.nextToken();
    if (token.getType() == Parser.OPEN_PARENTHESIS) {
      // format must be something like (gslc_tsColor){255,0,0}
      token = tokenizer.nextToken();
      if (token.getToken().equals("gslc_tsColor")) {
        token = tokenizer.nextToken();
        if (token.getType() != Parser.CLOSE_PARENTHESIS) parseError(")");
        token = tokenizer.nextToken();
        if (token.getType() != Parser.OPEN_BRACE) parseError("{");
        try {
          token = tokenizer.nextToken();
          if (token.getType() != Parser.INTEGER) parseError("red number");
          r = Integer.valueOf(token.getToken());
          token = tokenizer.nextToken();
          if (token.getType() != Parser.COMMA) parseError(",");
          token = tokenizer.nextToken();
          if (token.getType() != Parser.INTEGER) parseError("green number");
          g = Integer.valueOf(token.getToken());
          token = tokenizer.nextToken();
          if (token.getType() != Parser.COMMA) parseError(",");
          token = tokenizer.nextToken();
          if (token.getType() != Parser.INTEGER) parseError("blue number");
          b = Integer.valueOf(token.getToken());
          token = tokenizer.nextToken();
          if (token.getType() != Parser.CLOSE_BRACE) parseError("}");
          return new Color(r,g,b);
        } catch (NumberFormatException e) {
          throw new ParserException("NumberFormatException: " + e.toString());
        }
      }
    } else if (token.getType() == Parser.WORD) {
      // format must be a GUIslice color #define like GSLC_COL_RED
      String search = token.getToken();
      for (int i=1; i<colorsList.size(); i++) {
        ColorItem item = colorsList.get(i);
        if (item.getDisplayName().equals(search)) {
           return item.getColor();
        }
      }
    } 
    return null;
  }

  /**
   * Parses the error.
   *
   * @param s
   *          the s
   * @throws ParserException
   *           the parser exception
   */
  public void parseError(String s) throws ParserException {
    throw new ParserException("Missing '" + s + "' found <"+ token.getToken() +"> line: " + token.getLineNumber());
  }
}
