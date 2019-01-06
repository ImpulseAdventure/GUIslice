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
 * The Class AlignWidthCommand.
 * align the selected widgets to a uniform width.
 * 
 * @author Paul Conti
 * 
 */
public class AlignWidthCommand extends Command {
  
  /** The page that holds the selected widgets. */
  private PagePane page;
  
  /** The group list contains the models of all the selected widgets that will be aligned. */
  private List<WidgetModel> groupList = new ArrayList<WidgetModel>();
  
  /** The new width value. */
  private int newWidth = 0;
  
  /**
   * Instantiates a new align width command.
   *
   * @param page
   *          the <code>page</code> is the object that holds the widgets
   */
  public AlignWidthCommand(PagePane page) {
    this.page = page;
  }
  
  /**
   * Align will setup the align width command for later execution
   * and creates the required Memento object for undo/redo.
   *
   * @return <code>true</code>, if successful
   */
  public boolean align() {
    for (Widget w : page.getSelectedList()) {
      WidgetModel m = w.getModel();
      groupList.add(m);
      if (m.getWidth() > newWidth) {
        newWidth = m.getWidth();
      }
    }
    if (groupList.size() < 2) return false;
    String spacing = (String) JOptionPane.showInputDialog(null,
        "Enter width in pixels",
        "Enter width", JOptionPane.PLAIN_MESSAGE, null, null, String.valueOf(newWidth));
    try {
      if (spacing == null) // operation cancelled
        return true;
      newWidth = Integer.parseInt(spacing);
    } catch (NumberFormatException nfExc) {
      JOptionPane.showConfirmDialog(null, "Input must be number! You entered text. Please try again!", "Error!",
          JOptionPane.PLAIN_MESSAGE);
      return false;
    }
    memento = new AlignPropertyMemento(page, groupList, WidgetModel.PROP_WIDTH);
    return true;  // success
  }

  /**
   * execute - will actually run the command
   *
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    Integer width = Integer.valueOf(newWidth);
    for(WidgetModel m : groupList) {
      if(m.isCellEditable(WidgetModel.PROP_WIDTH, 1)) {
        m.changeValueAt(width, WidgetModel.PROP_WIDTH);
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
    return String.format("Align by Width: " + newWidth);
  }

}
