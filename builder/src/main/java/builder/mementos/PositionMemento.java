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

import java.util.ArrayList;
import java.util.List;

import builder.models.WidgetModel;
import builder.views.PagePane;
import builder.widgets.Widget;

/**
 * <p>
 * The Class PositionMemento implements the Memento Pattern which is
 * used in our undo framework to bring an object back to a previous state.
 * </p>
 * The PositionMemento works with the Drag Widget command.
 */
public class PositionMemento extends Memento {
  
  /** The list of widgets being moved. */
  private List<Widget> list = new ArrayList<Widget>();;
  
  /** The saved X coordinate list. */
  private List<Integer> savedXList = new ArrayList<Integer>();
  
  /** The saved Y coordinate list. */
  private List<Integer> savedYList = new ArrayList<Integer>();
  
  /** The page. */
  private PagePane page;
  
  /**
   * Instantiates a new position memento.
   *
   * @param page
   *          the page containing the moving widgets.
   * @param list
   *          the list of widgets being moved.
   */
  public PositionMemento(PagePane page, List<Widget> list) {
    this.page = page;
    this.list = list;
    for (Widget w : list) {
      savedXList.add((Integer) w.getModel().getValueAt(WidgetModel.PROP_X, 1));
      savedYList.add((Integer) w.getModel().getValueAt(WidgetModel.PROP_Y, 1));
    }
  }
  
  /**
   * restore
   *
   * @see builder.mementos.Memento#restore()
   */
  @Override
  public void restore() {
    int i = 0;
    for (Widget w : list) {
      w.getModel().restore(savedXList.get(i), WidgetModel.PROP_X);
      w.getModel().restore(savedYList.get(i), WidgetModel.PROP_Y);
      i++;
    }
    page.refreshView();
  }

}
