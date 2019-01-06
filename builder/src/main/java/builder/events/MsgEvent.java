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

/**
 * The Class MsgEvent is part of the Observer Pattern.
 * <p>
 * MsgEvent holds the object's state change that has been reported.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class MsgEvent {
  
  /** The Constant OBJECT_SELECTED_PAGEPANE. */
  static final public int OBJECT_SELECTED_PAGEPANE = 0;
  
  /** The Constant OBJECT_SELECTED_TREEVIEW. */
  static final public int OBJECT_SELECTED_TREEVIEW = 1;
  
  /** The Constant OBJECT_UNSELECT_PAGEPANE. */
  static final public int OBJECT_UNSELECT_PAGEPANE = 2;
  
  /** The Constant OBJECT_UNSELECT_TREEVIEW. */
  static final public int OBJECT_UNSELECT_TREEVIEW = 3;
  
  /** The Constant DELETE_KEY. */
  static final public int DELETE_KEY               = 4;
  
  /** The Constant WIDGET_DELETE. */
  static final public int WIDGET_DELETE            = 5;
  
  /** The Constant WIDGET_CHANGE_ZORDER. */
  static final public int WIDGET_CHANGE_ZORDER     = 6;
  
  /** The Constant WIDGET_REPAINT. */
  static final public int WIDGET_REPAINT           = 7;
  
  /** The Constant CANVAS_MODEL_CHANGE. */
  static final public int CANVAS_MODEL_CHANGE      = 8;
 
  /** The code contains one of the MsgEvent constants. */
  public int    code;
  
  /** The message varies but most often will be the Widget's enum that has changed state. */
  public String message;
  
  /** The parent refers to the Page enum that contains the widget 
   * or null if its a page itself. 
   */
  public String parent;
  
  /** The from index position is used when changing Z order of the widget. */
  public int    fromIdx;
  
  /** The to index position is used when changing Z order of the widget. */
  public int    toIdx;
  
  /** The x position of the widget. */
  public int    x;
  
  /** The y position of the widget. */
  public int    y;
  
  /**
   * Instantiates a new message event.
   */
  public MsgEvent() {
    
  }
  
  /**
   * toString - converts a MsgEvent to a string for debugging.
   *
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    switch(code) {
      case OBJECT_SELECTED_PAGEPANE:
        return String.format("OBJECT_SELECTED_PAGEPANE Enum: " + message);
      case OBJECT_SELECTED_TREEVIEW:
        return String.format("OBJECT_SELECTED_TREEVIEW Enum: " + message);
      case OBJECT_UNSELECT_PAGEPANE:
        return String.format("OBJECT_UNSELECT_PAGEPANE");
      case OBJECT_UNSELECT_TREEVIEW:
        return String.format("OBJECT_UNSELECT_TREEVIEW");
      case DELETE_KEY:
        return String.format("KEY_DELETE");
      case WIDGET_DELETE:
        return String.format("WIDGET_DELETE");
      case WIDGET_CHANGE_ZORDER:
        return String.format("WIDGET_CHANGE_ZORDER Enum: " + message + "Parent: " + parent 
          + " from: " + fromIdx + " to: " + toIdx);
      case WIDGET_REPAINT:
        return String.format("WIDGET_REPAINT Enum: " + message);
      case CANVAS_MODEL_CHANGE:
        return new String("CANVAS_MODEL_CHANGE");
      default:
        return new String("Invalid code");
    }
  }
}
