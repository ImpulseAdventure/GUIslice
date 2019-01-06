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
package builder.common;

import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;

import javax.swing.Icon;

/**
 * The Class ColorIcon.
 * A simple class to represent a single color as an icon.
 * 
 * @author Paul Conti
 * 
 */
public class ColorIcon implements Icon {

    /** The Constant WIDE. */
    private static final int WIDE = 20;
    
    /** The Constant HIGH. */
    private static final int HIGH = 20;
    
    /** The width. */
    private int width;
    
    /** The height. */
    private int height;
    
    /** The color. */
    private Color color;

    /**
     * Instantiates a new color icon.
     *
     * @param color
     *          the color
     */
    public ColorIcon(Color color) {
        this.color = color;
        this.width = WIDE;
        this.height = HIGH;
    }

    /**
     * Instantiates a new color icon.
     *
     * @param color
     *          the color
     * @param width
     *          the width
     * @param height
     *          the height
     */
    public ColorIcon(Color color, int width, int height) {
      this.color = color;
      this.width = width;
      this.height = height;
  }

    /**
     * Gets the color.
     *
     * @return the color
     */
    public Color getColor() {
        return color;
    }

    /**
     * Sets the color.
     *
     * @param color
     *          the new color
     */
    public void setColor(Color color) {
        this.color = color;
    }

    /**
     * paintIcon
     *
     * @see javax.swing.Icon#paintIcon(java.awt.Component, java.awt.Graphics, int, int)
     */
    @Override
    public void paintIcon(Component c, Graphics g, int x, int y) {
        g.setColor(color);
        g.fillRect(x, y, width, height);
    }

    /**
     * getIconWidth
     *
     * @see javax.swing.Icon#getIconWidth()
     */
    public int getIconWidth() {
        return width;
    }

    /**
     * getIconHeight
     *
     * @see javax.swing.Icon#getIconHeight()
     */
    public int getIconHeight() {
        return height;
    }

}
