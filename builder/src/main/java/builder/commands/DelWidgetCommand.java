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

import java.util.List;

import builder.mementos.WidgetMemento;
import builder.views.PagePane;
import builder.widgets.Widget;

/**
 * The Class DelWidgetCommand will
 * delete the widget from the project.
 * 
 * @author Paul Conti
 * 
 */
public class DelWidgetCommand extends Command {
  
  /** The page that holds the selected widgets. */
  PagePane page;
  
  /** The group list contains a copy of the selected widgets that will be deleted. */
  List<Widget> groupList;
  
  /**
   * Instantiates a new del widget command and
   * creates the required Memento object for undo/redo.
   *
   * @param page
   *          the <code>page</code> object that contains the selected widgets.
   */
  public DelWidgetCommand(PagePane page) {
    this.page = page;
    memento = new WidgetMemento(page);
  }
  
  /**
   * Del will setup the delete widget(s) command.
   *
   * @return <code>true</code>, if successful
   */
  public boolean del() {
    groupList = page.getSelectedList();
    if (groupList.size() < 1) return false;
    return true;
  }

  /**
   * execute - perform theactual deleteion of the widget from the project.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 s                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
   *
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    for (Widget w : groupList) {
      page.delWidget(w);
    }
  }

  /**
   * toString
   *
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Delete widget: ");
  }

}
