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
package builder.prefs;

import java.awt.Color;

/* Remove import Observable for Java 9 and above */
import java.util.Observable;
import java.util.prefs.Preferences;

import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.table.DefaultTableCellRenderer;

import builder.events.ClipboardKeyAdapter;
import builder.models.WidgetModel;
import builder.tables.ColorEditor;
import builder.tables.ColorRenderer;
import builder.tables.FontEditor;
import builder.tables.MultiClassTable;
import builder.tables.NonEditableCellRenderer;
import builder.tables.SelectAllCellEditor;

/**
 * The Class ModelEditor is the base class for modifying user preferences.
 * 
 * @author Paul Conti
 * 
 */
/* Remove Observable for Java 9 and above */
public class ModelEditor extends Observable {

  /** The scroll pane. */
  private JScrollPane scrollPane;
  
  /** The table. */
  private MultiClassTable table;
  
  /** The prefs. */
  Preferences fPrefs;
  
  /** The model. */
  WidgetModel model;
  
  /** The save data. */
  Object[][] save_data;
  
  /**
   * Gets the java swing ui elements
   *
   * @return the ui
   */
  public JComponent getUI() {
    table = new MultiClassTable(model);
    
    table.setPreferredScrollableViewportSize(table.getPreferredSize());

    // Set up renderer and editor for our Color cells.
    table.setDefaultRenderer(Color.class, new ColorRenderer(true));
    table.setDefaultEditor(Color.class, new ColorEditor());

    // Set up our gray out non editable cells renderer.
    table.setDefaultRenderer(String.class, new NonEditableCellRenderer());
    DefaultTableCellRenderer IntegerNonEditableCellRenderer = new NonEditableCellRenderer();
    IntegerNonEditableCellRenderer.setHorizontalAlignment( JLabel.CENTER );
    table.setDefaultRenderer(Integer.class, IntegerNonEditableCellRenderer);

    // Setup editor for our Font cells.
    table.setDefaultEditor(JTextField.class, new FontEditor());

    // Setup editor for Integer cells.
    table.setDefaultEditor(Integer.class, new SelectAllCellEditor());

    // set our preferred column widths
    table.getColumnModel().getColumn(0).setPreferredWidth(100);
    table.getColumnModel().getColumn(1).setPreferredWidth(120);
    table.setAutoResizeMode(JTable.AUTO_RESIZE_LAST_COLUMN);

    // restrict selections to one cell of our table
    table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    table.setRowSelectionAllowed(true);
    table.setColumnSelectionAllowed(true);
    
    // disable user column dragging
    table.getTableHeader().setReorderingAllowed(false);

    // set keyboard listener for copy and paste
    table.addKeyListener(new ClipboardKeyAdapter(table));
    
    scrollPane = new JScrollPane(table);
    return scrollPane;
  }
  
  /**
   * Gets the title.
   *
   * @return the title
   */
  public String getTitle() {
    return null;
  }
  
  /**
   * Gets the model.
   *
   * @return the model
   */
  public WidgetModel getModel() {
    return model;
  }

  /**
   * Save preferences.
   */
  public void savePreferences(){
    int rows = model.getPropertyCount();
    for (int i=0; i<rows; i++) {
      String key = (String) model.getValueAt(i, WidgetModel.COLUMN_NAME);
      Object o = model.getValueAt(i, WidgetModel.COLUMN_VALUE);
      if(o instanceof String) {
        fPrefs.put(key, (String)o);
      } else if(o instanceof Integer) {
        int def = ((Integer)o).intValue();
        fPrefs.putInt(key, def);
      } else if(o instanceof Boolean) {
        boolean def = ((Boolean)o).booleanValue();
        fPrefs.putBoolean(key, def);
      } else if (o instanceof Color) {
        int def = ((Color)o).getRGB();
        fPrefs.putInt(key, def);
      }
    }
    /* remove setChanged() and notifyObservers() for Java 9 and above */
    setChanged();
    notifyObservers();
  }

  /**
   * Save data.
   */
  public void saveData(){
    int rows = model.getPropertyCount();
    Object[][] data = model.getData();
    save_data = new Object[rows][2];
    for (int i=0; i<rows; i++) {
      save_data[i][0] = data[i][WidgetModel.PROP_VAL_ID];
      save_data[i][1] = data[i][WidgetModel.PROP_VAL_VALUE];
    }
  }

  /**
   * Restore data.
   */
  public void restoreData(){
    int rows = model.getPropertyCount();
    Object[][] data = model.getData();
    for (int i=0; i<rows; i++) {
      if (save_data[i][0].equals(data[i][WidgetModel.PROP_VAL_ID])) {
        data[i][WidgetModel.PROP_VAL_VALUE] = save_data[i][1];
      }
    }
  }

  /**
   * Update model.
   */
  public void updateModel() {
    model.TurnOffEvents();
    int rows = model.getPropertyCount();
    for (int i=0; i<rows; i++) {
      String key = (String) model.getValueAt(i, WidgetModel.COLUMN_NAME);
      Object o = model.getValueAt(i, WidgetModel.COLUMN_VALUE);
      if(o instanceof String) {
        model.changeValueAt(fPrefs.get(key, (String)o), i);
      } else if(o instanceof Integer) {
        int def = ((Integer)o).intValue();
        int value = fPrefs.getInt(key, def);
        if (value != def)
          model.changeValueAt(Integer.valueOf(value), i);
      } else if(o instanceof Boolean) {
        boolean def = ((Boolean)o).booleanValue();
        boolean value = fPrefs.getBoolean(key, def);
        if (value != def)
          model.changeValueAt(Boolean.valueOf(value), i);
      } else if (o instanceof Color) {
        int def = ((Color)o).getRGB();
        int value = fPrefs.getInt(key, def);
        if (value != def)
          model.changeValueAt(new Color(value), i);
      }
    }
    model.TurnOnEvents();
  }
  
}
