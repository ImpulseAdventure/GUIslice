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
package builder;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.GridLayout;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.PrintStream;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JDesktopPane;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.WindowConstants;

import builder.common.CommonUtil;
import builder.common.TestException;
import builder.controller.CodeGenerator;
import builder.controller.Controller;
import builder.controller.PropManager;
import builder.controller.UserPrefsManager;
import builder.events.MsgBoard;
import builder.events.MsgEvent;
import builder.prefs.GeneralEditor;
import builder.prefs.ModelEditor;
import builder.views.MenuBar;
import builder.views.PagePane;
import builder.views.ToolBar;
import builder.views.Toolbox;
import builder.views.TreeView;

/**
 * GUIsliceBuilder is the main class of the application.
 * <p>
 * GUIslice C library is a lightweight platform-independent GUI designed 
 * for embedded system TFT displays.
 * </p>
 * <p>
 * The purpose of GUIsliceBuilder application is to take away some of
 * the drudgery required by the GUIslice C library.   It allows users to
 * layout their UI visually and generate a skeleton C file for the target
 * platform, either Arduino, Arduino min (using flash storage), or Linux.
 * </p>
 *<p>
 * An important design goal was to allow users to incrementally build 
 * their UI, generate a .ino or .c file, add code for handling some of
 * the UI pieces (say for some sensors), and then go back to the builder
 * and add more UI bits without losing their code additions.
 * </p>
 * <p>
 * This application follows the Model View Controller Pattern.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class Builder  extends JDesktopPane {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant VERSION_NO. */
  public static final String VERSION_NO = "1.01";
  
  /** The Constant PROGRAM_TITLE. */
  public static final String PROGRAM_TITLE = "GUIslice Builder";
  
  /** The Constant NEW_PROJECT. */
  public static final String NEW_PROJECT = " - unnamed project";
  
  /** The canvas width */
  public static int CANVAS_WIDTH;
  
  /** The canvas height */
  public static int CANVAS_HEIGHT;
  
  /** The frame. */
  private JFrame frame;
  
  /** The user preferences. */
  private UserPrefsManager userPreferences;
  
  /** The controller. */
  protected Controller controller;
  
  /** The base pane. */
  private JPanel basePane;
  
  /** The toolbar. */
  private ToolBar toolbar;
  
  /** The toolbox. */
  private Toolbox toolbox;
  
  /** The property views. */
  private JPanel propertyViews;
  
  /** The test platform, if any. */
  public static String testPlatform = null;
  
  /** The boolean indicating running on a MacOS system */
  public static boolean isMAC = false;
  
  /**
   * The main method.
   *
   * @param args
   *          the arguments
   */
  public static void main(String[] args) {
    Builder builder = new Builder();
    if (args.length == 3) {
      boolean result = builder.testCodeGen(args[0], args[1], args[2]); 
      if (result)
        System.exit(0);
      else
        System.exit(1);
    }

    double version = Double.parseDouble(System.getProperty("java.specification.version"));
    if (version < 1.8) {
      String msg = String.format("Java 8 is needed to run. Yours is %.2f", version);
      JOptionPane.showMessageDialog(null, 
        msg, "Failure", JOptionPane.ERROR_MESSAGE);
    }
/*  Use this code for Java 9 and above
    if (version < 9) {
      String msg = String.format("Java 9 or higher is needed to run. Yours is %.2f", version);
      JOptionPane.showMessageDialog(null, 
        msg, "Failure", JOptionPane.ERROR_MESSAGE);
    }
*/
    builder.startUp();
  }

  /**
   * Instantiates a new GUIslice builder.
   */
  public Builder() {
  }
  
  /**
   * testCodeGen used by JUnit tests.
   *
   * @param testFile
   *          name of project file to read as a test case
   * @param outputFile
   *          name of output file to write as a test case
   * @return true if successful
   */
  public boolean testCodeGen(String target, String testFile, String outputFile) {
    try {
      testPlatform = target;
      String workDir = CommonUtil.getInstance().getWorkingDir();
      String fileName = new String(workDir + testFile + ".prj");
//      System.out.println("testCodeGen: " + fileName);
      CodeGenerator cg = CodeGenerator.getTestInstance();
      List<PagePane> pages = ReadTestCaseProject(fileName);
      fileName = new String(workDir + outputFile);
      cg.generateTestCode(testPlatform, fileName, pages);
      return true;
    } catch (Exception e) {
      e.printStackTrace();
      return false;
    }
  }
  
  /**
   * starts the builder program.
   */
  public void startUp() {

    EventQueue.invokeLater(new Runnable() {
        @Override
        public void run() {
          setDragMode(JDesktopPane.OUTLINE_DRAG_MODE);
          initUI();
          try {
            String osName = System.getProperty("os.name").toLowerCase();
            isMAC = osName.startsWith("mac os x");
            if (isMAC) 
            {
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } else{
                UIManager.setLookAndFeel(GeneralEditor.getInstance().getThemeClassName());
            }            
            SwingUtilities.updateComponentTreeUI(frame);
          } catch (ClassNotFoundException | InstantiationException | IllegalAccessException | UnsupportedLookAndFeelException ex) {
          }
          frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent we) {
              String title = "Confirm Dialog";
              String message = "You're about to quit the application -- are you sure?";
              int answer = JOptionPane.showConfirmDialog(null,message,title, JOptionPane.YES_NO_OPTION); 
              if(answer == JOptionPane.YES_OPTION) {
                System.exit(0);
              } else
                frame.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
            }
          });
          frame.setVisible(true);
        }
    });

    Thread.setDefaultUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
      @Override
      public void uncaughtException(Thread t, Throwable e) {
          Calendar cal = Calendar.getInstance();
          SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd_HHmmss");
          String workingDir = CommonUtil.getInstance().getWorkingDir();
          File directory = new File(workingDir + "logs");
          if(!directory.exists()){
            directory.mkdir();
          }
          String fileName = workingDir + "logs" 
            + System.getProperty("file.separator")
            + sdf.format(cal.getTime())+".txt";
          PrintStream writer;
          try {
              writer = new PrintStream(fileName, "UTF-8");
              writer.println(e.getClass() + ": " + e.getMessage());
              for (int i = 0; i < e.getStackTrace().length; i++) {
                  writer.println(e.getStackTrace()[i].toString());
              }
              String msg = String.format("A fatal error has occurred. A crash log created as %s", fileName);
              JOptionPane.showMessageDialog(null, 
                  msg, "Failure", JOptionPane.ERROR_MESSAGE);

          } catch (IOException e1) {
              e1.printStackTrace();
          }
      }
    });

  }
  
  /**
   * Creates the UI pieces.  
   */
  private void initUI() {
    // access our controllers
    PropManager propManager = PropManager.getInstance();
    controller = Controller.getInstance();
    toolbox = new Toolbox();
    
    // create our menu bar
    MenuBar mb = new MenuBar();
    mb.addListeners(controller);
    
    // create out tool bar
    toolbar = ToolBar.getInstance();
    toolbar.addListeners(controller);
    toolbar.setFloatable(false);


    // create our main frame and add our panels
    String frameTitle = PROGRAM_TITLE + NEW_PROJECT;
    frame = new JFrame(frameTitle);
    frame.addComponentListener(new FrameListen());;

    frame.setIconImage(new ImageIcon(Builder.class.getResource("/resources/icons/application.png")).getImage());
    // pass on top level frame to controller so it can change project names
    controller.setFrame(frame); 
    // trap the red X on our main frame
    frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

    // setup our User Preference UI
    List<ModelEditor> prefEditors = controller.initUserPrefs();
    userPreferences = new UserPrefsManager(frame, prefEditors);
    controller.setUserPrefs(userPreferences);
    
    frame.setJMenuBar(mb);
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.setLayout(new BorderLayout());

    basePane = new JPanel(new BorderLayout());
    basePane.add(toolbar,BorderLayout.NORTH);
    basePane.add(toolbox, BorderLayout.WEST);
    
    // our property views are made up of one JTable for viewing and editing properties 
    // and one JTree to view our object hierarchy and allow changing the drawing Z-Order.
    propertyViews = new JPanel(new GridLayout(0,1));
    propertyViews.add(propManager);
    propertyViews.add(TreeView.getInstance());
    
    basePane.add(propertyViews, BorderLayout.EAST);
    basePane.add(controller,BorderLayout.CENTER);
    frame.add(basePane);
    
    int width = Math.max(GeneralEditor.getInstance().getWidth()+600, 700);
    int height = Math.max(GeneralEditor.getInstance().getHeight()+40, 700);
    frame.setPreferredSize(new Dimension(width, height));
    frame.pack();
    
    // setup our canvas offsets
    // NOTE: we can't use controller.getPanel().getSize()
    // since its a scrollpane thats much larger than actual screen size
    CANVAS_WIDTH = GeneralEditor.getInstance().getWidth()+205;
    CANVAS_HEIGHT = GeneralEditor.getInstance().getHeight()+235;
    Dimension canvasSz = new Dimension(CANVAS_WIDTH, CANVAS_HEIGHT);
    CommonUtil.getInstance().setWinOffsets(canvasSz,
        GeneralEditor.getInstance().getWidth(),
        GeneralEditor.getInstance().getHeight());
    frame.setLocationRelativeTo(null);
    frame.setVisible(true);
  }
  
  /**
   * The Class FrameListen traps the resize frame event so we can
   * reset our point mapping function to the new settings.
   */
  private class FrameListen implements ComponentListener{
    
    /**
     * componentHidden
     *
     * @see java.awt.event.ComponentListener#componentHidden(java.awt.event.ComponentEvent)
     */
    public void componentHidden(ComponentEvent arg0) {
    }
    
    /**
     * componentMoved
     *
     * @see java.awt.event.ComponentListener#componentMoved(java.awt.event.ComponentEvent)
     */
    public void componentMoved(ComponentEvent arg0) {   
    }
    
    /**
     * componentResized
     *
     * @see java.awt.event.ComponentListener#componentResized(java.awt.event.ComponentEvent)
     */
    public void componentResized(ComponentEvent arg0) {
      MsgEvent ev = new MsgEvent();
      ev.message ="";
      ev.parent = "";
      ev.code = MsgEvent.CANVAS_MODEL_CHANGE;
      MsgBoard.getInstance().publish(ev);
    }
    
    /**
     * componentShown
     *
     * @see java.awt.event.ComponentListener#componentShown(java.awt.event.ComponentEvent)
     */
    public void componentShown(ComponentEvent arg0) {

    }
  }
  
  /**
   * Read test case project.
   *
   * @param testFile
   *          the test file
   * @return list of pages
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   * @throws TestException
   *           Signals that an test exception has occurred.
   * @throws ClassNotFoundException
   *           Signals that an class exception has occurred.
   */
  private List<PagePane> ReadTestCaseProject(String testFile) 
      throws IOException, TestException, ClassNotFoundException {
    ObjectInputStream in = new ObjectInputStream(new FileInputStream(new File(testFile)));
    String pageKey = null;
    String pageEnum = null;
//    System.out.println("test project: " + testFile);
    List<PagePane> pages = new ArrayList<PagePane>();
    PagePane p = null;
    // Read in version number
    // Read in version number
    String strVersion = (String)in.readObject();
    if (strVersion.equals("1.01")) {
      // read in target platform
      String target = (String)in.readObject();
      if (!testPlatform.equals(target)) {
        in.close();
        throw new TestException("Project file's target does not match expected platform");
      }
    } else {
      in.close();
      throw new TestException("Corrupted Test Project");
    }
    @SuppressWarnings("unused")
    String currentPageKey = (String)in.readObject();
    int cnt = in.readInt();
//      System.out.println("pages: " + cnt);
    for (int i=0; i<cnt; i++) {
      pageKey = (String)in.readObject();
//        System.out.println("restore page: " + pageKey);
      pageEnum = (String)in.readObject();
//      System.out.println("restore page: " + pageEnum);
      p = new PagePane(pageKey, pageEnum);
      p.restoreForTest((String)in.readObject());
      pages.add(p);
    }
    in.close();
    return pages;
  }
}
