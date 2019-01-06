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

import builder.mementos.WidgetMemento;
import builder.views.PagePane;

/**
 * The Class ChangeZOrderCommand will
 * allow users to rearrange the order of widget display so
 * that a widget doesn't cover and block out a smaller widget.
 * 
 * @author Paul Conti
 * 
 */
public class ChangeZOrderCommand extends Command {
  
  /** The page that holds the selected widget. */
  PagePane page;
  
  /** The widget's enum. */
  String widgetEnum;
  
  /** The from index position. */
  int fromIdx;
  
  /** The to index position. */
  int toIdx;
    
  /**
   * Instantiates a new change Z order command and
   * creates the required Memento object for undo/redo.
   *
   * @param page
   *          the page that contains the widget
   * @param tree_backup
   *          the tree view backup
   */
  public ChangeZOrderCommand(PagePane page, String tree_backup) {
    this.page = page;
    memento = new WidgetMemento(page, tree_backup);
  }
  
  /**
   * Change will setup the change Z order command for later execution.
   *
   * @param widgetEnum
   *          the widget enum
   * @param fromIdx
   *          the from index position.
   * @param toIdx
   *          the to index position.
   * @return <code>true</code>, if successful
   */
  public boolean change(String widgetEnum, int fromIdx, int toIdx) {
    this.widgetEnum = widgetEnum;
    this.fromIdx = fromIdx;
    this.toIdx = toIdx;
    return true;
  }

  /**
   * execute - calls the object that will actually run the command
   * 
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    page.changeZOrder(widgetEnum, fromIdx, toIdx);
  }

  /**
   * toString - converts command to string for debugging
   *
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Change ZOrder: " + widgetEnum);
  }

}
