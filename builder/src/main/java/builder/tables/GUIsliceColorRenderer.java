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
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.ListCellRenderer;

import builder.common.ColorIcon;
import builder.common.ColorItem;

/**
 * <p>
 * Custom renderer to display a GUIslice Library Color names along 
 * side the actual color icon.
 * </p>
 *
 * @author Paul Conti
 */
public class GUIsliceColorRenderer extends JLabel implements ListCellRenderer<ColorItem> {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;

  /**
   * Instantiates a new GUIslice color renderer.
   */
  public GUIsliceColorRenderer() {
    setOpaque(true); //MUST do this for background to show up.
  }

  /**
   * getListCellRendererComponent
   *
   * @see javax.swing.ListCellRenderer#getListCellRendererComponent(javax.swing.JList, java.lang.Object, int, boolean, boolean)
   */
  @Override
  public Component getListCellRendererComponent(JList<? extends ColorItem> list, ColorItem item, int index,
      boolean isSelected, boolean cellHasFocus) {

    ColorIcon imageIcon = new ColorIcon(item.getColor());

    setIcon(imageIcon);
    setText(item.getDisplayName());

    if (isSelected) {
      setBackground(list.getSelectionBackground());
      setForeground(list.getSelectionForeground());
    } else {
      setBackground(list.getBackground());
      setForeground(list.getForeground());
    }

    return this;
  }
}
