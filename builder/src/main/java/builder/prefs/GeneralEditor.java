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
package builder.prefs;


import java.io.IOException;
import java.io.OutputStream;
import java.io.ObjectOutputStream;
import java.io.PrintStream;
import java.util.prefs.Preferences;

import builder.models.GeneralModel;

/**
 * The Class GeneralEditor manages the user preferences for the builder.
 * 
 * @author Paul Conti
 *  
 */
public class GeneralEditor extends ModelEditor {
  
  /** The Constant MY_NODE. */
  public static final String MY_NODE = "com/impulseadventure/builder/general";
  
  /** The Constant TITLE. */
  private static final String TITLE = "General";

  /** The instance. */
  private static GeneralEditor instance = null;
  
  /**
   * Gets the single instance of GeneralEditor.
   *
   * @return single instance of GeneralEditor
   */
  public static synchronized GeneralEditor getInstance()  {
      if (instance == null) {
          instance = new GeneralEditor();
      }
      return instance;
  }  
  
  /**
   * Instantiates a new general editor.
   */
  public GeneralEditor() {
    // get rid of the bugged Preferences warning - not needed in Java 9 and above
    System.setErr(new PrintStream(new OutputStream() {
        public void write(int b) throws IOException {}
    }));
    fPrefs = Preferences.userRoot().node(MY_NODE);
    model = new GeneralModel();
    model.TurnOffEvents();
    updateModel();
    System.setErr(System.err);  
  }

  /**
   * Sets the model.
   *
   * @param model
   *          the new model
   */
  public void setModel(GeneralModel model) {
    this.model = model;
  }
  
  /**
   * Save preferences.
   */
  public void savePreferences() {
    super.savePreferences();
  }
  
  /**
   * Gets the recent colors.
   *
   * @return the recent colors
   */
  public String getRecentColors() {
    return ((GeneralModel) model).getRecentColors();
  }

  /**
   * setRecentColors sets the recent colors
   * called by our color chooser.
   * @param s
   */
  public void setRecentColors(String s) { 
    ((GeneralModel) model).setRecentColors(s);
  }

  /**
   * getTitle
   *
   * @see builder.prefs.ModelEditor#getTitle()
   */
  @Override 
  public String getTitle() {
    return TITLE;
  }

  /**
   * Gets the theme class name.
   *
   * @return the theme class name
   */
  public String getThemeClassName() {
    return ((GeneralModel) model).getThemeClassName();
  }
  
  /**
   * Gets the target.
   *
   * @return the target
   */
  public String getTarget() {
    return ((GeneralModel) model).getTarget();
  }
  
  /**
   * Gets the width.
   *
   * @return the width
   */
  public int getWidth() {
    return ((GeneralModel) model).getWidth();
  }
  
  /**
   * Sets the width.
   *
   * @param width
   *          the new width
   */
  public void setWidth(int width) {
    ((GeneralModel) model).setWidth(width);
  }

  /**
   * Gets the height.
   *
   * @return the height
   */
  public int getHeight() {
    return ((GeneralModel) model).getHeight();
  }

  /**
   * Sets the height.
   *
   * @param height
   *          the new height
   */
  public void setHeight(int height) {
    ((GeneralModel) model).setHeight(height);
  }

  /**
   * Gets the dpi.
   *
   * @return the dpi
   */
  public int getDPI() {
    return ((GeneralModel) model).getDPI();
  }
  
  /**
   * Gets the margins.
   *
   * @return the margins
   */
  public int getMargins() {
    return ((GeneralModel) model).getMargins();
  }

  /**
   * Gets the h spacing.
   *
   * @return the h spacing
   */
  public int getHSpacing() {
    return ((GeneralModel) model).getHSpacing();
  }

  /**
   * Gets the v spacing.
   *
   * @return the v spacing
   */
  public int getVSpacing() {
    return ((GeneralModel) model).getVSpacing();
  }

 /**
  * Write model.
  *
  * @param out
  *          the out
  */
 public void writeModel(ObjectOutputStream out) {
    try {
      out.writeObject((GeneralModel)model);
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
  
}
