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

import java.awt.Color;

import javax.swing.table.TableCellEditor;

import builder.common.EnumFactory;

/**
 * The Class GridModel implements the model for the Grid on our design canvas.
 * 
 * @author Paul Conti
 * 
 */
public class GridModel extends WidgetModel {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The Constant GRID. */
  public static final int GRID              = 1;
  
  /** The Constant GRID_SNAP_TO. */
  public static final int GRID_SNAP_TO      = 2;
  
  /** The Constant GRID_WIDTH. */
  public static final int GRID_WIDTH        = 3;
  
  /** The Constant GRID_HEIGHT. */
  public static final int GRID_HEIGHT       = 4;
  
  /** The Constant GRID_MAJOR_WIDTH. */
  public static final int GRID_MAJOR_WIDTH  = 5;
  
  /** The Constant GRID_MAJOR_HEIGHT. */
  public static final int GRID_MAJOR_HEIGHT = 6;
  
  /** The Constant GRID_MAJOR_COLOR. */
  public static final int GRID_MAJOR_COLOR  = 7;
  
  /** The Constant GRID_MINOR_COLOR. */
  public static final int GRID_MINOR_COLOR  = 8;
  
  /**
   * Instantiates a new grid model.
   */
  public GridModel() {
    initProperties();
  }
  
  /**
   * Initializes the properties.
   */
  protected void initProperties()
  {
    widgetType = EnumFactory.GRID;
    data = new Object[9][5];

    initProp(PROP_KEY, String.class, "COM-001", Boolean.TRUE,"Key",widgetType);
    initProp(GRID, Boolean.class, "GRID-100", Boolean.FALSE,"Grid",Boolean.TRUE);
    initProp(GRID_SNAP_TO, Boolean.class, "GRID-101", Boolean.FALSE,"Grid Snap To",Boolean.TRUE);
    initProp(GRID_WIDTH, Integer.class, "GRID-102", Boolean.FALSE,"Grid Minor Width",Integer.valueOf(10));
    initProp(GRID_HEIGHT, Integer.class, "GRID-103", Boolean.FALSE,"Grid Minor Height",Integer.valueOf(10));
    initProp(GRID_MAJOR_WIDTH, Integer.class, "GRID-104", Boolean.FALSE,"Grid Major Width",Integer.valueOf(50));
    initProp(GRID_MAJOR_HEIGHT, Integer.class, "GRID-105", Boolean.FALSE,"Grid Major Height",Integer.valueOf(50));

    initProp(GRID_MAJOR_COLOR, Color.class, "COL-311", Boolean.FALSE,"Grid Major Color", new Color(0,0,255));
    initProp(GRID_MINOR_COLOR, Color.class, "COL-312", Boolean.FALSE,"Grid Minor Color",new Color(0,0,192));
    
  }
  
  /**
   * getEditorAt
   *
   * @see builder.models.WidgetModel#getEditorAt(int)
   */
  @Override
  public TableCellEditor getEditorAt(int rowIndex) {
    return null;
  }

  /**
   * Toggle grid.
   */
  public void toggleGrid() {
    if (getGrid()) {
      data[GRID][PROP_VAL_VALUE] = Boolean.FALSE;
      data[GRID_SNAP_TO][PROP_VAL_VALUE] = Boolean.FALSE;
    } else {
      data[GRID][PROP_VAL_VALUE] = Boolean.TRUE;
      data[GRID_SNAP_TO][PROP_VAL_VALUE] = Boolean.TRUE;
    }
  }
  
  /**
   * Gets the grid.
   *
   * @return the grid
   */
  public boolean getGrid() {
    return (((Boolean) data[GRID][PROP_VAL_VALUE]).booleanValue());
  }
  
  /**
   * Gets the grid snap to.
   *
   * @return the grid snap to
   */
  public boolean getGridSnapTo() {
    return (((Boolean) data[GRID_SNAP_TO][PROP_VAL_VALUE]).booleanValue());
  }
  
  /**
   * Gets the grid minor width.
   *
   * @return the grid minor width
   */
  public int getGridMinorWidth() {
    return (((Integer) (data[GRID_WIDTH][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the grid minor height.
   *
   * @return the grid minor height
   */
  public int getGridMinorHeight() {
    return (((Integer) (data[GRID_HEIGHT][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the grid major width.
   *
   * @return the grid major width
   */
  public int getGridMajorWidth() {
    return (((Integer) (data[GRID_MAJOR_WIDTH][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the grid major height.
   *
   * @return the grid major height
   */
  public int getGridMajorHeight() {
    return (((Integer) (data[GRID_MAJOR_HEIGHT][PROP_VAL_VALUE])).intValue());
  }

  /**
   * Gets the grid minor color.
   *
   * @return the grid minor color
   */
  public Color getGridMinorColor() {
    return (((Color) data[GRID_MINOR_COLOR][PROP_VAL_VALUE]));
  }

  /**
   * Gets the grid major color.
   *
   * @return the grid major color
   */
  public Color getGridMajorColor() {
    return (((Color) data[GRID_MAJOR_COLOR][PROP_VAL_VALUE]));
  }

  /**
   * isCellEditable
   *
   * @see builder.models.WidgetModel#isCellEditable(int, int)
   */
  @Override
  public boolean isCellEditable(int row, int col) {
    if (col == 0 || row == 0)
      return false;
    return true;
  }

}
