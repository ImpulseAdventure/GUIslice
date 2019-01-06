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

import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

/**
 * <p>
 * The listener interface for receiving deleteKey events. The class that is
 * interested in processing a deleteKey event implements this interface, and the
 * object created with that class is registered with a component using the
 * component's <code>addDeleteKeyListener</code> method. When the deleteKey event
 * occurs, that object's appropriate method is invoked.
 *</p>
 *
 * @author Paul Conti
 *
 */
public class DeleteKeyListener implements KeyListener {

  /**
   * keyTyped
   *
   * @see java.awt.event.KeyListener#keyTyped(java.awt.event.KeyEvent)
   */
  @Override
  public void keyTyped(KeyEvent e) {
    // TODO Auto-generated method stub
  }

  /**
   * keyPressed
   *
   * @see java.awt.event.KeyListener#keyPressed(java.awt.event.KeyEvent)
   */
  @Override
  public void keyPressed(KeyEvent e) {
    if((KeyEvent.VK_DELETE == e.getKeyChar()) ||
       (KeyEvent.VK_D == e.getKeyCode()  && 
       ((e.getModifiersEx() & InputEvent.CTRL_DOWN_MASK) == InputEvent.CTRL_DOWN_MASK))) {
      MsgEvent ev = new MsgEvent();
      ev.message = "";
      ev.parent = "";
      ev.code = MsgEvent.DELETE_KEY;
      MsgBoard.getInstance().publish(ev);
    }
  }

  /**
   * keyReleased
   *
   * @see java.awt.event.KeyListener#keyReleased(java.awt.event.KeyEvent)
   */
  @Override
  public void keyReleased(KeyEvent e) {
    // TODO Auto-generated method stub

  }

}
