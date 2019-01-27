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
package builder.views;

import java.awt.event.ActionListener;

import javax.swing.ImageIcon;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JSeparator;

import builder.Builder;

/**
 * The Class MenuBar.
 * 
 * @author Paul Conti
 * 
 */
public class MenuBar extends JMenuBar {
  
  /** The Constant serialVersionUID. */
  private static final long serialVersionUID = 1L;
  
  /** The mb help. */
  JMenu mbFile, mbEdit, mbHelp;
  
  /** The exit menu item. */
  private JMenuItem newMenuItem, openMenuItem, saveMenuItem, 
    saveAsMenuItem, importMenuItem, codeMenuItem, closeMenuItem, exitMenuItem;
  
  /** The delete menu item. */
  private JMenuItem optionsMenuItem, deleteMenuItem;
  
  /** The about menu item. */
  private JMenuItem aboutMenuItem;
  
  /**
   * Instantiates a new menu bar.
   */
  public MenuBar() {
    initFileMenus();
    initEditMenus();
    initHelp();
  }
  
  /**
   * Initializes the file menus.
   */
  public void initFileMenus() {
    mbFile = new JMenu("File");
    
    newMenuItem = new JMenuItem("New", 
        new ImageIcon(Builder.class.getResource("/resources/icons/new.png")));
    newMenuItem.setActionCommand("new");
    mbFile.add(newMenuItem);
    
    openMenuItem = new JMenuItem("Open", 
        new ImageIcon(Builder.class.getResource("/resources/icons/open.png")));
    openMenuItem.setActionCommand("open");
    mbFile.add(openMenuItem);
    
    closeMenuItem = new JMenuItem("Close", 
        new ImageIcon(Builder.class.getResource("/resources/icons/close.png")));
    closeMenuItem.setActionCommand("close");
    mbFile.add(closeMenuItem);
    
    saveMenuItem = new JMenuItem("Save", 
        new ImageIcon(Builder.class.getResource("/resources/icons/save.png")));
    saveMenuItem.setActionCommand("save");
    mbFile.add(saveMenuItem);
    
    saveAsMenuItem = new JMenuItem("Save As...", 
        new ImageIcon(Builder.class.getResource("/resources/icons/saveas.png")));
    saveAsMenuItem.setActionCommand("saveas");
    mbFile.add(saveAsMenuItem);
    
    mbFile.add(new JSeparator()); 
    
    importMenuItem = new JMenuItem("Import", 
        new ImageIcon(Builder.class.getResource("/resources/icons/import.png")));
    importMenuItem.setActionCommand("import");
    mbFile.add(importMenuItem);
    
    codeMenuItem = new JMenuItem("Generate Code", 
        new ImageIcon(Builder.class.getResource("/resources/icons/export.png")));
    codeMenuItem.setActionCommand("code");
    mbFile.add(codeMenuItem);
    
    mbFile.add(new JSeparator()); 
    
    exitMenuItem = new JMenuItem("Exit", 
        new ImageIcon(Builder.class.getResource("/resources/icons/logout.png")));
    exitMenuItem.setActionCommand("exit");
    mbFile.add(exitMenuItem);
    
    this.add(mbFile);
  }
  
  /**
   * Initializes the edit menus.
   */
  public void initEditMenus() {
    mbEdit = new JMenu("Edit");
    
    deleteMenuItem = new JMenuItem("Delete", 
        new ImageIcon(Builder.class.getResource("/resources/icons/delete.png")));
    deleteMenuItem.setActionCommand("delete");
    mbEdit.add(deleteMenuItem);
    
    optionsMenuItem = new JMenuItem("Options", 
        new ImageIcon(Builder.class.getResource("/resources/icons/options.png")));
    optionsMenuItem.setActionCommand("Options");
    mbEdit.add(optionsMenuItem);
    
    this.add(mbEdit);
  }

  /**
   * Initializes the help.
   */
  public void initHelp() {
    mbHelp = new JMenu("Help");
    
    aboutMenuItem= new JMenuItem("About", 
        new ImageIcon(Builder.class.getResource("/resources/icons/about.png")));
    aboutMenuItem.setActionCommand("about");
    mbHelp.add(aboutMenuItem);
    
    this.add(mbHelp);
  }
  
  /**
   * Adds the listeners.
   *
   * @param al
   *          the object that implements ActionListener
   */
  public void addListeners(ActionListener al)
  {
    newMenuItem.addActionListener(al);
    openMenuItem.addActionListener(al); 
    saveMenuItem.addActionListener(al);
    saveAsMenuItem.addActionListener(al);
    importMenuItem.addActionListener(al);
    codeMenuItem.addActionListener(al);
    deleteMenuItem.addActionListener(al); 
    optionsMenuItem.addActionListener(al); 
    aboutMenuItem.addActionListener(al);
    closeMenuItem.addActionListener(al);
    exitMenuItem.addActionListener(al);
  }
  
}
