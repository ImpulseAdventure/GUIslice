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

import builder.mementos.Memento;

/**
 * The Class Command.
 * Base class for the Command Pattern.
 * <p>
 * The pattern intends to encapsulate in an object all the data required 
 * for performing a given action (command), including what method to call, 
 * the method’s arguments, and the object to which the method belongs.
 * </p>
 * <p>
 * However, instead of following this pattern exactly, it will be extended
 * by storing Memento objects that will contain data for undo'ing and
 * redo'ing commands.
 * </p>
 * <p>
 * This model allows us to decouple objects that produce the commands
 * from their consumers, and will give users the ability to undo and redo
 * operations.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class Command {
  
  /** The memento object. */
  Memento memento;

  /**
   * Execute method.
   */
  public void execute() {
    
  }
  
  /**
   * Restore method.
   */
  public void restore() {
    memento.restore();
  }
  
}
