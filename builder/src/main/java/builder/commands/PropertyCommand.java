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

import builder.mementos.PropertyMemento;
import builder.models.WidgetModel;

/**
 * The Class PropertyCommand will update a widget property.
 * This class is used by our widget models when a user changes
 * a property value directly inside the JTable views.
 * 
 * @author Paul Conti
 * 
 */
public class PropertyCommand extends Command {
  
  /** The widget model. */
  private WidgetModel model;
  
  /** The property row within the model. */
  private int row;
  
  /** The new value of the property. */
  private Object newValue;
  
  /**
   * Instantiates a new property command
   * and creates the required Memento object for undo/redo.
   *
   * @param model
   *          the widget model
   * @param newValue
   *          the new value
   * @param row
   *          the property row within the widget model
   */
  public PropertyCommand(WidgetModel model, Object newValue, int row) {
    this.model = model;
    this.row = row;
    this.newValue = newValue;
    memento = new PropertyMemento(model, row);
  }
  
  /**
   * execute - update the property to a new value.
   *
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    model.changeValueAt(newValue, row);
  }

  /**
   * toString - convert property command to a string for debugging.
   *
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("model: " + model.getKey() + " Property Change: " 
        + newValue.toString() + " row=" + row);
  }

}
