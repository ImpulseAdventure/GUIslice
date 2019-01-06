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


import java.awt.Color;
import java.io.IOException;
import java.io.OutputStream;
import java.io.ObjectOutputStream;
import java.io.PrintStream;
import java.util.prefs.Preferences;

import builder.models.GridModel;

/**
 * The Class GridEditor manages user preferences for the Grid.
 * 
 * @author Paul Conti
 * 
 */
public class GridEditor extends ModelEditor {
  
  /** The Constant MY_NODE. */
  public static final String MY_NODE = "com/impulseadventure/builder/grid";
  
  /** The Constant TITLE. */
  private static final String TITLE = "Grid";

  /** The instance. */
  private static GridEditor instance = null;
  
  /**
   * Gets the single instance of GridEditor.
   *
   * @return single instance of GridEditor
   */
  public static synchronized GridEditor getInstance()  {
      if (instance == null) {
          instance = new GridEditor();
      }
      return instance;
  }  
  
  /**
   * Instantiates a new grid editor.
   */
  public GridEditor() {
    // get rid of the bugged Preferences warning - not needed in Java 9 and above
    System.setErr(new PrintStream(new OutputStream() {
        public void write(int b) throws IOException {}
    }));
    fPrefs = Preferences.userRoot().node(MY_NODE);
    model = new GridModel();
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
  public void setModel(GridModel model) {
    this.model = model;
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
   * Gets the grid.
   *
   * @return the grid
   */
  public boolean getGrid() {
    return ((GridModel) model).getGrid();
  }
  
  /**
   * Gets the grid snap to.
   *
   * @return the grid snap to
   */
  public boolean getGridSnapTo() {
    return ((GridModel) model).getGridSnapTo();
  }
  
  /**
   * Gets the grid minor width.
   *
   * @return the grid minor width
   */
  public int getGridMinorWidth() {
    return ((GridModel) model).getGridMinorWidth();
  }

  /**
   * Gets the grid minor height.
   *
   * @return the grid minor height
   */
  public int getGridMinorHeight() {
    return ((GridModel) model).getGridMinorHeight();
  }

  /**
   * Gets the grid major width.
   *
   * @return the grid major width
   */
  public int getGridMajorWidth() {
    return ((GridModel) model).getGridMajorWidth();
  }

  /**
   * Gets the grid major height.
   *
   * @return the grid major height
   */
  public int getGridMajorHeight() {
    return ((GridModel) model).getGridMajorHeight();
  }

  /**
   * Gets the grid minor color.
   *
   * @return the grid minor color
   */
  public Color getGridMinorColor() {
    return ((GridModel) model).getGridMinorColor();
  }

  /**
   * Gets the grid major color.
   *
   * @return the grid major color
   */
  public Color getGridMajorColor() {
    return ((GridModel) model).getGridMajorColor();
  }

  /**
   * Write model.
   *
   * @param out
   *          the out
   */
  public void writeModel(ObjectOutputStream out) {
    try {
      out.writeObject((GridModel)model);
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
  
}
