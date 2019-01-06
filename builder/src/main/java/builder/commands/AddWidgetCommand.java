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
import builder.widgets.Widget;

/**
 * The Class AddWidgetCommand will
 * add a new widget to the project on a specific page.
 * 
 * @author Paul Conti
 * 
 */
public class AddWidgetCommand extends Command {
  
  /** The page that will hold the new widget. */
  PagePane page;
  
  /** The target is the new widget. */
  private Widget target=null;
  
  /**
   * Instantiates a new add the widget command and
   * creates the required Memento object for undo/redo.
   *
   * @param page
   *          the <code>page</code> object that will contain the new widget
   */
  public AddWidgetCommand(PagePane page) {
    this.page = page;
    memento = new WidgetMemento(page);
  }
  
  /**
   * Add will setup the command to add the new widget for later execution.
   *
   * @param w
   *          the <code>w</code> widget object
   */
  public void add(Widget w) {
    target = w;
  }

  /**
   * execute - calls the object that will actually run the command
   * 
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    page.addWidget(target);
  }

  /**
   * toString - converts command to string for debugging
   * 
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Add widget: " + target.getEnum());
  }

}
