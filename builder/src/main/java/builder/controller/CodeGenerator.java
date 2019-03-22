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
package builder.controller;

import java.awt.Color;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import builder.Builder;
import builder.common.ColorFactory;
import builder.common.CommonUtil;
import builder.common.EnumFactory;
import builder.common.FontFactory;
import builder.common.FontItem;
import builder.common.TestException;
import builder.models.BoxModel;
import builder.models.CheckBoxModel;
import builder.models.GeneralModel;
import builder.models.GraphModel;
import builder.models.ImageModel;
import builder.models.ImgButtonModel;
import builder.models.ProgressBarModel;
import builder.models.RadioButtonModel;
import builder.models.SliderModel;
import builder.models.TextBoxModel;
import builder.models.TextModel;
import builder.models.TxtButtonModel;
import builder.models.WidgetModel;
import builder.prefs.GeneralEditor;
import builder.views.PagePane;
import builder.widgets.Widget;

/**
 * The Class CodeGenerator does the actual creation of the C skeleton
 * used by GUIslice API mapped to the specific platform, arduino, or linux.
 * <p>
 * Much of the code is driven by templates that are keyed by Tags within the
 * C skeletons.
 * </p>
 * <ul>
 * <li>ino.t for arduino C skeleton
 * <li>min.t for arduino using flash C skeleton
 * <li>c.t for linux C skeleton
 * <li>arduino.t for arduino code templates
 * <li>arduino_min.t for arduino flash code templates
 * <li>linux.t for linux code templates
 * 
 * Kind of a Quick and Dirty implementation.
 * 
 * @author Paul Conti
 * 
 */
public class CodeGenerator {
  
  /** The instance. */
  private static CodeGenerator instance = null;
  
  /** The Constant ARDUINO_PLATFORM. */
  private final static int    ARDUINO_PLATFORM       = 0;
  
  /** The Constant LINUX_PLATFORM. */
  private final static int    LINUX_PLATFORM         = 1;
  
  /** The Constant ARDUINO_MIN_PLATFORM. */
  private final static int    ARDUINO_MIN_PLATFORM   = 2;
  
  /** The target platform. */
  private static int    targetPlatform         = 0;
  
  /** The max string length */
  private static int maxstr_len;

  /** The Constant ARDUINO_FONT_TEMPLATE. */
  public  final static String ARDUINO_FONT_TEMPLATE  = "arduinofonts.csv";
  
  /** The Constant DROIDFONTS_TEMPLATE. */
  public  final static String DROIDFONTS_TEMPLATE    = "droidfonts.csv";
  
  /** The Constant FREEFONTS_TEMPLATE. */
  public  final static String FREEFONTS_TEMPLATE     = "freefonts.csv";
  
  /** The Constant LINUX_FONT_TEMPLATE. */
  public  final static String LINUX_FONT_TEMPLATE    = "linuxfonts.csv";
  
  /** The Constant SRC_SD for reading image from SD card. */
  public  final static String SRC_SD   = "gslc_GetImageFromSD((const char*)";
  
  /** The Constant SRC_PROG for reading image from FLASH or PROGMEM. */
  public  final static String SRC_PROG = "gslc_GetImageFromProg((const unsigned char*)";

  /** The Constant SRC_RAM for reading image from SRAM. */
  public  final static String SRC_RAM  = "gslc_GetImageFromRam((unsigned char*)";
  
  /** The Constant FROM_SD for reading image from SD card. */
  public  final static String FROM_SD   = "From File";
  
  /** The Constant FROM_PROG for reading image from FLASH or PROGMEM. */
  public  final static String FROM_PROG = "From PROGMEM (FLASH)";

  /** The Constant FROM_RAM for reading image from SRAM. */
  public  final static String FROM_RAM  = "From SRAM";
  
  /** The Constant ARDUINO_RES. */
  public  final static String ARDUINO_RES            = "arduino_res";
  
  /** The Constant LINUX_RES. */
  public  final static String LINUX_RES              = "linux_res";
  
  /** The Constant ARDUINO_FILE. */
  public  final static String ARDUINO_FILE           = "ino.t";
  
  /** The Constant LINUX_FILE. */
  public  final static String LINUX_FILE             = "c.t";
  
  /** The Constant ARDUINO_MIN_FILE. */
  public  final static String ARDUINO_MIN_FILE       = "min.t";
  
  /** The Constant ALIGN_TEMPLATE. */
  private final static String ALIGN_TEMPLATE         = "<ALIGN>";
  
  /** The Constant BACKGROUND_TEMPLATE. */
  private final static String BACKGROUND_TEMPLATE    = "<BACKGROUND>";
  
  /** The Constant BOX_TEMPLATE. */
  private final static String BOX_TEMPLATE           = "<BOX>";
  
  /** The Constant BUTTON_CB_TEMPLATE. */
  private final static String BUTTON_CB_TEMPLATE     = "<BUTTON_CB>";
  
  /** The Constant BUTTON_LOOP_TEMPLATE. */
  private final static String BUTTON_LOOP_TEMPLATE   = "<BUTTON_CB_LOOP>";
  
  /** The Constant BUTTON_CHGPG_TEMPLATE. */
  private final static String BUTTON_CHGPG_TEMPLATE  = "<BUTTON_CB_CHGPAGE>";
  
  /** The Constant CHECKBOX_TEMPLATE. */
  private final static String CHECKBOX_TEMPLATE      = "<CHECKBOX>";
  
  /** The Constant COLOR_TEMPLATE. */
  private final static String COLOR_TEMPLATE         = "<COLOR>";
  
  /** The Constant DEFINE_PG_MAX */
  private final static String DEFINE_PG_MAX_TEMPLATE = "<DEFINE_PG_MAX>";
  
  /** The Constant DRAWFUNC_TEMPLATE. */
  private final static String DRAWFUNC_TEMPLATE      = "<DRAWFUNC>";
  
  /** The Constant DRAWBOX_CB_TEMPLATE. */
  private final static String DRAWBOX_CB_TEMPLATE    = "<DRAWBOX_CB>";
  
  /** The Constant FONT_LOAD_TEMPLATE. */
  private final static String FONT_LOAD_TEMPLATE     = "<FONT_LOAD>";
  
  /** The Constant GRAPH_TEMPLATE. */
  private final static String GRAPH_TEMPLATE         = "<GRAPH>";
  
  /** The Constant GUI_ELEMENT_TEMPLATE. */
  private final static String GUI_ELEMENT_TEMPLATE   = "<GUI_ELEMENT>";
  
  /** The Constant GROUP_TEMPLATE. */
  private final static String GROUP_TEMPLATE         = "<GROUP>";
  
  /** The Constant IMAGE_TEMPLATE. */
  private final static String IMAGE_TEMPLATE         = "<IMAGE>";
  
  /** The Constant IMGBUTTON_TEMPLATE. */
  private final static String IMGBUTTON_TEMPLATE     = "<IMGBUTTON>";
  
  /** The Constant IMAGETRANSPARENT_TEMPLATE. */
  private final static String IMAGETRANSPARENT_TEMPLATE = "<IMAGETRANSPARENT>";
  
  /** The Constant PAGEADD_TEMPLATE. */
  private final static String PAGEADD_TEMPLATE       = "<PAGEADD>"; 
  
  /** The Constant PROGMEM_TEMPLATE. */
  private final static String PROGMEM_TEMPLATE       = "<PROGMEM>"; 
  
  /** The Constant PROGRESSBAR_TEMPLATE. */
  private final static String PROGRESSBAR_TEMPLATE   = "<PROGRESSBAR>";
  
  /** The Constant PROGRESSBARSTYLE_TEMPLATE. */
  private final static String PROGRESSBARSTYLE_TEMPLATE = "<PROGRESSBARSTYLE>";
  
  /** The Constant PROGRESSBARIND_TEMPLATE. */
  private final static String PROGRESSBARIND_TEMPLATE   = "<PROGRESSBARIND>";
  
  /** The Constant PROGRESSBARTICKS_TEMPLATE. */
  private final static String PROGRESSBARTICKS_TEMPLATE = "<PROGRESSBARTICKS>";
  
  /** The Constant RADIOBUTTON_TEMPLATE. */
  private final static String RADIOBUTTON_TEMPLATE   = "<RADIOBUTTON>";
  
  /** The Constant ROTATE_TEMPLATE. */
  private final static String ROTATE_TEMPLATE        = "<ROTATE>";
  
  /** The Constant SLIDER_TEMPLATE. */
  private final static String SLIDER_TEMPLATE        = "<SLIDER>";
  
  /** The Constant SLIDER_CB_TEMPLATE. */
  private final static String SLIDER_CB_TEMPLATE     = "<SLIDER_CB>";
  
  /** The Constant SLIDER_LOOP_TEMPLATE. */
  private final static String SLIDER_LOOP_TEMPLATE   = "<SLIDER_CB_LOOP>";
  
  /** The Constant STARTUP_TEMPLATE. */
  private final static String STARTUP_TEMPLATE       = "<STARTUP>";
  
  /** The Constant TEXT_TEMPLATE. */
  private final static String TEXT_TEMPLATE          = "<TEXT>";
  
  /** The Constant TEXTBOX_TEMPLATE. */
  private final static String TEXTBOX_TEMPLATE       = "<TEXTBOX>";
  
  /** The Constant TEXTCOLOR_TEMPLATE. */
  private final static String TEXTCOLOR_TEMPLATE     = "<TEXT_COLOR>";
  
  /** The Constant TEXT_UPDATE_TEMPLATE. */
  private final static String TEXT_UPDATE_TEMPLATE   = "<TEXT_UPDATE>";
  
  /** The Constant TICK_CB_TEMPLATE. */
  private final static String TICK_CB_TEMPLATE       = "<TICK_CB>";
  
  /** The Constant TICKFUNC_TEMPLATE. */
  private final static String TICKFUNC_TEMPLATE      = "<TICKFUNC";
  
  /** The Constant TXTBUTTON_TEMPLATE. */
  private final static String TXTBUTTON_TEMPLATE     = "<TXTBUTTON>";
  
  /** The Constant TXTBUTTON_UPDATE_TEMPLATE. */
  private final static String TXTBUTTON_UPDATE_TEMPLATE = "<TXTBUTTON_UPDATE>";
  
  /** The Constant STOP_TEMPLATE. */
  private final static String STOP_TEMPLATE          = "<STOP>";
  
  /** The Constant END_TEMPLATE. */
  private final static String END_TEMPLATE           = "<END>";
  
  /** The Constant ARDUINO_EXT. */
  public  final static String ARDUINO_EXT            = ".ino";
  
  /** The Constant LINUX_EXT. */
  public  final static String LINUX_EXT              = ".c";

  /** The Constant RESOURCES_PATH. */
  private final static String RESOURCES_PATH         = "/resources/templates/";

  /** The Constant PREFIX. */
  private final static String PREFIX                 = "//";
  
  /** The Constant BUTTON_CB_SECTION. */
  private final static String BUTTON_CB_SECTION      = "//<Button Callback !Start!>";
  
  /** The Constant BUTTON_CB_END. */
  private final static String BUTTON_CB_END          = "//<Button Callback !End!>";
  
  /** The Constant BUTTON_ENUMS_SECTION. */
  private final static String BUTTON_ENUMS_SECTION   = "//<Button Enums !Start!>";
  
  /** The Constant BUTTON_ENUMS_END. */
  private final static String BUTTON_ENUMS_END       = "//<Button Enums !End!>";
  
  /** The Constant DEFINES_SECTION. */
  private final static String DEFINES_SECTION        = "//<ElementDefines !Start!>";
  
  /** The Constant DEFINES_END. */
  private final static String DEFINES_END            = "//<ElementDefines !End!>";
  
  /** The Constant DRAW_CB_SECTION. */
  private final static String DRAW_CB_SECTION        = "//<Draw Callback !Start!>";
  
  /** The Constant DRAW_CB_END. */
  private final static String DRAW_CB_END            = "//<Draw Callback !End!>";
  
  /** The Constant ENUM_SECTION. */
  private final static String ENUM_SECTION           = "//<Enum !Start!>";
  
  /** The Constant ENUM_END. */
  private final static String ENUM_END               = "//<Enum !End!>";
  
  /** The Constant FILE_SECTION. */
  private final static String FILE_SECTION           = "//<File !Start!>";
  
  /** The Constant FILE_END. */
  private final static String FILE_END               = "//<File !End!>";
  
  /** The Constant FONT_SECTION. */
  private final static String FONT_SECTION           = "//<Fonts !Start!>";
  
  /** The Constant FONT_END. */
  private final static String FONT_END               = "//<Fonts !End!>";
  
  /** The Constant GUI_SECTION. */
  private final static String GUI_SECTION            = "//<GUI_Extra_Elements !Start!>";
  
  /** The Constant GUI_END. */
  private final static String GUI_END                = "//<GUI_Extra_Elements !End!>";
  
  /** The Constant INITGUI_SECTION. */
  private final static String INITGUI_SECTION        = "//<InitGUI !Start!>";
  
  /** The Constant INITGUI_END. */
  private final static String INITGUI_END            = "//<InitGUI !End!>";
  
  /** The Constant LOADFONT_SECTION. */
  private final static String LOADFONT_SECTION       = "//<Load_Fonts !Start!>";
  
  /** The Constant LOADFONT_END. */
  private final static String LOADFONT_END           = "//<Load_Fonts !End!>";
  
  /** The Constant PATH_SECTION. */
  private final static String PATH_SECTION           = "//<PathStorage !Start!>";
  
  /** The Constant PATH_END. */
  private final static String PATH_END               = "//<PathStorage !End!>";
  
  /** The Constant QUICK_SECTION. */
  private final static String QUICK_SECTION          = "//<Quick_Access !Start!>";
  
  /** The Constant QUICK_END. */
  private final static String QUICK_END              = "//<Quick_Access !End!>";
  
  /** The Constant RESOURCES_SECTION. */
  private final static String RESOURCES_SECTION      = "//<Resources !Start!>";
  
  /** The Constant RESOURCES_END. */
  private final static String RESOURCES_END          = "//<Resources !End!>";
  
  /** The Constant SAVEREF_SECTION. */
  private final static String SAVEREF_SECTION        = "//<Save_References !Start!>";
  
  /** The Constant SAVEREF_END. */
  private final static String SAVEREF_END            = "//<Save_References !End!>";
  
  /** The Constant SLIDER_CB_SECTION. */
  private final static String SLIDER_CB_SECTION      = "//<Slider Callback !Start!>";
  
  /** The Constant SLIDER_CB_END. */
  private final static String SLIDER_CB_END          = "//<Slider Callback !End!>";
  
  /** The Constant SLIDER_ENUMS_SECTION. */
  private final static String SLIDER_ENUMS_SECTION   = "//<Slider Enums !Start!>";
  
  /** The Constant SLIDER_ENUMS_END. */
  private final static String SLIDER_ENUMS_END       = "//<Slider Enums !End!>";
  
  /** The Constant STARTUP_SECTION. */
  private final static String STARTUP_SECTION         = "//<Startup !Start!>";
  
  /** The Constant STARTUP_END. */
  private final static String STARTUP_END             = "//<Startup !End!>";
  
  /** The Constant TICK_CB_SECTION. */
  private final static String TICK_CB_SECTION        = "//<Tick Callback !Start!>";
  
  /** The Constant TICK_CB_END. */
  private final static String TICK_CB_END            = "//<Tick Callback !End!>";
  
  /** The Constant BACKGROUND_COLOR_MACRO. */
  private final static String BACKGROUND_COLOR_MACRO = "BACKGROUND_COLOR";
  
  /** The Constant BOOL_MACRO. */
  private final static String BOOL_MACRO             = "BOOL";

  /** The Constant CHECKED_MACRO. */
  private final static String CHECKED_MACRO          = "CHECKED";
  
  /** The Constant COLS_MACRO. */
  private final static String COLS_MACRO             = "COLS";
  
  /** The Constant COUNT_MACRO. */
  private final static String COUNT_MACRO            = "COUNT";
  
  /** The Constant ENUM_MACRO. */
  private final static String ENUM_MACRO             = "WIDGET_ENUM";
  
  /** The Constant DIVISIONS_MACRO. */
  private final static String DIVISIONS_MACRO        = "DIVISIONS";
  
  /** The Constant DRAWFUNC_MACRO. */
  private final static String DRAWFUNC_MACRO         = "DRAWFUNC";
  
  /** The Constant FLASH_MACRO. */
  private final static String FLASH_MACRO            = "FLASH";
  
  /** The Constant FILL_COLOR_MACRO. */
  private final static String FILL_COLOR_MACRO       = "FILL_COLOR";
  
  /** The Constant FILL_EN_MACRO. */
  private final static String FILL_EN_MACRO          = "FILL_EN";
  
  /** The Constant FONT_COUNT_MACRO. */
  private final static String FONT_COUNT_MACRO       = "FONT_COUNT";
  
  /** The Constant FONT_ID_MACRO. */
  private final static String FONT_ID_MACRO          = "FONT_ID";
  
  /** The Constant FONT_REF_MACRO. */
  private final static String FONT_REF_MACRO         = "FONT_REF";
  
  /** The Constant FONT_REFTYPE_MACRO. */
  private final static String FONT_REFTYPE_MACRO     = "FONT_REFTYPE";
  
  /** The Constant FONT_SZ_MACRO. */
  private final static String FONT_SZ_MACRO          = "FONT_SZ";
  
  /** The Constant FRAME_COLOR_MACRO. */
  private final static String FRAME_COLOR_MACRO      = "FRAME_COLOR";
  
  /** The Constant FRAME_EN_MACRO. */
  private final static String FRAME_EN_MACRO         = "FRAME_EN";
  
  /** The Constant GLOW_COLOR_MACRO. */
  private final static String GLOW_COLOR_MACRO       = "GLOW_COLOR";
  
  /** The Constant GRAPH_COLOR_MACRO. */
  private final static String GRAPH_COLOR_MACRO      = "GRAPH_COLOR";
  
  /** The Constant GROUP_ID_MACRO. */
  private final static String GROUP_ID_MACRO         = "GROUP_ID";
  
  /** The Constant HEIGHT_MACRO. */
  private final static String HEIGHT_MACRO           = "HEIGHT";
  
  /** The Constant ID_MACRO. */
  private final static String ID_MACRO               = "ID";
  
  /** The Constant IMAGE_MACRO. */
  private final static String IMAGE_MACRO            = "IMAGE_DEFINE";
  
  /** The Constant IMAGE_FROM_MACRO. */
  private final static String IMAGE_FROM_MACRO       = "IMAGE_FROM_SRC";
  
  /** The Constant IMAGE_SEL_MACRO. */
  private final static String IMAGE_SEL_MACRO        = "IMAGE_SEL_DEFINE";
  
  /** The Constant IMAGE_SOURCE_MACRO. */
  private final static String IMAGE_SOURCE_MACRO     = "IMAGE_SOURCE";
  
  /** The Constant IMAGE_SOURCE_MACRO. */
  private final static String IMAGE_SEL_SOURCE_MACRO = "IMAGE_SEL_SOURCE";
  
  /** The Constant IMAGE_FORMAT_MACRO. */
  private final static String IMAGE_FORMAT_MACRO     = "IMAGE_FORMAT";
  
  /** The Constant MARK_COLOR_MACRO. */
  private final static String MARK_COLOR_MACRO       = "MARK_COLOR";
  
  /** The Constant MIN_MACRO. */
  private final static String MIN_MACRO              = "MIN";
  
  /** The Constant MAX_MACRO. */
  private final static String MAX_MACRO              = "MAX";
  
  /** The Constant PAGE_ENUM_MACRO. */
  private final static String PAGE_ENUM_MACRO        = "PAGE_ENUM";
  
  /** The Constant ROTATION_MACRO. */
  private final static String ROTATION_MACRO         = "ROTATION";
  
  /** The Constant ROWS_MACRO. */
  private final static String ROWS_MACRO             = "ROWS";
  
  /** The Constant SIZE_MACRO. */
  private final static String SIZE_MACRO             = "SIZE";
  
  /** The Constant STRIP_ENUM_MACRO. */
  private final static String STRIP_ENUM_MACRO        = "STRIP_ENUM";
  
  /** The Constant STYLE_MACRO. */
  private final static String STYLE_MACRO            = "STYLE";
  
  /** The Constant TEXT_MACRO. */
  private final static String TEXT_MACRO             = "TEXT";
  
  /** The Constant TEXT_ALIGN_MACRO. */
  private final static String TEXT_ALIGN_MACRO       = "TEXT_ALIGN";
  
  /** The Constant TEXT_COLOR_MACRO. */
  private final static String TEXT_COLOR_MACRO       = "TEXT_COLOR";
  
  /** The Constant THUMBSZ_MACRO. */
  private final static String THUMBSZ_MACRO          = "THUMBSZ";
  
  /** The Constant TICKFUNC_MACRO. */
  private final static String TICKFUNC_MACRO         = "TICKFUNC";
  
  /** The Constant TICKSZ_MACRO. */
  private final static String TICKSZ_MACRO           = "TICKSZ";
  
  /** The Constant TIPSZ_MACRO. */
  private final static String TIPSZ_MACRO            = "TIPSZ";
  
  /** The Constant TRIM_COLOR_MACRO. */
  private final static String TRIM_COLOR_MACRO       = "TRIM_COLOR";
  
  /** The Constant VALUE_MACRO. */
  private final static String VALUE_MACRO            = "VALUE";
  
  /** The Constant WIDTH_MACRO. */
  private final static String WIDTH_MACRO            = "WIDTH";
  
  /** The Constant X_MACRO. */
  private final static String X_MACRO                = "X";
  
  /** The Constant Y_MACRO. */
  private final static String Y_MACRO                = "Y";
  
  /** The Constant MACRO_PATTERN is our regex search pattern '$<xxx>'. */
  private static final Pattern MACRO_PATTERN = Pattern.compile("\\$\\<(.+?)\\>");

  /** The Constant BEGIN_LINE. */
  // finite state machine for printing enums
  private final static int BEGIN_LINE    = 0;
  
  /** The Constant WRITE_NEXT. */
  private final static int WRITE_NEXT    = 1;
  
  /** The Constant OVERFLOW_LINE. */
  private final static int OVERFLOW_LINE = 2;
  
  /** The print state. */
  int printState = BEGIN_LINE;
  
  /** The backup file. */
  private File backupFile=null;
  
  /** The backup name. */
  private String backupName=null;
  
  /** The fr. */
  private FileReader fr;
  
  /** The br. */
  private BufferedReader br;
  
  /** The fw. */
  private FileWriter fw;
  
  /** The pw. */
  private PrintWriter pw;
  
  /** The cf. */
  private static ColorFactory cf;
  
  /** The ff. */
  private static FontFactory ff=null;
  
  /** The pages. */
  private List<PagePane> pages;
  
  /** The project file. */
  File projectFile = null;
  
  /** The target. */
  static String target = null;
  
  /** The file extension. */
  static String fileExtension = null;
  
  /** The default file name. */
  String defaultFileName = null;
  
  /** The template file name. */
  String templateFileName = null;
  
  /** The font list of enums. */
  List<Pair> fontEnums = new ArrayList<Pair>();
  
  /** The template map. */
  HashMap<String, Integer> templateMap;
  
  /** The ref list. */
  List<WidgetModel> refList = new ArrayList<WidgetModel>();

  /** The list of templates. */
  List<String>[] listOfTemplates = null;
  
  /** The macro. */
  String[] macro = new String[30];
  
  /** The replacement. */
  String[] replacement = new String[30];
  
  /** The count check boxes. */
  // its possible that a better way of handling this exists but brute force works so...
  int countCheckBoxes = 0;
  
  /** The count gauges. */
  int countGauges = 0;
  
  /** The count sliders. */
  int countSliders = 0;

  /** The line. */
  String line  = "";
  
  /** The background color */
  private Color background;
  
  /** The Screen Width 
   * Set here for JUnit testing 
   * Otherwise set by GeneralModel values
   */
  private int screenWidth = 320;
  
  /** The Screen Height 
   * Set here for JUnit testing 
   * Otherwise set by GeneralModel values
   */
  private int screenHeight = 240;
  
  /**
   * Gets the single instance of CodeGenerator.
   *
   * @return single instance of CodeGenerator
   */
  public static synchronized CodeGenerator getInstance() {
    if (instance == null) {
      instance = new CodeGenerator();
    }
    return instance;
  }
  
  /**
   * Get a Test Instance for JUnit tests
   * @return instance of CodeGenerator
   * @throws TestException on read of resource failure
   */
  public static synchronized CodeGenerator getTestInstance() throws TestException {
    if (instance == null) {
      instance = new CodeGenerator();
      cf = ColorFactory.getInstance();
      ff = FontFactory.getInstance();
    }
    return instance;
  }
  
  /**
   * Instantiates a new code generator.
   */
  public CodeGenerator() {
  }
  
  /**
   * Generate code setup.
   *
   * @param fileName
   *          the file name
   * @param pages
   *          the pages
   */
  @SuppressWarnings("unchecked")
  public String generateCode(String folder, String fileName, List<PagePane> pages) {
    this.pages = pages;
    countCheckBoxes = 0;
    countGauges = 0;
    countSliders = 0;
    GeneralModel generalModel = (GeneralModel) GeneralEditor.getInstance().getModel();
    screenWidth = generalModel.getWidth();
    screenHeight = generalModel.getHeight();
    cf = ColorFactory.getInstance();
    ff = FontFactory.getInstance();
    target = generalModel.getTarget();
    maxstr_len = generalModel.getMaxStr();
    background = generalModel.getFillColor();
    cf = ColorFactory.getInstance();
    ff = FontFactory.getInstance();
    target = generalModel.getTarget();
    maxstr_len = generalModel.getMaxStr();
    background = generalModel.getFillColor();
    listOfTemplates = new ArrayList[64];
    switch(target) {
      case "linux":
        defaultFileName = LINUX_FILE;
        fileExtension = LINUX_EXT;
        storeTemplates("linux.t");
        targetPlatform = LINUX_PLATFORM;
        break;      
      case "arduino_min":
        defaultFileName = ARDUINO_MIN_FILE;
        fileExtension = ARDUINO_EXT;
        storeTemplates("arduino_min.t");
        targetPlatform = ARDUINO_MIN_PLATFORM;
        break;
      default:
        defaultFileName = ARDUINO_FILE;
        fileExtension = ARDUINO_EXT;
        storeTemplates("arduino.t");
        targetPlatform = ARDUINO_PLATFORM;
        break;
    }
    if (fileName == null) {
      return null;
    }
    int n = fileName.indexOf(".prj");
    String tmp = fileName.substring(0,n);
    String skeletonName = new String(folder + System.getProperty("file.separator") + tmp + fileExtension);
    projectFile = new File(skeletonName);
    backupName = null;
    backupFile = null;
    if(projectFile.exists()) {
      // Make a backup copy of projectFile
      backupName = new String(skeletonName + ".bak");
      CommonUtil.getInstance().backupFile(projectFile);
    }
    try {
      // Here we are either going to use a previously generated file as input
      // or we are generating a brand new file from one of our templates.
      // if backupFile == null then its a brand new file
      // I do all of this so users can create a file, then edit it do a run on a target platform
      // and go back and add or subtract widgets from the same file and not lose edits.
      if (backupName == null) {
        String fullPath = CommonUtil.getInstance().getWorkingDir();
        String tName = fullPath + "templates" + System.getProperty("file.separator") 
            + defaultFileName;
        File template = new File(tName);
        fr = new FileReader(template);
      } else {
        backupFile = new File(backupName);
        fr = new FileReader(backupFile);
      }
      br = new BufferedReader(fr);
      doCodeGen();
      return skeletonName;
    } catch (IOException e) {
      e.printStackTrace();
      return null;
    }      
  }      
  
  /**
   * Generate test code setup.
   *
   * @param outName
   *          the file name for output of code generation
   * @param  pages
   *          the pages
   * @throws TestException on read of resources
   * @throws IOException on file i/o
   */
  @SuppressWarnings("unchecked")
  public void generateTestCode(String testPlatform, String outName, List<PagePane> pages) 
      throws TestException, IOException {
    target = testPlatform;
    this.pages =  pages;
    countCheckBoxes = 0;
    countGauges = 0;
    countSliders = 0;
    background = Color.GRAY;
    maxstr_len = 100;
    listOfTemplates = new ArrayList[32];
    if (outName == null) 
      throw new TestException("Missing Name of Output Project File");
    switch(target) {
      case "linux":
        defaultFileName = LINUX_FILE;
        fileExtension = LINUX_EXT;
        storeTemplates("linux.t");
        targetPlatform = LINUX_PLATFORM;
        break;      
      case "arduino_min":
        defaultFileName = ARDUINO_MIN_FILE;
        fileExtension = ARDUINO_EXT;
        storeTemplates("arduino_min.t");
        targetPlatform = ARDUINO_MIN_PLATFORM;
        break;
      default:
        defaultFileName = ARDUINO_FILE;
        fileExtension = ARDUINO_EXT;
        storeTemplates("arduino.t");
        targetPlatform = ARDUINO_PLATFORM;
        break;
    }
    String fileName = new String(outName + fileExtension);
    // prompt the user to enter output name
    projectFile = new File(fileName);
    if (projectFile == null) 
      throw new TestException("Creation of Output File Failed");
    
    String resourcePath = "/resources/templates/" + defaultFileName;
    InputStream in;
    in = Builder.class.getResourceAsStream(resourcePath);
    if (in == null) {
      throw new TestException("missing resource: " + resourcePath);
    }
    br = new BufferedReader(new InputStreamReader(in));
    doCodeGen();
  }
  
  /**
   * doCodeGen is the main code generation loop.
   */
  public void doCodeGen() { 
    fontEnums.clear();
    refList.clear();
    try {
			fw = new FileWriter(projectFile);
			pw = new PrintWriter(fw);
      while ((line = br.readLine()) != null) {
        if (!line.startsWith(PREFIX)) {
          // we need to remove old artifacts
          if (line.startsWith("gslc_tsElem") && line.contains("m_asMainElem"))
            continue;
          pw.printf("%s%n",line);
        } else {
          if (line.equals(FONT_SECTION)) {
            fontSection();
          } else if (line.equals(RESOURCES_SECTION)) {
            resourcesSection();
          } else if (line.equals(ENUM_SECTION)) {
            enumSection();
          } else if (line.equals(DEFINES_SECTION)) {
            definesSection();
          } else if (line.equals(GUI_SECTION)) {
            guiSection();
          } else if (line.equals(SAVEREF_SECTION)) {
            saveRefSection();
          } else if (line.equals(BUTTON_CB_SECTION)) {
            buttonCbSection();
          } else if (line.equals(DRAW_CB_SECTION)) {
            drawCbSection();
          } else if (line.equals(SLIDER_CB_SECTION)) {
            sliderCbSection();
          } else if (line.equals(STARTUP_SECTION)) {
            startupSection();
          } else if (line.equals(TICK_CB_SECTION)) {
            tickCbSection();
          } else if (line.equals(INITGUI_SECTION)) {
            initGUISection();
          } else if (line.equals(QUICK_SECTION)) {
            quickAccessSection();
          } else if (line.equals(LOADFONT_SECTION)) {
            loadFontSection();
          } else if (line.equals(BUTTON_ENUMS_SECTION)) {
            buttonEnumsSection();
          } else if (line.equals(SLIDER_ENUMS_SECTION)) {
            sliderEnumsSection();
          } else if (line.equals(FILE_SECTION)) {
            fileNameSection();
          } else if (line.equals(PATH_SECTION)) {
            pathSection();
          } else {
            pw.printf("%s%n",line);
          }
        }
      }
    } catch (IOException e) {
      e.printStackTrace();
		} finally {
			try {
				if (br != null)
					br.close();
				if (pw != null)
					pw.close();
				if (fw != null)
					fw.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}
  }
  
  /**
   * File name section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void fileNameSection() throws IOException {
    pw.printf("// FILE: %s%n", projectFile.getName());
    while ((line = br.readLine()) != null) {
      if (line.equals(FILE_END)) break;
    }
  }
  
  /**
   * Path section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void pathSection() throws IOException {
    // this section currently only occurs in linux platform.
    // find our Image and ImageButton models since no one else needs path name storage
    List<WidgetModel> modelList = new ArrayList<WidgetModel>();
    List<String> widgetTypes = new ArrayList<String>();
    widgetTypes.add(EnumFactory.IMAGE);
    widgetTypes.add(EnumFactory.IMAGEBUTTON);
    for (PagePane p : pages) {
      getModelsByType(p.getWidgets(), widgetTypes, modelList);
    }
    // now generate path name storage
    String strKey = "";
    String strCount = "";
    for (WidgetModel m : modelList) {
      strKey = m.getKey();
      int n = strKey.indexOf("$");
      strCount = strKey.substring(n+1);
      if (m.getType().equals(EnumFactory.IMAGE)) {
        pw.printf("char m_strImgPath%s[MAX_PATH];%n", strCount);
      } else { // must be EnumFactory.IMAGEBUTTON
        pw.printf("char m_strImgBtnPath%s[MAX_PATH];%n", strCount);
        pw.printf("char m_strImgBtnSelPath%s[MAX_PATH];%n", strCount);
      }
    }
    // finish up by scanning pass this section of our template
    readPassString(PATH_END);
  }
  
  /**
   * Font section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void fontSection() throws IOException {
    pw.printf("%s%n",line);
    // build up a list of GUISlice fonts in use
    // Find our Text, TextButton, and TextBox models
    // and pull out from the models the font keys that we can map 
    // to something GUIslice API can understand.
    for (PagePane p : pages) {
      for (Widget w : p.getWidgets()) {
        if (w.getType().equals(EnumFactory.TEXT)) {
          fontEnums.add(new Pair(((TextModel)w.getModel()).getFontDisplayName(),
                                 ((TextModel)w.getModel()).getFontEnum()));
        } else if (w.getType().equals(EnumFactory.TEXTBUTTON)) {
          fontEnums.add(new Pair(((TxtButtonModel)w.getModel()).getFontDisplayName(),
                                 ((TxtButtonModel)w.getModel()).getFontEnum()));
        } else if (w.getType().equals(EnumFactory.TEXTBOX)) {
          fontEnums.add(new Pair(((TextBoxModel)w.getModel()).getFontDisplayName(),
                                 ((TextBoxModel)w.getModel()).getFontEnum()));
        }
      }
    }
    if (fontEnums.size() > 1) {
      // Now we have a full list of font enums but we also may have duplicates
      // so we sort the list in order and remove duplicates.
      Collections.sort(fontEnums, new Comparator<Pair>() {
          public int compare(Pair one, Pair other) {
              return one.getValue().compareTo(other.getValue());
          }
      }); 
      Pair pairCur = null;
      Pair pairPrev = new Pair("","");
      ListIterator<Pair> pairIter = fontEnums.listIterator();
      while(pairIter.hasNext()) {
        pairCur = pairIter.next();
        if (pairCur.getValue().equals(pairPrev.getValue())) {
          if (!pairCur.getKey().equals(pairPrev.getKey())) {
            pw.printf("// Warning: Found Duplicate Font Enum with different Fonts <%s -> %s> mapping ignored%n", 
                      pairCur.getValue(), pairCur.getKey());
          }
          pairIter.remove();
        } else {
          pairPrev = pairCur;
        }
      }
    }
    // Now make pass using our now compact set of font enums to 
    // create a compact list of font names and since many enums 
    // may point to a single font name we will sort and remove dups.
    List<FontItem> fontList = new ArrayList<FontItem>();
    for (Pair pair : fontEnums) {
      FontItem item = ff.getFontItem(pair.getKey());
      fontList.add(item);
    }
    if (fontList.size() > 1) {
      Collections.sort(fontList, new Comparator<FontItem>() {
          public int compare(FontItem one, FontItem other) {
              return one.getDisplayName().compareTo(other.getDisplayName());
          }
      }); 
      FontItem itemCur = null;
      FontItem itemPrev = null;
      ListIterator<FontItem> itemIter = fontList.listIterator();
      while(itemIter.hasNext()) {
        itemCur = itemIter.next();
        if (itemPrev != null && itemCur.getDisplayName().equals(itemPrev.getDisplayName()))
          itemIter.remove();
        else 
          itemPrev = itemCur;
      }
    }
    // finish off by outputting font includes or defines, if any
    List<String> dups = new ArrayList<String>();
    for (FontItem f : fontList) {
      if (!f.getIncludeFile().equals("NULL")) {
        // This code only affects arduino implementation.
        pw.printf("#include \"%s\"%n", f.getIncludeFile());
      } else if (!f.getDefineFile().equals("NULL")) {
        // This code only affects linux implementation.
        // I have removed duplicate enums but duplicate fontrefs can and will still exist
        // I don't expect too many so i'll just use brute force on checking
        boolean bFound = false;
        for (String s : dups) {
          if (s.equals(f.getFontRef())) bFound = true;
        }
        if (!bFound) {
          pw.printf("#define %-20s \"%s\"%n", f.getFontRef(), f.getDefineFile());
          dups.add(f.getFontRef());
        }
      }
    }
    // finish up by scanning pass this section of our template
    readPassString(FONT_END);
  }
  
  /**
   * Resources section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void resourcesSection() throws IOException {
    pw.printf("%s%n",line);
    // build up a list of GUISlice resources in use
    List<String> resources = new ArrayList<String>();
    // first find our Image and ImageButton models
    // since no one else uses resources
    List<WidgetModel> modelList = new ArrayList<WidgetModel>();
    List<String> widgetTypes = new ArrayList<String>();
    widgetTypes.add(EnumFactory.IMAGE);
    widgetTypes.add(EnumFactory.IMAGEBUTTON);
    for (PagePane p : pages) {
      getModelsByType(p.getWidgets(), widgetTypes, modelList);
    }
    // now pull out from the models the resources as strings that GUIslice can understand
    String snorm = null;
    String sel = null;
    String sMemory = null;
    String sCType = null;
    for (WidgetModel m : modelList) {
      if (m.getType().equals(EnumFactory.IMAGE)) {
        if (!((ImageModel)m).getDefine().isEmpty()) {
          snorm = String.format("#define %-25s    \"%s\"", 
              ((ImageModel) m).getDefine(),
              ((ImageModel) m).getImageName());
          resources.add(snorm);
        } else if (!((ImageModel)m).getExternName().isEmpty()) {
          if (((ImageModel)m).getMemory().equals("PROGMEM")) {
            sMemory = "PROGMEM";
            sCType = "const unsigned short";
          } else {
            sMemory = "";
            sCType = "unsigned char";
          }
          snorm = String.format("extern \"C\" %s %s[] %s;", 
              sCType, ((ImageModel) m).getExternName(), sMemory);
          resources.add(snorm);
        }
      } else { // must be EnumFactory.IMAGEBUTTON
        if (!((ImgButtonModel)m).getDefine().isEmpty()) {
          snorm = String.format("#define %-25s    \"%s\"", 
              ((ImgButtonModel) m).getDefine(),
              ((ImgButtonModel) m).getImageName());
          resources.add(snorm);
        }
        if (!((ImgButtonModel)m).getSelDefine().isEmpty()) {
          sel = String.format("#define %-25s    \"%s\"", 
              ((ImgButtonModel) m).getSelDefine(),
              ((ImgButtonModel) m).getSelectImageName());
          resources.add(sel);
        }
        if (!((ImgButtonModel)m).getExternName().isEmpty()) {
          if (((ImgButtonModel)m).getMemory().equals("PROGMEM")) {
            sMemory = "PROGMEM";
            sCType = "const unsigned short";
          } else {
            sMemory = "";
            sCType = "unsigned char";
          }
          snorm = String.format("extern \"C\" %s %s[] %s;", 
              sCType, ((ImgButtonModel) m).getExternName(), sMemory);
          resources.add(snorm);
        }
        if (!((ImgButtonModel)m).getSelExternName().isEmpty()) {
          if (((ImgButtonModel)m).getSelMemory().equals("PROGMEM")) {
            sMemory = "PROGMEM";
            sCType = "const unsigned short";
          } else {
            sMemory = "";
            sCType = "unsigned char";
          }
          sel = String.format("extern \"C\" %s %s[] %s;", 
              sCType, ((ImgButtonModel) m).getSelExternName(), sMemory);
          resources.add(sel);
        }
      }
    }
    // Sort the list in order then make another pass to remove dups.
    // We might have the same image on different pages of the UI, like a Logo.
    sortListandRemoveDups(resources);
    // finish off by outputting resources, if any
    for (String s : resources) {
      pw.printf("%s%n", s);
    }
    // finish up by scanning pass this section of our template
    readPassString(RESOURCES_END);
  }
  
  /**
   * Enum section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void enumSection() throws IOException {
    pw.printf("%s%n",line);
    // build up a list of GUISlice enums in use
    // first find our Page models
    List<String> enumList = new ArrayList<String>();
    for (PagePane p : pages) {
      enumList.add(p.getEnum());
    }
    printEnums(enumList);
    // Now build up a list of our remaining UI widget enums
    enumList.clear();
    String ref = "";
    String strKey = "";
    String strCount = "";
    String strEnum = "";
    int n;
    for (PagePane p : pages) {
      for (Widget w : p.getWidgets()) {
        strEnum = w.getModel().getEnum();
        if (!strEnum.equals("GSLC_ID_AUTO")) 
          enumList.add(strEnum);
        // textbox has an embedded scrollbar so add it.
        if (w.getType().equals(EnumFactory.TEXTBOX)) {
          ref = "E_SCROLLBAR";
          strKey = w.getModel().getKey();
          n = strKey.indexOf("$");
          strCount = strKey.substring(n+1);
          ref = ref + strCount;
          enumList.add(ref);
        }
      }
    }
    if (enumList.size() > 0) {
      // Now we have a full list of enum names we can sort the list.
      Collections.sort(enumList);
      // Now output the list
      printEnums(enumList);
    }
    // Final pass output any font enums
    enumList.clear();
    for (Pair pair : fontEnums) {
        enumList.add(pair.getValue());
    }
    if (enumList.size() > 0) 
      printEnums(enumList);
    // finish up by scanning pass this section of our template
    readPassString(ENUM_END);
  }
  
  /**
   * Defines section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void definesSection() throws IOException {
    pw.printf("%s%n",line);
    List<String> templateLines = null;
    List<String> outputLines = null;
    int i;
    // output number of pages
    String elemTitle = "MAX_PAGE";
    pw.printf("#define %-24s%d%n", elemTitle, pages.size());
    // output number of fonts
    elemTitle = "MAX_FONT";
    pw.printf("#define %-24s%d%n", elemTitle, fontEnums.size());
    // build up a list of counts for out various UI widgets
    // build up a list of counts for out various UI widgets
    // first are we doing arduino minimum? if so, count _P functions stored in flash
    i = 0;
    if (targetPlatform == ARDUINO_MIN_PLATFORM) {
      int[] elem_cnt = new int[pages.size()];
      int[] flash_cnt = new int[pages.size()];
      List<String> widgetTypes = new ArrayList<String>();
      widgetTypes.add(EnumFactory.CHECKBOX);
      widgetTypes.add(EnumFactory.RADIOBUTTON);
      widgetTypes.add(EnumFactory.BOX);
      widgetTypes.add(EnumFactory.PROGRESSBAR);
      widgetTypes.add(EnumFactory.SLIDER);
      widgetTypes.add(EnumFactory.TEXT);
      widgetTypes.add(EnumFactory.TEXTBUTTON);
      i=0;
      for (PagePane p : pages) {
        flash_cnt[i] = countByType(p.getWidgets(), widgetTypes);
        elem_cnt[i] = p.getWidgets().size();
        i++;
      }
      // we also need to output some comments
      templateLines = loadTemplate(PROGMEM_TEMPLATE);
      writeTemplate(templateLines);
      templateLines = loadTemplate(DEFINE_PG_MAX_TEMPLATE);
      i=0;
      for (PagePane p : pages) {
        macro[0] = STRIP_ENUM_MACRO;
        replacement[0] = convertEnum(p.getEnum());
        macro[1] = COUNT_MACRO;
        replacement[1] = String.valueOf(elem_cnt[i]);
        macro[2] = FLASH_MACRO;
        replacement[2] = String.valueOf(flash_cnt[i]);
        macro[3] = null;
        outputLines = expandMacros(templateLines, macro, replacement);
        writeTemplate(outputLines);
        i++;
      }
    } else {
      templateLines = loadTemplate(DEFINE_PG_MAX_TEMPLATE);
      for (PagePane p : pages) {
        int count = 0;
        for (Widget w : p.getWidgets()) {
          if (w.getType().equals(EnumFactory.TEXTBOX)) {
            count += 3;  // TEXTBOX has embedded a wrapper Box and a scrollbar along with text box
          } else {
            count++;
          }
        }
        macro[0] = STRIP_ENUM_MACRO;
        replacement[0] = convertEnum(p.getEnum());
        macro[1] = COUNT_MACRO;
        replacement[1] = String.valueOf(count);
        macro[2] = null;
        outputLines = expandMacros(templateLines, macro, replacement);
        writeTemplate(outputLines);
      }
    }
    readPassString(DEFINES_END);
  }
  
  /**
   * Gui Elements section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void guiSection() throws IOException {
    pw.printf("%s%n",line);
    // output GUI elements
    String strElement = "";
    List<String> templateLines = loadTemplate(GUI_ELEMENT_TEMPLATE);
    List<String> outputLines = null;
    int i=0;
    for (PagePane p : pages) {
      macro[0] = STRIP_ENUM_MACRO;
      replacement[0] = convertEnum(p.getEnum());
      macro[1] = COUNT_MACRO;
      replacement[1] = String.valueOf(++i);
      macro[2] = null;
      outputLines = expandMacros(templateLines, macro, replacement);
      writeTemplate(outputLines);
    }
    // extension widgets have separate storage
    // Except, for reasons unknown to me extra storage isn't used by arduino min
    // Which begs the question why does standard arduino and linux need it?

    // how many text boxes?  
    // we need to do text boxes first because they have an embedded scrollbar 
    // that needs to be taken into account.
    int textbox_cnt = 0;
    int nRows, nCols;
    String strKey = "";
    int n = 0;
    String strCount = ""; 
    String ref = "";    
    for (PagePane p : pages) {
      for (Widget w : p.getWidgets()) {
        if (w.getType().equals(EnumFactory.TEXTBOX)) {
          textbox_cnt++;
          nRows = ((TextBoxModel)w.getModel()).getNumTextRows();
          nCols = ((TextBoxModel)w.getModel()).getNumTextColumns();
          strKey = w.getKey();
          n = strKey.indexOf("$");
          strCount = strKey.substring(n+1);
          ref = "m_sTextbox" + strCount;
          strElement = "gslc_tsXTextbox";
          pw.printf("%-32s%s;%n", strElement, ref);
          strElement = "char";
          ref = "m_acTextboxBuf" + strCount;
          pw.printf("%-32s%s[%d]; // NRows=%d NCols=%d%n", 
              strElement, ref, nRows*nCols, nRows, nCols);
        }
      }
    }
    // how many graphs?  
    for (PagePane p : pages) {
      for (Widget w : p.getWidgets()) {
        if (w.getType().equals(EnumFactory.GRAPH)) {
          nRows = ((GraphModel)w.getModel()).getNumRows();
          strKey = w.getKey();
          n = strKey.indexOf("$");
          strCount = strKey.substring(n+1);
          ref = "m_sGraph" + strCount;
          strElement = "gslc_tsXGraph";
          pw.printf("%-32s%s;%n", strElement, ref);
          strElement = "int16_t";
          ref = "m_anGraphBuf" + strCount;
          pw.printf("%-32s%s[%d]; // NRows=%d%n", 
              strElement, ref, nRows, nRows);
        }
      }
    }
    // Extension widgets have separate storage
    // Except, for reasons unknown to me extra storage isn't used by arduino min _P API
    if (targetPlatform != ARDUINO_MIN_PLATFORM) {
      List<String> widgetTypes = new ArrayList<String>();
      int count = 0;
      // how many checkboxes?
      widgetTypes.add(EnumFactory.CHECKBOX);
      widgetTypes.add(EnumFactory.RADIOBUTTON);
      for (PagePane p : pages) {
        count += countByType(p.getWidgets(), widgetTypes);
      }
      if (count > 0) {
        strElement = "gslc_tsXCheckbox";
        pw.printf("%-32sm_asXCheck[%d];%n", strElement, count);
      }
      widgetTypes.clear();
      // how many gauges?
      widgetTypes.add(EnumFactory.PROGRESSBAR);
      count = 0;
      for (PagePane p : pages) {
        count += countByType(p.getWidgets(), widgetTypes);
      }
      if (count > 0) {
        strElement = "gslc_tsXGauge";
        pw.printf("%-32sm_sXGauge[%d];%n", strElement, count);
      }
      // how many sliders?
      widgetTypes.clear();
      widgetTypes.add(EnumFactory.SLIDER);
      count = 0;
      for (PagePane p : pages) {
        count += countByType(p.getWidgets(), widgetTypes);
      }
      count = count + textbox_cnt; // don't forget our embedded scrollbars within textboxes
      if (count > 0) {
        strElement = "gslc_tsXSlider";
        pw.printf("%-32sm_sXSlider[%d];%n", strElement, count);
      }
    }
    // output MAX String size
    strElement = "MAX_STR";
    pw.printf("%n#define %-24s%d%n%n", strElement, maxstr_len);
      
    readPassString(GUI_END);
  }
  
  /**
   * Save ref section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void saveRefSection() throws IOException {
    pw.printf("%s%n",line);
    // save references to elements that may need quick access and while we are at it
    // save page enum for our quick access code generation section
    
    // quick access may be needed for progressbars, sliders, and text that is changed dynamically at runtime.
    String ref = null;
    for (PagePane p : pages) {
      for (Widget w : p.getWidgets()) {
        if (w.getType().equals(EnumFactory.TEXT)) {
          ref = ((TextModel)w.getModel()).getElementRef();
          if (ref != null && !ref.isEmpty()) {
            refList.add(w.getModel());
            w.getModel().setPageEnum(p.getEnum());
          }
        } else if (w.getType().equals(EnumFactory.PROGRESSBAR)) {
          ref = ((ProgressBarModel)w.getModel()).getElementRef();
          if (ref != null && !ref.isEmpty()) {
            refList.add(w.getModel());
            w.getModel().setPageEnum(p.getEnum());
          }
        } else if (w.getType().equals(EnumFactory.TEXTBOX)) {
          ref = ((TextBoxModel)w.getModel()).getElementRef();
          if (ref != null && !ref.isEmpty()) {
            refList.add(w.getModel());
            w.getModel().setPageEnum(p.getEnum());
          }
        } else if (w.getType().equals(EnumFactory.TEXTBUTTON)) {
          ref = ((TxtButtonModel)w.getModel()).getElementRef();
          if (ref != null && !ref.isEmpty()) {
            refList.add(w.getModel());
            w.getModel().setPageEnum(p.getEnum());
          }
        } else if (w.getType().equals(EnumFactory.GRAPH)) {
          ref = ((TextModel)w.getModel()).getElementRef();
          if (ref != null && !ref.isEmpty()) {
            refList.add(w.getModel());
            w.getModel().setPageEnum(p.getEnum());
          }
        } else if (w.getType().equals(EnumFactory.IMAGE)) {
          ref = ((ImageModel)w.getModel()).getElementRef();
          if (ref != null && !ref.isEmpty()) {
            refList.add(w.getModel());
            w.getModel().setPageEnum(p.getEnum());
          }
        } else if (w.getType().equals(EnumFactory.IMAGEBUTTON)) {
          ref = ((ImgButtonModel)w.getModel()).getElementRef();
          if (ref != null && !ref.isEmpty()) {
            refList.add(w.getModel());
            w.getModel().setPageEnum(p.getEnum());
          }
        } else if (w.getType().equals(EnumFactory.SLIDER)) {
          ref = ((SliderModel)w.getModel()).getElementRef();
          if (ref != null && !ref.isEmpty()) {
            refList.add(w.getModel());
            w.getModel().setPageEnum(p.getEnum());
          }
        }
      }
    }
    if (refList.size() > 0) {
      // we need to sort our model list in key value order
      Collections.sort(refList, new Comparator<WidgetModel>() {
          public int compare(WidgetModel one, WidgetModel other) {
              return one.getKey().compareTo(other.getKey());
          }
      }); 
      for (WidgetModel m : refList) {
        if (m.getType().equals(EnumFactory.TEXT)) {
            ref = ((TextModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.PROGRESSBAR)) {
            ref = ((ProgressBarModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.TEXTBOX)) {
          ref = ((TextBoxModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.TEXTBUTTON)) {
          ref = ((TxtButtonModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.IMAGE)) {
          ref = ((ImageModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.IMAGEBUTTON)) {
          ref = ((ImgButtonModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.GRAPH)) {
          ref = ((GraphModel) m).getElementRef();
        } else { // must be EnumFactory.SLIDER
          ref = ((SliderModel) m).getElementRef();
        }
        pw.printf("gslc_tsElemRef*  %-18s= NULL;%n", ref);
      }
    }
    readPassString(SAVEREF_END);
  }
  
  /**
   * Button cb section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void buttonCbSection() throws IOException {
    // create any callbacks for buttons in use
    List<WidgetModel> modelList = new ArrayList<WidgetModel>();
    List<String> widgetTypes = new ArrayList<String>();
    widgetTypes.add(EnumFactory.TEXTBUTTON);
    widgetTypes.add(EnumFactory.IMAGEBUTTON);
    for (PagePane p : pages) {
      getModelsByType(p.getWidgets(), widgetTypes, modelList);
    }
    if (modelList.size() > 0) {
      outputButtonCB(modelList);
      while ((line = br.readLine()) != null) {
        if (line.equals(BUTTON_CB_END)) break;
      }
    } else {
      pw.printf("%s%n", BUTTON_CB_SECTION);
      readPassString(BUTTON_CB_END);
    }
  }
  
  /**
   * Slider cb section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void sliderCbSection() throws IOException {
    List<String> widgetTypes = new ArrayList<String>();
    // create any call backs for sliders in use
    widgetTypes.clear();
    widgetTypes.add(EnumFactory.SLIDER);
    widgetTypes.add(EnumFactory.TEXTBOX);
    List<String>enumList = getListOfEnums(widgetTypes);
    if (enumList.size() > 0) {
      outputCallback(enumList, SLIDER_ENUMS_SECTION, SLIDER_CB_TEMPLATE, SLIDER_LOOP_TEMPLATE);
      while ((line = br.readLine()) != null) {
        if (line.equals(SLIDER_CB_END)) break;
      }
    } else {
      pw.printf("%s%n", SLIDER_CB_SECTION);
      readPassString(SLIDER_CB_END);
    }
  }
  
  /**
   * Startup section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void startupSection() throws IOException {
    pw.printf("%s%n",line);
    // set gslc_SetPageCur() to our first page's ENUM
    List<String> templateLines = loadTemplate(STARTUP_TEMPLATE);
    List<String> outputLines;
    macro[0] = PAGE_ENUM_MACRO;
    replacement[0] = pages.get(0).getEnum();
    macro[1] = null;
    outputLines = expandMacros(templateLines, macro, replacement);
    writeTemplate(outputLines);
    // do we need to rotate display?
    if (screenHeight > screenWidth) {
      templateLines = loadTemplate(ROTATE_TEMPLATE);
      // need to store the value inside general model someday
      macro[0] = ROTATION_MACRO;
      replacement[0] = "0";
      macro[1] = null;
      outputLines = expandMacros(templateLines, macro, replacement);
      writeTemplate(outputLines);
    }
    readPassString(STARTUP_END);
  }
  
  /**
   * Draw cb section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void drawCbSection() throws IOException {
    List<String> widgetTypes = new ArrayList<String>();
    // how many draw boxes?
    List<WidgetModel> modelList = new ArrayList<WidgetModel>();
    widgetTypes.clear();
    widgetTypes.add(EnumFactory.BOX);
    for (PagePane p : pages) {
      getModelsByType(p.getWidgets(), widgetTypes, modelList);
    }
    // now search the models for any BOX widget that has a callback.
    boolean bFoundDrawFunc = false;
    for (WidgetModel m : modelList) {
      if (((BoxModel) m).hasDrawFunc()) {
        bFoundDrawFunc = true;
        break;
      }
    }
    if (bFoundDrawFunc) {
      // only one draw function callback created no matter how many requested.
      //TODO - a better job some day
      List<String> templateLines = loadTemplate(DRAWBOX_CB_TEMPLATE);
      writeTemplate(templateLines);
      while ((line = br.readLine()) != null) {
        if (line.equals(DRAW_CB_END)) break;
      }
    } else {
      pw.printf("%s%n", DRAW_CB_SECTION);
      readPassString(DRAW_CB_END);
    }
  }
  
  /**
   * Tick cb section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void tickCbSection() throws IOException {
    List<String> widgetTypes = new ArrayList<String>();
    // how many draw boxes?
    List<WidgetModel> modelList = new ArrayList<WidgetModel>();
    widgetTypes.clear();
    widgetTypes.add(EnumFactory.BOX);
    for (PagePane p : pages) {
      getModelsByType(p.getWidgets(), widgetTypes, modelList);
    }
    // now search the models for any BOX widget that has a callback.
    boolean bFoundTickFunc = false;
    for (WidgetModel m : modelList) {
      if (((BoxModel) m).hasTickFunc()) {
        bFoundTickFunc = true;
        break;
      }
    }
    if (bFoundTickFunc) {
      // only one tick function callback created no matter how many requested.
      //TODO - a better job some day
      List<String> templateLines = loadTemplate(TICK_CB_TEMPLATE);
      writeTemplate(templateLines);
      while ((line = br.readLine()) != null) {
        if (line.equals(TICK_CB_END)) break;
      }
    } else {
      pw.printf("%s%n", TICK_CB_SECTION);
      readPassString(TICK_CB_END);
    }
  }
  
  /**
   * Initializes the GUI section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void initGUISection() throws IOException {
    pw.printf("%s%n",line);
    List<String> templateLines = loadTemplate(PAGEADD_TEMPLATE);
    List<String> outputLines = null;
    int i=0;
    for (PagePane p : pages) {
      macro[0] = PAGE_ENUM_MACRO;
      replacement[0] = p.getEnum();
      macro[1] = STRIP_ENUM_MACRO;
      replacement[1] = convertEnum(p.getEnum());
      macro[2] = COUNT_MACRO;
      replacement[2] = String.valueOf(++i);
      macro[3] = null;
      outputLines = expandMacros(templateLines, macro, replacement);
      writeTemplate(outputLines);
    }
    // deal with background
    String color = cf.colorAsString(background);
    macro[0] = BACKGROUND_COLOR_MACRO;
    macro[1] = null;
    replacement[0] = color;
    templateLines = loadTemplate(BACKGROUND_TEMPLATE);
    outputLines = expandMacros(templateLines, macro, replacement);
    writeTemplate(outputLines);
    for (PagePane p : pages) {
      pw.printf("%n  // -----------------------------------%n");
      pw.printf("  // PAGE: %s%n", p.getEnum());
      for (Widget w : p.getWidgets()) {
        outputAPI(p.getEnum(), w.getModel());
      }
    }
    readPassString(INITGUI_END);
  }
  
  /**
   * Quick access section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void quickAccessSection() throws IOException {
    pw.printf("%s%n",line);
    // find text elements with storage
    if (refList.size() > 0) {
      String ref = "";    
      for (WidgetModel m : refList) {
        if (m.getType().equals(EnumFactory.TEXT)) {
            ref = ((TextModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.PROGRESSBAR)) {
          ref = ((ProgressBarModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.GRAPH)) {
          ref = ((GraphModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.IMAGE)) {
          ref = ((ImageModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.IMAGEBUTTON)) {
          ref = ((ImgButtonModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.TEXTBOX)) {
          ref = ((TextBoxModel) m).getElementRef();
        } else if (m.getType().equals(EnumFactory.TEXTBUTTON)) {
          ref = ((TxtButtonModel) m).getElementRef();
        } else { // must be EnumFactory.SLIDER
          ref = ((SliderModel) m).getElementRef();
        }
        pw.printf("  %-18s= gslc_PageFindElemById(&m_gui,%s,%s);%n", 
            ref,m.getPageEnum(),m.getEnum());
      }
    }
    readPassString(QUICK_END);
  }
  
  /**
   * Load font section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void loadFontSection() throws IOException {
    pw.printf("%s%n",line);
    List<String> templateLines = loadTemplate(FONT_LOAD_TEMPLATE);
    List<String> outputLines;
    macro[0] = FONT_ID_MACRO;
    macro[1] = FONT_REFTYPE_MACRO;
    macro[2] = FONT_REF_MACRO;
    macro[3] = FONT_SZ_MACRO;
    macro[4] = null;
    for (Pair pair : fontEnums) {
      FontItem item = ff.getFontItem(pair.getKey());
      replacement[0] = pair.getValue();
      replacement[1] = item.getFontRefType();
      replacement[2] = item.getFontRef();
      replacement[3] = item.getFontSz();
      outputLines = expandMacros(templateLines, macro, replacement);
      writeTemplate(outputLines);
    }
    readPassString(LOADFONT_END);
  }
  
  /**
   * Button enums section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void buttonEnumsSection() throws IOException {
    pw.printf("%s%n",line);
    // build up a list of buttons in use
    List<WidgetModel> modelList = new ArrayList<WidgetModel>();
    List<String> widgetTypes = new ArrayList<String>();
    widgetTypes.add(EnumFactory.TEXTBUTTON);
    widgetTypes.add(EnumFactory.IMAGEBUTTON);
    for (PagePane p : pages) {
      getModelsByType(p.getWidgets(), widgetTypes, modelList);
    }
    if (modelList.size() > 0) {
      List<String> templateStandard;
      List<String> templateChgPage;
      List<String> outputLines;
      List<String> scanLines = new ArrayList<String>();
      // our callback section already exists - so pass it through storing each line output
      // so we can scan them next
      while((line = br.readLine()) != null) {
        if (line.equals(BUTTON_ENUMS_END))
          break;
        scanLines.add(line);
        pw.printf("%s%n",line);
      }
      templateStandard = loadTemplate(BUTTON_LOOP_TEMPLATE);
      templateChgPage = loadTemplate(BUTTON_CHGPG_TEMPLATE);
      macro[0] = ENUM_MACRO;
      macro[1] = PAGE_ENUM_MACRO;
      macro[2] = null;
      for (WidgetModel m : modelList) {
        replacement[0] = m.getEnum();
        replacement[1] = "";
        // search our saved lines for this enum, if found skip it, otherwise expand the macros
        if (!searchForEnum(scanLines, replacement[0])) {
          if (m.getType().equals(EnumFactory.TEXTBUTTON)) {
             if (((TxtButtonModel)m).isChangePageFunct()) {
              replacement[1] = ((TxtButtonModel)m).getChangePageEnum();
              outputLines = expandMacros(templateChgPage, macro, replacement);
              writeTemplate(outputLines);
             } else {
              outputLines = expandMacros(templateStandard, macro, replacement);
              writeTemplate(outputLines);
             }
          } else if (((ImgButtonModel)m).isChangePageFunct()) {
            replacement[1] = ((ImgButtonModel)m).getChangePageEnum();
            outputLines = expandMacros(templateChgPage, macro, replacement);
            writeTemplate(outputLines);
          } else {
            outputLines = expandMacros(templateStandard, macro, replacement);
            writeTemplate(outputLines);
          }
        }
      }
      pw.printf("%s%n",BUTTON_ENUMS_END);
      return;
    }
    readPassString(BUTTON_ENUMS_END);
  }
  
  /**
   * Slider enums section.
   *
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void sliderEnumsSection() throws IOException {
    pw.printf("%s%n",line);
    // build up a list of sliders in use
    List<String> widgetTypes = new ArrayList<String>();
    widgetTypes.add(EnumFactory.SLIDER);
    widgetTypes.add(EnumFactory.TEXTBOX);
    List<String>enumList = getListOfEnums(widgetTypes);
    if (enumList.size() >= 1) {
      List<String> templateLines;
      List<String> outputLines;
      List<String> scanLines = new ArrayList<String>();
      // our callback section already exists - so pass it through storing each line output
      // so we can scan them next
      while((line = br.readLine()) != null) {
        if (line.equals(SLIDER_ENUMS_END))
          break;
        scanLines.add(line);
        pw.printf("%s%n",line);
      }
      templateLines = loadTemplate(SLIDER_LOOP_TEMPLATE);
      macro[0] = ENUM_MACRO;
      macro[1] = null;
      for (int i=0; i<enumList.size(); i++) {
        replacement[0] = enumList.get(i);
        // search our saved lines for this enum, if found skip it, otherwise expand the macros
        if (!searchForEnum(scanLines, replacement[0])) {
          outputLines = expandMacros(templateLines, macro, replacement);
          writeTemplate(outputLines);
        }
      }
      pw.printf("%s%n",SLIDER_ENUMS_END);
      return;
    }
    readPassString(SLIDER_ENUMS_END);
  }
  
//-------------------------------------------------------------------------------------------------------
// output API methods
//-------------------------------------------------------------------------------------------------------

  /**
 * Output API.
 *
 * @param pageEnum
 *          the page enum
 * @param m
 *          the m
 */
private void outputAPI(String pageEnum, WidgetModel m) {
//    System.out.println("outputAPI page: " + pageEnum + " widget: " + m.getType());
    switch(m.getType()) {
      case "Box":
        if (targetPlatform == ARDUINO_MIN_PLATFORM) {
          outputAPI_Box_P(pageEnum, ((BoxModel)m));
        } else {
          outputAPI_Box(pageEnum, ((BoxModel)m));
        }
        break;
      case "CheckBox":
        if (targetPlatform == ARDUINO_MIN_PLATFORM) {
          outputAPI_CheckBox_P(pageEnum, ((CheckBoxModel)m));
        } else {
          outputAPI_CheckBox(pageEnum, ((CheckBoxModel)m));
        }
        break;
      case "Graph":
        outputAPI_Graph(pageEnum, ((GraphModel)m));
        break;
      case "ImageButton":
        outputAPI_ImgButton(pageEnum, ((ImgButtonModel)m));
        break;
      case "Image":
        outputAPI_Image(pageEnum, ((ImageModel)m));
        break;
      case "ProgressBar":
        if (targetPlatform == ARDUINO_MIN_PLATFORM) {
          outputAPI_ProgressBar_P(pageEnum, ((ProgressBarModel)m));
        } else {
          outputAPI_ProgressBar(pageEnum, ((ProgressBarModel)m));
        }
        break;
      case "RadioButton":
        if (targetPlatform == ARDUINO_MIN_PLATFORM) {
          outputAPI_RadioButton_P(pageEnum, ((RadioButtonModel)m));
        } else {
          outputAPI_RadioButton(pageEnum, ((RadioButtonModel)m));
        }
        break;
      case "Slider":
        outputAPI_Slider(pageEnum, ((SliderModel)m));
        break;
      case "Text":
        if (targetPlatform == ARDUINO_MIN_PLATFORM) {
          outputAPI_Text_P(pageEnum, ((TextModel)m));
        } else {
          outputAPI_Text(pageEnum, ((TextModel)m));
        }
        break;
      case "TextBox":
        outputAPI_TextBox(pageEnum, ((TextBoxModel)m));
        break;
      case "TextButton":
        if (targetPlatform == ARDUINO_MIN_PLATFORM) {
          outputAPI_TxtButton_P(pageEnum, ((TxtButtonModel)m));
        } else {
          outputAPI_TxtButton(pageEnum, ((TxtButtonModel)m));
        }
        break;
      default:
        break;
    }
  }

  /**
   * Common API.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   * @return the <code>int</code> object
   */
  private int commonAPI(String pageEnum, WidgetModel m) {
    int n =0;
    // setup common attributes
    macro[n] = ENUM_MACRO;
    replacement[n++] = m.getEnum();
    macro[n] = PAGE_ENUM_MACRO;
    replacement[n++] = pageEnum;
    macro[n] = X_MACRO;
    replacement[n++] = String.valueOf(m.getX());
    macro[n] = Y_MACRO;
    replacement[n++] = String.valueOf(m.getY());
    macro[n] = WIDTH_MACRO;
    replacement[n++] = String.valueOf(m.getWidth());
    macro[n] = HEIGHT_MACRO;
    replacement[n++] = String.valueOf(m.getHeight());
    return n;
  }
  
  /**
   * Output API box.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_Box(String pageEnum, BoxModel m) {
    List<String> template = null;
    List<String> outputLines = null;

    int n = commonAPI(pageEnum, m);
    macro[n] = null;
    template = loadTemplate(BOX_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (m.hasDrawFunc()) {
      template = loadTemplate(DRAWFUNC_TEMPLATE);
      writeTemplate(template);
    } 
    if (m.hasTickFunc()) {
      template = loadTemplate(TICKFUNC_TEMPLATE);
      writeTemplate(template);
    } 
    if (!m.useDefaultColors()) {
      n = 0;
      macro[n] = FILL_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFillColor());
      macro[n] = FRAME_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFrameColor());
      macro[n] = GLOW_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getSelectedColor());
      macro[n] = null;
      template = loadTemplate(COLOR_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
  }
  
  /**
   * Output API box P.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_Box_P(String pageEnum, BoxModel m) {
    List<String> template = null;
    List<String> outputLines = null;
    
    String strDrawFunc = "NULL";
    String strTickFunc = "NULL";

    if (m.hasDrawFunc()) {
      strDrawFunc = "&CbDrawScanner";
    } 
    if (m.hasTickFunc()) {
      strTickFunc = "&CbTickScanner";
    } 
    int n = commonAPI(pageEnum, m);
    macro[n] = FILL_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFillColor());
    macro[n] = FRAME_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFrameColor());
    macro[n] = DRAWFUNC_MACRO;
    replacement[n++] = strDrawFunc;
    macro[n] = TICKFUNC_MACRO;
    replacement[n++] = strTickFunc;
    macro[n] = null;
    template = loadTemplate(BOX_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
  }

  /**
   * Output API check box.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_CheckBox(String pageEnum, CheckBoxModel m) {
    List<String> template = null;
    List<String> outputLines = null;

    int n = commonAPI(pageEnum, m);
    macro[n] = MARK_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getMarkColor());
    macro[n] = CHECKED_MACRO;
    replacement[n++] = String.valueOf(m.isChecked());
    macro[n] = COUNT_MACRO;
    replacement[n++] = String.valueOf(countCheckBoxes);
    countCheckBoxes++;
    macro[n] = null;
    template = loadTemplate(CHECKBOX_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.useDefaultColors()) {
      n = 0;
      macro[n] = FILL_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFillColor());
      macro[n] = FRAME_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFrameColor());
      macro[n] = GLOW_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getSelectedColor());
      macro[n] = null;
      template = loadTemplate(COLOR_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
  }
  
  /**
   * Output API check box P.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_CheckBox_P(String pageEnum, CheckBoxModel m) {
    List<String> template = null;
    List<String> outputLines = null;

    int n = commonAPI(pageEnum, m);
    macro[n] = MARK_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getMarkColor());
    macro[n] = CHECKED_MACRO;
    replacement[n++] = String.valueOf(m.isChecked());
    macro[n] = COUNT_MACRO;
    replacement[n++] = String.valueOf(countCheckBoxes);
    countCheckBoxes++;
    String groupId = m.getGroupId();
    if (groupId.length() == 0) 
      groupId = "GSLC_GROUP_ID_NONE";
    macro[n] = GROUP_ID_MACRO;
    replacement[n++] = groupId;
    macro[n] = null;
    template = loadTemplate(CHECKBOX_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.useDefaultColors()) {
      n = 0;
      macro[n] = FILL_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFillColor());
      macro[n] = FRAME_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFrameColor());
      macro[n] = GLOW_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getSelectedColor());
      macro[n] = null;
      template = loadTemplate(COLOR_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
  }
  
  /**
   * Output API graph.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_Graph(String pageEnum, GraphModel m) {
    List<String> template = null;
    List<String> outputLines = null;
    String fontId = "";
    String strKey = "";
    String strCount = "";

    int n = commonAPI(pageEnum, m);
    strKey = m.getKey();
    int ts = strKey.indexOf("$");
    strCount = strKey.substring(ts+1);
    macro[n] = ID_MACRO;
    replacement[n++] = strCount;
    macro[n] = FONT_ID_MACRO;
    fontId = ff.getFontItem(m.getFontDisplayName()).getFontId();
    replacement[n++] = fontId; 
    macro[n] = STYLE_MACRO;
    switch(m.getGraphStyle()) {
    case "Dot":
      replacement[n++] = "GSLCX_GRAPH_STYLE_DOT";
      break;
    case "Line":
      replacement[n++] = "GSLCX_GRAPH_STYLE_LINE";
      break;
    case "Fill":
      replacement[n++] = "GSLCX_GRAPH_STYLE_FILL";
      break;
    default:
      replacement[n++] = m.getGraphStyle();
      break;
    }
    macro[n] = ROWS_MACRO;
    replacement[n++] = String.valueOf(m.getNumRows());
    macro[n] = GRAPH_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getGraphColor());
    macro[n] = null;
    template = loadTemplate(GRAPH_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.useDefaultColors()) {
      n = 0;
      macro[n] = FILL_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFillColor());
      macro[n] = FRAME_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFrameColor());
      macro[n] = GLOW_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getSelectedColor());
      macro[n] = null;
      template = loadTemplate(COLOR_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
  }
  
  /**
   * Output API img button.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_ImgButton(String pageEnum, ImgButtonModel m) {
    List<String> template = null;
    List<String> outputLines = null;
    int n = commonAPI(pageEnum, m);
    
    // NOTE: at some point we need to check for SRAM
    if (m.getDefine() != null && !m.getDefine().isEmpty()) {
      macro[n] = IMAGE_MACRO;
      replacement[n++] = ((ImgButtonModel)m).getDefine();
      macro[n] = IMAGE_SOURCE_MACRO;
      replacement[n++] = SRC_SD;
    } 
    if (m.getExternName() != null && !m.getExternName().isEmpty()) {
      macro[n] = IMAGE_MACRO;
      replacement[n++] = ((ImgButtonModel)m).getExternName();
      macro[n] = IMAGE_SOURCE_MACRO;
      if (m.getMemory().equals("PROGMEM")) 
        replacement[n++] = SRC_PROG;
      else 
        replacement[n++] = SRC_RAM;
    }
    // NOTE: at some point we need to check for SRAM
    if (m.getSelDefine() != null && !m.getSelDefine().isEmpty()) {
      macro[n] = IMAGE_SEL_MACRO;
      replacement[n++] = ((ImgButtonModel)m).getSelDefine();
      macro[n] = IMAGE_SEL_SOURCE_MACRO;
      replacement[n++] = SRC_SD;
    } 
    if (m.getSelExternName() != null && !m.getSelExternName().isEmpty()) {
      macro[n] = IMAGE_SEL_MACRO;
      replacement[n++] = ((ImgButtonModel)m).getSelExternName();
      macro[n] = IMAGE_SEL_SOURCE_MACRO;
      if (m.getSelMemory().equals("PROGMEM")) 
        replacement[n++] = SRC_PROG;
      else 
        replacement[n++] = SRC_RAM;
    }
    macro[n] = IMAGE_FORMAT_MACRO;
    replacement[n++] = ((ImgButtonModel)m).getImageFormat();
    macro[n] = null;
    template = loadTemplate(IMGBUTTON_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    // handle transparency
    n = 0;
    macro[n] = BOOL_MACRO;
    if (m.isTransparent()) 
      replacement[n++] = "false";
    else
      replacement[n++] = "true";
    macro[n] = null;
    template = loadTemplate(IMAGETRANSPARENT_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.useDefaultColors()) {
      if (!cf.getDefFillCol().equals(m.getFillColor())    || 
          !cf.getDefFrameCol().equals(m.getFrameColor())  ||
          !cf.getDefGlowCol().equals(m.getSelectedColor())) {
        n = 0;
        macro[n] = FILL_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getFillColor());
        macro[n] = FRAME_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getFrameColor());
        macro[n] = GLOW_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getSelectedColor());
        macro[n] = null;
        template = loadTemplate(COLOR_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
      }
    }
  }
  
  /**
   * Output API image.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_Image(String pageEnum, ImageModel m) {
    List<String> template = null;
    List<String> outputLines = null;

    int n = commonAPI(pageEnum, m);
    // NOTE: at some point we need to check for SRAM
    if (m.getDefine() != null && !m.getDefine().isEmpty()) {
      macro[n] = IMAGE_MACRO;
      replacement[n++] = m.getDefine();
      macro[n] = IMAGE_FROM_MACRO;
      replacement[n++] = FROM_SD;
      macro[n] = IMAGE_SOURCE_MACRO;
      replacement[n++] = SRC_SD;
    } 
    if (m.getExternName() != null && !m.getExternName().isEmpty()) {
      macro[n] = IMAGE_MACRO;
      replacement[n++] = m.getExternName();
      macro[n] = IMAGE_FROM_MACRO;
      if (m.getMemory().equals("PROGMEM")) {
        replacement[n++] = FROM_PROG;
        macro[n] = IMAGE_SOURCE_MACRO;
        replacement[n++] = SRC_PROG;
      } else {
        replacement[n++] = FROM_RAM;
        macro[n] = IMAGE_SOURCE_MACRO;
        replacement[n++] = SRC_RAM;
      }
    }
    macro[n] = IMAGE_FORMAT_MACRO;
    replacement[n++] = m.getImageFormat();
    macro[n] = null;
    template = loadTemplate(IMAGE_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    // handle transparency
    n = 0;
    macro[n] = BOOL_MACRO;
    if (m.isTransparent()) 
      replacement[n++] = "false";
    else
      replacement[n++] = "true";
    macro[n] = null;
    template = loadTemplate(IMAGETRANSPARENT_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.useDefaultColors()) {
      if (!cf.getDefFillCol().equals(m.getFillColor())    || 
          !cf.getDefFrameCol().equals(m.getFrameColor())  ||
          !cf.getDefGlowCol().equals(m.getSelectedColor())) {
        n = 0;
        macro[n] = FILL_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getFillColor());
        macro[n] = FRAME_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getFrameColor());
        macro[n] = GLOW_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getSelectedColor());
        macro[n] = null;
        template = loadTemplate(COLOR_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
      }
    }
  }
  
  /**
   * Output API progress bar.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_ProgressBar(String pageEnum, ProgressBarModel m) {
    List<String> template = null;
    List<String> outputLines = null;

    int n = commonAPI(pageEnum, m);
    macro[n] = MARK_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getIndicatorColor());
    macro[n] = CHECKED_MACRO;
    replacement[n++] = String.valueOf(m.isVertical());
    macro[n] = COUNT_MACRO; 
    replacement[n++] = String.valueOf(countGauges);
    countGauges++;
    macro[n] = MIN_MACRO;
    replacement[n++] = String.valueOf(m.getMin());
    macro[n] = MAX_MACRO;
    replacement[n++] = String.valueOf(m.getMax());
    macro[n] = VALUE_MACRO;
    replacement[n++] = String.valueOf(m.getCurValue());
    macro[n] = null;
    template = loadTemplate(PROGRESSBAR_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.useDefaultColors()) {
      n = 0;
      macro[n] = FILL_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFillColor());
      macro[n] = FRAME_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFrameColor());
      macro[n] = GLOW_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getSelectedColor());
      macro[n] = null;
      template = loadTemplate(COLOR_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
    if (!m.getGaugeStyle().equals("Bar")) {
      n=0;
      macro[n] = STYLE_MACRO;
      if (m.getGaugeStyle().equals("Ramp")) {
        replacement[n++] = "GSLCX_GAUGE_STYLE_RAMP";
        macro[n] = null;
        template = loadTemplate(PROGRESSBARSTYLE_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
      } else {
        replacement[n++] = "GSLCX_GAUGE_STYLE_RADIAL";
        macro[n] = null;
        template = loadTemplate(PROGRESSBARSTYLE_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
        // now to check for indicator and ticks, only works with Radial
        // only output if different from default values
        if (m.getIndicatorSize() != 10 && m.getIndicatorTipSize() != 3) {
          n=0;
          macro[n] = MARK_COLOR_MACRO;
          replacement[n++] = cf.colorAsString(m.getIndicatorColor());
          macro[n] = SIZE_MACRO;
          replacement[n++] = String.valueOf(m.getIndicatorSize());
          macro[n] = TIPSZ_MACRO;
          replacement[n++] = String.valueOf(m.getIndicatorTipSize());
          macro[n] = null;
          template = loadTemplate(PROGRESSBARIND_TEMPLATE);
          outputLines = expandMacros(template, macro, replacement);
          writeTemplate(outputLines);
        }
        if (!m.getTickColor().equals(Color.GRAY) &&
             m.getDivisions() != 8 && 
             m.getTickSize() != 5) {
          n=0;
          macro[n] = MARK_COLOR_MACRO;
          replacement[n++] = cf.colorAsString(m.getTickColor());
          macro[n] = DIVISIONS_MACRO;
          replacement[n++] = String.valueOf(m.getDivisions());
          macro[n] = TICKSZ_MACRO;
          replacement[n++] = String.valueOf(m.getTickSize());
          macro[n] = null;
          template = loadTemplate(PROGRESSBARTICKS_TEMPLATE);
          outputLines = expandMacros(template, macro, replacement);
          writeTemplate(outputLines);
        }
      }
    }
  }   
  
  /**
   * Output API progress bar P.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_ProgressBar_P(String pageEnum, ProgressBarModel m) {
    List<String> template = null;
    List<String> outputLines = null;

    int n = commonAPI(pageEnum, m);
    macro[n] = MARK_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getIndicatorColor());
    macro[n] = CHECKED_MACRO;
    replacement[n++] = String.valueOf(m.isVertical());
    macro[n] = COUNT_MACRO; 
    replacement[n++] = String.valueOf(countGauges);
    countGauges++;
    macro[n] = MIN_MACRO;
    replacement[n++] = String.valueOf(m.getMin());
    macro[n] = MAX_MACRO;
    replacement[n++] = String.valueOf(m.getMax());
    macro[n] = VALUE_MACRO;
    replacement[n++] = String.valueOf(m.getCurValue());
    macro[n] = FILL_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFillColor());
    macro[n] = FRAME_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFrameColor());
    macro[n] = null;
    template = loadTemplate(PROGRESSBAR_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.getGaugeStyle().equals("Bar")) {
      n=0;
      macro[n] = STYLE_MACRO;
      if (m.getGaugeStyle().equals("Ramp")) {
        replacement[n++] = "GSLCX_GAUGE_STYLE_RAMP";
        macro[n] = null;
        template = loadTemplate(PROGRESSBARSTYLE_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
      } else {
        replacement[n++] = "GSLCX_GAUGE_STYLE_RADIAL";
        macro[n] = null;
        template = loadTemplate(PROGRESSBARSTYLE_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
        // now to check for indicator and ticks, only works with Radial
        // only output if different from default values
        if (m.getIndicatorSize() != 10 && m.getIndicatorTipSize() != 3) {
          n=0;
          macro[n] = MARK_COLOR_MACRO;
          replacement[n++] = cf.colorAsString(m.getIndicatorColor());
          macro[n] = SIZE_MACRO;
          replacement[n++] = String.valueOf(m.getIndicatorSize());
          macro[n] = TIPSZ_MACRO;
          replacement[n++] = String.valueOf(m.getIndicatorTipSize());
          macro[n] = null;
          template = loadTemplate(PROGRESSBARIND_TEMPLATE);
          outputLines = expandMacros(template, macro, replacement);
          writeTemplate(outputLines);
        }
        if (!m.getTickColor().equals(Color.GRAY) &&
             m.getDivisions() != 8 && 
             m.getTickSize() != 5) {
          n=0;
          macro[n] = MARK_COLOR_MACRO;
          replacement[n++] = cf.colorAsString(m.getTickColor());
          macro[n] = DIVISIONS_MACRO;
          replacement[n++] = String.valueOf(m.getDivisions());
          macro[n] = TICKSZ_MACRO;
          replacement[n++] = String.valueOf(m.getTickSize());
          macro[n] = null;
          template = loadTemplate(PROGRESSBARTICKS_TEMPLATE);
          outputLines = expandMacros(template, macro, replacement);
          writeTemplate(outputLines);
        }
      }
    }
  }   
  
  /**
   * Output AP I radio button.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_RadioButton(String pageEnum, RadioButtonModel m) {
    List<String> template = null;
    List<String> outputLines = null;

    int n = commonAPI(pageEnum, m);
    macro[n] = MARK_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getMarkColor());
    macro[n] = CHECKED_MACRO;
    replacement[n++] = String.valueOf(m.isChecked());
    macro[n] = COUNT_MACRO;
    replacement[n++] = String.valueOf(countCheckBoxes);
    countCheckBoxes++;
    macro[n] = null;
    template = loadTemplate(RADIOBUTTON_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.useDefaultColors()) {
      n = 0;
      macro[n] = FILL_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFillColor());
      macro[n] = FRAME_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFrameColor());
      macro[n] = GLOW_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getSelectedColor());
      macro[n] = null;
      template = loadTemplate(COLOR_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
    String groupId = ((RadioButtonModel)m).getGroupId();
    if (groupId.length() > 0) {
      n=0;
      macro[n] = GROUP_ID_MACRO;
      replacement[n++] = groupId;
      macro[n] = null;
      template = loadTemplate(GROUP_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
  }
  
  /**
   * Output API radio button P.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_RadioButton_P(String pageEnum, RadioButtonModel m) {
    List<String> template = null;
    List<String> outputLines = null;

    int n = commonAPI(pageEnum, m);
    macro[n] = MARK_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getMarkColor());
    macro[n] = CHECKED_MACRO;
    replacement[n++] = String.valueOf(m.isChecked());
    macro[n] = COUNT_MACRO;
    replacement[n++] = String.valueOf(countCheckBoxes);
    countCheckBoxes++;
    String groupId = m.getGroupId();
    if (groupId.length() == 0) 
      groupId = "GSLC_GROUP_ID_NONE";
    macro[n] = GROUP_ID_MACRO;
    replacement[n++] = groupId;
    macro[n] = null;
    template = loadTemplate(RADIOBUTTON_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.useDefaultColors()) {
      n = 0;
      macro[n] = FILL_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFillColor());
      macro[n] = FRAME_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getFrameColor());
      macro[n] = GLOW_COLOR_MACRO;
      replacement[n++] = cf.colorAsString(m.getSelectedColor());
      macro[n] = null;
      template = loadTemplate(COLOR_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
  }
  
  /**
   * Output API slider.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_Slider(String pageEnum, SliderModel m) {
    List<String> template = null;
    List<String> outputLines = null;

    int n = commonAPI(pageEnum, m);
    macro[n] = MARK_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getTickColor());
    macro[n] = CHECKED_MACRO;
    replacement[n++] = String.valueOf(m.isVertical());
    macro[n] = COUNT_MACRO; 
    replacement[n++] = String.valueOf(countSliders);
    countSliders++;
    macro[n] = MIN_MACRO;
    replacement[n++] = String.valueOf(m.getMin());
    macro[n] = MAX_MACRO;
    replacement[n++] = String.valueOf(m.getMax());
    macro[n] = VALUE_MACRO;
    replacement[n++] = String.valueOf(m.getCurValue());
    macro[n] = STYLE_MACRO;
    replacement[n++] = String.valueOf(m.isTrimStyle());
    macro[n] = TRIM_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getTrimColor());
    macro[n] = DIVISIONS_MACRO;
    replacement[n++] = String.valueOf(m.getDivisions());
    macro[n] = TICKSZ_MACRO;
    replacement[n++] = String.valueOf(m.getTickSize());
    macro[n] = THUMBSZ_MACRO;
    replacement[n++] = String.valueOf(m.getThumbSize());
    macro[n] = FILL_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFillColor());
    macro[n] = FRAME_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFrameColor());
    macro[n] = GLOW_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getSelectedColor());
    macro[n] = null;
    template = loadTemplate(SLIDER_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
  }
  
  /**
   * Output API text.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_Text(String pageEnum, TextModel m) {
    String templateFile = null;
    List<String> template = null;
    List<String> outputLines = null;
    String fontId = "";
    String strKey = "";
    String strCount = "";
    String strText = "";
    int n =0;
    // setup common attributes
    macro[n] = ENUM_MACRO;
    replacement[n++] = m.getEnum();
    macro[n] = PAGE_ENUM_MACRO;
    replacement[n++] = pageEnum;
    macro[n] = X_MACRO;
    replacement[n++] = String.valueOf(m.getX());
    macro[n] = Y_MACRO;
    replacement[n++] = String.valueOf(m.getY());
    macro[n] = WIDTH_MACRO;
    replacement[n++] = String.valueOf(m.getTargetWidth());
    macro[n] = HEIGHT_MACRO;
    replacement[n++] = String.valueOf(m.getTargetHeight());
    macro[n] = FONT_ID_MACRO;
    fontId = m.getFontEnum();
    replacement[n++] = fontId; 
    macro[n] = FONT_COUNT_MACRO;
    replacement[n++] = String.valueOf(getFontIndex(fontId));
    int ts = m.getTextStorage();
    if (ts > 0) {
      macro[n] = SIZE_MACRO;
      replacement[n++] = String.valueOf(ts+1); // leave room for trailing '\0' in C Language
      macro[n] = TEXT_MACRO;
      strText = m.getText();
      if (strText.length() > ts)
        strText = strText.substring(0,  ts);  
      replacement[n++] = strText;
      strKey = m.getKey();
      ts = strKey.indexOf("$");
      strCount = strKey.substring(ts+1);
      macro[n] = ID_MACRO;
      replacement[n++] = strCount;
      templateFile = TEXT_UPDATE_TEMPLATE;
    } else {
      macro[n] = TEXT_MACRO;
      replacement[n++] = m.getText();
      templateFile = TEXT_TEMPLATE;
    }
    macro[n] = null;
    template = loadTemplate(templateFile);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    String strAlign = m.getAlignment();
    if (!strAlign.equals("Left")) {
      macro[0] = TEXT_ALIGN_MACRO;
      replacement[0] = convertAlignment(strAlign);
      macro[n] = null;
      template = loadTemplate(ALIGN_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
    if (!m.useDefaultColors()) {
      if (!cf.getDefTextCol().equals(m.getTextColor())) {
        macro[0] = TEXT_COLOR_MACRO;
        replacement[0] = cf.colorAsString(m.getTextColor());
        macro[n] = null;
        template = loadTemplate(TEXTCOLOR_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
      }
      if (!cf.getDefFillCol().equals(m.getFillColor())    || 
          !cf.getDefFrameCol().equals(m.getFrameColor())  ||
          !cf.getDefGlowCol().equals(m.getSelectedColor())) {
        n = 0;
        macro[n] = FILL_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getFillColor());
        macro[n] = FRAME_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getFrameColor());
        macro[n] = GLOW_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getSelectedColor());
        macro[n] = null;
        template = loadTemplate(COLOR_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
      }
    }
    if (m.isUTF8()) {
      pw.printf("  gslc_ElemSetTxtEnc(&m_gui,pElemRef,GSLC_TXT_ENC_UTF8);%n");
    }
    if (!m.isFillEnabled()) {
      pw.printf("  gslc_ElemSetFillEn(&m_gui,pElemRef,false);%n");
    }
    if (m.isFrameEnabled()) {
      pw.printf("  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);%n");
    }
  }
   
  /**
   * Output API text P.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_Text_P(String pageEnum, TextModel m) {
    String templateFile = null;
    List<String> template = null;
    List<String> outputLines = null;
    String fontId = "";
    String strKey = "";
    String strCount = "";
    String strText = "";

    int n =0;
    // setup common attributes
    macro[n] = ENUM_MACRO;
    replacement[n++] = m.getEnum();
    macro[n] = PAGE_ENUM_MACRO;
    replacement[n++] = pageEnum;
    macro[n] = X_MACRO;
    replacement[n++] = String.valueOf(m.getX());
    macro[n] = Y_MACRO;
    replacement[n++] = String.valueOf(m.getY());
    macro[n] = WIDTH_MACRO;
    replacement[n++] = String.valueOf(m.getTargetWidth());
    macro[n] = HEIGHT_MACRO;
    replacement[n++] = String.valueOf(m.getTargetHeight());
    macro[n] = FONT_ID_MACRO;
    fontId = m.getFontEnum();
    replacement[n++] = fontId; 
    macro[n] = FONT_COUNT_MACRO;
    replacement[n++] = String.valueOf(getFontIndex(fontId));
    macro[n] = TEXT_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getTextColor());
    macro[n] = FILL_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFillColor());
    macro[n] = FRAME_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFrameColor());
    macro[n] = TEXT_ALIGN_MACRO;
    replacement[n++] = convertAlignment(m.getAlignment());
    macro[n] = FRAME_EN_MACRO;
    if (m.isFrameEnabled()) 
      replacement[n++] = "true";
    else
      replacement[n++] = "false";
    macro[n] = FILL_EN_MACRO;
    if (m.isFillEnabled()) 
      replacement[n++] = "true";
    else
      replacement[n++] = "false";
    int ts = m.getTextStorage();
    if (ts > 0) {
      macro[n] = SIZE_MACRO;
      replacement[n++] = String.valueOf(ts+1); // leave room for trailing '\0' in C Language
      macro[n] = TEXT_MACRO;
      strText = m.getText();
      if (strText.length() > ts)
        strText = strText.substring(0,  ts); 
      replacement[n++] = strText;
      strKey = m.getKey();
      ts = strKey.indexOf("$");
      strCount = strKey.substring(ts+1);
      macro[n] = ID_MACRO;
      replacement[n++] = strCount;
      templateFile = TEXT_UPDATE_TEMPLATE;
    } else {
      macro[n] = TEXT_MACRO;
      replacement[n++] = m.getText();
      templateFile = TEXT_TEMPLATE;
    }
    macro[n] = null;
    template = loadTemplate(templateFile);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
  }
   
  /**
   * Output API text box.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_TextBox(String pageEnum, TextBoxModel m) {
    List<String> template = null;
    List<String> outputLines = null;
    String fontId = "";
    String strKey = "";
    String strCount = "";

    int n = commonAPI(pageEnum, m);
    macro[n] = CHECKED_MACRO;
    replacement[n++] = String.valueOf(((TextBoxModel)m).wrapText());
    strKey = m.getKey();
    int tb = strKey.indexOf("$");
    strCount = strKey.substring(tb+1);
    macro[n] = ID_MACRO; 
    replacement[n++] = strCount;
    macro[n] = COUNT_MACRO; 
    replacement[n++] = String.valueOf(countSliders);
    countSliders++;
    macro[n] = ROWS_MACRO;
    replacement[n++] = String.valueOf(((TextBoxModel)m).getNumTextRows());
    macro[n] = COLS_MACRO;
    replacement[n++] = String.valueOf(((TextBoxModel)m).getNumTextColumns());
    macro[n] = FONT_ID_MACRO;
    fontId = m.getFontEnum();
    replacement[n++] = fontId; 
    macro[n] = FILL_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFillColor());
    macro[n] = FRAME_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFrameColor());
    macro[n] = GLOW_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getSelectedColor());
    macro[n] = null;
    template = loadTemplate(TEXTBOX_TEMPLATE);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
  }
  
  /**
   * Output API txt button.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_TxtButton(String pageEnum, TxtButtonModel m) {
    String templateFile = null;
    List<String> template = null;
    List<String> outputLines = null;
    String fontId = "";
    String strKey = "";
    String strCount = "";
    String strText = "";

    int n = commonAPI(pageEnum, m);
    macro[n] = FONT_ID_MACRO;
    fontId = m.getFontEnum();
    replacement[n++] = fontId; 
    macro[n] = FONT_COUNT_MACRO;
    replacement[n++] = String.valueOf(getFontIndex(fontId));
    int ts = m.getTextStorage();
    if (ts > 0) {
      macro[n] = SIZE_MACRO;
      replacement[n++] = String.valueOf(ts+1); // leave room for trailing '\0' in C Language
      macro[n] = TEXT_MACRO;
      strText = m.getText();
      if (strText.length() > ts)
        strText = strText.substring(0,  ts);  
      replacement[n++] = strText;
      strKey = m.getKey();
      ts = strKey.indexOf("$");
      strCount = strKey.substring(ts+1);
      macro[n] = ID_MACRO;
      replacement[n++] = strCount;
      templateFile = TXTBUTTON_UPDATE_TEMPLATE;
    } else {
      macro[n] = TEXT_MACRO;
      replacement[n++] = m.getText();
      templateFile = TXTBUTTON_TEMPLATE;
    }
    macro[n] = null;
    template = loadTemplate(templateFile);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (!m.useDefaultColors()) {
      if (!cf.getDefTextCol().equals(m.getTextColor())) {
        macro[0] = TEXT_COLOR_MACRO;
        replacement[0] = cf.colorAsString(m.getTextColor());
        macro[n] = null;
        template = loadTemplate(TEXTCOLOR_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
      }
      if (!cf.getDefFillCol().equals(m.getFillColor())    || 
          !cf.getDefFrameCol().equals(m.getFrameColor())  ||
          !cf.getDefGlowCol().equals(m.getSelectedColor())) {
        n = 0;
        macro[n] = FILL_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getFillColor());
        macro[n] = FRAME_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getFrameColor());
        macro[n] = GLOW_COLOR_MACRO;
        replacement[n++] = cf.colorAsString(m.getSelectedColor());
        macro[n] = null;
        template = loadTemplate(COLOR_TEMPLATE);
        outputLines = expandMacros(template, macro, replacement);
        writeTemplate(outputLines);
      }
    }
    String strAlign = m.getAlignment();
    if (!strAlign.equals("Center")) {
      macro[0] = TEXT_ALIGN_MACRO;
      replacement[0] = convertAlignment(strAlign);
      macro[n] = null;
      template = loadTemplate(ALIGN_TEMPLATE);
      outputLines = expandMacros(template, macro, replacement);
      writeTemplate(outputLines);
    }
    if (m.isUTF8()) {
      pw.printf("  gslc_ElemSetTxtEnc(&m_gui,pElemRef,GSLC_TXT_ENC_UTF8);%n");
    }
    if (!m.isFillEnabled()) {
      pw.printf("  gslc_ElemSetFillEn(&m_gui,pElemRef,false);%n");
    }
    if (m.isFrameEnabled()) {
      pw.printf("  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);%n");
    }
  }
  
  /**
   * Output API txt button P.
   *
   * @param pageEnum
   *          the page enum
   * @param m
   *          the m
   */
  private void outputAPI_TxtButton_P(String pageEnum, TxtButtonModel m) {
    String templateFile = null;
    List<String> template = null;
    List<String> outputLines = null;
    String fontId = "";
    String strKey = "";
    String strCount = "";
    String strText = "";

    int n = commonAPI(pageEnum, m);
    macro[n] = FONT_ID_MACRO;
    fontId = m.getFontEnum();
    replacement[n++] = fontId; 
    macro[n] = FONT_COUNT_MACRO;
    replacement[n++] = String.valueOf(getFontIndex(fontId));
    macro[n] = TEXT_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getTextColor());
    macro[n] = FILL_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFillColor());
    macro[n] = FRAME_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getFrameColor());
    macro[n] = GLOW_COLOR_MACRO;
    replacement[n++] = cf.colorAsString(m.getSelectedColor());
    macro[n] = TEXT_ALIGN_MACRO;
    replacement[n++] = convertAlignment(m.getAlignment());
    macro[n] = FRAME_EN_MACRO;
    if (m.isFrameEnabled()) 
      replacement[n++] = "true";
    else
      replacement[n++] = "false";
    macro[n] = FILL_EN_MACRO;
    if (m.isFillEnabled()) 
      replacement[n++] = "true";
    else
      replacement[n++] = "false";
    int ts = m.getTextStorage();
    if (ts > 0) {
      macro[n] = SIZE_MACRO;
      replacement[n++] = String.valueOf(ts+1); // leave room for trailing '\0' in C Language
      macro[n] = TEXT_MACRO;
      strText = m.getText();
      if (strText.length() > ts)
        strText = strText.substring(0,  ts); 
      replacement[n++] = strText;
      strKey = m.getKey();
      ts = strKey.indexOf("$");
      strCount = strKey.substring(ts+1);
      macro[n] = ID_MACRO;
      replacement[n++] = strCount;
      templateFile = TXTBUTTON_UPDATE_TEMPLATE;
    } else {
      macro[n] = TEXT_MACRO;
      replacement[n++] = m.getText();
      templateFile = TXTBUTTON_TEMPLATE;
    }
    macro[n] = null;
    template = loadTemplate(templateFile);
    outputLines = expandMacros(template, macro, replacement);
    writeTemplate(outputLines);
    if (m.isUTF8()) {
      pw.printf("  gslc_ElemSetTxtEnc(&m_gui,pElemRef,GSLC_TXT_ENC_UTF8);%n");
    }
    if (!m.isFillEnabled()) {
      pw.printf("  gslc_ElemSetFillEn(&m_gui,pElemRef,false);%n");
    }
    if (m.isFrameEnabled()) {
      pw.printf("  gslc_ElemSetFillEn(&m_gui,pElemRef,true);%n");
    }
  }

  /**
   * Output button CB.
   *
   * @param mList
   *          the m list
   */
  private void outputButtonCB(List<WidgetModel> mList) {
    if (mList.size() >= 1) {
      List<String> templateLines;
      List<String> templateStandard;
      List<String> templateChgPage;
      List<String> outputLines;
      String t = "";
      int n =0;
      // create our callback section - start by opening our templates
      templateLines = loadTemplate(BUTTON_CB_TEMPLATE);
      for(int i=0; i<templateLines.size(); i++) {
        t = templateLines.get(i);
        pw.printf("%s%n", t);
        if (t.equals(BUTTON_ENUMS_SECTION)) {
          n = i;
          break;
        }
      }
      templateStandard = loadTemplate(BUTTON_LOOP_TEMPLATE);
      templateChgPage = loadTemplate(BUTTON_CHGPG_TEMPLATE);
      macro[0] = ENUM_MACRO;
      macro[1] = PAGE_ENUM_MACRO;
      macro[2] = null;
      for (WidgetModel m : mList) {
        replacement[0] = m.getEnum();
        replacement[1] = "";
        if (m.getType().equals(EnumFactory.TEXTBUTTON)) {
           if (((TxtButtonModel)m).isChangePageFunct()) {
            replacement[1] = ((TxtButtonModel)m).getChangePageEnum();
            outputLines = expandMacros(templateChgPage, macro, replacement);
            writeTemplate(outputLines);
           } else {
            outputLines = expandMacros(templateStandard, macro, replacement);
            writeTemplate(outputLines);
           }
        } else if (((ImgButtonModel)m).isChangePageFunct()) {
          replacement[1] = ((ImgButtonModel)m).getChangePageEnum();
          outputLines = expandMacros(templateChgPage, macro, replacement);
          writeTemplate(outputLines);
        } else {
          outputLines = expandMacros(templateStandard, macro, replacement);
          writeTemplate(outputLines);
        }
      }
      for(int j=n+1; j<templateLines.size(); j++) {
        pw.printf("%s%n", templateLines.get(j));
      }
    }
  }
  
  /**
   * Output callback.
   *
   * @param eList
   *          the e list
   * @param section
   *          the section
   * @param cbTemplate
   *          the cb template
   * @param loopTemplate
   *          the loop template
   */
  private void outputCallback(List<String> eList , String section, String cbTemplate, String loopTemplate) {
    if (eList.size() >= 1) {
      List<String> templateLines;
      List<String> loopLines;
      List<String> outputLines;
      String t = "";
      int n =0;
      // create our callback section - start by opening our templates
      templateLines = loadTemplate(cbTemplate);
      for(int i=0; i<templateLines.size(); i++) {
        t = templateLines.get(i);
        pw.printf("%s%n", t);
        if (t.equals(section)) {
          n = i;
          break;
        }
      }
      loopLines = loadTemplate(loopTemplate);
      macro[0] = ENUM_MACRO;
      macro[1] = null;
      for (int e=0; e<eList.size(); e++) {
        replacement[0] = eList.get(e);
        outputLines = expandMacros(loopLines, macro, replacement);
        writeTemplate(outputLines);
      }
      for(int j=n+1; j<templateLines.size(); j++) {
        pw.printf("%s%n", templateLines.get(j));
      }
    }
  }
  
//-------------------------------------------------------------------------------------------------------
//Utility methods
//-------------------------------------------------------------------------------------------------------

  /**
 * Gets the list of enums.
 *
 * @param widgetTypes
 *          the widget types
 * @return the list of enums
 */
private List<String> getListOfEnums(List<String> widgetTypes) {
   // build up a list of widgets that match
   List<String> eList = new ArrayList<String>();
   List<WidgetModel> mList = new ArrayList<WidgetModel>();
   for (PagePane p : pages) {
     getModelsByType(p.getWidgets(), widgetTypes, mList);
   }
   // now pull out from the models our matching widget's enums
   String ref = "";
   String strKey = "";
   String strCount = "";
   int n;
   for (WidgetModel m : mList) {
     if (m.getType().equals(EnumFactory.TEXTBOX)) {
       // textbox has an embedded scrollbar so add it not TextBox
       ref = "E_SCROLLBAR";
       strKey = m.getKey();
       n = strKey.indexOf("$");
       strCount = strKey.substring(n+1);
       ref = ref + strCount;
       eList.add(ref);
     } else {
       eList.add(m.getEnum());
     }
   }
   if (eList.size() > 1) {
     Collections.sort(eList);
   }
   return eList;
 }
 
 /**
  * Search for enum.
  *
  * @param lines
  *          the lines
  * @param search
  *          the search
  * @return <code>true</code>, if successful
  */
 private boolean searchForEnum(List<String> lines, String search) {
   int n = 0;
   for(String l : lines) {
      n = l.indexOf(search);
      if (n >= 0) return true;
   }
   return false;
 }
 
  /**
   * Read pass string.
   *
   * @param endString
   *          the end string
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void readPassString(String endString)  throws IOException {
    while ((line = br.readLine()) != null) {
      if (line.equals(endString)) break;
    }
    pw.printf("%s%n",endString);
  }
  
  /**
   * Load template.
   *
   * @param templateName
   *          the template name
   * @return the <code>list</code> object
   */
  private List<String> loadTemplate(String templateName) {
    Integer idx = Integer.valueOf(0);  // always return something...
    if (templateMap.containsKey(templateName)) {
      idx = templateMap.get(templateName);
    } else {
      List<String> retErr = new ArrayList<String>();
      retErr.add(new String("Missing template: " + templateName));
    }
    return listOfTemplates[idx.intValue()];
  }

  /**
   * Store templates.
   *
   * @param templateFileName
   *          the template file name
   */
  private void storeTemplates(String templateFileName) {
    templateMap = new HashMap<String, Integer>(64);
    String pathName = RESOURCES_PATH + templateFileName;
    BufferedReader tbr = new BufferedReader(new InputStreamReader(
                  this.getClass().getResourceAsStream(pathName)));
    String l = "";
    String templateName = "";
    int i = 0;
    try {
      while((templateName = tbr.readLine()) != null) {
        if (templateName.equals(END_TEMPLATE))
          break;
        List<String> lines = new ArrayList<String>();
        while (!(l = tbr.readLine()).equals(STOP_TEMPLATE)) {
          lines.add(l);
        }
        templateMap.put(templateName, i);
        listOfTemplates[i] = lines;
//        System.out.println("Stored Template: " + templateName + " idx=" + i);
        i++;
      }
      
    } catch (IOException e) {
      e.printStackTrace();
    } finally {
      try {
        tbr.close();
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
  }
    
  /**
   * Expand macros.
   *
   * @param lines
   *          the lines
   * @param search
   *          the search
   * @param replace
   *          the replace
   * @return the <code>list</code> object
   */
  private List<String> expandMacros(List<String> lines, String[] search, String[] replace) {
    List<String> outputList = new ArrayList<String>();
    for (String l : lines) {
      Matcher m = MACRO_PATTERN.matcher(l);
      StringBuffer sb = new StringBuffer();
      while (m.find()) {
        String value = m.group(1);
        for (int j=0; search[j]!=null; j++) {
          if(value.equals(search[j])) {
            m.appendReplacement(sb, replace[j]);
            break;
          } 
        }
      }
      m.appendTail(sb);
      outputList.add(sb.toString());
    }
    return outputList;
  }

  /**
   * Write template.
   *
   * @param lines
   *          the lines
   */
  private void writeTemplate(List<String> lines) {
    for (String l : lines)
      pw.printf("%s%n", l);
  }
  
  /**
   * Gets the widget models by type.
   *
   * @param widgets
   *          the widgets
   * @param widgetTypes
   *          the widget types
   * @param selected
   *          the selected
   * @return the models by type
   */
  private void getModelsByType(List<Widget> widgets, List<String> widgetTypes, List<WidgetModel> selected) {
    for (Widget w : widgets) {
      for (String type : widgetTypes) {
        if (w.getType().equals(type)) {
          selected.add(w.getModel());
        }
      }
    }
  }

  /**
   * Count by type.
   *
   * @param widgets
   *          the widgets
   * @param widgetTypes
   *          the widget types
   * @return the <code>int</code> object
   */
  private int countByType(List<Widget> widgets, List<String> widgetTypes) {
    int count = 0;
    for (Widget w : widgets) {
      for (String type : widgetTypes) {
        if (w.getType().equals(type)) {
          count++;
        }
      }
    }
    return count;
  }

  /**
   * Sort list and remove duplicates.
   *
   * @param list
   *          the list
   */
  private void sortListandRemoveDups(List<String> list) {
    if (list.size() > 1) {
      Collections.sort(list);
      String s = null;
      String prev = null;
      ListIterator<String> litr = list.listIterator();
      while(litr.hasNext()) {
        s = litr.next();
        if (s.equals(prev))
          litr.remove();
        else 
          prev = s;
      }
    }
  }
  
  /**
   * Prints the enums.
   *
   * @param enumList
   *          the enum list
   */
  private void printEnums(List<String> enumList) {
    for (int i=0; i<enumList.size(); i++) {
      if (i%4 == 0 && i != 0) printState=OVERFLOW_LINE;
      switch (printState) {
        case BEGIN_LINE:
          pw.printf("enum {%s", enumList.get(i));
          printState = WRITE_NEXT;
          break;
        case WRITE_NEXT:
          pw.printf(",%s", enumList.get(i));
          break;
        case OVERFLOW_LINE:
          pw.printf("%n      ,%s", enumList.get(i));
          printState = WRITE_NEXT;
          break;
      }
    }
    pw.printf("};%n");
    printState = BEGIN_LINE;
    return;
  }
  
  /**
   * getFontIndex() - scan fontList for fontId and return its index in the list.
   *
   * @param fontId
   *          the font id
   * @return index
   */
  private int getFontIndex(String fontId) {
    int i = 0;
    for (Pair f : fontEnums) {
      if (f.getValue().equals(fontId)) {
        break;
      }
      i++;
    }
    return i;
  }

  /**
   * Convert enum 
   * strips leading "E_" off of ENUM.
   *
   * @param enum
   *          the enum
   * @return the <code>String</code> without 'E_' at beginning
   */
  private String convertEnum(String strEnum) {
    if (strEnum.startsWith("E_")) {
      return strEnum.substring(2);
    }
    return strEnum;
  }
  
  /**
   * Convert alignment.
   *
   * @param align
   *          the align
   * @return the <code>string</code> object
   */
  private String convertAlignment(String align) {
    switch (align)
    {
      case "Left":
        return "GSLC_ALIGN_MID_LEFT";
      case "Center":
        return "GSLC_ALIGN_MID_MID";
      case "Right":
        return "GSLC_ALIGN_MID_RIGHT";
      default:
        return "UNKOWN_ALIGNMENT";
    }
  }
 
  /**
   * The Private Class Pair used to store Font Enum (ID) and Font Display Name relationship.
   * It allows the Code Generator to create the gslc_FontAdd() mapping.
   */
  private class Pair {
    
    /** The key. */
    String key;
    
    /** The value. */
    String value;

    /**
     * Instantiates a new pair.
     *
     * @param key
     *          the key
     * @param value
     *          the value
     */
    Pair(String key, String value) {
      this.key = key;
      this.value = value;
    }

    /**
     * Gets the key.
     *
     * @return the key
     */
    private String getKey() {
      return key;
    }
    
    /**
     * Gets the value.
     *
     * @return the value
     */
    private String getValue() {
      return value;
    }

    /**
     * toString
     *
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
      return String.format("%s->%s",getKey(),getValue());
    }
  }
 
}
