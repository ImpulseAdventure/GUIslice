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

import javax.imageio.ImageIO;
import javax.swing.*;

import builder.common.HexToImgConv;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.beans.*;
import java.io.File;
import java.io.IOException;
 
/**
 * The Class ImagePreviewPanel implements an extension to JFileChooser 
 * to show an Image Preview of selected files.
 * 
 * Image preview panel implements PropertyChangeListener so that we can
 * register it with the JFileChooser to receive change notifications. 
 * When we get a notification, we make sure we are responding to the right
 * event, and then get the selected file name by calling getNewValue() on
 * the property change event object. 
 * 
 * The idea and much of the code came from a blog posted by Michael Urban
 * on 5:51 AM on Oct 4, 2005.  
 * 
 * @author Paul Conti
 */
public class ImagePreviewPanel extends JPanel implements PropertyChangeListener {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant SCALESZ. */
  private static final int SCALESZ = 125;

  /** The height. */
  private int width, height;
    
    /** The image. */
    private BufferedImage image;
    
    /** The toolkit image. */
    private Image toolkitImage;
    
    /** The convert. */
    private HexToImgConv convert;
    
    /** The bg. */
    private Color bg;
    
    /**
     * Instantiates a new image preview panel.
     */
    public ImagePreviewPanel() {
        setPreferredSize(new Dimension(SCALESZ, -1));
        convert = new HexToImgConv();
        bg = getBackground();
    }
    
    /**
     * propertyChange
     *
     * @see java.beans.PropertyChangeListener#propertyChange(java.beans.PropertyChangeEvent)
     */
    public void propertyChange(PropertyChangeEvent e) {
        String propertyName = e.getPropertyName();
        
        // Make sure we are responding to the right event.
        if (propertyName.equals(JFileChooser.SELECTED_FILE_CHANGED_PROPERTY)) {
            File selection = (File)e.getNewValue();
            String imageName;
            
            if (selection == null)
                return;
            else
                imageName = selection.getAbsolutePath();
            
            /*
             * GUIslice API only supports BMP and C Files
             */
            if (imageName != null) {
              if (imageName.toLowerCase().endsWith(".bmp")) {
                try {
                  image = ImageIO.read(new File(imageName));
                  scaleImage();
                  repaint();
                } catch (IOException e1) {
                  e1.printStackTrace();
                } 
              } else if (imageName.toLowerCase().endsWith(".c")) {
                image = convert.doConvert(new File(imageName));
                scaleImage();
                repaint();
              }
            }
        }
    }
    
    /**
     * Scale image.
     */
    private void scaleImage() {
        width = image.getWidth(this);
        height = image.getHeight(this);
        double ratio = 1.0;
       
        /* 
         * Determine how to scale the image. Since the accessory can expand
         * vertically make sure we don't go larger than SCALESZ-5 when scaling
         * vertically.
         */
        if (width >= height) {
            ratio = (double)(SCALESZ-5) / width;
            width = SCALESZ-5;
            height = (int)(height * ratio);
        }
        else {
            if (getHeight() > 150) {
                ratio = (double)(SCALESZ-5) / height;
                height = SCALESZ-5;
                width = (int)(width * ratio);
            }
            else {
                ratio = (double)getHeight() / height;
                height = getHeight();
                width = (int)(width * ratio);
            }
        }
        // To get a scaled instance of a buffered image we need
        // to create a new image, a BufferedImage with the TookitImage.
        // width and height are for the scaled image
        toolkitImage = image.getScaledInstance(width, height, 
            Image.SCALE_SMOOTH);
        // now replace the original image with a new scaled image
        image = new BufferedImage(width, height, 
              BufferedImage.TYPE_INT_ARGB);
        Graphics g = image.getGraphics();
        g.drawImage(toolkitImage, 0, 0, null);
        g.dispose();
    }
    
    /**
     * paintComponent
     *
     * @see javax.swing.JComponent#paintComponent(java.awt.Graphics)
     */
    public void paintComponent(Graphics g) {
        g.setColor(bg);
        g.fillRect(0, 0, SCALESZ, getHeight());
        g.drawImage(image, getWidth() / 2 - width / 2 + 5,
                getHeight() / 2 - height / 2, this);
    }
    
}
