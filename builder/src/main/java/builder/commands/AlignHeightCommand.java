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
package builder.commands;

import java.util.ArrayList;
import java.util.List;

import javax.swing.JOptionPane;

import builder.mementos.AlignPropertyMemento;
import builder.models.WidgetModel;
import builder.views.PagePane;
import builder.widgets.Widget;

/**
 * The Class AlignHeightCommand will
 * align the selected widgets to a uniform height.
 * 
 * @author Paul Conti
 * 
 */
public class AlignHeightCommand extends Command {
  
  /** The page that holds the selected widgets. */
  private PagePane page;
  
  /** The group list contains the models of all the selected widgets that will be aligned. */
  private List<WidgetModel> groupList = new ArrayList<WidgetModel>();
  
  /** The new height value. */
  private int newHeight = 0;
  
  /**
   * Instantiates a new align height command.
   *
   * @param page
   *          the <code>page</code> is the object that holds the widgets
   */
  public AlignHeightCommand(PagePane page) {
    this.page = page;
  }
  
  /**
   * Align will setup the align height command for later execution
   * and creates the required Memento object for undo/redo.
   *
   * @return true, if successful
   */
  public boolean align() {
    for (Widget w : page.getSelectedList()) {
      WidgetModel m = w.getModel();
      groupList.add(m);
      if (m.getHeight() > newHeight) {
        newHeight = m.getHeight();
      }
    }
    if (groupList.size() < 2) return false;
    String spacing = (String) JOptionPane.showInputDialog(null,
        "Enter height in pixels",
        "Enter height", JOptionPane.PLAIN_MESSAGE, null, null, String.valueOf(newHeight));
    try {
      if (spacing == null) // operation cancelled
        return true;
      newHeight = Integer.parseInt(spacing);
    } catch (NumberFormatException nfExc) {
      JOptionPane.showConfirmDialog(null, "Input must be number! You entered text. Please try again!", "Error!",
          JOptionPane.PLAIN_MESSAGE);
      return false;
    }
    memento = new AlignPropertyMemento(page, groupList, WidgetModel.PROP_HEIGHT);
    return true;  // success
  }

  /**
   * execute - will actually run the command
   *
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    Integer height = Integer.valueOf(newHeight);
    for(WidgetModel m : groupList) {
      if(m.isCellEditable(WidgetModel.PROP_HEIGHT, 1)) {
        m.changeValueAt(height, WidgetModel.PROP_HEIGHT);
      }
    }
  }

  /**
   * toString - converts command to string for debugging
   *
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Align by Height: " + newHeight);
  }

}
