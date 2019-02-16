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

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.border.Border;
import javax.swing.table.TableCellRenderer;
import java.awt.Color;
import java.awt.Component;

/**
 * <p>
 * The Class ColorRenderer.
 * Displays Colors in JTable cells as a colored label.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class ColorRenderer extends JLabel 
  implements TableCellRenderer {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;

  /** The unselected border. */
  Border unselectedBorder = null;
  
  /** The selected border. */
  Border selectedBorder = null;
  
  /** The is bordered. */
  boolean isBordered = true;

  /**
   * Instantiates a new color renderer.
   *
   * @param isBordered
   *          the is bordered
   */
  public ColorRenderer(boolean isBordered) {
      this.isBordered = isBordered;
      setOpaque(true); //MUST do this for background to show up.
  }

  /**
   * getTableCellRendererComponent
   *
   * @see javax.swing.table.TableCellRenderer#getTableCellRendererComponent(javax.swing.JTable, java.lang.Object, boolean, boolean, int, int)
   */
  public Component getTableCellRendererComponent(
                          JTable table, Object color,
                          boolean isSelected, boolean hasFocus,
                          int row, int column) {
    Color newColor = (Color)color;
    if (color != null) {
      if (column == 1 && !table.getModel().isCellEditable(row, column)){
        setBackground(new Color(255,228,225));  // misty rose
      } else {
        setBackground(newColor);
      }
      if (isBordered) {
          if (isSelected) {
              if (selectedBorder == null) {
                  selectedBorder = BorderFactory.createMatteBorder(2,5,2,5,
                                            table.getSelectionBackground());
              }
              setBorder(selectedBorder);
          } else {
              if (unselectedBorder == null) {
                  unselectedBorder = BorderFactory.createMatteBorder(2,5,2,5,
                                            table.getBackground());
              }
              setBorder(unselectedBorder);
          }
      }
        
      setToolTipText("RGB value: " + newColor.getRed() + ", "
                                   + newColor.getGreen() + ", "
                                   + newColor.getBlue());
    }
    return this;
  }
}
