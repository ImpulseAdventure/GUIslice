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

import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.geom.AffineTransform;
import java.awt.geom.NoninvertibleTransformException;
import java.awt.geom.Point2D;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Base64;
import java.util.Iterator;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.JPanel;

import builder.Builder;
import builder.commands.Command;
import builder.commands.DragWidgetCommand;
import builder.commands.History;
import builder.common.CommonUtil;
import builder.common.EnumFactory;
import builder.controller.PropManager;
import builder.events.DeleteKeyListener;
import builder.events.MsgBoard;
import builder.events.MsgEvent;
import builder.events.iSubscriber;
import builder.models.GeneralModel;
import builder.models.GridModel;
import builder.models.PageModel;
import builder.models.WidgetModel;
import builder.prefs.GeneralEditor;
import builder.prefs.GridEditor;
import builder.widgets.Widget;
import builder.widgets.WidgetFactory;

// TODO: Auto-generated Javadoc
/**
 * The Class PagePane provides the view of selected widgets for one page.
 * 
 * @author Paul Conti
 * 
 */
public class PagePane extends JPanel implements iSubscriber {

  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The widgets. */
  private List<Widget> widgets = new ArrayList<Widget>();
  
  /** The u. */
  private CommonUtil u;
  
  /** The mouse pt. */
  private Point mousePt;
  
  /** The mouse rect. */
  private Rectangle mouseRect = new Rectangle();
  
  /** The selecting using a rubber band. */
  private boolean bMultiSelectionBox = false;
  
  /** The dragging indicator. */
  private boolean bDragging = false;

  /** The general model. */
  private GeneralModel generalModel = null;
  
  /** The grid model. */
  private GridModel gridModel;
  
  /** The model. */
  private PageModel model=null;
  
  /** The instance. */
  private PagePane instance = null;
  
  /** The ToolBar. */
  private ToolBar toolBar = null;
  
  /** The drag command. */
  public  DragWidgetCommand dragCommand = null;

  /** the string Key value for this page. */ 
  private String strKey;
  
  /** the string Enum value for this page. */
  private String strEnum;

  /** the number of selected widgets. */
  private int selectedCnt = 0;
  
  /**
   * the number of selected widgets that are either radio buttons or checkboxes.
   */
  private int selectedGroupCnt = 0;
  
  /** The zoom factor. */
  private double zoomFactor = 1;
  
  /** The prev zoom factor. */
  private double prevZoomFactor = 1;
  
  /** boolean that indicates we need to do zoom calculation. */
  private boolean bZoom;
  
  /** The zoom x offset. */
  private double xOffset = 0;
  
  /** The zoom y offset. */
  private double yOffset = 0;
  
  /** The zoom AffineTransform at. */
  private AffineTransform at;
  
  /** The inverse AffineTransform at. */
  private AffineTransform inv_at;
  
  /** The canvas width */
  int canvasWidth;
  
  /** The canvas height */
  int canvasHeight;
  
  /**
   * Instantiates a new page pane.
   */
  public PagePane() {
    instance = this;
    u = CommonUtil.getInstance();
    toolBar = ToolBar.getInstance();
    MsgBoard.getInstance().subscribe(this);
    generalModel = (GeneralModel) GeneralEditor.getInstance().getModel();
    gridModel = (GridModel) GridEditor.getInstance().getModel();
    model = new PageModel();
    mousePt = new Point(generalModel.getWidth() / 2, generalModel.getHeight() / 2);
    canvasWidth = Builder.CANVAS_WIDTH;
    canvasHeight = Builder.CANVAS_HEIGHT;
    this.addMouseListener(new MouseHandler());
    this.addMouseMotionListener(new MouseMotionHandler());
    this.addKeyListener(new DeleteKeyListener());
    this.setLocation(0, 0);
    this.setOpaque(true);
    this.setFocusable( true ); 
    this.setBorder(BorderFactory.createLineBorder(Color.black));
    this.setVisible(true);
    bZoom = true;
  }

  /**
   * Instantiates a new page pane for JUnit tests.
   *
   * @param strKey
   *          the str key
   * @param strEnum
   *          the str enum
   */
  public PagePane(String strKey, String strEnum) {
    this.strKey = strKey;
    this.strEnum = strEnum;
  }
  
  /**
   * paintComponent.
   *
   * @param g
   *          the g
   * @see javax.swing.JComponent#paintComponent(java.awt.Graphics)
   */
  @Override
  protected void paintComponent(Graphics g) {
    // This is a little more complex then you might think.
    // I want the TFT screen centered inside our panel
    // Our widgets think their window starts at 0,0
    // which isn't the case since I'm centering the
    // drawing inside the panel.
    // I therefore calculate offsets from our panel using
    super.paintComponent(g);
    Graphics2D g2d = (Graphics2D) g.create();
    if (bZoom) {
      at = new AffineTransform();
      Dimension d = new Dimension(canvasWidth, canvasHeight);
      double xRel = ((double)d.getWidth() - (double)gridModel.getWidth()) / 2.0;
      double yRel = ((double)d.getHeight() - (double)gridModel.getHeight()) / 2.0;;

      double zoomDiv = zoomFactor / prevZoomFactor;

      xOffset = (zoomDiv) * (xOffset) + (1 - zoomDiv) * xRel;
      
      yOffset = (zoomDiv) * (yOffset) + (1 - zoomDiv) * yRel;
      at.translate(xOffset, yOffset);
      at.scale(zoomFactor, zoomFactor);
      try {
        inv_at = at.createInverse();
      } catch (NoninvertibleTransformException e) {
        // TODO Auto-generated catch block
        e.printStackTrace();
      }
      prevZoomFactor = zoomFactor;
      bZoom = false;
    }
    g2d.transform(at);
    // Now set to overwrite
    g2d.setComposite(AlphaComposite.Src);
    int width = generalModel.getWidth();
    int height = generalModel.getHeight();
    g2d.setColor(Color.BLACK);
    g2d.drawRect(u.toWinX(0)-1, u.toWinY(0)-1, width+2, height+2);
    g2d.setColor(generalModel.getFillColor());
    g2d.fillRect(u.toWinX(0),  u.toWinY(0), width, height);
    if (gridModel.getGrid()) {
      drawCoordinates(g2d, width, height);
    }
    for (Widget w : widgets) {
      w.draw(g2d);
    }
    if (bMultiSelectionBox) {
      // draw our selection rubber band 
      g2d.setColor(Color.RED);
      g2d.setStroke(Widget.dashed);
      g2d.drawRect(mouseRect.x, mouseRect.y,
          mouseRect.width, mouseRect.height);
    }
    // gets rid of the copy
    g2d.dispose();
  };

  /**
   * Zoom in.
   */
  public void zoomIn() {
    zoomFactor *= 1.1;
    bZoom = true;
    ToolBar.getInstance().btn_zoom_out.setEnabled(true);
    canvasWidth = (int)(Builder.CANVAS_WIDTH * zoomFactor);
    canvasHeight = (int)(Builder.CANVAS_HEIGHT * zoomFactor);
    System.out.println("in: " + zoomFactor + " zw: " + canvasWidth + " zh: " + canvasHeight);
    Dimension canvasSz = new Dimension((int)canvasWidth, (int)canvasHeight);
    CommonUtil.getInstance().setWinOffsets(canvasSz,
        GeneralEditor.getInstance().getWidth(),
        GeneralEditor.getInstance().getHeight());
    repaint();
  }
  
  /**
   * Zoom out.
   */
  public void zoomOut() {
    zoomFactor /= 1.1;
    if (zoomFactor < 1.1)
      ToolBar.getInstance().btn_zoom_out.setEnabled(false);
    bZoom = true;
    canvasWidth = (int)(Builder.CANVAS_WIDTH * zoomFactor);
    canvasHeight = (int)(Builder.CANVAS_HEIGHT * zoomFactor);
    System.out.println("out: " + zoomFactor + " zw: " + canvasWidth + " zh: " + canvasHeight);
    Dimension canvasSz = new Dimension((int)canvasWidth, (int)canvasHeight);
    CommonUtil.getInstance().setWinOffsets(canvasSz,
        GeneralEditor.getInstance().getWidth(),
        GeneralEditor.getInstance().getHeight());
    repaint();
  }

  /**
   * Draw grid coordinates.
   *
   * @param g2d
   *          the graphics object
   * @param w
   *          the width of simulated TFT screen
   * @param h
   *          the height of simulated TFT screen
   */
  private void drawCoordinates(Graphics2D g2d, int w, int h) {
    int x, y, dx, dy, dw, dh;
    int minorW = gridModel.getGridMinorWidth();
    int minorH = gridModel.getGridMinorHeight();
    int majorW = gridModel.getGridMajorWidth();
    int majorH = gridModel.getGridMajorHeight();
    Color minorCol = gridModel.getGridMinorColor();
    Color majorCol = gridModel.getGridMajorColor();
    // draw X axis
    dy = u.toWinY(0);
    dh = u.toWinY(h);
    for (x=0; x<w; x+=minorW) {
      if (x%majorW == 0) {
        g2d.setColor(majorCol);
      } else {
        g2d.setColor(minorCol);
      }
      dx = u.toWinX(x);
      g2d.drawLine(dx, dy, dx, dh);
      
    }
    // draw Y axis  
    dx = u.toWinX(0);
    dw = u.toWinX(w);
    for (y=0; y<h; y+=minorH) {
      if (y%majorH==0) {
        g2d.setColor(majorCol);
      } else {
        g2d.setColor(minorCol);
      }
      dy = u.toWinY(y);
      g2d.drawLine(dx, dy, dw, dy);
    }
  }
  
  /**
   * Gets the page model.
   *
   * @return the <code>PageModel</code> object
   */
  public WidgetModel getModel() {
    return model;
  }

  /**
   * Gets the key for this page.
   *
   * @return the key
   */
  public String getKey() {
    if (model == null)
      return strKey;
    return model.getKey();
  }
  
  /**
   * Gets this page's GUIslice enum.
   *
   * @return the enum
   */
  public String getEnum() {
    if (model == null)
      return strEnum;
    return model.getEnum();
  }
  
  /**
   * Gets the count of widgets on this page.
   *
   * @return the widget count
   */
  public int getWidgetCount() {
    return widgets.size();  
  }
  
  /**
   * Find the named widget in list.
   *
   * @param widgetKey
   *          the widget key
   * @return the <code>widget</code> object
   */
  public Widget findWidget(String widgetKey) {
    for (Widget w : widgets) {
      if (w.getModel().getKey().equals(widgetKey)) {
        return w;
      }
    }
    return null;
  }

  /**
   * selectWidget is a central point for selecting any widget it allows the
   * keeping of accurate selection counts for enabling or disabling ToolBar icons.
   *
   * @param w
   *          the w
   */
  public void selectWidget(Widget w) {
    if (w.isSelected()) return;
    w.select();
    doSelectedCount(w);
    toolBar.setEditButtons(selectedCnt, selectedGroupCnt);
  }
  
  /**
   * unSelectWidget is a central point for selecting any widget it allows the
   * keeping of accurate selection counts for enabling or disabling ToolBar icons.
   * Also, for keeping views in sync.
   * @param w
   *          the w
   */
  public void unSelectWidget(Widget w) {
    if (!w.isSelected()) return;
    w.unSelect();
    doSelectedCount(w);
    toolBar.setEditButtons(selectedCnt, selectedGroupCnt);
  }
  
  /**
   * Count all the selected widgets.
   *
   * @param w the widget
   */
  public void doSelectedCount(Widget w) {
    if (w.isSelected()) {
      if (w.getType().equals(EnumFactory.RADIOBUTTON) ||
          w.getType().equals(EnumFactory.CHECKBOX)) {
            selectedGroupCnt++;
      }
      selectedCnt++;
    } else {
      if (w.getType().equals(EnumFactory.RADIOBUTTON) ||
          w.getType().equals(EnumFactory.CHECKBOX)) {
            if (selectedGroupCnt > 0) selectedGroupCnt--;
      }
      if (selectedCnt > 0) selectedCnt--;
    }
    if (selectedCnt == 0) {
      MsgEvent ev = new MsgEvent();
      ev.message ="";
      ev.parent = getKey();
      ev.code = MsgEvent.OBJECT_UNSELECT_PAGEPANE;
      MsgBoard.getInstance().publish(ev);
    }
  }

  /**
   * Select no widgets.
   */
  public void selectNone() {
    if (selectedCnt == 0) return;
    for (Widget w : widgets) {
      w.unSelect();
    }
    selectedCnt=0;
    selectedGroupCnt=0;
  }

  /**
   * Select the named widget in list.
   *
   * @param widgetKey
   *          the widget key
   * @return the <code>widget</code> object
   */
  public Widget selectName(String widgetKey) {
    for (Widget w : widgets) {
      if (w.getModel().getKey().equals(widgetKey)) {
        selectWidget(w);
      }
    }
    return null;
  }

  /**
   * Adds the widget to this page and to the tree view of widgets.
   *
   * @param w
   *          the widget
   */
  public void addWidget(Widget w) {
    widgets.add(w);
    selectNone();
    selectWidget(w);
    PropManager.getInstance().addPropEditor(w.getModel());
    TreeView.getInstance().addWidget(getKey(), w.getKey());
    repaint();
  }

  /**
   * Delete widget.
   *
   * @param w
   *          the widget
   */
  public void delWidget(Widget w) {
    Iterator<?> itr = widgets.iterator();
    while (itr.hasNext())
    {
        Widget x = (Widget)itr.next();
        if (x.getEnum().equals(w.getEnum())) {
            itr.remove();
            break;
        }
    }
    TreeView.getInstance().delWidget(getKey(), w.getKey());
    repaint();
    MsgEvent ev = new MsgEvent();
    ev.message =  w.getKey();
    ev.parent = getKey();
    ev.code = MsgEvent.WIDGET_DELETE;
    MsgBoard.getInstance().publish(ev);
  }

  /**
   * Gets the full list of widgets.
   *
   * @return the widgets
   */
  public List<Widget> getWidgets() {
    return widgets;
  }
  
  /**
   * Collect all the selected widgets into a list.
   *
   * @return the selected list
   */
  public List<Widget> getSelectedList() {
    List<Widget> selected = new ArrayList<Widget>();
    for (Widget w : widgets) {
      if (w.isSelected()) {
        selected.add(w);
      }
    }
    return selected;
  }

  /**
   * Select each widget in rectangle.
   *
   * @param r
   *          the rectangle
   */
  public void selectRect(Rectangle r) {
    Widget first = null;
    Rectangle s = new Rectangle(u.fromWinX(r.x), 
        u.fromWinY(r.y), r.width, r.height);
    for (Widget w : widgets) {
      if (s.contains(w.getLocation())) {
        if (first == null) first = w;
        selectWidget(w);
      } else {
        unSelectWidget(w);
      }
    }
    if (first != null) { // send off a new selected message
      MsgEvent ev = new MsgEvent();
      ev.message = first.getKey();
      ev.parent = getKey();
      ev.code = MsgEvent.OBJECT_SELECTED_PAGEPANE;
      MsgBoard.getInstance().publish(ev);
    }
  }

  /**
   * Find a single node that is inside point p. We only want the highest Z-Order
   * widget, otherwise we will drag around a panel instead of the button on top
   * that the user really wanted to move.
   *
   * @param p
   *          the <code>Point</code> object
   * @return the matching <code>Widget</code> object or null.
   */
  public Widget findOne(Point p) {
    // we need to deal with scaled points because of zoom feature
    Point2D.Double scaledPos = new Point2D.Double();
    scaledPos.x = (double)p.x;
    scaledPos.y = (double)p.y;
    if (zoomFactor > 1) // transforms are only needed in zoom mode
      inv_at.transform(scaledPos, scaledPos);
    Point2D.Double mappedPos = u.fromWinPoint(scaledPos);
//    System.out.println("findOne: Z=" + zoomFactor  + " p=[" + p.x + "," + p.y + "] " 
//        + " s=[" + scaledPos.getX() + "," + scaledPos.getY() + "]"
//        + " m=[" + mappedPos.getX() + "," + mappedPos.getY() + "]");
    Widget ret = null;
    for (Widget w : widgets) {
      if (w.contains(mappedPos)) {
//        System.out.println("found: " + w.getKey() + " p= " + p + w.getBounded());
        ret = w;
      }
    }
    return ret;
  }
  
  /**
   * Change Z order.
   *
   * @param widgetKey
   *          the widget key
   * @param fromIdx
   *          the from idx
   * @param toIdx
   *          the to idx
   */
  public void changeZOrder(String widgetKey, int fromIdx, int toIdx) {
  Widget w = widgets.get(fromIdx);
  widgets.remove(fromIdx);
  widgets.add(toIdx, w);
  MsgEvent ev = new MsgEvent();
  ev.message = w.getModel().getKey();
  ev.parent = getKey();
  ev.code = MsgEvent.OBJECT_SELECTED_PAGEPANE;
  MsgBoard.getInstance().publish(ev);
  repaint();
}

  /**
   * Refresh view.
   */
  public void refreshView() {
    ToolBar.getInstance().setEditButtons(selectedCnt, selectedGroupCnt); // needed on page changes
    repaint();
  }
  
  /**
   * The Class MouseHandler.
   */
  private class MouseHandler extends MouseAdapter {

    /**
     * mouseClicked.
     *
     * @param e
     *          the e
     * @see java.awt.event.MouseAdapter#mouseClicked(java.awt.event.MouseEvent)
     */
    @Override
    public void mouseClicked(MouseEvent e) {
      bMultiSelectionBox = false;
      mousePt = e.getPoint();
      Widget w = findOne(mousePt);
      int button = e.getButton();
      if (button != MouseEvent.BUTTON1) return;
      if (e.isShiftDown() && e.isControlDown()) return;
      if (e.isControlDown()) {
        // Single Left-click + Ctrl means to toggle select under cursor
        if (w != null) {
          if (w.isSelected()) {
            unSelectWidget(w);
            /* after a multi-selection and a toggle off
             * we might be left with treeview and propview
             * showing a widget thats no longer selected.
             * So just pick a new one if that's the case.  
             */
            List<Widget> list = getSelectedList();
            if (list.size() > 0) {
              Widget selected = list.get(0);
              MsgEvent ev = new MsgEvent();
              ev.message = selected.getModel().getKey();
              ev.parent = getKey();
              ev.code = MsgEvent.OBJECT_SELECTED_PAGEPANE;
              MsgBoard.getInstance().publish(ev);
            }
          } else {
            selectWidget(w);
            MsgEvent ev = new MsgEvent();
            ev.message = w.getModel().getKey();
            ev.parent = getKey();
            ev.code = MsgEvent.OBJECT_SELECTED_PAGEPANE;
            MsgBoard.getInstance().publish(ev);
          }
          e.getComponent().repaint();
        }
      } else if (e.isShiftDown()) {
        // Single Left-click + Shift means to select under cursor
        if (w != null) {
          selectWidget(w);
          MsgEvent ev = new MsgEvent();
          ev.message = w.getModel().getKey();
          ev.parent = getKey();
          ev.code = MsgEvent.OBJECT_SELECTED_PAGEPANE;
          MsgBoard.getInstance().publish(ev);
          e.getComponent().repaint();
        } 
      } else { 
        // Single Left-click - deselect all widgets then select widget under cursor
        selectNone();
        if (w == null) {
          bMultiSelectionBox = true;
          toolBar.setEditButtons(selectedCnt, selectedGroupCnt);
          MsgEvent ev = new MsgEvent();
          ev.message ="";
          ev.parent = getKey();
          ev.code = MsgEvent.OBJECT_UNSELECT_PAGEPANE;
          MsgBoard.getInstance().publish(ev);
        } else {
          selectWidget(w);
          MsgEvent ev = new MsgEvent();
          ev.message = w.getModel().getKey();
          ev.parent = getKey();
          ev.code = MsgEvent.OBJECT_SELECTED_PAGEPANE;
          MsgBoard.getInstance().publish(ev);
          e.getComponent().repaint();
        }
      }
    }  // end mouseClicked

    /**
     * mouseReleased.
     *
     * @param e
     *          the e
     * @see java.awt.event.MouseAdapter#mouseReleased(java.awt.event.MouseEvent)
     */
    @Override
    public void mouseReleased(MouseEvent e) {
      mouseRect.setBounds(0, 0, 0, 0);
      if (dragCommand != null) {
        dragCommand.stop();
        execute(dragCommand);
        dragCommand = null;
      }
      bMultiSelectionBox = false;
      bDragging = false;
      e.getComponent().repaint();
    }  // end mouseReleased

    /**
     * mousePressed.
     *
     * @param e
     *          the e
     * @see java.awt.event.MouseAdapter#mousePressed(java.awt.event.MouseEvent)
     */
    @Override
    public void mousePressed(MouseEvent e) {
      mousePt = e.getPoint();
      bDragging = false;
      Widget w = findOne(mousePt);
      if (w != null) {
        if (w.isSelected()) bDragging = true;
      } 
    } // end mousePressed
  } // end MouseHandler

  /**
   * The Class MouseMotionHandler.
   */
  private class MouseMotionHandler extends MouseMotionAdapter {
     
     /**
      * mouseDragged.
      *
      * @param e
      *          the e
      * @see java.awt.event.MouseMotionAdapter#mouseDragged(java.awt.event.MouseEvent)
      */
     @Override
    public void mouseDragged(MouseEvent e) {
      if (bMultiSelectionBox) {
        // Here I'm working out the size and position of my rubber band
        mouseRect.setBounds(
            Math.min(mousePt.x, e.getX()),
            Math.min(mousePt.y, e.getY()),
            Math.abs(mousePt.x - e.getX()),
            Math.abs(mousePt.y - e.getY()));
        // Now select any widgets that fit inside our rubber band
        selectRect(mouseRect);
     } else if (bDragging ){
        if (dragCommand == null) {
          dragCommand = new DragWidgetCommand(instance);
          dragCommand.start();
        }
        // No need to adjust our points using u.fromWinPoint() 
        // because here we are calculating offsets not absolute points.
        Point dragPt= new Point(e.getX()-mousePt.x, e.getY()-mousePt.y);
        dragCommand.move(dragPt);
        mousePt = e.getPoint();
      }
      e.getComponent().repaint();
    } // end mouseDragged
  }  // end MouseMotionHandler
  
  /**
   * getPreferredSize.
   *
   * @return the preferred size
   * @see javax.swing.JComponent#getPreferredSize()
   */
  @Override
  public Dimension getPreferredSize() {
//    return new Dimension(gridModel.getWidth(), gridModel.getHeight());
    return new Dimension(1200, 1200);
  }

  /**
   * Execute.
   *
   * @param c
   *          the <code>Command</code> object
   * @see builder.commands.Command#execute()
   */
  public void execute(Command c) {
    History.getInstance().push(c);
    c.execute();
  }

  /**
   * updateEvent provides the implementation of Observer Pattern. It monitors
   * selection of widgets in the tree view, modification of widgets by commands,
   * and size changes to the simulated TFT screen.
   *
   * @param e
   *          the e
   * @see builder.events.iSubscriber#updateEvent(builder.events.MsgEvent)
   */
  @Override
  public void updateEvent(MsgEvent e) {
//    System.out.println("PagePane: " + e.toString());
    if (e.code == MsgEvent.WIDGET_REPAINT) {
      Widget w = findWidget(e.message);
      if (w != null) {
        repaint();
      }
    } else if (e.code == MsgEvent.OBJECT_SELECTED_TREEVIEW && 
               e.parent.equals(getKey())) {
      selectNone();
      Widget w = findWidget(e.message);
      if (w != null) {
        selectWidget(w);
        repaint();
      }
    } else if (e.code == MsgEvent.OBJECT_UNSELECT_TREEVIEW) {
/*
    // Can't call selectNone() or we will go into a loop.
      for (Widget w : widgets) {
        unSelectWidget(w);
      }
*/
      selectNone();
      repaint();
    } else if (e.code == MsgEvent.CANVAS_MODEL_CHANGE) {
      canvasWidth = (int)(Builder.CANVAS_WIDTH * zoomFactor);
      canvasHeight = (int)(Builder.CANVAS_HEIGHT * zoomFactor);
      CommonUtil.getInstance().setWinOffsets(new Dimension(canvasWidth, canvasHeight), 
          generalModel.getWidth(),
          generalModel.getHeight());
      repaint();
    }
  }

  /**
   * Backup all widgets into a serialized string object.
   *
   * @return the <code>String</code> object
   */
  public String backup() {
    try {
      ByteArrayOutputStream baos = new ByteArrayOutputStream();
      ObjectOutputStream out = new ObjectOutputStream(baos);
      out.writeInt(widgets.size());
//      System.out.println("widgets: " + widgets.size());
      for (Widget w : widgets) {
        w.writeObject(out);
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
   * Restore all widgets from a serialized string object.
   *
   * @param state
   *          the state backup string
   */
  public void restore(String state) {
    try {
      byte[] data = Base64.getDecoder().decode(state);
      ObjectInputStream in = new ObjectInputStream(new ByteArrayInputStream(data));
      widgets = new ArrayList<Widget>();
      int cnt = in.readInt();  // size of array list
//      System.out.println("widgets: " + cnt);
      Widget w = null;
      String widgetType = null;
      for (int i=0; i<cnt; i++) {
        widgetType = (String)in.readObject();
        w = WidgetFactory.getInstance().createWidget(widgetType,0,0);
        w.readObject(in, widgetType);
        widgets.add(w);
        PropManager.getInstance().addPropEditor(w.getModel());
//        TreeView.getInstance().addWidget(getKey(), w.getKey());
      }
      in.close();
      selectedCnt = 0;
      selectedGroupCnt = 0;
      repaint();
    } catch (ClassNotFoundException e) {
      System.out.println("ClassNotFoundException occurred.");
      e.printStackTrace();
    } catch (IOException e) {
      System.out.println("IOException occurred." + e.toString());
      System.out.flush();
      e.printStackTrace();
    }
  }

  /**
   * Restore for test.
   *
   * @param state
   *          the state
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   * @throws ClassNotFoundException
   *           the class not found exception
   */
  public void restoreForTest(String state) throws IOException, ClassNotFoundException  {
    byte[] data = Base64.getDecoder().decode(state);
    ObjectInputStream in = new ObjectInputStream(new ByteArrayInputStream(data));
    widgets = new ArrayList<Widget>();
    int cnt = in.readInt();  // size of array list
    Widget w = null;
    String widgetType = null;
    for (int i=0; i<cnt; i++) {
      widgetType = (String)in.readObject();
      w = WidgetFactory.getInstance().createWidget(widgetType,0,0);
      w.readObject(in, widgetType);
      widgets.add(w);
    }
    in.close();
  }
}

