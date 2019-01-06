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
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.List;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import builder.common.ColorFactory;
import builder.common.ColorIcon;
import builder.common.ColorItem;
import builder.tables.ColoredThumbSliderUI;
import builder.tables.GUIsliceColorRenderer;

import java.awt.Dimension;

import javax.swing.DefaultListModel;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
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
  private JPanel rgbPanel;
  
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
  
  /** The is spinner. */
  private boolean isSpinner = false;
  
  /** The is list. */
  private boolean isList = false;
  
  /** The cf. */
  ColorFactory cf;
  
  /** The b save dialog. */
  boolean bSaveDialog;
  
  /** The b init UI. */
  boolean bInitUI = false;

  /**
   * Instantiates a new GU islice color chooser.
   *
   * @param owner
   *          the owner
   */
  public GUIsliceColorChooser(JFrame owner) {
    super(owner, "GUIslice Color Chooser", true);
    cf = ColorFactory.getInstance();
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

    redValue = currentColor.getRed();
    greenValue = currentColor.getGreen();
    blueValue = currentColor.getBlue();

    contentPane = (JPanel) getContentPane();
    contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
    contentPane.setLayout(new BorderLayout(2, 0));
    setContentPane(contentPane);
    
    tabbedPane = new JTabbedPane(JTabbedPane.TOP);
    contentPane.add(tabbedPane, BorderLayout.CENTER);
    
    JScrollPane scrollPane = new JScrollPane();
    tabbedPane.addTab("GUIslice Colors", null, scrollPane, null);
    scrollPane.setPreferredSize(new Dimension(250, 300));
    //create the model and add elements
    listModel = new DefaultListModel<>();
    itemList = cf.getColorList();
    for (int i=0; i<itemList.size(); i++) {
      listModel.addElement(itemList.get(i));
    }
    colorList = new JList<ColorItem>(listModel);
    listSelectionModel = colorList.getSelectionModel();
    colorList.setCellRenderer(new GUIsliceColorRenderer());
    
    scrollPane.setViewportView(colorList);
    
    rgbPanel = new JPanel();
    tabbedPane.addTab("RGB Colors", null, rgbPanel, null);
    
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
    
    GroupLayout gl_rgbPanel = new GroupLayout(rgbPanel);
    gl_rgbPanel.setHorizontalGroup(
      gl_rgbPanel.createParallelGroup(Alignment.LEADING)
        .addGroup(gl_rgbPanel.createSequentialGroup()
          .addGap(57)
          .addGroup(gl_rgbPanel.createParallelGroup(Alignment.LEADING)
            .addComponent(lblRed)
            .addComponent(lblGreen)
            .addComponent(lblBlue))
          .addGap(30)
          .addGroup(gl_rgbPanel.createParallelGroup(Alignment.TRAILING)
            .addComponent(sliderG, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
            .addComponent(sliderB, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
            .addComponent(sliderR, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
          .addContainerGap(54, Short.MAX_VALUE))
    );
    gl_rgbPanel.setVerticalGroup(
      gl_rgbPanel.createParallelGroup(Alignment.LEADING)
        .addGroup(gl_rgbPanel.createSequentialGroup()
          .addGap(57)
          .addGroup(gl_rgbPanel.createParallelGroup(Alignment.LEADING)
            .addComponent(lblRed)
            .addComponent(sliderR, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
          .addGap(53)
          .addGroup(gl_rgbPanel.createParallelGroup(Alignment.LEADING)
            .addComponent(lblGreen)
            .addComponent(sliderG, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
          .addGap(61)
          .addGroup(gl_rgbPanel.createParallelGroup(Alignment.LEADING)
            .addComponent(lblBlue)
            .addComponent(sliderB, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
          .addContainerGap(74, Short.MAX_VALUE))
    );
    rgbPanel.setLayout(gl_rgbPanel);
    
    colorPanel = new JPanel();
    colorPanel.setPreferredSize(new Dimension(125, 350));
    contentPane.add(colorPanel, BorderLayout.EAST);
    
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
    
    GroupLayout gl_colorPanel = new GroupLayout(colorPanel);
    gl_colorPanel.setHorizontalGroup(
      gl_colorPanel.createParallelGroup(Alignment.LEADING)
        .addGroup(gl_colorPanel.createSequentialGroup()
          .addContainerGap()
          .addGroup(gl_colorPanel.createParallelGroup(Alignment.TRAILING)
            .addGroup(gl_colorPanel.createSequentialGroup()
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
                    .addComponent(spinnerR, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))))
              .addContainerGap(28, Short.MAX_VALUE))
            .addGroup(gl_colorPanel.createSequentialGroup()
              .addGap(12)
              .addGroup(gl_colorPanel.createParallelGroup(Alignment.TRAILING)
                .addComponent(lblTxtPreview)
                .addComponent(lblPreview, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
              .addGap(39))))
    );
    gl_colorPanel.setVerticalGroup(
      gl_colorPanel.createParallelGroup(Alignment.TRAILING)
        .addGroup(gl_colorPanel.createSequentialGroup()
          .addContainerGap(103, Short.MAX_VALUE)
          .addComponent(lblTxtPreview)
          .addPreferredGap(ComponentPlacement.UNRELATED)
          .addComponent(lblPreview, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
          .addGap(38)
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
    contentPane.add(commandPanel, BorderLayout.SOUTH);
    
    btnOK = new JButton("OK");
    ActionListener actionListener = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
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
    gl_commandPanel.setHorizontalGroup(
      gl_commandPanel.createParallelGroup(Alignment.LEADING)
        .addGroup(Alignment.TRAILING, gl_commandPanel.createSequentialGroup()
          .addContainerGap(230, Short.MAX_VALUE)
          .addComponent(btnCANCEL)
          .addPreferredGap(ComponentPlacement.RELATED)
          .addComponent(btnOK)
          .addContainerGap())
    );
    gl_commandPanel.setVerticalGroup(
      gl_commandPanel.createParallelGroup(Alignment.LEADING)
        .addGroup(Alignment.TRAILING, gl_commandPanel.createSequentialGroup()
          .addContainerGap(16, Short.MAX_VALUE)
          .addGroup(gl_commandPanel.createParallelGroup(Alignment.BASELINE)
            .addComponent(btnOK)
            .addComponent(btnCANCEL))
          .addContainerGap())
    );
    commandPanel.setLayout(gl_commandPanel);
    
    //create the model and add elements
    listModel = new DefaultListModel<>();
    itemList = cf.getColorList();
    for (int i=0; i<itemList.size(); i++) {
      listModel.addElement(itemList.get(i));
    }
    listSelectionModel.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    listSelectionModel.addListSelectionListener(new ColorSelectionHandler());

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

    /**
     * The listener interface for receiving spinner events. The class that is
     * interested in processing a spinner event implements this interface, and the
     * object created with that class is registered with a component using the
     * component's <code>addSpinnerListener<code> method. When the spinner event
     * occurs, that object's appropriate method is invoked.
     *
     * @see SpinnerEvent
     */
    class SpinnerListener  implements ChangeListener {
      
      /**
       * stateChanged
       *
       * @see javax.swing.event.ChangeListener#stateChanged(javax.swing.event.ChangeEvent)
       */
      @Override
      public void stateChanged(ChangeEvent e) {
        JSpinner spinner = (JSpinner) e.getSource();
        if (isList == false) {
          isSpinner = true;
          if (spinner == spinnerR) {
            redValue = ((Integer)spinner.getValue()).intValue();
            sliderR.setValue(Integer.valueOf(redValue));
          } else if (spinner == spinnerG) {
            greenValue = ((Integer)spinner.getValue()).intValue();
            sliderG.setValue(Integer.valueOf(greenValue));
          } else if (spinner == spinnerB) {
            blueValue = ((Integer)spinner.getValue()).intValue();
            sliderB.setValue(Integer.valueOf(blueValue));
          }
          currentColor = new Color(redValue, greenValue, blueValue);
          previewIcon.setColor(currentColor);
          ColorItem item = cf.findColorItem(currentColor);
          colorIndex = item.getIndex();
          colorList.setSelectedIndex(colorIndex);
          colorList.ensureIndexIsVisible(colorIndex);
          isSpinner = false;
          repaint();
        }
      }
    };
     
    /**
     * The listener interface for receiving slider events. The class that is
     * interested in processing a slider event implements this interface, and the
     * object created with that class is registered with a component using the
     * component's <code>addSliderListener<code> method. When the slider event
     * occurs, that object's appropriate method is invoked.
     *
     * @see SliderEvent
     */
    class SliderListener implements ChangeListener {
      
      /**
       * stateChanged
       *
       * @see javax.swing.event.ChangeListener#stateChanged(javax.swing.event.ChangeEvent)
       */
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

    /**
     * The Class ColorSelectionHandler.
     */
    class ColorSelectionHandler implements ListSelectionListener {
    
    /**
     * valueChanged
     *
     * @see javax.swing.event.ListSelectionListener#valueChanged(javax.swing.event.ListSelectionEvent)
     */
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

}
