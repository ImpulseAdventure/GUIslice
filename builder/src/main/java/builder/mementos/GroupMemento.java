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

import builder.common.EnumFactory;
import builder.models.CheckBoxModel;
import builder.models.RadioButtonModel;
import builder.models.WidgetModel;
import builder.views.PagePane;

/**
 * <p>
 * The Class GroupMemento implements the Memento Pattern which is
 * used in our undo framework to bring an object back to a previous state.
 * </p>
 * <p>
 * The GroupMemento works with the Group Radio Buttons command.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class GroupMemento extends Memento {
  
  /** The page holding the widget(s). */
  private PagePane page;
  
  /** The group list contains the models of all the selected widgets that will be grouped. */
  private List<WidgetModel> groupList;
  
  /** The enum backup. */
  private String enum_backup;
  
  /** The saved property values list. */
  private List<String> savedList = new ArrayList<String>();

  /**
   * Instantiates a new group memento.
   *
   * @param page
   *          the page holding the selected widgets 
   * @param groupList
   *          the group list containing the models of the selected widgets
   */
  public GroupMemento(PagePane page, List<WidgetModel> groupList) {
    this.page = page;
    this.enum_backup = EnumFactory.getInstance().backup();
    this.groupList = groupList;
    for(WidgetModel m : groupList) {
      if (m.getType().equals(EnumFactory.RADIOBUTTON))
        savedList.add((String) m.getValueAt(RadioButtonModel.PROP_GROUP, 1));
      else
        savedList.add((String) m.getValueAt(CheckBoxModel.PROP_GROUP, 1));
    }
  }
  
  /**
   * restore
   *
   * @see builder.mementos.Memento#restore()
   */
  @Override
  public void restore() {
    EnumFactory.getInstance().restore(enum_backup);
    int i = 0;
    for(WidgetModel m : groupList) {
      if (m.getType().equals(EnumFactory.RADIOBUTTON))
        m.restore(savedList.get(i++), RadioButtonModel.PROP_GROUP);
      else
        m.restore(savedList.get(i++), CheckBoxModel.PROP_GROUP);
      page.selectName(m.getKey());
    }
    page.refreshView();
  }

}
