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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.List;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.border.EtchedBorder;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;

import builder.common.FontFactory;
import builder.common.FontItem;

/**
 * The Class FontChooser - Allows users to pick fonts using GUIslice API library names.
 * 
 * @author Paul Conti
 * 
 */
public class FontChooser extends JDialog {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;

  /** The Closed option. */
  protected int Closed_Option = JOptionPane.CLOSED_OPTION;
  
  /** The ff. */
  FontFactory ff = null;
  
  /** The current font. */
  private Font   currentFont;
  
  /** The current name. */
  private String currentName;
  
  /** The fonts. */
  List<FontItem> fonts;
  
  /** The cb font. */
  JComboBox<String> cbFont;
  
  /** The fill color. */
  Color fillColor;
  
  /** The text color. */
  Color textColor;
  
  /** The b save dialog. */
  boolean bSaveDialog;
  
  /** The b init UI. */
  boolean bInitUI = false;

  /** The preview label. */
  protected FontLabel previewLabel;

  /**
   * Instantiates a new font chooser.
   *
   * @param owner
   *          the owner
   */
  public FontChooser(JFrame owner) {
    super(owner, "Font Chooser", true);
    ff = FontFactory.getInstance();
  }
  
  /**
   * Show dialog.
   *
   * @param fontName
   *          the current font name
   * @return the <code>String</code> object
   */
  public String showDialog(String fontName) {
    bSaveDialog = false;
    currentName = fontName;
    if (!bInitUI) {
      initUI();
      bInitUI = true;
    }
    updatePreview();
    this.setLocationRelativeTo(null);
    this.setVisible(true);
    if (bSaveDialog) {
      fontName = currentName;
    }
    this.setVisible(false);
    return fontName;
  }
  
  /**
   * Initializes the UI.
   */
  private void initUI() {
    getContentPane().setLayout(
        new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
    JPanel p = new JPanel(new GridLayout(1, 2, 10, 2));
    p.setBorder(new TitledBorder(new EtchedBorder(), "Font"));
    
    ActionListener previewListener = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        updatePreview();
      }
    };

    fonts = ff.getFontList();
    cbFont = new JComboBox<String>();
    for (FontItem fontItem : fonts) {
      cbFont.addItem(fontItem.getDisplayName());
    }
    cbFont.setSelectedItem(currentName);
    cbFont.addActionListener(previewListener);
    cbFont.setToolTipText("Click to Select Font");
    p.add(cbFont);

    getContentPane().add(Box.createVerticalStrut(5));
    
    p.add(Box.createHorizontalStrut(10));
    getContentPane().add(p);

    p = new JPanel(new BorderLayout());
    p.setBorder(new TitledBorder(new EtchedBorder(), "Preview"));
    previewLabel = new FontLabel("Preview Font");

    p.add(previewLabel, BorderLayout.CENTER);
    getContentPane().add(p);

    p = new JPanel(new FlowLayout());
    JPanel p1 = new JPanel(new GridLayout(1, 2, 10, 2));
    JButton btnOK = new JButton("OK");
    btnOK.setToolTipText("Save and exit");
    getRootPane().setDefaultButton(btnOK);
    ActionListener actionListener = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        bSaveDialog = true;
        Closed_Option = JOptionPane.OK_OPTION;
        dispose();
      }
    };
    btnOK.addActionListener(actionListener);
    p1.add(btnOK);

    JButton btnCancel = new JButton("Cancel");
    btnCancel.setToolTipText("Exit without save");
    actionListener = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Closed_Option = JOptionPane.CANCEL_OPTION;
        dispose();
      }
    };
    btnCancel.addActionListener(actionListener);
    p1.add(btnCancel);
    p.add(p1);
    getContentPane().add(p);

    pack();
    setResizable(false);
  }

  /**
   * Gets the option.
   *
   * @return the option
   */
  public int getOption() {
    return Closed_Option;
  }
  
  /**
   * Update preview.
   */
  protected void updatePreview() {
    int nameIdx = cbFont.getSelectedIndex();
    FontItem item = fonts.get(nameIdx);
    currentName = item.getDisplayName();
    currentFont = item.getFont();
    previewLabel.setFont(currentFont);
    previewLabel.setBackground(fillColor);
    previewLabel.setForeground(textColor);
    previewLabel.repaint();
  }

  /**
   * The Class FontLabel.
   */
  class FontLabel extends JLabel {
    
    /** The Constant serialVersionUID. */
    private static final long serialVersionUID = 1L;

    /**
     * Instantiates a new font label.
     *
     * @param text
     *          the text
     */
    public FontLabel(String text) {
      super(text, JLabel.CENTER);
      setBackground(Color.white);
      setForeground(Color.black);
      setOpaque(true);
      setBorder(new LineBorder(Color.black));
      setPreferredSize(new Dimension(120, 40));
    }
  }
 }


