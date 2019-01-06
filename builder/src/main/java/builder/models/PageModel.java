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
package builder.models;

import builder.common.EnumFactory;

/**
 * The Class PageModel implements the model for the Page widget.
 * 
 * @author Paul Conti
 * 
 */
public class PageModel extends WidgetModel {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /**
   * Instantiates a new page model.
   */
  public PageModel() {
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.PAGE;
    data = new Object[2][5];

    initProp(PROP_KEY, String.class, "COM-001", Boolean.TRUE,"Key",widgetType);
    initProp(PROP_ENUM, String.class, "COM-002", Boolean.FALSE,"ENUM",widgetType);
  }
  
  /**
   * isCellEditable
   *
   * @see builder.models.WidgetModel#isCellEditable(int, int)
   */
  @Override
  public boolean isCellEditable(int row, int col) {
    if (col == 0 || row == 0) {
      return false;
    } 
    return true;
 }

}
