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

import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;

/**
 * The Class MsgBoard implements the Observer Pattern.
 * <p>
 * The Observer Pattern specifies communication between objects: 
 * observable and observers. An observable is an object which notifies 
 * observers about the changes in its state.
 * </p>
 * <p>
 * MsgBoard is an example of an observable object and state changes are 
 * recorded inside an MsgEvent object.
 * </p>
 * 
 * @author Paul Conti
 * 
 */
public class MsgBoard implements iSubject {
  
  /** The subscribers. */
  private static List<iSubscriber> subscribers = new ArrayList<iSubscriber>();

  /** The instance. */
  private static MsgBoard instance = null;
  
  /**
   * Gets the single instance of MsgBoard.
   *
   * @return single instance of MsgBoard
   */
  public static synchronized MsgBoard getInstance()  {
      if (instance == null) {
          instance = new MsgBoard();
      }
      return instance;
  }  

  /**
   * Instantiates a new messsage board.
   */
  public MsgBoard() {

  }

  /**
   * Publish.
   *
   * @param event
   *          the event
   */
  public void publish(MsgEvent event) {
    notifySubscribers(event);
  }
  
  /**
   * subscribe
   *
   * @see builder.events.iSubject#subscribe(builder.events.iSubscriber)
   */
  @Override
  public void subscribe(iSubscriber subscriber) {
    subscribers.add(subscriber);
  }

  /**
   * remove
   *
   * @see builder.events.iSubject#remove(builder.events.iSubscriber)
   */
  @Override
  public void remove(iSubscriber subscriber) {
    subscribers.remove(subscriber);
  }

  /**
   * notifySubscribers
   *
   */
  @Override
  public void notifySubscribers(MsgEvent e) {
    synchronized(subscribers) {
      ListIterator<iSubscriber> litr = subscribers.listIterator();
  
      while(litr.hasNext()){
  //      System.out.println("Notifying Observers on event");
        litr.next().updateEvent(e);
      }
    }
  }

}
