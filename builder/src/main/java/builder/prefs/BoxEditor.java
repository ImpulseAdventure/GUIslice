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
import java.io.PrintStream;
import java.util.prefs.Preferences;

import builder.common.EnumFactory;
import builder.models.BoxModel;

/**
 * The Class BoxEditor manages user preferences for the BOX widget.
 * 
 * @author Paul Conti
 * 
 */
public class BoxEditor extends ModelEditor {
  
  /** The Constant MY_NODE. */
  public static final String MY_NODE = "com/impulseadventure/builder/box";
  
  /** The Constant TITLE. */
  private static final String TITLE = EnumFactory.BOX;

  /** The instance. */
  private static BoxEditor instance = null;
  
  /**
   * Gets the single instance of BoxEditor.
   *
   * @return single instance of BoxEditor
   */
  public static synchronized BoxEditor getInstance()  {
      if (instance == null) {
          instance = new BoxEditor();
      }
      return instance;
  }  
  
  /**
   * Instantiates a new box editor.
   */
  public BoxEditor() {
    // get rid of the bugged Preferences warning - not needed in Java 9 and above
    System.setErr(new PrintStream(new OutputStream() {
        public void write(int b) throws IOException {}
    }));
    fPrefs = Preferences.userRoot().node(MY_NODE);
    model = new BoxModel();
    model.TurnOffEvents();
    updateModel();
    System.setErr(System.err);  
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
  
}
