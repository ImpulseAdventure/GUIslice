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

import java.awt.Component;
import java.awt.event.FocusAdapter;
import java.awt.event.FocusEvent;

import javax.swing.DefaultCellEditor;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;

/**
 * The SelectAllCellEditor.
 * <p>
 * Select all characters editing - when a cell is placed in editing mode 
 * the default JTable behavior is to append characters to the end of the 
 * current value.  
 * </p>
 * <p>  
 * This class instead will cause all characters within the cell to be selected 
 * and will wipe them out when the first character is entered.  
 * Since its implemented as a cell editor you can decide on a column or class
 * basis to enable this behavior.
 * </p>
 * Example calling sequence:
 *     table.setDefaultEditor(Integer.class, new SelectAllCellEditor());
 * 
 * @author Paul Conti
 *
 */
public class SelectAllCellEditor extends DefaultCellEditor {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;

  /**
   * Instantiates a new select all cell editor.
   */
  public SelectAllCellEditor() {
      super(new JTextField());
      final JTextField textField = (JTextField) getComponent();
      textField.addFocusListener(new FocusAdapter() {
          @Override
          public void focusGained(FocusEvent e) {
              SwingUtilities.invokeLater(new Runnable() {
                  @Override
                  public void run() {
                    textField.selectAll();
                  }
              });
          }
      });
  }

  /**
   * getTableCellEditorComponent
   *
   * @see javax.swing.DefaultCellEditor#getTableCellEditorComponent(javax.swing.JTable, java.lang.Object, boolean, int, int)
   */
  @Override
  public Component getTableCellEditorComponent(
          JTable table, Object value, boolean isSelected, int row, int column) {
      final JTextField textField = (JTextField)
              super.getTableCellEditorComponent(table, value, isSelected, row, column);
      textField.selectAll();
      return textField;
  }

}
