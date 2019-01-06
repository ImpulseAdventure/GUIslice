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

import java.awt.Color;
import java.awt.Component;

import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;

/**
 * <p>
 * The Class NonEditableCellRenderer will flag non-editable cells in the JTable
 * a different background color to better inform users not to try and edit them.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class NonEditableCellRenderer extends DefaultTableCellRenderer {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;

  /**
   * getTableCellRendererComponent
   *
   * @see javax.swing.table.DefaultTableCellRenderer#getTableCellRendererComponent(javax.swing.JTable, java.lang.Object, boolean, boolean, int, int)
   */
  @Override
  public Component getTableCellRendererComponent(JTable table, 
          Object value, boolean isSelected, boolean hasFocus,
          int row, int col) {
      Component c = super.getTableCellRendererComponent(table, 
          value, isSelected, hasFocus, row, col);
      if (col == 1) {
        if (!table.getModel().isCellEditable(row, col)){
          c.setBackground(new Color(255,228,225));  // misty rose
          // c.setBackground(new Color(232,232,232)); // very light gray
        } else {
          c.setBackground(Color.WHITE);
        }
        c.setForeground(Color.BLACK);
      }
      return c;
  };

}
