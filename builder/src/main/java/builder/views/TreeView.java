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
package builder.views;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Base64;
import java.util.Enumeration;
import java.util.EventObject;
import java.util.List;

import javax.swing.DropMode;
import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JInternalFrame;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.TransferHandler;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellEditor;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import builder.Builder;
import builder.events.DeleteKeyListener;
import builder.events.MsgBoard;
import builder.events.MsgEvent;
import builder.events.iSubscriber;

/**
 * The Class TreeView provides a view of all widgets on all pages.
 * <p>
 * Users may select a single widget and it will be selected
 * inside the owner page view.  A widget may also be selected
 * then dragged to a new Z position. A selected widget or page
 * may also be deleted from the project.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class TreeView extends JInternalFrame implements iSubscriber {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The tree. */
  private JTree tree;
  
  /** The root. */
  private DefaultMutableTreeNode root;
  
  /** The current page. */
  private DefaultMutableTreeNode currentPage;
  
  /** The tree model. */
  protected DefaultTreeModel treeModel;
  
  /** The image icon. */
  private ImageIcon imageIcon;
  
  /** The parent icon. */
  private ImageIcon parentIcon;
  
  /** The scroll pane. */
  private JScrollPane scrollPane;
  
  /** The lastest backup. */
  private String lastestBackup = null;
  
  /** The b dragging node. */
  private boolean bDraggingNode = false;
  
  /** The instance. */
  private static TreeView instance = null;
  
  /** The currently selected widget */
  private String SelectWidgetEnum = "";
  
  /**
   * Gets the single instance of TreeView.
   *
   * @return single instance of TreeView
   */
  public static synchronized TreeView getInstance()  {
      if (instance == null) {
          instance = new TreeView();
      }
      return instance;
  }  

  /**
   * Instantiates a new tree view.
   */
  public TreeView() {
    initUI();
    MsgBoard.getInstance().subscribe(this);
  }
  
  /**
   * Initializes the UI.
   */
  private void initUI() {
    // create the root node
    root = new DefaultMutableTreeNode("Root");

    treeModel = new DefaultTreeModel(root);

    // create the tree by passing in our tree model
    tree = new JTree(treeModel);

    imageIcon = new ImageIcon(Builder.class.getResource("/resources/icons/widget.png"));
    parentIcon = new ImageIcon(Builder.class.getResource("/resources/icons/brick.png"));
    
    DefaultTreeCellRenderer renderer = new DefaultTreeCellRenderer();
    renderer.setOpenIcon(parentIcon);
    renderer.setClosedIcon(parentIcon);
    renderer.setLeafIcon(imageIcon);
    tree.setCellRenderer(renderer);

    tree.setRootVisible(false);
    tree.setEditable(true);
    tree.setDragEnabled(true);
    tree.setDropMode(DropMode.ON_OR_INSERT);
    MyTreeCellEditor editor = new MyTreeCellEditor(tree,
        (DefaultTreeCellRenderer) tree.getCellRenderer());
    tree.setCellEditor(editor);
    tree.setTransferHandler(new TreeTransferHandler());
    tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
//    tree.setRootVisible(false);
    tree.setFocusable( true ); 
    tree.addMouseListener(new MouseHandler());
    tree.addKeyListener(new DeleteKeyListener());
    tree.getSelectionModel().addTreeSelectionListener(new TreeSelectionListener() {
      @Override
      public void valueChanged(TreeSelectionEvent e) {
        DefaultMutableTreeNode selectedNode = (DefaultMutableTreeNode) tree.getLastSelectedPathComponent();
        if (selectedNode == null) SelectWidgetEnum = "";
        if (selectedNode != null && !bDraggingNode) {
          String widgetEnum = selectedNode.getUserObject().toString();
          // this is necessary to avoid a loop with pagePane.
          if (!SelectWidgetEnum.equals(widgetEnum)) {
            SelectWidgetEnum = widgetEnum;
            TreePath parentPath = e.getPath().getParentPath();
            DefaultMutableTreeNode parentNode = (DefaultMutableTreeNode) parentPath.getLastPathComponent();
            MsgEvent ev = new MsgEvent();
            ev.code = MsgEvent.OBJECT_SELECTED_TREEVIEW;
            ev.message = SelectWidgetEnum;
            ev.parent = ((String) parentNode.getUserObject());
            MsgBoard.getInstance().publish(ev);
          }
        }
      }
    });
    scrollPane = new JScrollPane(tree);
    add(scrollPane);
    this.setTitle("Tree View");        
    this.setPreferredSize(new Dimension(200, 400));
    this.setVisible(true);
  }

  /**
   * The Class MouseHandler.
   */
  private class MouseHandler extends MouseAdapter {

    /**
     * mouseClicked
     *
     * @see java.awt.event.MouseAdapter#mouseClicked(java.awt.event.MouseEvent)
     */
    @Override
    public void mouseClicked(MouseEvent e) {
      int row=tree.getRowForLocation(e.getX(),e.getY());
      if(row==-1) { //When user clicks on the "empty surface"
        tree.clearSelection();
        SelectWidgetEnum = "";
        MsgEvent ev = new MsgEvent();
        ev.message = "";
        ev.parent = currentPage.toString();
        ev.code = MsgEvent.OBJECT_UNSELECT_TREEVIEW;
        MsgBoard.getInstance().publish(ev);
      }
    }
  }
  
  /**
   * Adds the page.
   *
   * @param pageID
   *          the page ID
   */
  public void addPage(String pageID) {
    //create and add the top level page node
    currentPage = addObject(null, pageID);
    TreePath path = new TreePath(currentPage.getPath());
    tree.setSelectionPath(path);
    tree.scrollPathToVisible(path);
  }
  
  /**
   * Del page.
   *
   * @param pageID
   *          the page ID
   */
  public void delPage(String pageID) {
    DefaultMutableTreeNode node = findNode(pageID);
    if (node != null) {
      node.removeAllChildren(); //this removes all nodes
      delObject(root, pageID);
    }
    List<DefaultMutableTreeNode> searchNodes = getSearchNodes((DefaultMutableTreeNode) tree.getModel().getRoot());
    currentPage = searchNodes.get(1);
    TreePath path = new TreePath(currentPage.getPath());
    tree.setSelectionPath(path);
    tree.scrollPathToVisible(path);
  }
  
  /**
   * Adds the widget.
   *
   * @param pageID
   *          the page ID
   * @param widget
   *          the widget
   */
  public void addWidget(String pageID, String widget) {
    if (!((String)currentPage.getUserObject()).equals(pageID)) {
      currentPage = findNode(pageID);
    }
    //create and add the child node to the page node
    DefaultMutableTreeNode newNode = addObject(currentPage, widget);
    TreePath path = new TreePath(newNode.getPath());
    tree.setSelectionPath(path);
    tree.scrollPathToVisible(path);
  }

  /**
   * Del widget.
   *
   * @param pageID
   *          the page ID
   * @param widget
   *          the widget
   */
  public void delWidget(String pageID, String widget) {
    if (!((String)currentPage.getUserObject()).equals(pageID)) {
      currentPage = findNode(pageID);
    }
    //remove the child node from the page node
    delObject(currentPage, widget);
  }
  
  /**
   * getSelectedWidget() gets the currently selected widget
   * @return current widget's Enum or null;
   */
  public String getSelectedWidget() {
    return SelectWidgetEnum;
  }
  
  /**
   * Close project.
   */
  public void closeProject() {
    root.removeAllChildren(); //this removes all nodes
    treeModel.reload(); //this notifies the listeners and changes the GUI
  }
  
  /**
   * Gets the saved backup.
   *
   * @return the saved backup
   */
  public String getSavedBackup() {
    return lastestBackup;
  }
  
  /**
   * Adds the object.
   *
   * @param parent
   *          the parent
   * @param child
   *          the child
   * @return the <code>default mutable tree node</code> object
   */
  public DefaultMutableTreeNode addObject(DefaultMutableTreeNode parent, String child) {
    DefaultMutableTreeNode childNode = new DefaultMutableTreeNode(child);

    if (parent == null) {
      parent = root;
    }

    // It is key to invoke this on the TreeModel, and NOT DefaultMutableTreeNode
    treeModel.insertNodeInto(childNode, parent, parent.getChildCount());

    // Make sure the user can see the lovely new node.
    tree.scrollPathToVisible(new TreePath(childNode.getPath()));
    return childNode;
  }

  /**
   * Del object.
   *
   * @param parent
   *          the parent
   * @param child
   *          the child
   */
  public void delObject(DefaultMutableTreeNode parent, String child) {

    if (parent == null) {
      parent = root;
    }
    DefaultMutableTreeNode childNode = findNode(child);
    // It is key to invoke this on the TreeModel, and NOT DefaultMutableTreeNode
    treeModel.removeNodeFromParent(childNode);
  }

  /**
   * Find node.
   *
   * @param searchString
   *          the search string
   * @return the <code>default mutable tree node</code> object
   */
  public DefaultMutableTreeNode findNode(String searchString) {

    List<DefaultMutableTreeNode> searchNodes = getSearchNodes((DefaultMutableTreeNode) tree.getModel().getRoot());
    DefaultMutableTreeNode currentNode = (DefaultMutableTreeNode) tree.getLastSelectedPathComponent();

    DefaultMutableTreeNode foundNode = null;
    int bookmark = -1;

    if (currentNode != null) {
      if (currentNode.toString().equals(searchString))
        return currentNode;
      for (int index = 0; index < searchNodes.size(); index++) {
        if (searchNodes.get(index) == currentNode) {
          bookmark = index;
          break;
        }
      }
    }

    for (int index = bookmark + 1; index < searchNodes.size(); index++) {
      if (searchNodes.get(index).toString().toLowerCase().contains(searchString.toLowerCase())) {
        foundNode = searchNodes.get(index);
        break;
      }
    }

    if (foundNode == null) {
      for (int index = 0; index <= bookmark; index++) {
        if (searchNodes.get(index).toString().toLowerCase().contains(searchString.toLowerCase())) {
          foundNode = searchNodes.get(index);
          break;
        }
      }
    }
    return foundNode;
  }

  /**
   * Gets the search nodes.
   *
   * @param root
   *          the root
   * @return the search nodes
   */
  public List<DefaultMutableTreeNode> getSearchNodes(DefaultMutableTreeNode root) {
    List<DefaultMutableTreeNode> searchNodes = new ArrayList<DefaultMutableTreeNode>();

    Enumeration<?> e = root.preorderEnumeration();
    while (e.hasMoreElements()) {
      searchNodes.add((DefaultMutableTreeNode) e.nextElement());
    }
    return searchNodes;
  }

  /**
   * Gets the selected index.
   *
   * @param parent
   *          the parent
   * @param s
   *          the s
   * @return the selected index
   */
  public int getSelectedIndex(DefaultMutableTreeNode parent, String s) {
    int row = -1;
    for (int i=0;i<parent.getChildCount();i++) {
      DefaultMutableTreeNode child = ((DefaultMutableTreeNode)parent.getChildAt(i));
      if(s.equals((String) child.getUserObject())){
          row = i;
          break;
      }
    }
    return row;
    
  }

  /**
   * The Class MyTreeCellEditor will prevent anyone from editing a widget node.
   * Since we only show keys; A user changing a key value would crash the system.
   */
  private static class MyTreeCellEditor extends DefaultTreeCellEditor {

    /**
     * Instantiates a new my tree cell editor.
     *
     * @param tree
     *          the tree
     * @param renderer
     *          the renderer
     */
    public MyTreeCellEditor(JTree tree, DefaultTreeCellRenderer renderer) {
        super(tree, renderer);
    }

    /**
     * getTreeCellEditorComponent
     *
     * @see javax.swing.tree.DefaultTreeCellEditor#getTreeCellEditorComponent(javax.swing.JTree, java.lang.Object, boolean, boolean, boolean, int)
     */
    @Override
    public Component getTreeCellEditorComponent(JTree tree, Object value,
            boolean isSelected, boolean expanded, boolean leaf, int row) {
        return super.getTreeCellEditorComponent(tree, value, isSelected, expanded,
                leaf, row);
    }

    /**
     * isCellEditable
     *
     * @see javax.swing.tree.DefaultTreeCellEditor#isCellEditable(java.util.EventObject)
     */
    @Override
    public boolean isCellEditable(EventObject e) {
        return false;
    }
  }
  
  /**
   * The Class TreeTransferHandler allows a widget to be dragged within the tree view.
   * <p>
   * NOTE: Widgets cannot be dragged between pages, only reordered within a page.
   */
  class TreeTransferHandler extends TransferHandler {
    
    /** The Constant serialVersionUID. */
    private static final long serialVersionUID = 1L;
    
    /** The selected node. */
    private DefaultMutableTreeNode selectedNode;
    
    /** The parent node. */
    private DefaultMutableTreeNode parentNode;
    
    /** The from row. */
    private int fromRow;
    
    /**
     * canImport
     * for a string-flavored, drop transfer over a non-null tree path.
     *
     * @see javax.swing.TransferHandler#canImport(javax.swing.TransferHandler.TransferSupport)
     */
    public boolean canImport(TransferHandler.TransferSupport support) {
      if (!support.isDataFlavorSupported(DataFlavor.stringFlavor) || !support.isDrop()) {
        return false;
      }
      // Only support moves within our tree branch
      int action = support.getDropAction();
      if(action != MOVE) return false;
   
      JTree.DropLocation dropLocation = (JTree.DropLocation) support.getDropLocation();
      TreePath parentPath = dropLocation.getPath();
      parentNode = (DefaultMutableTreeNode) parentPath.getLastPathComponent();

      // do not allow drop past end of branch
      int childIndex = dropLocation.getChildIndex();
      int parentIndex = parentNode.getChildCount();
      
      if (childIndex+1 >= parentIndex) return false;

      DefaultMutableTreeNode selectedNode = (DefaultMutableTreeNode) tree.getLastSelectedPathComponent();
      TreePath dropPath = new TreePath(selectedNode.getPath());

      // Do not allow MOVE-action drops if a non-leaf node is selected
      // non-leaf node?
      if(!selectedNode.isLeaf()) return false;
      
      // Do not allow moves between parent nodes
      if (!parentPath.isDescendant(dropPath)) return false;

      // make sure we have a valid drop point
      bDraggingNode = dropLocation.getPath() != null;
      if (bDraggingNode) {
        lastestBackup = backup(); // save for undo command
      }
      return bDraggingNode;
    }

    /**
     * importData
     *
     * @see javax.swing.TransferHandler#importData(javax.swing.TransferHandler.TransferSupport)
     */
    public boolean importData(TransferHandler.TransferSupport support) {
      if (!canImport(support)) {
        return false;
      }
      // dropLocation will give you the location in the tree to add the new node. 
      JTree.DropLocation dropLocation =
      (JTree.DropLocation) support.getDropLocation();
      // grab the path to the dropLocation
      TreePath path = dropLocation.getPath();
      /*
       * we don't pass around the actual objects during Drag N Drop just the 
       * data contained in the object in our case just the string value.
       */
      Transferable transferable = support.getTransferable();

      String transferData;
      try {
        transferData = (String) transferable.getTransferData(
        DataFlavor.stringFlavor);
      } catch (IOException e) {
        return false;
      } catch (UnsupportedFlavorException e) {
        return false;
      }
      
      // create the new node using the transfer data
      DefaultMutableTreeNode newNode = new DefaultMutableTreeNode(transferData);
      // grab the parent node
      parentNode = (DefaultMutableTreeNode) path.getLastPathComponent();
      fromRow = getSelectedIndex(parentNode, (String) newNode.getUserObject()); 
      
      // add the new node to the tree path
      int childIndex = dropLocation.getChildIndex();
      treeModel.insertNodeInto(newNode, parentNode, childIndex);

      // ensure the new path element is visible.
      TreePath newPath = path.pathByAddingChild(newNode);
      tree.makeVisible(newPath);
      tree.scrollRectToVisible(tree.getPathBounds(newPath));

      return true;
    }
    
    /**
     * getSourceActions
     *
     * @see javax.swing.TransferHandler#getSourceActions(javax.swing.JComponent)
     */
    public int getSourceActions(JComponent c) {
      return COPY_OR_MOVE;
    }

    /**
     * createTransferable
     *
     * @see javax.swing.TransferHandler#createTransferable(javax.swing.JComponent)
     */
    public Transferable createTransferable(JComponent c) {
      if (c instanceof JTree) {
        TreePath[] paths = tree.getSelectionPaths();
        if(paths != null) {
          // grab the selected node
          selectedNode = (DefaultMutableTreeNode)paths[0].getLastPathComponent();
          return new StringSelection((String) selectedNode.getUserObject());
        }
      }
      return null;
    }

    /**
     * exportDone
     *
     * @see javax.swing.TransferHandler#exportDone(javax.swing.JComponent, java.awt.datatransfer.Transferable, int)
     */
    public void exportDone(JComponent c, Transferable t, int action) {
      if (action == MOVE) {
        JTree tree = (JTree)c;
        DefaultTreeModel treeModel = (DefaultTreeModel)tree.getModel();
        // Remove node saved in selectedNode in createTransferable.
        treeModel.removeNodeFromParent(selectedNode);
        // determine our destination row where we have been moved.
        String widgetEnum = ((String)selectedNode.getUserObject());
        int toRow = getSelectedIndex(parentNode, widgetEnum); 
        MsgEvent ev = new MsgEvent();
        ev.code = MsgEvent.WIDGET_CHANGE_ZORDER;
        ev.message = widgetEnum;
        ev.parent = ((String) parentNode.getUserObject());
        ev.fromIdx = fromRow;
        ev.toIdx = toRow;
        MsgBoard.getInstance().publish(ev);
      }
      bDraggingNode = false;
    } 
  }

  /**
   * Backup our tree model.
   * <p>
   * Since TreeModel is serializable we could do this in one
   * writeObject() but getting our view to refresh is a nightmare best avoided.
   * Our tree only contains string values so we will also be faster and smaller on
   * backup and restores.
   *
   * @param list
   *          - pairs of parent and child are added to this list as we traverse
   *          the tree.
   * @param node
   *          - parent node to search, must start with root
   */
  private void backupTree(List<Pair> list, DefaultMutableTreeNode node) {
    // We have to walk our tree to not only find parents and children but also
    // to get an accurate count of nodes for restore.
    int childCount = node.getChildCount();

    //    System.out.println("---" + node.toString() + "---");
    if (!((String)node.getUserObject()).equals("Root")) 
      list.add(new Pair(node.toString()));
    for (int i = 0; i < childCount; i++) {

      DefaultMutableTreeNode childNode = (DefaultMutableTreeNode) node.getChildAt(i);
      if (childNode.getChildCount() > 0) {
        backupTree(list, childNode);
      } else {
//        System.out.println(childNode.toString());
        if (((String)node.getUserObject()).equals("Root")) 
          list.add(new Pair(childNode.toString()));
        else
          list.add(new Pair(node.toString(), childNode.toString()));
      }
    }
  }

  /**
   * Backup all widget keys and their parent keys into a serialized string object.
   *
   * @return the <code>String</code> object
   */
  public String backup() {
    try {
//      System.out.println(">>>>tree backup");
      ByteArrayOutputStream baos = new ByteArrayOutputStream();
      ObjectOutputStream out = new ObjectOutputStream(baos);
      List<Pair> list = new ArrayList<Pair>();
      backupTree(list, root);
      // now that we have our backup list we need to write it to our stream
      int nodeCount = list.size();
      out.writeInt(nodeCount);
      for(Pair p : list) {
        out.writeObject(p.getParent());
        out.writeObject(p.getChild());
      }
      out.close();
      return Base64.getEncoder().encodeToString(baos.toByteArray());
    } catch (IOException e) {
      System.out.print("IOException occurred." + e.toString());
      e.printStackTrace();
      return "";
    }
  }

  /**
   * Restore tree.
   *
   * @param in
   *          the in
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   * @throws ClassNotFoundException
   *           the class not found exception
   */
  private void restoreTree(ObjectInputStream in) 
      throws IOException, ClassNotFoundException {

    int nodeCount = in.readInt();
    Pair p = new Pair();
    for (int i=0; i<nodeCount; i++) {
      p.setParent((String)in.readObject());
      p.setChild((String)in.readObject());
      if (p.getChild().equals("*")) 
        addPage(p.getParent());
      else
        addWidget(p.getParent(), p.getChild());
    }
  }

  /**
   * Restore all widgets from a serialized string object.
   *
   * @param state
   *          the state backup string
   */
  public void restore(String state) {
    try {
//      System.out.println("tree restore>>>");
      byte[] data = Base64.getDecoder().decode(state);
      ObjectInputStream in = new ObjectInputStream(new ByteArrayInputStream(data));
      closeProject();
      restoreTree(in);
      in.close();
      repaint();
    } catch (ClassNotFoundException e) {
      System.out.print("ClassNotFoundException occurred.");
      e.printStackTrace();
    } catch (IOException e) {
      System.out.print("IOException occurred." + e.toString());
      e.printStackTrace();
    }
  }

  /**
   * The Class Pair used to store widget parent and child relationship.
   */
  private class Pair {
    
    /** The parent. */
    String parent;
    
    /** The child. */
    String child;

    /**
     * Instantiates a new pair.
     */
    Pair() {
      
    };
    
    /**
     * Instantiates a new pair.
     *
     * @param parent
     *          the parent
     */
    Pair(String parent) {
      this.parent = parent;
      this.child = "*";
    }

    /**
     * Instantiates a new pair.
     *
     * @param parent
     *          the parent
     * @param child
     *          the child
     */
    Pair(String parent, String child) {
      this.parent = parent;
      this.child = child;
    }

    /**
     * Gets the parent.
     *
     * @return the parent
     */
    private String getParent() {
      return parent;
    }
    
    /**
     * Sets the parent.
     *
     * @param parent
     *          the new parent
     */
    private void setParent(String parent) {
      this.parent = parent;
    }

    /**
     * Gets the child.
     *
     * @return the child
     */
    private String getChild() {
      return child;
    }

    /**
     * Sets the child.
     *
     * @param child
     *          the new child
     */
    private void setChild(String child) {
      this.child = child;
    }

    /**
     * toString
     *
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
      if (getChild().equals("*"))
        return String.format("%s",getParent());
      else
        return String.format("%s->%s",getParent(),getChild());
    }
  }

  @Override
  public void updateEvent(MsgEvent e) {
//    System.out.println("TreeView: " + e.toString());
    if (e.code == MsgEvent.OBJECT_SELECTED_PAGEPANE) {
      DefaultMutableTreeNode w = findNode(e.message);
      if (w != null) {
        SelectWidgetEnum = e.message;
        TreePath path = new TreePath(w.getPath());
        tree.setSelectionPath(path);
        tree.scrollPathToVisible(path);
        repaint();
      }
    } else if (e.code == MsgEvent.OBJECT_UNSELECT_PAGEPANE) {
      tree.clearSelection();
      scrollPane.repaint();
    } 
  }
  
}