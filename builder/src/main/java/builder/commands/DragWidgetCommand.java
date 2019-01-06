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

import java.awt.Point;
import java.util.ArrayList;
import java.util.List;

import builder.mementos.PositionMemento;
import builder.views.PagePane;
import builder.widgets.Widget;

/**
 * The Class DragWidgetCommand will
 * allow users to drag widgets around the design canvas.
 * 
 * @author Paul Conti
 * 
 */
public class DragWidgetCommand extends Command {
  
  /** The page that holds the selected widgets. */
  private PagePane page;
  
  /** The p is our adjusted drag point to fit to our design canvas 
   * (TFT screen simulation). Each widget gets one Point (p) object.
   */
  private Point[] p;
  
  /** The targets list contains the set of widgets to drag. */
  private List<Widget> targets = new ArrayList<Widget>();
  
  /**
   * Instantiates a new drag widget command.
   *
   * @param page
   *          the <code>page</code> object that contains the widgets to drag.
   */
  public DragWidgetCommand(PagePane page) {
    this.page = page;
  }
  
  /**
   * Start will setup the drag widgets command for later execution.
   */
  public void start() {
    targets = page.getSelectedList();
    p = new Point[targets.size()];
    memento = new PositionMemento(page, targets);
  }

  /**
   * Move will perform dragging the widgets.
   *
   * @param m
   *          the <code>m</code> is the new relative position of our dragged widgets. 
   */
  public void move(Point m) {
    for (int i=0; i<targets.size(); i++) {
      /* p[i] will be our adjusted drag position to fit to our TFT screen.
       * It does not take into account snap to grid
       */
      p[i] = targets.get(i).updateLocation(m);
    }
  }

  /**
   * Stop ends the drag and corrects p[i] for snap to grid,
   * if its enabled.
   */
  public void stop() {
    // adjust out drop point to snap to grid
    for (int i=0; i<targets.size(); i++) {
      p[i] = targets.get(i).drop(p[i]);
    }
  }

  /**
   * execute - will freeze the final drag point for each widget.
   *
   * @see builder.commands.Command#execute()
   */
  @Override
  public void execute() {
    for (int i=0; i<targets.size(); i++) {
      targets.get(i).moveBy(p[i]);
    }
  }

  /**
   * toString - converts drag command to a string for debugging.
   *
   * @see java.lang.Object#toString()
   */
  @Override
  public String toString() {
    return String.format("Drag");
  }

}
