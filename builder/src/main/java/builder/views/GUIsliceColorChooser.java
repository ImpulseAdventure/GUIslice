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

import java.awt.AWTException;
//import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Font;
import java.awt.Point;
import java.awt.Robot;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.List;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import builder.Builder;
import builder.common.ColorFactory;
import builder.common.ColorIcon;
import builder.common.ColorItem;
import builder.tables.ColoredThumbSliderUI;
import builder.tables.GUIsliceColorRenderer;

import java.awt.Dimension;

import javax.swing.DefaultListModel;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.ImageIcon;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.LayoutStyle.ComponentPlacement;
import javax.swing.ListSelectionModel;
import javax.swing.JLabel;
import javax.swing.JSpinner;
import javax.swing.JTabbedPane;
import javax.swing.SpinnerNumberModel;

/**
 * GUIsliceColorChooser - Allows users to pick RGB colors 
 * <p>
 * In case you are wondering about the very inefficient code in this  
 * module its because I used WindowBuider to create and layout this UI.  
 * </p>
 * 
 * @author Paul Conti
 *
 */
public class GUIsliceColorChooser extends JDialog {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Closed option. */
  protected int Closed_Option = JOptionPane.CLOSED_OPTION;
  
  /** The content pane. */
  private JPanel contentPane;
  
  /** The btn CANCEL. */
  private JButton btnCANCEL;
  
  /** The btn OK. */
  private JButton btnOK;
  
  /** The lbl preview. */
  private JLabel lblPreview;
  
  /** The color list. */
  private JList<ColorItem> colorList;
  
  /** The item list. */
  private List<ColorItem> itemList;
  
  /** The list model. */
  private DefaultListModel<ColorItem> listModel;
  
  /** The preview icon. */
  private ColorIcon previewIcon;
  
  /** The tabbed pane. */
  private JTabbedPane tabbedPane;
  
  /** The rgb panel. */
  private JPanel rgbTab;
  
  /** The color panel. */
  private JPanel colorPanel;
  
  /** The color index. */
  private int colorIndex;
  
  /** The current item. */
  private ColorItem currentItem;
  
  /** The list selection model. */
  private ListSelectionModel listSelectionModel;
  
  /** The current color. */
  private Color currentColor;
  
  /** recent color panel */
  private SwatchPanel recentColorsPanel;

  /** The red value. */
  private int redValue;
  
  /** The green value. */
  private int greenValue;
  
  /** The blue value. */
  private int blueValue;
  
  /** The slider R. */
  private JSlider sliderR;
  
  /** The slider G. */
  private JSlider sliderG;
  
  /** The slider B. */
  private JSlider sliderB;
  
  /** The spinner R. */
  private JSpinner spinnerR;
  
  /** The spinner G. */
  private JSpinner spinnerG;
  
  /** The spinner B. */
  private JSpinner spinnerB;
  
  /** The data from spinner. */
  private boolean isSpinner = false;
  
  /** The data from list. */
  private boolean isList = false;
  
  /** The data from eye dropper. */
  private boolean isEyeDropper = false;
  
  Robot robot;
  
  /** The cf. */
  ColorFactory cf;
  
  /** The b save dialog. */
  boolean bSaveDialog;
  
  /** The flag to indicate UI initialized. */
  boolean bInitUI = false;
  
  /** our cross hair cursor */
  Cursor crossHairCursor;

  /**
   * Instantiates a new GUIslice color chooser.
   *
   * @param owner
   *          the owner
   */
  public GUIsliceColorChooser(JFrame owner) {
    super(owner, "GUIslice Color Chooser", true);
    cf = ColorFactory.getInstance();
    try {
      this.robot = new Robot();
    } catch (AWTException e) {
      e.printStackTrace();
    }
  }
  
  /**
   * Show dialog.
   *
   * @param color
   *          the current color
   * @return the <code>Color</code> object
   */
  public Color showDialog(Color color) {
    bSaveDialog = false;
    currentColor = color;
    if (!bInitUI) {
      initUI();
      bInitUI = true;
    } else {
      setRGB(color.getRed(), color.getGreen(), color.getBlue());
    }
    this.setLocationRelativeTo(null);
    this.setVisible(true);
    if (bSaveDialog) {
      color = currentColor;
    }
    this.setVisible(false);
    return color;
  }
  
  /**
   * Initializes the UI.
   */
  private void initUI() {
    setBounds(100, 100, 550, 450);
    crossHairCursor = new Cursor(Cursor.CROSSHAIR_CURSOR);

    redValue = currentColor.getRed();
    greenValue = currentColor.getGreen();
    blueValue = currentColor.getBlue();

    contentPane = (JPanel) getContentPane();

    contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
//    contentPane.setLayout(new BorderLayout(2, 0));
    setContentPane(contentPane);
    
    tabbedPane = new JTabbedPane(JTabbedPane.TOP);

    JScrollPane guisliceTab = new JScrollPane();
    tabbedPane.addTab("GUIslice Colors", null, guisliceTab, null);
    guisliceTab.setPreferredSize(new Dimension(250, 300));
    // create the model and add elements
    listModel = new DefaultListModel<>();
    itemList = cf.getColorList();
    for (int i = 0; i < itemList.size(); i++) {
      listModel.addElement(itemList.get(i));
    }
    colorList = new JList<ColorItem>(listModel);
    listSelectionModel = colorList.getSelectionModel();
    colorList.setCellRenderer(new GUIsliceColorRenderer());

    guisliceTab.setViewportView(colorList);

    rgbTab = new JPanel();
    tabbedPane.addTab("RGB Colors", null, rgbTab, null);

    JLabel lblRed = new JLabel("Red:");
    lblRed.setFont(new Font("Tahoma", Font.PLAIN, 14));

    JLabel lblGreen = new JLabel("Green:");
    lblGreen.setFont(new Font("Tahoma", Font.PLAIN, 14));

    JLabel lblBlue = new JLabel("Blue:");
    lblBlue.setFont(new Font("Tahoma", Font.PLAIN, 14));

    sliderR = new JSlider(JSlider.HORIZONTAL, 0, 255, redValue);
    sliderR.setUI(new ColoredThumbSliderUI(sliderR, Color.RED));
    sliderR.setPaintTicks(true);
    sliderR.setMajorTickSpacing(50);
    sliderR.setMinorTickSpacing(5);
    sliderR.setPaintLabels(true);
    sliderR.addChangeListener(new SliderListener());

    sliderG = new JSlider(JSlider.HORIZONTAL, 0, 255, greenValue);
    sliderG.setUI(new ColoredThumbSliderUI(sliderG, Color.GREEN));
    sliderG.setPaintTicks(true);
    sliderG.setMajorTickSpacing(50);
    sliderG.setMinorTickSpacing(5);
    sliderG.setPaintLabels(true);
    sliderG.addChangeListener(new SliderListener());

    sliderB = new JSlider(JSlider.HORIZONTAL, 0, 255, blueValue);
    sliderB.setUI(new ColoredThumbSliderUI(sliderB, Color.BLUE));
    sliderB.setPaintTicks(true);
    sliderB.setMajorTickSpacing(50);
    sliderB.setMinorTickSpacing(5);
    sliderB.setPaintLabels(true);
    sliderB.addChangeListener(new SliderListener());

    GroupLayout gl_rgbTab = new GroupLayout(rgbTab);
    gl_rgbTab.setHorizontalGroup(gl_rgbTab.createParallelGroup(Alignment.LEADING).addGroup(gl_rgbTab
        .createSequentialGroup().addGap(57)
        .addGroup(gl_rgbTab
            .createParallelGroup(Alignment.LEADING).addComponent(lblRed).addComponent(lblGreen).addComponent(lblBlue))
        .addGap(30)
        .addGroup(gl_rgbTab.createParallelGroup(Alignment.TRAILING)
            .addComponent(sliderG, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
            .addComponent(sliderB, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
            .addComponent(sliderR, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
        .addContainerGap(54, Short.MAX_VALUE)));
    gl_rgbTab.setVerticalGroup(gl_rgbTab.createParallelGroup(Alignment.LEADING).addGroup(gl_rgbTab
        .createSequentialGroup().addGap(57)
        .addGroup(gl_rgbTab.createParallelGroup(Alignment.LEADING).addComponent(lblRed)
            .addComponent(sliderR, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
        .addGap(53)
        .addGroup(gl_rgbTab.createParallelGroup(Alignment.LEADING).addComponent(lblGreen)
            .addComponent(sliderG, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
        .addGap(61)
        .addGroup(gl_rgbTab.createParallelGroup(Alignment.LEADING).addComponent(lblBlue).addComponent(sliderB,
            GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
        .addContainerGap(74, Short.MAX_VALUE)));
    rgbTab.setLayout(gl_rgbTab);

    colorPanel = new JPanel();
    colorPanel.setPreferredSize(new Dimension(125, 350));

    JLabel lblspinnerR = new JLabel("Red:");
    spinnerR = new JSpinner();
    spinnerR.setModel(new SpinnerNumberModel(redValue, 0, 255, 5));
    spinnerR.addChangeListener(new SpinnerListener());

    JLabel lblspinnerG = new JLabel("Green:");
    spinnerG = new JSpinner();
    spinnerG.setModel(new SpinnerNumberModel(greenValue, 0, 255, 5));
    spinnerG.addChangeListener(new SpinnerListener());

    JLabel lblspinnerB = new JLabel("Blue:");
    spinnerB = new JSpinner();
    spinnerB.setModel(new SpinnerNumberModel(blueValue, 0, 255, 5));
    spinnerB.addChangeListener(new SpinnerListener());

    previewIcon = new ColorIcon(currentColor, 40, 40);
    lblPreview = new JLabel();
    lblPreview.setMinimumSize(new Dimension(40, 40));
    lblPreview.setMaximumSize(new Dimension(40, 40));
    lblPreview.setSize(new Dimension(40, 40));
    lblPreview.setIcon(previewIcon);

    JLabel lblTxtPreview = new JLabel("Preview");
    lblTxtPreview.setFont(new Font("Tahoma", Font.BOLD, 12));

    JLabel lblRecentColors = new JLabel("Recent Colors");

    recentColorsPanel = new SwatchPanel();
    RecentSwatchListener recentSwatchListener = new RecentSwatchListener();
    recentColorsPanel.addMouseListener(recentSwatchListener);

    GroupLayout gl_colorPanel = new GroupLayout(colorPanel);
    gl_colorPanel.setHorizontalGroup(
      gl_colorPanel.createParallelGroup(Alignment.LEADING)
        .addGroup(gl_colorPanel.createSequentialGroup()
          .addGroup(gl_colorPanel.createParallelGroup(Alignment.LEADING)
            .addGroup(gl_colorPanel.createSequentialGroup()
              .addContainerGap()
              .addGroup(gl_colorPanel.createParallelGroup(Alignment.LEADING, false)
                .addGroup(gl_colorPanel.createSequentialGroup()
                  .addComponent(lblspinnerB)
                  .addPreferredGap(ComponentPlacement.RELATED, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                  .addComponent(spinnerB, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
                .addGroup(gl_colorPanel.createSequentialGroup()
                  .addGroup(gl_colorPanel.createParallelGroup(Alignment.LEADING)
                    .addComponent(lblspinnerR)
                    .addComponent(lblspinnerG))
                  .addPreferredGap(ComponentPlacement.RELATED)
                  .addGroup(gl_colorPanel.createParallelGroup(Alignment.LEADING)
                    .addComponent(spinnerG, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
                    .addComponent(spinnerR, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
                  .addGap(1))))
            .addGroup(gl_colorPanel.createSequentialGroup()
              .addGap(38)
              .addComponent(lblPreview, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)))
          .addGap(30))
        .addGroup(gl_colorPanel.createSequentialGroup()
          .addGap(21)
          .addComponent(lblRecentColors)
          .addGap(37))
        .addGroup(gl_colorPanel.createSequentialGroup()
          .addGap(30)
          .addGroup(gl_colorPanel.createParallelGroup(Alignment.LEADING)
            .addComponent(recentColorsPanel, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
            .addComponent(lblTxtPreview, GroupLayout.DEFAULT_SIZE, 85, Short.MAX_VALUE))
          .addContainerGap())
    );
    gl_colorPanel.setVerticalGroup(
      gl_colorPanel.createParallelGroup(Alignment.TRAILING)
        .addGroup(gl_colorPanel.createSequentialGroup()
          .addContainerGap()
          .addComponent(lblRecentColors)
          .addGap(18)
          .addComponent(recentColorsPanel, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
          .addPreferredGap(ComponentPlacement.RELATED, 36, Short.MAX_VALUE)
          .addComponent(lblTxtPreview)
          .addPreferredGap(ComponentPlacement.RELATED)
          .addComponent(lblPreview, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
          .addGap(13)
          .addGroup(gl_colorPanel.createParallelGroup(Alignment.BASELINE)
            .addComponent(lblspinnerR)
            .addComponent(spinnerR, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
          .addPreferredGap(ComponentPlacement.UNRELATED)
          .addGroup(gl_colorPanel.createParallelGroup(Alignment.BASELINE)
            .addComponent(lblspinnerG)
            .addComponent(spinnerG, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
          .addPreferredGap(ComponentPlacement.UNRELATED)
          .addGroup(gl_colorPanel.createParallelGroup(Alignment.BASELINE)
            .addComponent(lblspinnerB)
            .addComponent(spinnerB, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
          .addGap(62))
    );
    colorPanel.setLayout(gl_colorPanel);

    JPanel commandPanel = new JPanel();
    commandPanel.setPreferredSize(new Dimension(450, 50));

    btnOK = new JButton("OK");
    ActionListener actionListener = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        recentColorsPanel.setMostRecentColor(currentColor);
        bSaveDialog = true;
        Closed_Option = JOptionPane.OK_OPTION;
        dispose();
      }
    };
    btnOK.addActionListener(actionListener);

    btnCANCEL = new JButton("CANCEL");
    btnCANCEL.setToolTipText("Exit without save");
    actionListener = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Closed_Option = JOptionPane.CANCEL_OPTION;
        dispose();
      }
    };
    btnCANCEL.addActionListener(actionListener);
    
    GroupLayout gl_commandPanel = new GroupLayout(commandPanel);
    gl_commandPanel
        .setHorizontalGroup(gl_commandPanel.createParallelGroup(Alignment.LEADING).addGroup(Alignment.TRAILING,
            gl_commandPanel.createSequentialGroup().addContainerGap(230, Short.MAX_VALUE).addComponent(btnCANCEL)
                .addPreferredGap(ComponentPlacement.RELATED).addComponent(btnOK).addContainerGap()));
    gl_commandPanel.setVerticalGroup(gl_commandPanel.createParallelGroup(Alignment.LEADING).addGroup(Alignment.TRAILING,
        gl_commandPanel.createSequentialGroup().addContainerGap(16, Short.MAX_VALUE)
            .addGroup(
                gl_commandPanel.createParallelGroup(Alignment.BASELINE).addComponent(btnOK).addComponent(btnCANCEL))
            .addContainerGap()));
    commandPanel.setLayout(gl_commandPanel);
    GroupLayout gl_contentPane = new GroupLayout(contentPane);
    gl_contentPane.setHorizontalGroup(gl_contentPane.createParallelGroup(Alignment.LEADING)
        .addGroup(gl_contentPane.createSequentialGroup()
            .addComponent(tabbedPane, GroupLayout.PREFERRED_SIZE, 397, GroupLayout.PREFERRED_SIZE).addGap(2)
            .addComponent(colorPanel, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
        .addComponent(commandPanel, GroupLayout.PREFERRED_SIZE, 524, GroupLayout.PREFERRED_SIZE));
    gl_contentPane.setVerticalGroup(gl_contentPane.createParallelGroup(Alignment.LEADING)
        .addGroup(gl_contentPane.createSequentialGroup()
            .addGroup(gl_contentPane.createParallelGroup(Alignment.LEADING)
                .addComponent(tabbedPane, GroupLayout.PREFERRED_SIZE, 351, GroupLayout.PREFERRED_SIZE)
                .addComponent(colorPanel, GroupLayout.PREFERRED_SIZE, 351, GroupLayout.PREFERRED_SIZE))
            .addComponent(commandPanel, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE,
                GroupLayout.PREFERRED_SIZE)));

    JPanel wheelTab = new JPanel();
    tabbedPane.addTab("Color Wheel", null, wheelTab, null);

    JButton btnEyeDropper = new JButton("");
    btnEyeDropper.setIcon(new ImageIcon(Builder.class.getResource("/resources/icons/eyedropper.png")));
    btnEyeDropper.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent evt) {
          isEyeDropper = true;
          setCursor(crossHairCursor);
        }
    });

    JPanel wheelPanel = new ColorWheel();
    wheelPanel.addMouseListener(new MouseHandler());

    JLabel lblUseEyeDropper = new JLabel("Use Eye Dropper to Choose your Color");
    GroupLayout gl_wheelTab = new GroupLayout(wheelTab);
    gl_wheelTab.setHorizontalGroup(gl_wheelTab.createParallelGroup(Alignment.LEADING)
        .addGroup(gl_wheelTab.createSequentialGroup().addContainerGap()
            .addComponent(wheelPanel, GroupLayout.DEFAULT_SIZE, 289, Short.MAX_VALUE)
            .addPreferredGap(ComponentPlacement.UNRELATED).addComponent(btnEyeDropper).addGap(18))
        .addGroup(gl_wheelTab.createSequentialGroup().addGap(67)
            .addComponent(lblUseEyeDropper, GroupLayout.PREFERRED_SIZE, 218, GroupLayout.PREFERRED_SIZE)
            .addContainerGap(107, Short.MAX_VALUE)));
    gl_wheelTab.setVerticalGroup(gl_wheelTab.createParallelGroup(Alignment.LEADING)
        .addGroup(gl_wheelTab.createSequentialGroup().addGap(4).addComponent(lblUseEyeDropper)
            .addPreferredGap(ComponentPlacement.UNRELATED)
            .addGroup(gl_wheelTab.createParallelGroup(Alignment.LEADING)
                .addComponent(wheelPanel, GroupLayout.DEFAULT_SIZE, 259, Short.MAX_VALUE).addComponent(btnEyeDropper))
            .addContainerGap()));
    wheelTab.setLayout(gl_wheelTab);
    contentPane.setLayout(gl_contentPane);

    // create the model and add elements
    listModel = new DefaultListModel<>();
    itemList = cf.getColorList();
    for (int i = 0; i < itemList.size(); i++) {
      listModel.addElement(itemList.get(i));
    }
    listSelectionModel.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    listSelectionModel.addListSelectionListener(new ColorSelectionHandler());
  }

  public void setSelectedColor(Color c) {
    currentColor = c;
    isList = true;
    if (redValue != currentColor.getRed()) {
      redValue = currentColor.getRed();
      spinnerR.setValue(Integer.valueOf(redValue));
      sliderR.setValue(Integer.valueOf(redValue));
    }
    if (greenValue != currentColor.getGreen()) {
      greenValue = currentColor.getGreen();
      spinnerG.setValue(Integer.valueOf(greenValue));
      sliderG.setValue(Integer.valueOf(greenValue));
    }
    if (blueValue != currentColor.getBlue()) {
      blueValue = currentColor.getBlue();
      spinnerB.setValue(Integer.valueOf(blueValue));
      sliderB.setValue(Integer.valueOf(blueValue));
    }
    previewIcon.setColor(currentColor);
    ColorItem item = cf.findColorItem(currentColor);
    colorIndex = item.getIndex();
    colorList.setSelectedIndex(colorIndex);
    colorList.ensureIndexIsVisible(colorIndex);
    colorPanel.repaint();
  }

  class RecentSwatchListener extends MouseAdapter {
    public void mousePressed(MouseEvent e) {
      if (isEnabled()) {
        Color color = recentColorsPanel.getColorForLocation(e.getX(), e.getY());
        recentColorsPanel.setSelectedColorFromLocation(e.getX(), e.getY());
        // ignore if setting to default color
        if (color.getRGB() != -1118482)
          setSelectedColor(color);
      }
    }
  }

  /**
   * Sets the RGB.
   *
   * @param r
   *          the red value
   * @param g
   *          the green value
   * @param b
   *          the blue value
   */
  private void setRGB(int r, int g, int b) {
    currentColor = new Color(r, g, b);
    redValue = currentColor.getRed();
    greenValue = currentColor.getGreen();
    blueValue = currentColor.getBlue();
    previewIcon.setColor(currentColor);
    currentItem = cf.findColorItem(currentColor);
    colorIndex = currentItem.getIndex();
    isList = true;
    isSpinner = true;
    colorList.setSelectedIndex(colorIndex);
    colorList.ensureIndexIsVisible(colorIndex);
    spinnerR.setValue(Integer.valueOf(redValue));
    sliderR.setValue(Integer.valueOf(redValue));
    spinnerG.setValue(Integer.valueOf(greenValue));
    sliderG.setValue(Integer.valueOf(greenValue));
    spinnerB.setValue(Integer.valueOf(blueValue));
    sliderB.setValue(Integer.valueOf(blueValue));
    isList = false;
    isSpinner = false;
    repaint();
  }

  class SpinnerListener implements ChangeListener {
    @Override
    public void stateChanged(ChangeEvent e) {
      JSpinner spinner = (JSpinner) e.getSource();
      if (isList == false) {
        isSpinner = true;
        if (spinner == spinnerR) {
          redValue = ((Integer) spinner.getValue()).intValue();
          sliderR.setValue(Integer.valueOf(redValue));
        } else if (spinner == spinnerG) {
          greenValue = ((Integer) spinner.getValue()).intValue();
          sliderG.setValue(Integer.valueOf(greenValue));
        } else if (spinner == spinnerB) {
          blueValue = ((Integer) spinner.getValue()).intValue();
          sliderB.setValue(Integer.valueOf(blueValue));
        }
        currentColor = new Color(redValue, greenValue, blueValue);
        previewIcon.setColor(currentColor);
        ColorItem item = cf.findColorItem(currentColor);
        colorIndex = item.getIndex();
        colorList.setSelectedIndex(colorIndex);
        colorList.ensureIndexIsVisible(colorIndex);
        isSpinner = false;
        colorPanel.repaint();
      }
    }
  }

  class SliderListener implements ChangeListener {
    public void stateChanged(ChangeEvent e) {
      JSlider slider = (JSlider) e.getSource();
      if (isList == false && isSpinner == false) {
        if (slider == sliderR) {
          redValue = slider.getValue();
          spinnerR.setValue(Integer.valueOf(redValue));
        } else if (slider == sliderG) {
          greenValue = slider.getValue();
          spinnerG.setValue(Integer.valueOf(greenValue));
        } else if (slider == sliderB) {
          blueValue = slider.getValue();
          spinnerB.setValue(Integer.valueOf(blueValue));
        }

        currentColor = new Color(redValue, greenValue, blueValue);
        previewIcon.setColor(currentColor);
        ColorItem item = cf.findColorItem(currentColor);
        colorIndex = item.getIndex();
        colorList.setSelectedIndex(colorIndex);
        colorList.ensureIndexIsVisible(colorIndex);
        colorPanel.repaint();
      }
    }
  }

  class ColorSelectionHandler implements ListSelectionListener {
    public void valueChanged(ListSelectionEvent e) {
      ColorItem item;
      int idx = colorList.getSelectedIndex();
      if (idx != -1 && isSpinner == false) {
        colorIndex = idx;
        item = cf.findColorItem(colorIndex);
        if (item.getColor() != null) {
          isList = true;
          if (redValue != item.getRed()) {
            redValue = item.getRed();
            spinnerR.setValue(Integer.valueOf(redValue));
            sliderR.setValue(Integer.valueOf(redValue));
          }
          if (greenValue != item.getGreen()) {
            greenValue = item.getGreen();
            spinnerG.setValue(Integer.valueOf(greenValue));
            sliderG.setValue(Integer.valueOf(greenValue));
          }
          if (blueValue != item.getBlue()) {
            blueValue = item.getBlue();
            spinnerB.setValue(Integer.valueOf(blueValue));
            sliderB.setValue(Integer.valueOf(blueValue));
          }
          currentColor = new Color(redValue, greenValue, blueValue);
          previewIcon.setColor(currentColor);
          colorPanel.repaint();
          isList = false;
        }
      }
    }
  }
  /**
   * The Class MouseHandler.
   */
  private class MouseHandler extends MouseAdapter {

    /**
     * mouseClicked.
     *
     * @param e
     *          the e
     * @see java.awt.event.MouseAdapter#mouseClicked(java.awt.event.MouseEvent)
     */
    @Override
    public void mouseClicked(MouseEvent e) {
      if (isEyeDropper) {
        Point p = e.getLocationOnScreen();
        Color c = robot.getPixelColor(p.x, p.y);
        setSelectedColor(c);
        setCursor(Cursor.getDefaultCursor());
        isEyeDropper = false;
      }
    }  // end mouseClicked

    /**
     * mouseReleased.
     *
     * @param e
     *          the e
     * @see java.awt.event.MouseAdapter#mouseReleased(java.awt.event.MouseEvent)
     */
    @Override
    public void mouseReleased(MouseEvent e) {
    }  // end mouseReleased

    /**
     * mousePressed.
     *
     * @param e
     *          the e
     * @see java.awt.event.MouseAdapter#mousePressed(java.awt.event.MouseEvent)
     */
    @Override
    public void mousePressed(MouseEvent e) {
    } // end mousePressed
  } // end MouseHandler


}
