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

import builder.controller.Controller;
import builder.mementos.PageMemento;
import builder.views.PagePane;

/**
 * The Class AddPageCommand will
 * add a new page to the project.
 * 
 * @author Paul Conti
 * 
 */
public class AddPageCommand extends Command {
  
  /** The target is the new page. */
  private PagePane target=null;
  
  /** The controller object. */
  private Controller controller=null;
  
  /**
   * Instantiates a new add the page command and
   * creates the required Memento object for undo/redo.
   *
   * @param controller
   *          the <code>controller</code> object which manages the application.
   */
  public AddPageCommand(Controller controller) {
    this.controller = controller;
    memento = new PageMemento(controller);
  }
  
  /**
   * Add will setup the addition of the new page to project command for later execution.
   *
   * @param p
   *          the <code>p</code> is the new page object
   */
  public void add(PagePane p) {
    target = p;
  }

  /**
   * execute - calls the object that will actually run the command
   * 
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    controller.addPage(target);
  }

  /**
   * toString - converts command to string for debugging
   * 
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Add page: " + target.getEnum());
  }

}
