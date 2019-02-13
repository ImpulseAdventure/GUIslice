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

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.Random;

import javax.swing.AbstractButton;
import javax.swing.filechooser.FileFilter;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JInternalFrame;

import builder.common.CommonUtil;
import builder.common.EnumFactory;
import builder.controller.CodeGenerator;
import builder.controller.Controller;
import builder.models.GeneralModel;
import builder.prefs.GeneralEditor;
import builder.widgets.ImageWidget;
import builder.widgets.ImgButtonWidget;
import builder.widgets.Widget;
import builder.widgets.WidgetFactory;

/**
 * The Class Toolbox.
 * 
 * @author Paul Conti
 * 
 */
public class Toolbox extends JInternalFrame implements ActionListener {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The controller. */
  Controller controller;
  
  /** The general model. */
  private GeneralModel generalModel;
  
  /** The rand. */
  private Random rand = new Random();
  
  /**
   * Instantiates a new toolbox.
   */
  public Toolbox() {
    this.controller = Controller.getInstance();
    generalModel = (GeneralModel) GeneralEditor.getInstance().getModel();
    initUI();
  }

  /**
   * Initializes the UI.
   */
  private void initUI() {
    this.setTitle("ToolBox");        
    this.setSize(150, 500);
    this.setLayout(new GridLayout(0,1));

    JButton btnBox = new JButton(EnumFactory.BOX);
    JButton btnCheckBox = new JButton("Checkbox");
    JButton btnRadio = new JButton(EnumFactory.RADIOBUTTON);
    JButton btnGraph = new JButton(EnumFactory.GRAPH);
    JButton btnImage = new JButton(EnumFactory.IMAGE);
    JButton btnImgButton = new JButton(EnumFactory.IMAGEBUTTON);
    JButton btnProgressBar = new JButton(EnumFactory.PROGRESSBAR);
    JButton btnSlider = new JButton(EnumFactory.SLIDER);
    JButton btnText = new JButton(EnumFactory.TEXT);
    JButton btnTextBox = new JButton(EnumFactory.TEXTBOX);
    JButton btnTxtButton = new JButton(EnumFactory.TEXTBUTTON);
    JButton btnPage = new JButton(EnumFactory.PAGE);
    
    btnBox.addActionListener(this);
    btnBox.setActionCommand("box");
    btnText.addActionListener(this);
    btnText.setActionCommand("text");
    btnTextBox.addActionListener(this);
    btnTextBox.setActionCommand("textbox");
    btnTxtButton.addActionListener(this);
    btnTxtButton.setActionCommand("textbutton");
    btnCheckBox.addActionListener(this);
    btnCheckBox.setActionCommand("checkbox");
    btnRadio.addActionListener(this);
    btnRadio.setActionCommand("radio");
    btnGraph.addActionListener(this);
    btnGraph.setActionCommand("graph");
    btnImgButton.addActionListener(this);
    btnImgButton.setActionCommand("imagebutton");
    btnImage.addActionListener(this);
    btnImage.setActionCommand("image");
    btnProgressBar.addActionListener(this);
    btnProgressBar.setActionCommand("progressbar");
    btnSlider.addActionListener(this);
    btnSlider.setActionCommand("slider");
    btnPage.addActionListener(this);
    btnPage.setActionCommand("page");
    
    this.add(btnText);
    this.add(btnBox);
    this.add(btnTxtButton);
    this.add(btnImgButton);
    this.add(btnCheckBox);
    this.add(btnRadio);
    this.add(btnProgressBar);
    this.add(btnSlider);
    this.add(btnGraph);
    this.add(btnTextBox);
    this.add(btnImage);
    this.add(btnPage);
    
    this.pack();
    this.setVisible(true);
    
  }
  
  /**
   * Show image dialog.
   *
   * @param title
   *          the title
   * @return the <code>file</code> object
   */
  public File showImageDialog(String title) {
    String target = generalModel.getTarget();
    File file = null;
    JFileChooser fileChooser = new JFileChooser();
    fileChooser.setDialogTitle(title);
    fileChooser.setAcceptAllFileFilterUsed(false);
    fileChooser.addChoosableFileFilter(new FileFilter() {
      public String getDescription() {
        if (!target.equals("linux"))
          return "BMP Images (*.bmp), C File with extern image (*.c)";
        else
          return "BMP Images (*.bmp)";
      }

      public boolean accept(File f) {
        if (f.isDirectory()) {
          return true;
        } else {
          if ((f.getName().toLowerCase().endsWith(".bmp")) ||
              (f.getName().toLowerCase().endsWith(".c") && 
               !target.equals("linux")) )
            return true;
          return false;
        }
      }
    });
    ImagePreviewPanel preview = new ImagePreviewPanel();
    fileChooser.setAccessory(preview);
    fileChooser.addPropertyChangeListener(preview);

    String resDir = CodeGenerator.ARDUINO_RES;
    if (generalModel.getTarget().equals("linux")) 
      resDir = CodeGenerator.LINUX_RES;
    String workingDir = CommonUtil.getInstance().getWorkingDir();
    File currentDirectory = new File(workingDir + resDir);
    fileChooser.setCurrentDirectory(currentDirectory);
    int option = fileChooser.showDialog(new JFrame(), "Select");
    if (option == JFileChooser.APPROVE_OPTION) {
      file = fileChooser.getSelectedFile();
    } 
    return file;
  }
  
  /**
   * Creates the widget.
   *
   * @param name
   *          the name
   */
  public void createWidget(String name) {
    int  x = rand.nextInt(generalModel.getWidth()-25);
    int  y = rand.nextInt(generalModel.getHeight()-25);
    Widget w = WidgetFactory.getInstance().createWidget(name, x, y);
    if (w != null) {
      controller.addWidget(w);
    }
  }

  /**
   * Creates the image widget.
   */
  public void createImageWidget() {
    int  x = rand.nextInt(generalModel.getWidth()-50);
    int  y = rand.nextInt(generalModel.getHeight()-50);
    ImageWidget w = (ImageWidget) WidgetFactory.getInstance().createWidget(EnumFactory.IMAGE, x, y);
    File file = showImageDialog("Choose your Image file");
    if (file != null) {
      w.setImage(file, x, y);
      controller.addWidget(w);
    }
  }

  /**
   * Creates the img button widget.
   */
  public void createImgButtonWidget() {
    int  x = rand.nextInt(generalModel.getWidth()-50);
    int  y = rand.nextInt(generalModel.getHeight()-50);
    ImgButtonWidget w = (ImgButtonWidget) WidgetFactory.getInstance().createWidget(EnumFactory.IMAGEBUTTON, x, y);
    File file = showImageDialog("Choose your Button's Image");
    if (file != null) {
      w.setImage(file, x, y);
      file = showImageDialog("Choose your Button's Image when pressed");
      if (file != null) {
        w.setImageSelected(file);
        controller.addWidget(w);
      }
    }
  }

  /**
   * actionPerformed
   *
   * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
   */
  @Override
  public void actionPerformed(ActionEvent e) {
    String command = ((AbstractButton)e.getSource()).getActionCommand();
    switch(command) {
      case "box":
        createWidget(EnumFactory.BOX);
        break;
      
      case "checkbox":
        createWidget(EnumFactory.CHECKBOX);
        break;
      
      case "graph":
        createWidget(EnumFactory.GRAPH);
        break;
      
      case "image":
        createImageWidget();
        break;
      
      case "imagebutton":
        createImgButtonWidget();
        break;
      
      case "page":
        Controller.getInstance().createPage();
        break;
      
      case "progressbar":
        createWidget(EnumFactory.PROGRESSBAR);
        break;
        
      case "radio":
        createWidget(EnumFactory.RADIOBUTTON);
        break;
      
      case "slider":
        createWidget(EnumFactory.SLIDER);
        break;
        
      case "text":
        createWidget(EnumFactory.TEXT);
        break;
      
      case "textbox":
        createWidget(EnumFactory.TEXTBOX);
        break;
      
      case "textbutton":
        createWidget(EnumFactory.TEXTBUTTON);
        break;
      
        default:
          throw new IllegalArgumentException("Invalid menu item: " + command);
      }
    
  }

}
