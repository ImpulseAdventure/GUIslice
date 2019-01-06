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

/**
 * <p>
 * The Class AlignPropertyMemento implements the Memento Pattern which is
 * used in our undo framework to bring an object back to a previous state.
 * </p>
 * <p>
 * The AlignPropertyMemento works with all of our Alignment Commands,
 * Align Bottom, Top, Center, Left, Right, VSpacing, HSpacing, Height,
 * and Width.  This class only handles Integer property values.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class AlignPropertyMemento extends Memento {
  
  /** The page holding the widget(s). */
  private PagePane page;
  
  /** The group list contains the models of all the selected widgets that will be aligned. */
  private List<WidgetModel> groupList;
  
  /** The property row to be saved. */
  private int property;
  
  /** The property value(s) before alignment. */
  private List<Integer> savedList = new ArrayList<Integer>();

  /**
   * Instantiates a new align property memento.
   *
   * @param page
   *          the page holding the widget.
   * @param groupList
   *          the list all the selected widgets that will be aligned
   * @param property
   *          the property row to be saved
   */
  public AlignPropertyMemento(PagePane page, List<WidgetModel> groupList, int property) {
    this.page = page;
    this.property = property;
    this.groupList = groupList;
    for(WidgetModel m : groupList) {
      savedList.add((Integer) m.getValueAt(property, 1));
    }
  }
  
  /**
   * restore will bring a property back to its previous state.
   *
   * @see builder.mementos.Memento#restore()
   */
  @Override
  public void restore() {
    int i = 0;
    page.selectNone();
    for(WidgetModel m : groupList) {
      m.restore(savedList.get(i++), property);
      page.selectName(m.getKey());
    }
    page.refreshView();
  }

}
