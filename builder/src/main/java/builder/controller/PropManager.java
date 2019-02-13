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
import java.util.ArrayList;

import javax.swing.AbstractButton;
import javax.swing.JInternalFrame;
import javax.swing.JPanel;

import builder.events.MsgBoard;
import builder.events.MsgEvent;
import builder.events.iSubscriber;
import builder.models.WidgetModel;
import builder.views.PropEditor;

/**
 * <p>
 * The Class PropManager stores and managers widget's property layout views.
 * It acts as an observer (Observer Pattern) for widget changes and will 
 * cause a widget properties to be displayed.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class PropManager extends JInternalFrame implements ActionListener, iSubscriber {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The editors. */
  ArrayList<PropEditor> editors;
  
  /** The cards. */
  JPanel cards;  // a panel that uses CardLayout
  
  /** The layout. */
  CardLayout layout;
  
  /** The current widget key. */
  String currentWidgetKey;
  
  /** The instance. */
  private static PropManager instance = null;
  
  /**
   * Gets the single instance of PropManager.
   *
   * @return single instance of PropManager
   */
  public static synchronized PropManager getInstance()  {
      if (instance == null) {
          instance = new PropManager();
      }
      return instance;
  }  

  /**
   * Instantiates a new prop manager.
   */
  public PropManager() {
    MsgBoard.getInstance().subscribe(this);
    editors = new ArrayList<PropEditor>();
    layout = new CardLayout();
    cards = new JPanel(layout);
    add(cards);
    setTitle("Properties");        
    setPreferredSize(new Dimension(250, 350));
    setVisible(true);
  }

  /**
   * Gets the parent panel.
   *
   * @return the parent panel
   */
  public JPanel getParentPanel() {
    return cards;
  }
  
  /**
   * Close project.
   */
  public void closeProject() {
    setVisible(false);
    for (PropEditor editor : editors) {
      layout.removeLayoutComponent(editor.getPropPanel());
    }
    editors.clear();
  }

  /**
   * Open project.
   */
  public void openProject() {
    setVisible(true);
  }

  /**
   * Adds the prop editor.
   *
   * @param m
   *          the m
   */
  public void addPropEditor(WidgetModel m)
  {
    PropEditor propEditor = null;
    String widgetKey = m.getKey();
    for (PropEditor editor : editors) {
      if (editor.getKey().equals(widgetKey)) {
        propEditor = editor;
        currentWidgetKey = widgetKey;
        break;
      }
    }
    if (propEditor == null) {
      propEditor = new PropEditor(m);
      propEditor.setKey(widgetKey);
      editors.add(propEditor);
      currentWidgetKey = widgetKey;
      cards.add(propEditor.getPropPanel(), currentWidgetKey);
      layout.last(cards);
    } else {
      layout.show(cards, currentWidgetKey);
    }
  }
  
  /**
   * Show prop editor.
   *
   * @param widgetKey
   *          the widget key
   */
  public void showPropEditor(String widgetKey) {
    layout.show(cards, widgetKey);
    currentWidgetKey = widgetKey;
  }

  /**
   * actionPerformed
   *
   * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
   */
  @Override
  public void actionPerformed(ActionEvent e) {
    String compTitle= ((AbstractButton)e.getSource()).getActionCommand();
    showPropEditor(compTitle);
  }

  /**
   * updateEvent
   *
   * @see builder.events.iSubscriber#updateEvent(builder.events.MsgEvent)
   */
  @Override
  public void updateEvent(MsgEvent e) {
//    System.out.println("PropManager: " + e.toString());
    if (e.code == MsgEvent.OBJECT_SELECTED_PAGEPANE || 
        e.code == MsgEvent.WIDGET_REPAINT           ||
        e.code == MsgEvent.WIDGET_CHANGE_ZORDER     ||
        e.code == MsgEvent.OBJECT_SELECTED_TREEVIEW) {
      showPropEditor(e.message);
    } else if (e.code == MsgEvent.OBJECT_UNSELECT_PAGEPANE) {
      showPropEditor(e.parent);
    } else if (e.code == MsgEvent.WIDGET_DELETE) {
      showPropEditor(e.parent);
    } else if (e.code == MsgEvent.OBJECT_UNSELECT_TREEVIEW) {
      showPropEditor(e.parent);
    }
  }

 /**
  * Backup.
  *
  * @return the <code>card layout</code> object
  */
 public CardLayout backup() {
   return layout;
 }
 
 /**
  * Restore.
  *
  * @param layout
  *          the layout
  */
 public void restore(CardLayout layout) {
   this.layout = layout;
 }
 
}
