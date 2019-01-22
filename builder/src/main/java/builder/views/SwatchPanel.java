/**
 *
 * The MIT License
 *
 * Copyright 2019 Paul Conti
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

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.FocusAdapter;
import java.awt.event.FocusEvent;
import java.awt.event.MouseEvent;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Base64;
import java.util.LinkedList;

import javax.swing.JPanel;
import javax.swing.UIManager;

import builder.prefs.GeneralEditor;

/**
 * The Class SwatchPanel manages recent color selection
 * They are stored inside the users preferences General Model.
 * 
 * @author Paul Conti
 */
public class SwatchPanel extends JPanel {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant CUSTOM_COLORS_FILE. */
  public  static final String CUSTOM_COLORS_FILE = "custom_colors.list";
  
  /** size of color square in our recent colors panel  */
  public static final int NSQUARE = 15;

  /** number of columns in our recent colors panel  */
  public static final int NCOLUMNS = 4;
  
  /** number of rows in our recent colors panel  */
  public static final int NROWS = 4;
  
  /** The general editor. */
  private GeneralEditor generalEditor;
  
  /** lru cache of recent colors */
  LinkedList<Color> lruList = new LinkedList<Color>();
  
  /** The swatch size. */
  protected Dimension swatchSize;
  
  /** The num swatches. */
  protected Dimension numSwatches;
  
  /** The gap. */
  protected Dimension gap;

  /** The selected row. */
  private int selRow;
  
  /** The selected col. */
  private int selCol;
  
  /** The num colors. */
  private int numColors;

  /**
   * Instantiates a new swatch panel.
   */
  public SwatchPanel() {
    initValues();
    initColors();
    setToolTipText(""); // register for events
    setOpaque(true);
    setBackground(Color.white);
    setFocusable(true);
    setInheritsPopupMenu(true);
    
    addFocusListener(new FocusAdapter() {
      public void focusGained(FocusEvent e) {
        repaint();
      }

      public void focusLost(FocusEvent e) {
        repaint();
      }
    });
  }

  /**
   * Gets the selected color.
   *
   * @return the selected color
   */
  public Color getSelectedColor() {
    return getColorForCell(selCol, selRow);
  }

  /**
   * Initializes the values.
   */
  protected void initValues() {
    swatchSize = new Dimension(NSQUARE, NSQUARE);
    numSwatches = new Dimension(NCOLUMNS, NROWS);
    numColors = NCOLUMNS * NROWS;
    gap = new Dimension(1, 1);
    generalEditor = GeneralEditor.getInstance();
  }

  /**
   * paintComponent
   *
   * @see javax.swing.JComponent#paintComponent(java.awt.Graphics)
   */
  public void paintComponent(Graphics g) {
    g.setColor(getBackground());
    g.fillRect(0, 0, getWidth(), getHeight());
    for (int row = 0; row < numSwatches.height; row++) {
      int y = row * (swatchSize.height + gap.height);
      for (int column = 0; column < numSwatches.width; column++) {
        Color c = getColorForCell(column, row);
        g.setColor(c);
        int x;
        if (!this.getComponentOrientation().isLeftToRight()) {
          x = (numSwatches.width - column - 1) * (swatchSize.width + gap.width);
        } else {
          x = column * (swatchSize.width + gap.width);
        }
        g.fillRect(x, y, swatchSize.width, swatchSize.height);
        g.setColor(Color.black);
        g.drawLine(x + swatchSize.width - 1, y, x + swatchSize.width - 1, y + swatchSize.height - 1);
        g.drawLine(x, y + swatchSize.height - 1, x + swatchSize.width - 1, y + swatchSize.height - 1);

        if (selRow == row && selCol == column && this.isFocusOwner()) {
          Color c2 = new Color(c.getRed() < 125 ? 255 : 0, c.getGreen() < 125 ? 255 : 0, c.getBlue() < 125 ? 255 : 0);
          g.setColor(c2);

          g.drawLine(x, y, x + swatchSize.width - 1, y);
          g.drawLine(x, y, x, y + swatchSize.height - 1);
          g.drawLine(x + swatchSize.width - 1, y, x + swatchSize.width - 1, y + swatchSize.height - 1);
          g.drawLine(x, y + swatchSize.height - 1, x + swatchSize.width - 1, y + swatchSize.height - 1);
          g.drawLine(x, y, x + swatchSize.width - 1, y + swatchSize.height - 1);
          g.drawLine(x, y + swatchSize.height - 1, x + swatchSize.width - 1, y);
        }
      }
    }
  }

  /**
   * getPreferredSize
   *
   * @see javax.swing.JComponent#getPreferredSize()
   */
  public Dimension getPreferredSize() {
    int x = numSwatches.width * (swatchSize.width + gap.width) - 1;
    int y = numSwatches.height * (swatchSize.height + gap.height) - 1;
    return new Dimension(x, y);
  }

  /**
   * Initializes the colors.
   */
  @SuppressWarnings("unchecked")
  protected void initColors() {
    Color defaultColor = UIManager.getColor("ColorChooser.swatchesDefaultRecentColor", getLocale());
    String recentColors = generalEditor.getRecentColors();
    // do we have any user preferences for colors?
    if (recentColors.length() > 0) {
      ObjectInputStream in;
      try {
        byte[] data = Base64.getDecoder().decode(recentColors);
        in = new ObjectInputStream(new ByteArrayInputStream(data));
        lruList = (LinkedList<Color>) in.readObject();
        in.close();
      } catch (IOException e) {
        e.printStackTrace();
      } catch (ClassNotFoundException e) {
        e.printStackTrace();
      }
    } else {
      // first time we initialize lru with default colors
      for (int i = 0; i < numColors; i++) {
        lruList.addFirst(defaultColor);
      }
    }
  }

  /**
   * getToolTipText
   *
   * @see javax.swing.JComponent#getToolTipText(java.awt.event.MouseEvent)
   */
  public String getToolTipText(MouseEvent e) {
    Color color = getColorForLocation(e.getX(), e.getY());
    return color.getRed() + ", " + color.getGreen() + ", " + color.getBlue();
  }

  /**
   * Sets the selected color from location.
   *
   * @param x
   *          the x
   * @param y
   *          the y
   */
  public void setSelectedColorFromLocation(int x, int y) {
    if (!this.getComponentOrientation().isLeftToRight()) {
      selCol = numSwatches.width - x / (swatchSize.width + gap.width) - 1;
    } else {
      selCol = x / (swatchSize.width + gap.width);
    }
    selRow = y / (swatchSize.height + gap.height);
    repaint();
  }

  /**
   * Gets the color for location.
   *
   * @param x
   *          the x
   * @param y
   *          the y
   * @return the color for location
   */
  public Color getColorForLocation(int x, int y) {
    int column;
    if (!this.getComponentOrientation().isLeftToRight()) {
      column = numSwatches.width - x / (swatchSize.width + gap.width) - 1;
    } else {
      column = x / (swatchSize.width + gap.width);
    }
    int row = y / (swatchSize.height + gap.height);
    return getColorForCell(column, row);
  }

  /**
   * Gets the color for cell.
   *
   * @param column
   *          the column
   * @param row
   *          the row
   * @return the color for cell
   */
  private Color getColorForCell(int column, int row) {
    // convert a 2D array index into a 1D index
    int idx = row * NCOLUMNS + column;
    return lruList.get(idx);
  }

  /**
   * Sets the most recent color.
   *
   * @param c
   *          the new most recent color
   */
  public void setMostRecentColor(Color c) {
    // update our lru but first check and see if the color is already present
    // if so, remove it then add to front of list
    if (!lruList.remove(c)) {
      lruList.removeLast();
    }
    lruList.addFirst(c);
    repaint();
    // now push the recent colors panel to user preferences
    ObjectOutputStream out;
    try {
      ByteArrayOutputStream baos = new ByteArrayOutputStream();
      out = new ObjectOutputStream(baos);
      out.writeObject(lruList);
      out.close();
      String recentColors = Base64.getEncoder().encodeToString(baos.toByteArray());
      generalEditor.setRecentColors(recentColors);
      generalEditor.savePreferences(); 
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

}
