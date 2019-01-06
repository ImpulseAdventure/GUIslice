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
import builder.models.TxtButtonModel;

/**
 * The Class TxtButtonEditor manages user preferences for the Text Button widget.
 * 
 * @author Paul Conti
 * 
 */
public class TxtButtonEditor extends ModelEditor {
  
  /** The Constant MY_NODE. */
  public static final String MY_NODE = "com/impulseadventure/builder/txtbutton";
  
  /** The Constant TITLE. */
  private static final String TITLE = EnumFactory.TEXTBUTTON;

  /** The instance. */
  private static TxtButtonEditor instance = null;
  
  /**
   * Gets the single instance of TxtButtonEditor.
   *
   * @return single instance of TxtButtonEditor
   */
  public static synchronized TxtButtonEditor getInstance()  {
      if (instance == null) {
          instance = new TxtButtonEditor();
      }
      return instance;
  }  
  
  /**
   * Instantiates a new txt button editor.
   */
  public TxtButtonEditor() {
 // get rid of the bugged Preferences warning - not needed in Java 9 and above
    System.setErr(new PrintStream(new OutputStream() {
        public void write(int b) throws IOException {}
    }));
    fPrefs = Preferences.userRoot().node(MY_NODE);
    model = new TxtButtonModel();
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
