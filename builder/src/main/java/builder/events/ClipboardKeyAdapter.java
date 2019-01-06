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
package builder.events;

import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.StringSelection;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import javax.swing.JOptionPane;
import javax.swing.JTable;

/**
 * ClipboardKeyAdapter class supports copy and paste to/from the system
 * clipboard to JTables.  While its possible to extend this class to 
 * multiple rows and columns, I've chosen to only support a single cell 
 * at a time.
 * 
 * @author Paul Conti
 *
 */
public class ClipboardKeyAdapter extends KeyAdapter {

  /** The Constant LINE_BREAK. */
  private static final String LINE_BREAK = "\r";
  
  /** The Constant CELL_BREAK. */
  private static final String CELL_BREAK = "\t";
  
  /** The clipboard. */
  private Clipboard CLIPBOARD = Toolkit.getDefaultToolkit().getSystemClipboard();

  /** The table. */
  private final JTable table;

  /**
   * Instantiates a new clipboard key adapter.
   *
   * @param table
   *          the table
   */
  public ClipboardKeyAdapter(JTable table) {
    this.table = table;
  }

  /**
   * keyReleased.
   *
   * @param event
   *          the event
   * @see java.awt.event.KeyAdapter#keyReleased(java.awt.event.KeyEvent)
   */
  @Override
  public void keyReleased(KeyEvent event) {
//    System.out.println(event.toString());
    if (event.isControlDown()) {
      if (event.getKeyCode() == KeyEvent.VK_C) { // Copy
        cancelEditing();
        copyToClipboard(false);
      } else if (event.getKeyCode() == KeyEvent.VK_X) { // Cut
        cancelEditing();
        copyToClipboard(true);
      } else if (event.getKeyCode() == KeyEvent.VK_V) { // Paste
        cancelEditing();
        pasteFromClipboard();
      }
    }
  }

  /**
   * Copy to clipboard.
   *
   * @param isCut
   *          the is cut
   */
  private void copyToClipboard(boolean isCut) {
    int numCols = table.getSelectedColumnCount();
    int numRows = table.getSelectedRowCount();
    int[] rowsSelected = table.getSelectedRows();
    int[] colsSelected = table.getSelectedColumns();
    if (numRows > 1 || numCols > 1) {
      JOptionPane.showMessageDialog(null, "Error", "Select one cell only", JOptionPane.ERROR_MESSAGE);
      return;
    }

    StringBuffer transferStr = new StringBuffer();
    transferStr.append(escape(table.getValueAt(rowsSelected[0], colsSelected[0])));
    if (isCut) {
      table.setValueAt(null, rowsSelected[0], colsSelected[0]);
    }
    transferStr.append(CELL_BREAK);
    transferStr.append(LINE_BREAK);
    StringSelection sel = new StringSelection(transferStr.toString());
    CLIPBOARD.setContents(sel, sel);
  }

  /**
   * Paste from clipboard.
   */
  private void pasteFromClipboard() {
    int row = table.getSelectedRows()[0];
    int col = table.getSelectedColumns()[0];

    String pasteString = "";
    try {
      pasteString = (String) (CLIPBOARD.getContents(this).getTransferData(DataFlavor.stringFlavor));
    } catch (Exception e) {
      JOptionPane.showMessageDialog(null, "Error", "Paste Text Only", JOptionPane.ERROR_MESSAGE);
      return;
    }

    String[] lines = pasteString.split(LINE_BREAK);
    if (lines.length > 1) {
      JOptionPane.showMessageDialog(null, "Error", "Paste one cell's value only", JOptionPane.ERROR_MESSAGE);
    }
    
    String[] cells = lines[0].split(CELL_BREAK);
    if (cells.length > 1) {
      JOptionPane.showMessageDialog(null, "Error", "Paste one cell's value only", JOptionPane.ERROR_MESSAGE);
    }
    table.setValueAt(cells[0], row, col);
  }

  /**
   * Cancel editing.
   */
  private void cancelEditing() {
    if (table.getCellEditor() != null) {
      table.getCellEditor().cancelCellEditing();
    }
  }

  /**
   * Escape.
   *
   * @param cell
   *          the cell
   * @return the <code>string</code> object
   */
  private String escape(Object cell) {
    return cell.toString().replace(LINE_BREAK, " ").replace(CELL_BREAK, " ");
  }
}
