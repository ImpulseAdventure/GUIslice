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
package builder.tables;

import javax.swing.AbstractCellEditor;
import javax.swing.table.TableCellEditor;

import builder.views.GUIsliceColorChooser;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JTable;

import java.awt.Color;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * <p>
 * ColorEditor treats the color cell label as a button and
 * calls the custom GUIslice color chooser instead of the java swing chooser.
 * </p>
 * 
 * @author Paul Conti
 *
 */
public class ColorEditor extends AbstractCellEditor implements TableCellEditor, ActionListener {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;

  /** The current color. */
  Color currentColor;
  
  /** The button. */
  JButton button;
  
  /** The chooser. */
  GUIsliceColorChooser chooser;
  
  /** The dialog. */
  JDialog dialog;
  
  /** The Constant EDIT. */
  protected static final String EDIT = "edit";

  /**
   * Instantiates a new color editor.
   */
  public ColorEditor() {
    /* Set up the editor (from the table's point of view),
     * which is a button.
     * This button brings up the color chooser dialog,
     * which is the editor from the user's point of view.
     */
    button = new JButton();
    button.setActionCommand(EDIT);
    button.addActionListener(this);
    button.setBorderPainted(false);

    // Set up the dialog that the button brings up.
    chooser = new GUIsliceColorChooser(null);
  }

  /**
   * Handles events from the editor button and from the dialog's OK button.
   *
   * @param e
   *          the e <code>ActionEvent</code> object
   */
  public void actionPerformed(ActionEvent e) {
    if (EDIT.equals(e.getActionCommand())) {
      // The user has clicked the cell, so
      // bring up the dialog.
      button.setBackground(currentColor);
      Color newColor = chooser.showDialog(currentColor);
      currentColor = newColor;

      // Make the renderer reappear.
      fireEditingStopped();
    }
  }

  /**
   * getCellEditorValue
   * Implement the one CellEditor method that AbstractCellEditor doesn't.
   *
   * @see javax.swing.CellEditor#getCellEditorValue()
   */
  public Object getCellEditorValue() {
    return currentColor;
  }

  /**
   * getTableCellEditorComponent
   * Implement the one method defined by TableCellEditor.
   *
   * @see javax.swing.table.TableCellEditor#getTableCellEditorComponent(javax.swing.JTable, java.lang.Object, boolean, int, int)
   */
  public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) {
    currentColor = (Color) value;
    return button;
  }

}
