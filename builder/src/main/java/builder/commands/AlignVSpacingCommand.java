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
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import javax.swing.JOptionPane;

import builder.mementos.AlignPropertyMemento;
import builder.models.WidgetModel;
import builder.prefs.GeneralEditor;
import builder.views.PagePane;
import builder.widgets.Widget;

/**
 * The Class AlignVSpacingCommand will
 * align the selected widgets to a uniform vertical spacing.
 * 
 * @author Paul Conti
 * 
 */
public class AlignVSpacingCommand extends Command {
  
  /** The page that holds the selected widgets. */
  private PagePane page;
  
  /** The group list contains the models of all the selected widgets that will be aligned. */
  private List<WidgetModel> groupList = new ArrayList<WidgetModel>();
  
  /** The space Y value holds the spacing between widgets along Y axis. */
  private int spaceY = 0;   // spacing between widgets along Y axis
  
  /**
   * Instantiates a new align V spacing command.
   *
   * @param page
   *          the <code>page</code> is the object that holds the widgets
   */
  public AlignVSpacingCommand(PagePane page) {
    this.page = page;
  }
  
  /**
   * Align will setup the align vertical spacing command for later execution
   * and creates the required Memento object for undo/redo.
   *
   * @return <code>true</code>, if successful
   */
  public boolean align() {
    // no point to this routine unless you selected more than one widget
    List<Widget> list = page.getSelectedList();
    if (list.size() < 2) return false;
    spaceY = GeneralEditor.getInstance().getVSpacing();
    String spacing = (String) JOptionPane.showInputDialog(null,
        "Enter vertical spacing in pixels",
        "Enter spacing", JOptionPane.PLAIN_MESSAGE, null, null, String.valueOf(spaceY));
    try {
      if (spacing == null) // operation cancelled
        return true;
      spaceY = Integer.parseInt(spacing);
    } catch (NumberFormatException nfExc) {
      JOptionPane.showConfirmDialog(null, "Input must be number! You entered text. Please try again!", "Error!",
          JOptionPane.PLAIN_MESSAGE);
      return false;
    }

    // we need to sort our selected list in X axis order
    Collections.sort(list, new Comparator<Widget>() {
        public int compare(Widget one, Widget other) {
            return (((Integer) one.getModel().getValueAt(WidgetModel.PROP_Y,1))
                .compareTo((Integer) other.getModel().getValueAt(WidgetModel.PROP_Y,1)));
        }
    }); 
    
    // this loop will simply add sorted list's models to groupList
    for (Widget w : list) {
      WidgetModel m = w.getModel();
        groupList.add(m);
    }
    memento = new AlignPropertyMemento(page, groupList, WidgetModel.PROP_X);
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
    WidgetModel previous = null;
    int y = 0;
    for(int i=0; i<groupList.size(); i++) {
      m = groupList.get(i);
      if (i > 0) {
        y = previous.getY() + previous.getHeight() + spaceY;
        m.changeValueAt(Integer.valueOf(y), WidgetModel.PROP_Y);
      }
      previous = m;
    }
  }

  /**
   * toString - converts command to string for debugging
   *
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Align Horizontal Spacing: " + spaceY);
  }

}
