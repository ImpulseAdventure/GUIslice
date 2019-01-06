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

import builder.controller.Controller;
import builder.models.WidgetModel;

/**
 * <p>
 * The Class PropertyMemento implements the Memento Pattern which is
 * used in our undo framework to bring an object back to a previous state.
 * </p>
 * The PropertyMemento works with the Property command.
 * 
 * @author Paul Conti
 * 
 */
public class PropertyMemento extends Memento {
  
  /** The widget model containing the property to be changed. */
  private WidgetModel model;
  
  /** The saved property value. */
  private Object savedValue;
  
  /** The property row within the widget model. */
  private int row;

  /**
   * Instantiates a new property memento.
   *
   * @param model
   *          the model widget model containing the property to be changed.
   * @param row
   *          the property row within the widget model.
   */
  public PropertyMemento(WidgetModel model, int row) {
    this.model = model;
    this.row = row;
    this.savedValue = model.backup(row);
  }

  /**
   * restore
   *
   * @see builder.mementos.Memento#restore()
   */
  @Override
  public void restore() {
    model.restore(savedValue, row);
    Controller.getInstance().refreshView();
  }

}
