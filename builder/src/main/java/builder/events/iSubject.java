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
 * The Interface iSubject is for the subject part of the Observer Pattern.
 * <p>
 * Subject is an object having methods to attach and detach observers  
 * to itself.  Observers register themselves with the Subject,
 * while the Subject broadcasts events to all registered Observers.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public interface iSubject {
  
  /**
   * Subscribe.
   *
   * @param subscriber
   *          the subscriber
   */
  public void subscribe(iSubscriber subscriber);

  /**
   * Removes the.
   *
   * @param subscriber
   *          the subscriber
   */
  public void remove(iSubscriber subscriber);

  /**
   * Notify subscribers.
   *
   * @param e
   *          the e
   */
  public void notifySubscribers(MsgEvent e);
}
