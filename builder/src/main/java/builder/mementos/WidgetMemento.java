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
import builder.views.PagePane;
import builder.views.TreeView;

/**
 * <p>
 * The Class WidgetMemento implements the Memento Pattern which is
 * used in our undo framework to bring an object back to a previous state.
 * </p>
 * The WidgetMemento works with the Add and Del widget commands.
 * 
 * @author Paul Conti
 * 
 */
public class WidgetMemento extends Memento {
  
  /** The page that contains or will contain the widgets. */
  private PagePane page;
  
  /** The page backup. */
  private String page_backup;
  
  /** The tree backup. */
  private String tree_backup;
  
  /** The enum backup. */
  private String enum_backup;

  /**
   * Instantiates a new widget memento.
   *
   * @param page
   *          the page 
   */
  public WidgetMemento(PagePane page) {
    this.page = page;
    this.page_backup = page.backup();
    this.tree_backup = TreeView.getInstance().backup();
    this.enum_backup = EnumFactory.getInstance().backup();
  }

  /**
   * Instantiates a new widget memento.
   *
   * @param page
   *          the page
   * @param tree_backup
   *          the tree backup
   */
  public WidgetMemento(PagePane page, String tree_backup) {
    this.page = page;
    this.page_backup = page.backup();
    this.tree_backup = tree_backup;
    this.enum_backup = EnumFactory.getInstance().backup();
  }

  /**
   * restore
   *
   * @see builder.mementos.Memento#restore()
   */
  @Override
  public void restore() {
    page.restore(page_backup);
    TreeView.getInstance().restore(tree_backup);
    EnumFactory.getInstance().restore(enum_backup);
    Controller.getInstance().refreshView();
  }

}
