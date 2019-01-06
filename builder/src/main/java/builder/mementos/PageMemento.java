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
package builder.mementos;

import builder.common.EnumFactory;
import builder.controller.Controller;
import builder.views.TreeView;

/**
 * <p>
 * The Class PageMemento implements the Memento Pattern which is used
 * in our undo framework to bring an object back to a previous state.
 * The PageMemento works with our AddPageCommand, and DelPageCommand.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class PageMemento extends Memento {
  
  /** The controller. */
  private Controller controller;
  
  /** The controller backup. */
  private String controller_backup;
  
  /** The tree backup. */
  private String tree_backup;
  
  /** The enum backup. */
  private String enum_backup;

  /**
   * Instantiates a new page memento.
   *
   * @param controller
   *          the our controller object
   */
  public PageMemento(Controller controller) {
    this.controller = controller;
    this.controller_backup = this.controller.backup();
    this.tree_backup = TreeView.getInstance().backup();
    this.enum_backup = EnumFactory.getInstance().backup();
  }

  /**
   * restore
   *
   * @see builder.mementos.Memento#restore()
   */
  @Override
  public void restore() {
    controller.restore(controller_backup);
    TreeView.getInstance().restore(tree_backup);
    EnumFactory.getInstance().restore(enum_backup);
    Controller.getInstance().refreshView();
  }

}
