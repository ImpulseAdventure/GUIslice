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

import java.awt.CardLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Base64;
import java.util.List;
import java.util.ListIterator;
import java.util.Observable;
import java.util.Observer;
/*
Use these imports instead of Observable and Observer for Java 9 and up.
import java.util.prefs.PreferenceChangeEvent;
import java.util.prefs.PreferenceChangeListener;
import java.util.prefs.Preferences;
*/

import javax.swing.AbstractButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JInternalFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileView;

import builder.Builder;
import builder.commands.AddPageCommand;
import builder.commands.AddWidgetCommand;
import builder.commands.AlignBottomCommand;
import builder.commands.AlignCenterCommand;
import builder.commands.AlignHSpacingCommand;
import builder.commands.AlignHeightCommand;
import builder.commands.AlignLeftCommand;
import builder.commands.AlignRightCommand;
import builder.commands.AlignTopCommand;
import builder.commands.AlignVSpacingCommand;
import builder.commands.AlignWidthCommand;
import builder.commands.ChangeZOrderCommand;
import builder.commands.Command;
import builder.commands.DelPageCommand;
import builder.commands.DelWidgetCommand;
import builder.commands.GroupCommand;
import builder.commands.History;
import builder.common.CommonUtil;
import builder.common.EnumFactory;
import builder.common.FontFactory;
import builder.events.MsgBoard;
import builder.events.MsgEvent;
import builder.events.iSubscriber;
import builder.models.GeneralModel;
import builder.models.GridModel;
import builder.models.PageModel;
import builder.models.TextModel;
import builder.prefs.BoxEditor;
import builder.prefs.CheckBoxEditor;
import builder.prefs.GeneralEditor;
import builder.prefs.GridEditor;
import builder.prefs.ModelEditor;
import builder.prefs.RadioButtonEditor;
import builder.prefs.TextEditor;
import builder.prefs.TxtButtonEditor;
import builder.views.PagePane;
import builder.views.TreeView;
import builder.widgets.Widget;

/**
 * The Class Controller of the Model View Controller Pattern.
 *
 * 
 * @author Paul Conti
 * 
 */
public class Controller extends JInternalFrame 
  implements ActionListener, iSubscriber, Observer {
// Use PreferenceChangeListener instead of Observer for Java 9 and above
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The cards. */
  private JPanel cards;
  
  /** The top frame. */
  private JFrame topFrame;  // used to set title when changing projects

  /** The user preferences. */
  private UserPrefsManager userPreferences;
  
  /** The grid editor. */
  private GridEditor gridEditor;
  
  /** The general editor. */
  private GeneralEditor generalEditor;
  
  /** The str theme. */
  private String strTheme;
  
  /** The target DPI. */
  private int targetDPI;
  
  /** The display width. */
  private int displayWidth;
  
  /** The display height. */
  private int displayHeight;
  
  /** The current page. */
  private PagePane currentPage;
  
  /** The current page key. */
  private String currentPageKey;
  
  /** The project file. */
  private File projectFolder = null;
  
  /** The project file. */
  private File projectFile = null;
  
  /** The title. */
  private String title;
  
  /** The pages. */
  List<PagePane> pages = new ArrayList<PagePane>();
  
  /** The litr. */
  ListIterator<PagePane> litr;
  
  /** The layout. */
  CardLayout layout;
  
  /** The instance. */
  private static Controller instance = null;
  
  /**
   * Gets the single instance of Controller.
   *
   * @return single instance of Controller
   */
  public static synchronized Controller getInstance() {
      if (instance == null) {
          instance = new Controller();
      }
      return instance;
  }  

  /**
   * Instantiates a new controller.
   */
  public Controller() {
    title = "Simulated TFT Panel";
    this.generalEditor = GeneralEditor.getInstance();
    MsgBoard.getInstance().subscribe(this);
    layout = new CardLayout();
    cards = new JPanel(layout);
    JScrollPane scrollPane = new JScrollPane(cards);
    
    this.add(scrollPane);
    this.setTitle(title); 
    createFirstPage();
    this.setVisible(true);
  }
  
  /**
   * Sets the frame.
   *
   * @param frame
   *          the new frame
   */
  public void setFrame(JFrame frame) {
    topFrame = frame;
  }
  
  public JFrame getFrame() {
    return topFrame;
  }
  
  /**
   * Gets the panel.
   *
   * @return the panel
   */
  public JPanel getPanel() {
    return cards;
  }
  
  /**
   * Sets the user prefs.
   *
   * @param userPreferences
   *          the new user prefs
   */
  public void setUserPrefs(UserPrefsManager userPreferences) {
    this.userPreferences = userPreferences;
  }

  /**
   * Refresh view.
   */
  public void refreshView() {
    currentPage.refreshView();
  }
  
  /**
   * Find page.
   *
   * @param pageKey
   *          the page key
   * @return the <code>page pane</code> object
   */
  public PagePane findPage(String pageKey) {
    litr = pages.listIterator();
    PagePane p = null;
    String searchKey;
    while(litr.hasNext()){
      p = litr.next();
      searchKey = p.getKey();
      if (searchKey.equals(pageKey)) {
        break;
      }
    }
    return p;
  }
  
  /**
   * Adds the page to view.
   *
   * @param page
   *          the page
   */
  public void addPageToView(PagePane page) {
    pages.add(page);
    cards.add(page, page.getModel().getKey());
    layout.last(cards);
  }
  
  /**
   * Change page.
   *
   * @param pageKey
   *          the page key
   */
  public void changePage(String pageKey) {
    PagePane page = findPage(pageKey);
    if (page != null) {
      currentPageKey = pageKey;
      currentPage = page;
      layout.show(cards, currentPageKey);
      currentPage.refreshView();
      PropManager.getInstance().showPropEditor(currentPageKey);
    }
  }
  
  /**
   * Change Z order.
   *
   * @param e
   *          the e
   */
  public void changeZOrder(MsgEvent e) {
    if (!e.parent.equals(currentPageKey)) {
      changePage(e.parent);
    }
    String tree_backup = TreeView.getInstance().getSavedBackup();
    ChangeZOrderCommand c = new ChangeZOrderCommand(currentPage, tree_backup);
    c.change(e.message, e.fromIdx, e.toIdx);
    execute(c);
  }
  
  /**
   * Change view.
   *
   * @param e
   *          the e
   */
  public void changeView(MsgEvent e) {
    if (e.parent.equals("Root")) {
      if (!e.message.equals(currentPageKey)) {
        changePage(e.message);
        PropManager.getInstance().showPropEditor(currentPageKey);
      }
    } else {
      if (!e.parent.equals(currentPageKey)) {
        changePage(e.parent);
      }
      currentPage.selectName(e.message);
      PropManager.getInstance().showPropEditor(e.message);
    }
  }
  
  // this function is called in our constructor and by newProject()
  /**
   * Creates the first page.
   */
  // It prevents user from undo'ing our Page_1
  public void createFirstPage() {
    currentPage = new PagePane();
    PageModel m = (PageModel) currentPage.getModel();
    currentPageKey = EnumFactory.getInstance().createKey(EnumFactory.PAGE);
    m.setKey(currentPageKey);
    String pageEnum = EnumFactory.getInstance().createEnum(EnumFactory.PAGE);
    m.setEnum(pageEnum);
    addPageToView(currentPage);
    PropManager.getInstance().addPropEditor(currentPage.getModel());
    TreeView.getInstance().addPage(currentPageKey);
  }
  
  /**
   * Creates the page.
   * this function is called directly by toolbox when page button is pressed
   * It builds an AddPageCommand for undo and redo.
   */
  public void createPage() {
    AddPageCommand c = new AddPageCommand(this);
    PagePane p = new PagePane();
    PageModel m = (PageModel) p.getModel();
    String pageKey = EnumFactory.getInstance().createKey(EnumFactory.PAGE);
    m.setKey(pageKey);
    m.setEnum(EnumFactory.getInstance().createEnum(EnumFactory.PAGE));
    c.add(p);
    execute(c);
  }
  
  /**
   * Creates the page that being imported from an existing .ino or .c file
   * this function is called directly by the importer
   */
  public PagePane createPage(String strEnum) {
    PagePane p = new PagePane();
    PageModel m = (PageModel) p.getModel();
    String pageKey = EnumFactory.getInstance().createKey(EnumFactory.PAGE);
    m.setKey(pageKey);
    m.setEnum(strEnum);
    addPage(p);
    return p;
  }
  
  /**
   * Creates the page that being imported from an existing .ino or .c file
   * this function is called directly by the importer
   */
  public PagePane getFirstPage(String strEnum) {
    PagePane p = pages.get(0);
    PageModel m = (PageModel) p.getModel();
    m.setEnum(strEnum);
    return p;
  }
  
  /**
   * Adds the page.
   *
   * @param page
   *          the page
   */
  // this function is called by AddPageCommand
  public void addPage(PagePane page) {
    currentPage = page;
    currentPageKey = page.getKey();
    addPageToView(currentPage);
    PropManager.getInstance().addPropEditor(page.getModel());
    TreeView.getInstance().addPage(currentPageKey);
  }
  
  // this function is called when user presses delete button
  /**
   * Removes the component.
   */
  // It determines if we are to delete a widget or a page
  private void removeComponent() {
    List<Widget> list= currentPage.getSelectedList();
    if (list.size() < 1) { // could be a widget or page selected in TreeView
      // ask tree view if anyone selected?
      String selected = TreeView.getInstance().getSelectedWidget();
      if (selected != null && !selected.isEmpty()) {
        if (selected.startsWith("Page")) {
          PagePane p = findPage(selected);
          if (p != null) {
            removePage(p);
            return;
          }
        } else {  // widget it is...
          list = new ArrayList<Widget>();
          Widget w = currentPage.findWidget(selected);
          list.add(w);
          delWidget(list);
          return;
        }
      }

    } else {  // widget removal
        delWidget(list);
        return;
    }
    JOptionPane.showMessageDialog(topFrame, 
        "You must first select an object for deletion!", 
        "Warning",
        JOptionPane.WARNING_MESSAGE);
  }
  
  // this function is called when user selects deletion of a page
  /**
   * Removes the page.
   *
   * @param page
   *          the page
   */
  // It builds an DelPageCommand for undo and redo.
  private void removePage(PagePane page) {
    String msg = null;
/*
    if (page.getKey().equals("Page$1")) {
      // error can't remove first page
      JOptionPane.showMessageDialog(topFrame, 
          "Sorry, You can't remove Page$1", "Error",
          JOptionPane.ERROR_MESSAGE);
      return;
    } 
*/
    if (page.getWidgetCount() > 0) {
      msg = String.format("Sorry, you must delete all of %s widgets first.", page.getKey());
      // error can't remove first page
      JOptionPane.showMessageDialog(topFrame, 
          msg, "Error",
          JOptionPane.ERROR_MESSAGE);
        return;
    } else {
      msg = String.format("Are you sure you want to delete %s?", page.getKey());
      if (JOptionPane.showConfirmDialog(topFrame, 
          msg, 
          "Really Delete?", 
          JOptionPane.YES_NO_OPTION,
          JOptionPane.QUESTION_MESSAGE) == JOptionPane.NO_OPTION) {
        return;
      }
    }
    DelPageCommand c = new DelPageCommand(this);
    c.delete(page);
    execute(c);
  }

  /**
   * Del page.
   *
   * @param page
   *          the page
   */
  // function is called from DelPageCommand
  public void delPage(PagePane page) {
    PagePane p = null;
    litr = pages.listIterator();
    while(litr.hasNext()){
      p = litr.next();
      if (p.getKey().equals(page.getKey())) {
        layout.removeLayoutComponent(page);
        TreeView.getInstance().delPage(page.getKey());
        if (pages.size() > 0)
          changePage(pages.get(0).getKey());
        litr.remove();
        break;
      }
    }
  }
  
  /**
   * Restore page.
   *
   * @param pageKey
   *          the page key
   * @param pageEnum
   *          the page enum
   * @return the <code>page pane</code> object
   */
  private PagePane restorePage(String pageKey, String pageEnum){
    PagePane page = new PagePane();
    PageModel m = (PageModel) page.getModel();
    m.setKey(pageKey);
    m.setEnum(pageEnum);
    addPageToView(page);
    PropManager.getInstance().addPropEditor(m);
    return page;
  }
  
  /**
   * Adds the widget.
   *
   * @param w
   *          the w
   */
  public void addWidget(Widget w) {
    AddWidgetCommand c = new AddWidgetCommand(currentPage);
    c.add(w);
    currentPage.execute(c);
  }
  
  /**
   * Del widget.
   *
   * @param list
   *          the list
   */
  private void delWidget(List<Widget> list) {
    String keys = "";
    boolean first = true;
    for (Widget w : list) {
      if (!first) keys = keys+",";
      keys = keys + w.getKey();
      first = false;
    }
    keys = keys + " from " + currentPage.getKey();
     String msg = String.format("You want to delete %s?", keys);
    if (JOptionPane.showConfirmDialog(topFrame, 
        msg, "Really Delete?", 
        JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE) == JOptionPane.NO_OPTION){
          return;
    }
    DelWidgetCommand c = new DelWidgetCommand(currentPage);
    if (c.del()) {
      currentPage.execute(c);
      topFrame.repaint();
    } else {
      selectionWarning();
    }
  }
  
  /**
   * New project.
   */
  private void newProject() {
    projectFolder = null;
    projectFile = null;
    String frameTitle = Builder.PROGRAM_TITLE + Builder.NEW_PROJECT;
    topFrame.setTitle(frameTitle);
    closeProject();
    PropManager.getInstance().openProject();
    createFirstPage();
    this.setVisible(true);
  }

  /**
   * Close project.
   */
  private void closeProject() {
    this.setVisible(false);
    for (PagePane page : pages) {
      layout.removeLayoutComponent(page);
    }
    pages.clear();
    currentPageKey = null;
    EnumFactory.getInstance().clearCounts();
    TreeView.getInstance().closeProject();
    PropManager.getInstance().closeProject();
    History.getInstance().clearHistory();
  }

  /**
   * Save project.
   *
   * @param out
   *          the out
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void saveProject(ObjectOutputStream out) throws IOException {
    // output current version so we can make changes on future updates
    out.writeObject(Builder.VERSION_NO);
    out.writeObject(((GeneralModel) generalEditor.getModel()).getTarget());
    out.writeObject(pages.get(0).getKey());  // always point to first page.
    out.writeInt(pages.size());
    String pageKey = null;
    String pageEnum = null;
//    System.out.println("pages: " + pages.size());
    for (PagePane p : pages) {
      pageKey = (String)p.getKey();
      pageEnum = (String)p.getEnum();
//      System.out.println("save page: " + pageKey);
      out.writeObject(pageKey);
//    System.out.println("save page: " + pageEnum);
      out.writeObject(pageEnum);
      // now backup our model data to a base64 string
      out.writeObject(p.backup());
    }
    String tree_backup = TreeView.getInstance().backup();
    out.writeObject(tree_backup);
    String enum_backup = EnumFactory.getInstance().backup();
    out.writeObject(enum_backup);
    out.writeLong(0);  // extra value to avoid java.io.EOFException
    out.flush();
    out.close();
  }

  /**
   * Open project.
   *
   * @param in
   *          the in
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   */
  private void openProject(ObjectInputStream in) throws IOException {
    closeProject();
    PropManager pm = PropManager.getInstance();
    pm.openProject();
    String pageKey = null;
    String pageEnum = null;
    PagePane p = null;
    try {
      // Read in version number
      String strVersion = (String)in.readObject();
      if (strVersion.equals("1.01")) {
        // read in target platform
        String target = (String)in.readObject();
        ((GeneralModel) generalEditor.getModel()).setTarget(target);
      } else {
        if (!strVersion.equals("1.00")) {
          JOptionPane.showMessageDialog(null, "Corrupted Project", "Failure", JOptionPane.ERROR_MESSAGE);
          return;
        }
      }
      currentPageKey = (String)in.readObject();
      int cnt = in.readInt();
//    System.out.println("pages: " + cnt);
      for (int i=0; i<cnt; i++) {
        pageKey = (String)in.readObject();
        pageEnum = (String)in.readObject();
//      System.out.println("restore page: " + pageEnum);
        p = restorePage(pageKey, pageEnum);
        p.restore((String)in.readObject());
      }
      String tree_backup = (String)in.readObject();
      TreeView.getInstance().restore(tree_backup);
      String enum_backup = (String)in.readObject();
      EnumFactory.getInstance().restore(enum_backup);
      MsgEvent ev = new MsgEvent();
      ev.message ="";
      ev.parent = "";
      ev.code = MsgEvent.OBJECT_UNSELECT_PAGEPANE;
      MsgBoard.getInstance().publish(ev);
      ev.code = MsgEvent.OBJECT_UNSELECT_TREEVIEW;
      MsgBoard.getInstance().publish(ev);
    } catch (ClassNotFoundException e) {
      e.printStackTrace();
    }
    in.close();
    changePage(pages.get(0).getKey());
    this.setVisible(true);
  }

  /**
   * Group buttons.
   */
  public void groupButtons() {
    GroupCommand c = new GroupCommand(currentPage);
    if (c.group()) {
      execute(c);
    } else {
      JOptionPane.showMessageDialog(topFrame, 
          "You must select multiple RadioButtons to group them.",
          "Warning",
          JOptionPane.WARNING_MESSAGE);
    }
  }
  
  /**
   * Align top.
   */
  public void alignTop() {
    AlignTopCommand c = new AlignTopCommand(currentPage);
    if (c.align()) {
      execute(c);
    }
  }
  
  /**
   * Align bottom.
   */
  public void alignBottom() {
    AlignBottomCommand c = new AlignBottomCommand(currentPage);
    if (c.align()) {
      execute(c);
    }
  }
  
  /**
   * Align center.
   */
  public void alignCenter() {
    AlignCenterCommand c = new AlignCenterCommand(currentPage);
    if (c.align()) {
      execute(c);
    }
  }
  
  /**
   * Align width.
   */
  public void alignWidth() {
    AlignWidthCommand c = new AlignWidthCommand(currentPage);
    if (c.align()) {
      execute(c);
    } else {
      selectionWarning();
    }
  }
  
  /**
   * Align height.
   */
  public void alignHeight() {
    AlignHeightCommand c = new AlignHeightCommand(currentPage);
    if (c.align()) {
      execute(c);
    } else {
      selectionWarning();
    }
  }
  
  /**
   * Align left.
   */
  public void alignLeft() {
    AlignLeftCommand c = new AlignLeftCommand(currentPage);
    if (c.align()) {
      execute(c);
    } 
  }
  
  /**
   * Align right.
   */
  public void alignRight() {
    AlignRightCommand c = new AlignRightCommand(currentPage);
    if (c.align()) {
      execute(c);
    }
  }
  
  /**
   * Align V spacing.
   */
  public void alignVSpacing() {
    AlignVSpacingCommand c = new AlignVSpacingCommand(currentPage);
    if (c.align()) {
      execute(c);
    } else {
      selectionWarning();
    }
  }
  
  /**
   * Align H spacing.
   */
  public void alignHSpacing() {
    AlignHSpacingCommand c = new AlignHSpacingCommand(currentPage);
    if (c.align()) {
      execute(c);
    } else {
      selectionWarning();
    }
  }
  
  /**
   * Selection warning.
   */
  public void selectionWarning() {
    JOptionPane.showMessageDialog(topFrame, 
        "This command requires multiple widgets be selected.",
        "Warning",
        JOptionPane.WARNING_MESSAGE);
  }
  
  /**
   * Initializes the user prefs.
   *
   * @return the <code>list</code> object
   */
  public List<ModelEditor> initUserPrefs() {
    generalEditor = GeneralEditor.getInstance();
    gridEditor = GridEditor.getInstance();
    BoxEditor boxEditor = BoxEditor.getInstance();
    TextEditor textEditor = TextEditor.getInstance();
    TxtButtonEditor txtbuttonEditor = TxtButtonEditor.getInstance();
    CheckBoxEditor checkboxEditor = CheckBoxEditor.getInstance();
    RadioButtonEditor radiobuttonEditor = RadioButtonEditor.getInstance();
    List<ModelEditor> prefEditors = new ArrayList<ModelEditor>();
    prefEditors.add(generalEditor);
    prefEditors.add(gridEditor);
    prefEditors.add(boxEditor);
    prefEditors.add(textEditor);
    prefEditors.add(txtbuttonEditor);
    prefEditors.add(checkboxEditor);
    prefEditors.add(radiobuttonEditor);
/*  
 *  Java 9 and up needs addPreferenceChangeListener() instead of addObserver()
 *  and even then only for generalEditor
 */
//    Preferences.userRoot().node(GeneralEditor.MY_NODE).addPreferenceChangeListener(this);
    generalEditor.addObserver(this);
    gridEditor.addObserver(this);
    boxEditor.addObserver(this);
    textEditor.addObserver(this);
    txtbuttonEditor.addObserver(this);
    checkboxEditor.addObserver(this);
    radiobuttonEditor.addObserver(this);
    strTheme = generalEditor.getThemeClassName();
    targetDPI = generalEditor.getDPI();
    displayWidth = generalEditor.getWidth();
    displayHeight = generalEditor.getHeight();
   return prefEditors;
  }
  
  /**
   * getPreferredSize
   *
   * @see javax.swing.JComponent#getPreferredSize()
   */
  @Override
  public Dimension getPreferredSize() {
    return new Dimension(1200,1200);
  }


  /**
   * updateEvent
   *
   * @see builder.events.iSubscriber#updateEvent(builder.events.MsgEvent)
   */
  @Override
  public void updateEvent(MsgEvent e) {
//    System.out.println("Controller: " + e.toString());
    if (e.code == MsgEvent.OBJECT_SELECTED_PAGEPANE ||
        e.code == MsgEvent.OBJECT_SELECTED_TREEVIEW) {
      changeView(e);
    } else if (e.code == MsgEvent.DELETE_KEY) {
      removeComponent();
    } else if (e.code == MsgEvent.WIDGET_CHANGE_ZORDER) {
      changeZOrder(e);
    }
  }

  /**
   * Checks, whether the child directory is a subdirectory of the base 
   * directory.
   *
   * @param base the base directory.
   * @param child the suspected child directory.
   * @return true, if the child is a subdirectory of the base directory.
   * @throws IOException if an IOError occured during the test.
   */
  public boolean isSubDirectory(File base, File child) {

    boolean res = false;
    try {
      base = base.getCanonicalFile();
      if (child != null) {
        if (child.isDirectory()) {
          child = child.getCanonicalFile();
          File parentFile = child;
          while (!res && parentFile != null) {
              if (base.equals(parentFile)) {
                  res = true;
              }
              parentFile = parentFile.getParentFile();
          }
        }
      }
    } catch (IOException e) {
       e.printStackTrace();
    }
    return res;
  }

  /**
   * Create folder dialog.
   * 
   * @return the <code>file</code> object
   */
  public File createFolderDialog() {
    File currentDirectory;
    String folderPath = ((GeneralModel) generalEditor.getModel()).getProjectDir();
    // absolute path or relative?
    Path path = Paths.get(folderPath);
    if (path.isAbsolute()) {
      currentDirectory = new File(folderPath);
    } else {
      String workingDir = CommonUtil.getInstance().getWorkingDir();
      folderPath = workingDir + folderPath;
      currentDirectory = new File(folderPath);
    }
    File directorylock = new File(folderPath);
    JFileChooser chooser = new JFileChooser(folderPath);
    // lock the user into only using the project directory or its sub-directories
    chooser.setFileView(new FileView() {
        @Override
        public Boolean isTraversable(File f) {
             return isSubDirectory(directorylock, f);
        }
    });
    chooser.addChoosableFileFilter(new FileFilter() {
      public String getDescription() {
        String descr = new String("GUIslice Builder Project Folder");
        return descr;
      }
      public boolean accept(File f) {
          return f.isDirectory();
      }
    });
    chooser.setDialogTitle("Choose your Project Folder");
    chooser.setApproveButtonText("Select Folder");
    chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
    chooser.setAcceptAllFileFilterUsed(false);
    chooser.setAcceptAllFileFilterUsed(false);
    // Open Dialog  
    if (chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) { 
      currentDirectory = chooser.getSelectedFile();
    } else {
      return null;
    }
    String strProjectFolder = currentDirectory.getName();
    if (strProjectFolder.equals("New Folder")) {
      JOptionPane.showMessageDialog(null, "You can't use the name 'New Folder' for projects", 
          "Error", JOptionPane.ERROR_MESSAGE);
      currentDirectory.delete();
      return null;
    } 
    File project = new File(new String(currentDirectory.toString()
        + System.getProperty("file.separator")
        + strProjectFolder + ".prj"));
    projectFolder = currentDirectory;
    return project;
  }

  /**
   * Show file dialog.
   * 
   * @param title
   *          the title
   * @param fileExtension
   *          the file extension
   * @param suggestedFile
   *          the suggested file
   * @param bAcceptAll
   *          the boolean for accept all
   * @param btnText
   *          the text to show for the accept button
   * @return the <code>file</code> object
   */
  public File showFileDialog(String title, String[] fileExtension, String suggestedFile, 
      boolean bAcceptAll, String btnText) {
    File file = null;
    JFileChooser fileChooser = new JFileChooser();
    if (suggestedFile != null)
      fileChooser.setSelectedFile(new File(suggestedFile));
    fileChooser.setDialogTitle(title);
    if (!bAcceptAll)
      fileChooser.setAcceptAllFileFilterUsed(false);
    fileChooser.addChoosableFileFilter(new FileFilter() {
      public String getDescription() {
        String descr = new String("GUIslice Builder file (");
        for (int i=0; i<fileExtension.length; i++) {
          if (i > 0) 
            descr = descr + ", ";
          descr = descr + "*" + fileExtension[i];
        }
        descr = descr  + ")"; 
        return descr;
      }
      public boolean accept(File f) {
        if (f.isDirectory()) {
          return true;
        } else {
          String name = f.getName().toLowerCase();
          for (int i=0; i<fileExtension.length; i++) 
            if (name.endsWith(fileExtension[i]))
              return true;
          return false;
        }
      }
    });
    File currentDirectory;
    String projectDir = ((GeneralModel) generalEditor.getModel()).getProjectDir();
    // absolute path or relative?
    Path path = Paths.get(projectDir);
    if (path.isAbsolute()) {
      currentDirectory = new File(projectDir);
    } else {
      String workingDir = CommonUtil.getInstance().getWorkingDir();
      currentDirectory = new File(workingDir + projectDir);
    }
    fileChooser.setCurrentDirectory(currentDirectory);
    int option = fileChooser.showDialog(new JFrame(), btnText);
    if (option == JFileChooser.APPROVE_OPTION) {
      file = fileChooser.getSelectedFile();
      projectFolder = fileChooser.getCurrentDirectory();
    }
    return file;
  }

  /**
  * actionPerformed
  *
  * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
  */
  @Override
  public void actionPerformed(ActionEvent e) {
    String command = ((AbstractButton)e.getSource()).getActionCommand();
    ObjectInputStream in = null;
    ObjectOutputStream out = null;
    String title = null;
    String frameTitle = null;
    String message = null;
    int answer = 0;
    // System.out.println("command: " + command);
    switch(command) {
    case "about":
    JOptionPane.showMessageDialog(null, "GUIsliceBuider ver 0.11.0-rc4", "About", JOptionPane.INFORMATION_MESSAGE);
//      JOptionPane.showMessageDialog(null, "GUIsliceBuider ver SNAPSHOT", "About", JOptionPane.INFORMATION_MESSAGE);
      break;
    
    case "aligntop":
      alignTop();
      break;
      
    case "alignbottom":
      alignBottom();
      break;
      
    case "aligncenter":
      alignCenter();
      break;
      
    case "alignleft":
      alignLeft();
      break;
      
    case "alignright":
      alignRight();
      break;
      
    case "alignhspacing":
      alignHSpacing();
      break;
      
    case "alignvspacing":
      alignVSpacing();
      break;
      
    case "alignwidth":
      alignWidth();
      break;
      
    case "alignheight":
      alignHeight();
      break;
      
    case "grid":
      ((GridModel) gridEditor.getModel()).toggleGrid();
      refreshView();
      break;
      
    case "group":
      groupButtons();
      break;
      
    case "new":
      newProject();
      break;
      
    case "import":
      String [] fileExt = new String[2];
      fileExt[0] = ".ino";
      fileExt[1] = ".c";
      File file = showFileDialog("Import Project", fileExt, null, false, "Import");
      if (file == null) break;
      newProject();
      Importer.getInstance().doImport(file);
      break;
      
    case "open":
      String [] fileExtPrj = new String[1];
      fileExtPrj[0] = ".prj";
      projectFile = showFileDialog("Open Project", fileExtPrj, null, false, "Open");
      if (projectFile == null) break;
      frameTitle = Builder.PROGRAM_TITLE + " - " + projectFile.getName();
      topFrame.setTitle(frameTitle);
      try {
        in = new ObjectInputStream(new FileInputStream(projectFile));
        openProject(in);
      } catch (IOException e3) {
        e3.printStackTrace();
      }
      break;
      
    case "save":
      if (projectFolder == null) {
        projectFile = createFolderDialog();
        if (projectFile == null) { 
          JOptionPane.showMessageDialog(null, "Project Save Failed", "Error", JOptionPane.ERROR_MESSAGE);
          return;
        }
        frameTitle = Builder.PROGRAM_TITLE + " - " + projectFile.getName();
        topFrame.setTitle(frameTitle);
      }
      try {
        CommonUtil.getInstance().backupFile(projectFile);
        out = new ObjectOutputStream(new FileOutputStream(projectFile));
        saveProject(out);
        JOptionPane.showMessageDialog(null, "Project Saved into " + projectFile.getName(), null, JOptionPane.INFORMATION_MESSAGE);
      } catch (IOException e2) {
        JOptionPane.showMessageDialog(null, "Project Save Failed", "Error", JOptionPane.ERROR_MESSAGE);
        e2.printStackTrace();
      }
      break;
      
    case "saveas":
      File saveFolder = projectFolder;
      File saveFile = projectFile;
      projectFile = createFolderDialog();
      if (projectFile != null) {
        frameTitle = Builder.PROGRAM_TITLE + " - " + projectFile.getName();
        topFrame.setTitle(frameTitle);
        try {
          out = new ObjectOutputStream(new FileOutputStream(projectFile));
          saveProject(out);
          JOptionPane.showMessageDialog(null, "Project Saved into " + projectFile.getName(), null, JOptionPane.INFORMATION_MESSAGE);
        } catch (IOException e2) {
          JOptionPane.showMessageDialog(null, "Project SaveAs Failed", "Error", JOptionPane.ERROR_MESSAGE);
          e2.printStackTrace();
        }
      } else {
        projectFolder = saveFolder;
        projectFile = saveFile;
        JOptionPane.showMessageDialog(null, "Project SaveAs Failed", "Error", JOptionPane.ERROR_MESSAGE);
      }
      break;
      
    case "code":
      String skeleton=null;
      CodeGenerator cg = CodeGenerator.getInstance();
      if (projectFile != null) {
        skeleton = cg.generateCode(projectFolder.toString(), projectFile.getName(), pages);
        if (skeleton != null)
          JOptionPane.showMessageDialog(null, "Code Generated into " + skeleton, null, JOptionPane.INFORMATION_MESSAGE);
        else 
          JOptionPane.showMessageDialog(null, "Code Generation Failed " + skeleton, "Error", JOptionPane.ERROR_MESSAGE);
      } else {
          JOptionPane.showMessageDialog(topFrame, "Sorry, You must Save Project once before asking for code generation",
              "Error", JOptionPane.ERROR_MESSAGE);
      }
      break;
    
    case "close":
      if (History.getInstance().size() > 0) {
        title = "Confirm Dialog";
        message = "Would you like to save project before closing?";
        answer = JOptionPane.showConfirmDialog(null,message,title, JOptionPane.YES_NO_OPTION); 
        if(answer == JOptionPane.YES_OPTION)
        {
          if (projectFolder == null) {
            projectFile = createFolderDialog();
            if (projectFolder == null) {
              JOptionPane.showMessageDialog(null, "Project Save Failed", "Error", JOptionPane.ERROR_MESSAGE);
              return;
            }
          } 
          try {
            CommonUtil.getInstance().backupFile(projectFile);
            out = new ObjectOutputStream(new FileOutputStream(projectFile));
            saveProject(out);
            JOptionPane.showMessageDialog(null, "Project Saved into " + projectFile.getName(), null, JOptionPane.INFORMATION_MESSAGE);
            newProject();
          } catch (IOException e1) {
            JOptionPane.showMessageDialog(null, "Project Save Failed", "Error", JOptionPane.ERROR_MESSAGE);
            return;
          }
        }
      }
      newProject();
      break;
      
    case "exit":
      title = "Confirm Dialog";
      message = "Would you like to save project before exit?";
      answer = JOptionPane.showConfirmDialog(null,message,title, JOptionPane.YES_NO_OPTION); 
      if(answer == JOptionPane.YES_OPTION)
      {
        if (projectFolder == null) {
          projectFile = createFolderDialog();
          if (projectFolder == null) {
              JOptionPane.showMessageDialog(null, "Project Save Failed", "Error", JOptionPane.ERROR_MESSAGE);
              return;
          } 
        }
        try {
          CommonUtil.getInstance().backupFile(projectFile);
          out = new ObjectOutputStream(new FileOutputStream(projectFile));
          saveProject(out);
        } catch (IOException e1) {
          e1.printStackTrace();
        }
      }
      topFrame.dispose();
      System.exit(0);
      break;
      
    case "delete":
      removeComponent();
      break;
      
    case "zoomin":
      currentPage.zoomIn();
      break;
      
    case "zoomout":
      currentPage.zoomOut();
      break;
      
    case "undo":
      History.getInstance().undo();
      break;
      
    case "redo":
      History.getInstance().redo();
      break;
      
    case "Options":
      userPreferences.showDialog();
      break;

      /*
      case "help":
        JOptionPane.showMessageDialog(null, "Run in Circles, Scream & Shout!", "Help", JOptionPane.INFORMATION_MESSAGE);
        break;
*/
      
    default:
      throw new IllegalArgumentException("Invalid menu item: " + command);
    }

  }

  /**
  * Synchronize the GUI with the user preference for Look and Feel, Size of screen etc...
  * @param o the class object that changed value
  * @param arg the argument passed by the observable object, if any. (usally null)
  */
  @Override public void update(Observable o, Object arg) {

    if (o == generalEditor) {
      if (!generalEditor.getThemeClassName().equals(strTheme)) {
        strTheme = generalEditor.getThemeClassName();
        try { // change look and feel
          // NOTE: on mac os you can't get here
          UIManager.setLookAndFeel(generalEditor.getThemeClassName());
          // update components in this application
          SwingUtilities.updateComponentTreeUI(topFrame);
        } catch (Exception exception) {
          exception.printStackTrace();
        }
      }
      int dpi = generalEditor.getDPI();
      if (dpi != targetDPI) {
        targetDPI = dpi;
        FontFactory.getInstance().reloadFonts();
        for (PagePane p : pages) {
          for (Widget w : p.getWidgets()) {
            if (w.getType().equals(EnumFactory.TEXT)) {
              ((TextModel)w.getModel()).calcSizes(true);
            }
          }
        }
      }
      if (generalEditor.getWidth() != displayWidth || 
          generalEditor.getHeight() != displayHeight) {
        displayWidth = generalEditor.getWidth();
        displayHeight = generalEditor.getHeight();
//        getPanel().setPreferredSize(new Dimension(displayWidth, displayHeight));
        int width = Math.max(displayWidth + 600, 700);
        int height = Math.max(displayHeight + 40, 700);
        Dimension d = new Dimension(width, height);
        topFrame.setPreferredSize(d);
        topFrame.setMaximumSize(d);
        topFrame.setMinimumSize(d);
        Builder.CANVAS_WIDTH = GeneralEditor.getInstance().getWidth()+205;
        Builder.CANVAS_HEIGHT = GeneralEditor.getInstance().getHeight()+235;
        Dimension canvasSz = new Dimension(Builder.CANVAS_WIDTH, Builder.CANVAS_HEIGHT);
        CommonUtil.getInstance().setWinOffsets(canvasSz,
            GeneralEditor.getInstance().getWidth(),
            GeneralEditor.getInstance().getHeight());

        topFrame.revalidate();
        MsgEvent ev = new MsgEvent();
        ev.message ="";
        ev.parent = "";
        ev.code = MsgEvent.CANVAS_MODEL_CHANGE;
        MsgBoard.getInstance().publish(ev);
        topFrame.repaint();
      }
      refreshView();  // refresh view no matter what changed.
    }
  }
/* replace update() with this routine for Java 9 and above
  public void preferenceChange(PreferenceChangeEvent evt) {
    int width, height;
    String key = evt.getKey();
    String val = evt.getNewValue();

    if (key.equals("Theme")) {
      if (!generalEditor.getThemeClassName().equals(strTheme)) {
        strTheme = generalEditor.getThemeClassName();
        try { // change look and feel
          // NOTE: on mac os you can't get here
          UIManager.setLookAndFeel(generalEditor.getThemeClassName());
          // update components in this application
          SwingUtilities.updateComponentTreeUI(topFrame);
        } catch (Exception exception) {
          exception.printStackTrace();
        }
      }
    }
    if (key.equals("TFT Screen DPI")) {
      int dpi = Integer.parseInt(val);
      if (dpi != targetDPI) {
        targetDPI = dpi;
        FontFactory.getInstance().reloadFonts();
      }
      for (PagePane p : pages) {
        for (Widget w : p.getWidgets()) {
          if (w.getType().equals(EnumFactory.TEXT)) {
            ((TextModel)w.getModel()).calcSizes(true);
          }
        }
      }
      refreshView();
    }
    if (key.equals("TFT Screen Width") ||
        key.equals("TFT Screen Height") ) {
        displayWidth = generalEditor.getWidth();
        displayHeight = generalEditor.getHeight();
        getPanel().setPreferredSize(new Dimension(displayWidth, displayHeight));
        width = Math.max(displayWidth + 600, 700);
        height = Math.max(displayHeight + 40, 700);
        Dimension d = new Dimension(width, height);
        topFrame.setPreferredSize(d);
        topFrame.setMaximumSize(d);
        topFrame.setMinimumSize(d);
        topFrame.revalidate();
        CommonUtil.getInstance().setWinOffsets(getPanel().getSize(), displayWidth, displayHeight);
        topFrame.repaint();
        refreshView();
    }
  }
*/
  /**
   * Execute.
   *
   * @param c
   *          the c
   */
  public void execute(Command c) {
    History.getInstance().push(c);
    c.execute();
  }

  /**
   * Backup.
   *
   * @return the <code>string</code> object
   */
  public String backup() {
    try {
//      System.out.println("controller backup*****");
      ByteArrayOutputStream baos = new ByteArrayOutputStream();
      ObjectOutputStream out = new ObjectOutputStream(baos);
      out.writeObject(currentPageKey);
      out.close();
      return Base64.getEncoder().encodeToString(baos.toByteArray());
    } catch (IOException e) {
      System.out.print("IOException occurred." + e.toString());
      e.printStackTrace();
      return "";
    }
  }

  /**
   * Restore.
   *
   * @param state
   *          the state
   */
  public void restore(String state) {
    try {
//      System.out.println("controller restore*****");
      byte[] data = Base64.getDecoder().decode(state);
      ObjectInputStream in = new ObjectInputStream(new ByteArrayInputStream(data));
      currentPageKey = (String)in.readObject();
      in.close();
      changePage(currentPageKey);
    } catch (ClassNotFoundException e) {
      System.out.print("ClassNotFoundException occurred.");
      e.printStackTrace();
    } catch (IOException e) {
      System.out.print("IOException occurred." + e.toString());
      e.printStackTrace();
    }
  }
  
}
