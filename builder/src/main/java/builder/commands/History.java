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

import java.util.LinkedList;

import javax.swing.JOptionPane;

import builder.views.ToolBar;

/**
 * The Class History implements undo and redo functionality.
 * <p>
 * To make commands undo-/redoable We maintain two stacks, one for for commands
 * available for undo and one for commands available for redo.
 * </p>  
 * <p>
 * Every command executed is required to call History.push() with the command in question.  
 * This command is then placed on our undo stack and we clear the redo stack.
 * </p>  
 * <p>  
 * Each Command Object will have one internal Memento Object. The memento will have saved 
 * the “original” state of any objects required to restore the project back before  
 * a command was executed. Thus, to undo any command we simply pop it off the
 * undo stack and call command.restore() method which is actually implemented 
 * by the internal memento object held inside the command.  This will cause all 
 * objects involved to rollback to they're previous state.
 * </p>  
 * <p>
 * Redo is supported simply by pushing a command onto the redo stack on each undo
 * operation.  If a redo is asked for then a command is pop'ed off the redo
 * stack and the command is told to execute().  This works because each command
 * object is required to store any data needed for execution inside the command object, 
 * before changing any objects by the execute() method.
 * </p>  
 * 
 * @author Paul Conti
 * 
 */
public class History {
  
  /** The undo stack. */
  private LinkedList<Command> undoStack = new LinkedList<Command>();
  
  /** The redo stack. */
  private LinkedList<Command> redoStack = new LinkedList<Command>();

  /** The instance. */
  private static History instance = null;
  
  /** The ToolBar. */
  private static ToolBar toolBar;

  /**
   * Gets the single instance of History.
   *
   * @return single instance of History
   */
  public static synchronized History getInstance() {
    if (instance == null) {
      instance = new History();
      toolBar = ToolBar.getInstance();
    }
    return instance;
  }

  /**
   * size() gets the current number of commands on our undo stack
   * This is used by our controller to determine if we need
   * to ask our user if they want to save their project before exiting.
   * 
   * @return current number of commands on our undo stack
   */
  public int size() {
    return undoStack.size();
  }
  
  /**
   * clearHistory will be called by our controller when we close,
   * or save projects.  We don't want undo/redo between projects.
   */
  public void clearHistory() {
    undoStack.clear();
    redoStack.clear();
    toolBar.disableUndo();
    toolBar.disableRedo();
  }
  
  /**
   * push a command to save on our undo stack.
   *
   * @param c
   *          <code>c</code> is the command object
   */
  public void push(Command c) {
    undoStack.push(c);
    toolBar.enableUndo();
    redoStack.clear();
    toolBar.disableRedo();
  }
  
  /**
   * undo the latest command saved on our undo stack.
   *
   * @return <code>true</code> on success
   */
  public boolean undo() {
    if (undoStack.size() == 0) {
//  With the Undo button grayed out this shouldn't happen
      JOptionPane.showMessageDialog(null, 
          "Nothing to undo",
          "Information",
          JOptionPane.INFORMATION_MESSAGE);
      toolBar.disableUndo();
      return false;
    }
    Command c = undoStack.pop();
    if (undoStack.size() == 0) toolBar.disableUndo();
    if (c == null) {
      return false;
    }
    redoStack.push(c);
    toolBar.enableRedo();
//    System.out.println("Undoing: " + c.toString());
    c.restore();
    return true;
  }

  /**
   * redo the last command we rolled back from our undo stack.
   * 
   * @return <code>true</code> on success
   */
  public boolean redo() {
    if (redoStack.size() == 0) {
//  With the Redo button grayed out this shouldn't happen
      JOptionPane.showMessageDialog(null, 
          "Nothing to redo",
          "Information",
          JOptionPane.INFORMATION_MESSAGE);
      toolBar.disableRedo();
      return false;
    }
    Command c = redoStack.pop();
    if (redoStack.size() == 0) toolBar.disableRedo();
    if (c == null) {
      return false;
    }
//    System.out.println("Redoing: " + c.toString());
    undoStack.push(c);
    toolBar.enableUndo();
    c.restore();
    c.execute();
    return true;
  }
}
