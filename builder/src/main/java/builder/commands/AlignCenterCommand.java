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
import builder.prefs.GeneralEditor;
import builder.views.PagePane;
import builder.widgets.Widget;

/**
 * The Class AlignCenterCommand will 
 * center the selected widget horizontally along the X axis.
 * 
 * @author Paul Conti
 * 
 */
public class AlignCenterCommand extends Command {
  
  /** The page that holds the selected widgets. */
  private PagePane page;
  
  /** The group list contains the model of the selected widget.
   * It's a list in case support for centering multiple widgets 
   * is added in future. */
  private List<WidgetModel> groupList = new ArrayList<WidgetModel>();
  
  /** The new centered X value. */
  private int newX = 0;   // value of X for widget
  
  /**
   * Instantiates a new align center command.
   *
   * @param page
   *          the <code>page</code> is the object that holds the widgets
   */
  public AlignCenterCommand(PagePane page) {
    this.page = page;
  }
  
  /**
   * Align will setup the align center command for later execution
   * and creates the required Memento object for undo/redo.
   *
   * @return <code>true</code>, if successful
   */
  public boolean align() {
    // This routine allows one or more selected widgets
    List<Widget> list = page.getSelectedList();
    if (list.size() < 1) {
      JOptionPane.showMessageDialog(null, 
          "This command requires one widget to be selected.",
          "Warning",
          JOptionPane.WARNING_MESSAGE);
      return false; // need at least one widget
    }
    if (list.size() > 1) {
      JOptionPane.showMessageDialog(null, 
          "You can only select one widget to center.",
          "Warning",
          JOptionPane.WARNING_MESSAGE);
      return false; // need at least one widget
    }
    int displayWidth = GeneralEditor.getInstance().getWidth();
    WidgetModel model = list.get(0).getModel();
    groupList.add(model);
    memento = new AlignPropertyMemento(page, groupList, WidgetModel.PROP_X);
    newX = (displayWidth - model.getWidth()) / 2;
    return true;  // success
  }

  /**
   * execute - will actually run the command
   *
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    WidgetModel m = null;
    for(int i=0; i<groupList.size(); i++) {
      m = groupList.get(i);
      m.changeValueAt(Integer.valueOf(newX), WidgetModel.PROP_X);
    }
  }

  /**
   * toString - converts command to string for debugging
   *
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Align Center: " + newX);
  }

}
