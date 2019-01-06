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
 * The Class AlignBottomCommand will
 * align the selected widgets to the bottom most widget.
 * 
 * @author Paul Conti
 * 
 */
public class AlignBottomCommand extends Command {
  
  /** The page that holds the selected widgets. */
  private PagePane page;
  
  /** The group list contains the models of all the selected widgets that will be aligned. */
  private List<WidgetModel> groupList = new ArrayList<WidgetModel>();
  
  /** The bottom most Y value. */
  private int bottomY = -1;   // y axis value of bottom most widget
  
  /** bAlreadyAligned indicates that the widgets are already aligned. */
  private boolean bAlreadyAligned;
  
  /**
   * Instantiates a new align bottom command.
   *
   * @param page
   *          the <code>page</code> is the object that holds the widgets
   */
  public AlignBottomCommand(PagePane page) {
    this.page = page;
  }
  
  /**
   * Align will setup the align widget(s) to bottom command for later execution
   * and creates the required Memento object for undo/redo.
   *
   * @return <code>true</code>, if successful
   */
  public boolean align() {
    int margin = GeneralEditor.getInstance().getMargins();
    int height = GeneralEditor.getInstance().getHeight();
    margin = height - margin;
    // pass one finds Bottom-most widget (Highest Y value)
    List<Widget> list = page.getSelectedList();
    if (list.size() < 1) return false;
    int offsetY=0;
    for (Widget w : list) {
      WidgetModel m = w.getModel();
      offsetY = m.getY()+m.getHeight();
      if (offsetY > bottomY) {
        bottomY = offsetY;
      }
    }
    // test for already aligned and, if so, assume move to bottom margin
    // unless already there
    bAlreadyAligned = true;
    for (Widget w : list) {
      WidgetModel m = w.getModel();
      offsetY = m.getY()+m.getHeight();
      if (offsetY != bottomY) {
        bAlreadyAligned = false;
        break;
      }
    }
    if (bAlreadyAligned) {
      if (bottomY == margin) { // error out, can't move any further
        JOptionPane.showMessageDialog(null, 
            "The widgets are already aligned.",
            "Warning",
            JOptionPane.WARNING_MESSAGE);
      } else {
        bottomY = margin;
      }
    }
    // This pass will add to groupList selected widgets that will use the new Y value
    for (Widget w : list) {
      WidgetModel m = w.getModel();
      offsetY = m.getY()+m.getHeight();
      if(offsetY <= bottomY) {
        groupList.add(m);
      }   
    }
    // did we find any matches?
    if (groupList.size() < 1) {
      JOptionPane.showMessageDialog(null, 
          "This command requires at least one widget be selected.",
          "Warning",
          JOptionPane.WARNING_MESSAGE);
      return false;
    }
    memento = new AlignPropertyMemento(page, groupList, WidgetModel.PROP_Y);
    return true;  // success
  }

  /**
   * execute - will actually run the command
   * 
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    int offsetY = 0;
    for(WidgetModel m : groupList) {
      offsetY = bottomY - m.getHeight();
      m.changeValueAt(Integer.valueOf(offsetY), WidgetModel.PROP_Y);
    }
  }

  /**
   * toString - converts command to string for debugging
   * 
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Align Bottom: " + bottomY);
  }

}
