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
package builder.models;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import javax.imageio.ImageIO;

import builder.common.CommonUtil;
import builder.common.EnumFactory;
import builder.prefs.GeneralEditor;

/**
 * The Class ImageModel implements the model for the Image widget.
 */
public class ImageModel extends WidgetModel { 
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant PROP_IMAGE. */
  static private final int PROP_IMAGE   =6;
  
  /** The Constant PROP_DEFINE. */
  static private final int PROP_DEFINE  =7;
  
  /** The Constant PROP_FORMAT. */
  static private final int PROP_FORMAT  =8;

  /** The Constant PROP_FORMAT. */
  static private final int PROP_TRANSPARENCY  =9;

  /** The general model. */
  private GeneralModel generalModel;
  
  /** The image. */
  private BufferedImage image;

  /**
   * Instantiates a new image model.
   */
  public ImageModel() {
    initProperties();
    generalModel = (GeneralModel) GeneralEditor.getInstance().getModel();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.IMAGE;
    data = new Object[10][5];
    
    initProp(PROP_KEY, String.class, "COM-001", Boolean.TRUE,"Key",widgetType);
    initProp(PROP_ENUM, String.class, "COM-002", Boolean.FALSE,"ENUM",widgetType);
    initProp(PROP_X, Integer.class, "COM-003", Boolean.FALSE,"X",Integer.valueOf(0));
    initProp(PROP_Y, Integer.class, "COM-004", Boolean.FALSE,"Y",Integer.valueOf(0));
    initProp(PROP_WIDTH, Integer.class, "COM-005", Boolean.TRUE,"Width",Integer.valueOf(0));
    initProp(PROP_HEIGHT, Integer.class, "COM-006", Boolean.TRUE,"Height",Integer.valueOf(0));

    initProp(PROP_IMAGE, String.class, "IMG-100", Boolean.TRUE,"Image","");
    initProp(PROP_DEFINE, String.class, "IMG-101", Boolean.FALSE,"Image #defines","");
    initProp(PROP_FORMAT, String.class, "IMG-102", Boolean.TRUE,"Image Format","");
    initProp(PROP_TRANSPARENCY, Boolean.class, "IMG-107", Boolean.FALSE,"Transparent?",Boolean.FALSE);

  }

  /**
   * Use default colors.
   *
   * @return <code>true</code>, if successful
   */
  public boolean useDefaultColors() {
    return true;
  }
  
 /**
  * Gets the image name.
  *
  * @return the image name
  */
 public String getImageName() {
   return (String) data[PROP_IMAGE][PROP_VAL_VALUE];
 }
 
 /**
  * Sets the image name.
  *
  * @param name
  *          the new image name
  */
 public void setImageName(String name) {
   shortcutValue(name, PROP_IMAGE);
 }

 /**
  * Gets the image format.
  *
  * @return the image format
  */
 public String getImageFormat() {
   return (String) data[PROP_FORMAT][PROP_VAL_VALUE];
 }
 
 /**
  * Sets the image format.
  *
  * @param name
  *          the new image format
  */
 public void setImageFormat(String name) {
   data[PROP_FORMAT][PROP_VAL_VALUE]=name;
 }

 /**
  * is Transparent?
  *
  * @return <code>true</code>, if successful
  */
 public boolean isTransparent() {
   return (((Boolean) data[PROP_TRANSPARENCY][PROP_VAL_VALUE]).booleanValue());
 }
 
 /**
  * Gets the image.
  *
  * @return the image
  */
 public BufferedImage getImage() {
   return image;
 }


  /**
   * Sets the image.
   *
   * @param file
   *          the file
   * @param x
   *          the x
   * @param y
   *          the y
   */
  public void setImage(File file, int x, int y) {
    image = null;
    try {
        image = ImageIO.read(file);
    } catch(IOException e) {
        System.out.println("read error: " + e.getMessage());
    }
    setWidth(image.getWidth());
    setHeight(image.getHeight());
    if (image.getType() == BufferedImage.TYPE_3BYTE_BGR)
      setImageFormat("GSLC_IMGREF_FMT_BMP24");
    else if (image.getType() == BufferedImage.TYPE_USHORT_555_RGB) 
      setImageFormat("GSLC_IMGREF_FMT_BMP16");
    else
      setImageFormat("GSLC_IMGREF_FMT_RAW1");
    // now construct a #define to use during code generation
    String fileName = file.getName();
    int n = fileName.indexOf(".bmp");
    if (n > 0) {
      String tmp = fileName.substring(0,n);
      fileName = tmp.toUpperCase();
    }
    // remove all special characters
    fileName = fileName.replaceAll("\\W", ""); 
    fileName = "IMG_" + fileName;
    setDefine(fileName);
    // do we need to add a relative path for code generation?
    fileName = file.getName();
    setImageName(fileName);
  }
 
  /**
   * Gets the define.
   *
   * @return the define
   */
  public String getDefine() {
    String dir = generalModel.getImageDir();
    String fileName = (String) data[PROP_DEFINE][PROP_VAL_VALUE];
    // do we need to add a relative path for code generation?
    if (dir.length() > 0)
      fileName = dir + fileName;
    return fileName;
  }
  
  /**
   * Sets the define.
   *
   * @param s
   *          the new define
   */
  public void setDefine(String s) {
    shortcutValue(s, PROP_DEFINE);
  }

  /**
   * writeModel
   * @param out
   *          the out stream
   *
   * @see builder.models.WidgetModel#writeModel(java.io.ObjectOutputStream)
   */
  @Override
  public void writeModel(ObjectOutputStream out) throws IOException {
    super.writeModel(out);
    out.writeObject((String) CommonUtil.getInstance().encodeToString(image));
  }
  
  /**
   * readModel() will deserialize our model's data from a string object for backup
   * and recovery.
   *
   * @param in
   *          the in stream
   * @param widgetType
   *          the widget type
   * @throws IOException
   *           Signals that an I/O exception has occurred.
   * @throws ClassNotFoundException
   *           the class not found exception
   * @see builder.models.WidgetModel#readModel(java.io.ObjectInputStream, java.lang.String)
   */
  @Override
  public void readModel(ObjectInputStream in, String widgetType) throws IOException, ClassNotFoundException {
    super.readModel(in,  widgetType);
    String imageString = (String) in.readObject();
    image = CommonUtil.getInstance().decodeToImage(imageString);
  }

}
