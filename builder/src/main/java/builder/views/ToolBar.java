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

import java.awt.event.ActionListener;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JToolBar;

import builder.Builder;

/**
 * The Class ToolBar.
 * 
 * @author Paul Conti
 * 
 */
public class ToolBar extends JToolBar {
  
  /** The instance. */
  private static ToolBar instance = null;
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The btn exit. */
  public JButton btn_new, btn_open, btn_close,btn_save,btn_saveas,
    btn_import, btn_code, btn_exit;
  
  /** The undo, redo, and delete buttons. */
  public JButton btn_undo,btn_redo,btn_delete;
  
  /** The alignment buttons. */
  public JButton btn_aligntop,btn_alignbottom, btn_aligncenter,
    btn_alignleft,btn_alignright,btn_alignhspace,
    btn_alignvspace,btn_alignwidth, btn_alignheight, btn_group;
  
  /** The view buttons. */
  public JButton btn_grid,btn_zoom_in,btn_zoom_out;
  
  /**
   * Gets the single instance of ToolBar.
   *
   * @return single instance of ToolBar
   */
  public static synchronized ToolBar getInstance() {
    if (instance == null) {
      instance = new ToolBar();
    }
    return instance;
  }

  /**
   * Instantiates a new tool bar.
   */
  public ToolBar() {
    initFileButtons();  
    initQuitButtons();
    initAlignmentButtons();
    initUndoButtons();
    initViewButtons();
    setEditButtons(0, 0);
  }
  
  /**
   * Initializes the file buttons.
   */
  public void initFileButtons() {
    btn_new = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/new.png")));
    btn_new.setToolTipText("New Project");
    btn_new.setActionCommand("new");
    add(btn_new);

    btn_open = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/open.png")));
    btn_open.setToolTipText("Open Project");
    btn_open.setActionCommand("open");
    add(btn_open);

    btn_close = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/close.png")));
    btn_close.setToolTipText("Close Project");
    btn_close.setActionCommand("close");
    add(btn_close);

    btn_save = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/save.png")));
    btn_save.setToolTipText("Save Project");
    btn_save.setActionCommand("save");
    add(btn_save);

    btn_saveas = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/saveas.png")));
    btn_saveas.setToolTipText("Save As Project");
    btn_saveas.setActionCommand("saveas");
    add(btn_saveas);

    addSeparator();
  }
  
  /**
   * Initializes the undo buttons.
   */
  public void initUndoButtons() {
    btn_undo = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/undo.png")));
    btn_undo.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_undo.png")));
    btn_undo.setToolTipText("Undo Actions");
    btn_undo.setActionCommand("undo");
    add(btn_undo);

    btn_redo = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/redo.png")));
    btn_redo.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_redo.png")));
    btn_redo.setToolTipText("Redo Actions");
    btn_redo.setActionCommand("redo");
    add(btn_redo);
    
    btn_delete = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/delete.png")));
/* The builder's pagepane doesn't know if its selected or not and so we can't
 *  dynamically turn Delete button on/off. Symptoms: Users can't delete empty page
 *  btn_delete.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_delete.png")));
 */
    btn_delete.setToolTipText("Delete Widget");
    btn_delete.setActionCommand("delete");
    add(btn_delete);
    
    addSeparator();  
  }
  
  /**
   * Initializes the quit buttons.
   */
  public void initQuitButtons() {
    btn_import = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/import.png")));
    btn_import.setToolTipText("Import");
    btn_import.setActionCommand("import");
    add(btn_import);

    btn_code = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/export.png")));
    btn_code.setToolTipText("Generate Code");
    btn_code.setActionCommand("code");
    add(btn_code);

    btn_exit = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/logout.png")));
    btn_exit.setToolTipText("Exit Builder");
    btn_exit.setActionCommand("exit");
    add(btn_exit);
    
   addSeparator();  
  }
  
  /**
   * Initializes the alignment buttons.
   */
  public void initAlignmentButtons() {
    btn_aligntop = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/align_top.png")));
    btn_aligntop.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_top.png")));
    btn_aligntop.setToolTipText("Align Widgets to Top Most Widget");
    btn_aligntop.setActionCommand("aligntop");
    add(btn_aligntop);    
    
    btn_alignbottom = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/align_bottom.png")));
    btn_alignbottom.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_bottom.png")));
    btn_alignbottom.setToolTipText("Align Widgets to Bottom Most Widget");
    btn_alignbottom.setActionCommand("alignbottom");
    add(btn_alignbottom);    
    
    btn_aligncenter = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/align_center.png")));
    btn_aligncenter.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_center.png")));
    btn_aligncenter.setToolTipText("Center Widgets Horizontally along X axis");
    btn_aligncenter.setActionCommand("aligncenter");
    add(btn_aligncenter);    
    
    btn_alignleft = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/align_left.png")));
    btn_alignleft.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_left.png")));
    btn_alignleft.setToolTipText("Align Horizontally by Leftmost Widget");
    btn_alignleft.setActionCommand("alignleft");
    add(btn_alignleft);
    
    btn_alignright = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/align_right.png")));
    btn_alignright.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_right.png")));
    btn_alignright.setToolTipText("Align Horizontally by Rightmost Widget");
    btn_alignright.setActionCommand("alignright");
    add( btn_alignright);

    btn_alignhspace = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/align_hspacing.png")));
    btn_alignhspace.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_hspacing.png")));
    btn_alignhspace.setToolTipText("Align Horizontal Spacing");
    btn_alignhspace.setActionCommand("alignhspacing");
    add(btn_alignhspace);
    
    btn_alignvspace = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/align_vspacing.png")));
    btn_alignvspace.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_vspacing.png")));
    btn_alignvspace.setToolTipText("Align Vertical Spacing");
    btn_alignvspace.setActionCommand("alignvspacing");
    add(btn_alignvspace);

    btn_alignwidth = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/align_width.png")));
    btn_alignwidth.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_width.png")));
    btn_alignwidth.setToolTipText("Align Widgets to a Uniform Width");
    btn_alignwidth.setActionCommand("alignwidth");
    add( btn_alignwidth);
    
    btn_alignheight = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/align_height.png")));
    btn_alignheight.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_height.png")));
    btn_alignheight.setToolTipText("Align Widgets to a Uniform Widget's Height");
    btn_alignheight.setActionCommand("alignheight");
    add(btn_alignheight);

    addSeparator();

    btn_group = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/radiogroup.png")));
    btn_group.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_radiogroup.png")));
    btn_group.setToolTipText("Assign GroupID to RadioButtons and/or CheckBoxes");
    btn_group.setActionCommand("group");
    add(btn_group);

    addSeparator();  
  }

  /**
   * Initializes the grid, and zoom buttons.
   */
  public void initViewButtons() {
    btn_grid = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/grid.png")));
    btn_grid.setToolTipText("Toggle Grid ON/OFF");
    btn_grid.setActionCommand("grid");
    btn_zoom_in = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/zoom_in.png")));
    btn_zoom_in.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_zoom_in.png")));
    btn_zoom_in.setToolTipText("Zoom In");
    btn_zoom_in.setActionCommand("zoomin");
    btn_zoom_out = new JButton(new ImageIcon(Builder.class.getResource("/resources/icons/zoom_out.png")));
    btn_zoom_out.setDisabledIcon(new ImageIcon(Builder.class.getResource("/resources/icons/disable_zoom_out.png")));
    btn_zoom_out.setToolTipText("Zoom Out");
    btn_zoom_out.setActionCommand("zoomout");
    btn_zoom_out.setEnabled(false);
    add(btn_grid);
    add(btn_zoom_in);
    add(btn_zoom_out);

    addSeparator();  
  }

  /**
   * Adds the listeners.
   *
   * @param al
   *          the object that implements ActionListener
   */
  public void addListeners(ActionListener al)
  {
   
    btn_new.addActionListener(al);
    btn_open.addActionListener(al);
    btn_save.addActionListener(al);
    btn_close.addActionListener(al);
    btn_saveas.addActionListener(al);

    btn_import.addActionListener(al);
    btn_code.addActionListener(al);
    btn_exit.addActionListener(al);
 
    btn_undo.addActionListener(al);
    btn_redo.addActionListener(al);
    btn_delete.addActionListener(al);

    btn_aligntop.addActionListener(al);
    btn_alignbottom.addActionListener(al);
    btn_aligncenter.addActionListener(al);
    btn_alignleft.addActionListener(al);
    btn_alignright.addActionListener(al);
    btn_alignhspace.addActionListener(al);
    btn_alignvspace.addActionListener(al);
    btn_alignwidth.addActionListener(al);
    btn_alignheight.addActionListener(al);
    btn_group.addActionListener(al);

    btn_grid.addActionListener(al);
    btn_zoom_in.addActionListener(al);
    btn_zoom_out.addActionListener(al);
  }
  
  /**
   * setEditButtons used by PagePane to set/reset TooBar buttons
   * @param selectedCnt is the number of widgets thats currently selected on the page
   * @param selectedGroupCnt is the number of checkboxes selected on the page
   */
  public void setEditButtons(int selectedCnt, int selectedGroupCnt) {
    if (selectedCnt <= 0) {
      disableSingleEdits();
    } else if (selectedCnt == 1) {
      disableMultiEdits();
      enableSingleEdits();
    } else {
      enableMultiEdits();
    }
    if (selectedGroupCnt > 1) {
      btn_group.setEnabled(true);
    } else {
      btn_group.setEnabled(false);
    }
  }
  
  public void enableSingleEdits() {
    btn_aligntop.setEnabled(true);
    btn_alignbottom.setEnabled(true);
    btn_aligncenter.setEnabled(true);
    btn_alignleft.setEnabled(true);
    btn_alignright.setEnabled(true);
  }
  
  public void disableSingleEdits() {
    btn_aligntop.setEnabled(false);
    btn_alignbottom.setEnabled(false);
    btn_aligncenter.setEnabled(false);
    btn_alignleft.setEnabled(false);
    btn_alignright.setEnabled(false);
    btn_alignhspace.setEnabled(false);
    btn_alignvspace.setEnabled(false);
    btn_alignwidth.setEnabled(false);
    btn_alignheight.setEnabled(false);
  }

  public void enableMultiEdits() {
    btn_aligntop.setEnabled(true);
    btn_alignbottom.setEnabled(true);
    btn_aligncenter.setEnabled(false);
    btn_alignleft.setEnabled(true);
    btn_alignright.setEnabled(true);
    btn_alignhspace.setEnabled(true);
    btn_alignvspace.setEnabled(true);
    btn_alignwidth.setEnabled(true);
    btn_alignheight.setEnabled(true);
  }
  
  public void disableMultiEdits() {
    btn_aligncenter.setEnabled(true);
    btn_alignhspace.setEnabled(false);
    btn_alignvspace.setEnabled(false);
    btn_alignwidth.setEnabled(false);
    btn_alignheight.setEnabled(false);
  }
  
  public void enableUndo() {
    btn_undo.setEnabled(true);
  }
  
  public void disableUndo() {
    btn_undo.setEnabled(false);
  }
  
  public void enableRedo() {
    btn_redo.setEnabled(true);
  }
  
  public void disableRedo() {
    btn_redo.setEnabled(false);
  }
  
}
