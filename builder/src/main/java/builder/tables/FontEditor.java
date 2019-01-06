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

import builder.views.FontChooser;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JTable;

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;


/**
 * <p>
 * The Class FontEditor treats the font cell string as a button and
 * calls the custom GUIslice font chooser.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class FontEditor extends AbstractCellEditor implements TableCellEditor, ActionListener  {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;

  /** The font name. */
  String fontName;

  /** The button. */
  JButton button;
  
  /** The chooser. */
  FontChooser chooser;
  
  /** The dialog. */
  JDialog dialog;
  
  /** The Constant EDIT. */
  protected static final String EDIT = "edit";

  /**
   * Instantiates a new font editor.
   */
  public FontEditor() {
    //This button brings up the font chooser dialog,
    //which is the editor from the user's point of view.
    button = new JButton();
    button.setBackground(Color.white);
    button.setFont(button.getFont().deriveFont(Font.PLAIN));
    button.setActionCommand(EDIT);
    button.addActionListener(this);
//    button.setBorderPainted(false);
    button.setBorder(null);

    //Set up the dialog that the button brings up.
    chooser = new FontChooser(null);
  }

  /**
   * getCellEditorValue
   *
   * @see javax.swing.CellEditor#getCellEditorValue()
   */
  //Implement the one CellEditor method that AbstractCellEditor doesn't.
  public Object getCellEditorValue() {
      return fontName;
  }

  /**
   * getTableCellEditorComponent
   *
   * @see javax.swing.table.TableCellEditor#getTableCellEditorComponent(javax.swing.JTable, java.lang.Object, boolean, int, int)
   */
  //Implement the one method defined by TableCellEditor.
  public Component getTableCellEditorComponent(JTable table,
                                               Object value,
                                               boolean isSelected,
                                               int row,
                                               int column) 
  {
    fontName = ((String)value);
    button.setText(fontName);
    return button;
  }

  /**
   * actionPerformed
   *
   * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
   */
  @Override
  public void actionPerformed(ActionEvent e) {
    String newName = "";
    if (EDIT.equals(e.getActionCommand())) {
      //The user has clicked the cell, so
      //bring up the dialog.
      newName = chooser.showDialog(fontName);
      if (newName != null) {
        fontName = newName;
      }
      //Make the renderer reappear.
      fireEditingStopped();
    }
  }
}
