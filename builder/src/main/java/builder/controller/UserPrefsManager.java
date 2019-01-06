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
package builder.controller;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.LayoutManager;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Base64;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;

import builder.prefs.ModelEditor;

/**
 * The Class UserPrefsManager handles managing the UI layout views  
 * for all of the user preferences model editors.
 * 
 * @author Paul Conti
 * 
 */
public class UserPrefsManager {

  /** The frame. */
  private JFrame frame;
  
  /** The dialog. */
  private JDialog dialog;
  
  /** The JTabbedPane. */
  private JTabbedPane jTabbedPane;
  
  /** The preference editors. */
  private java.util.List<ModelEditor> prefEditors;
  
  /** The backup data. */
  private String backupData;
  
  /** The title. */
  private String title;
  
  /** The colors. */
  static Color colors[] = { Color.RED, Color.ORANGE, Color.YELLOW, Color.GREEN, Color.BLUE,
      Color.MAGENTA };

//  private Editor fEditor;

  /**
 * Instantiates a new user preferences manager.
 *
 * @param frame
 *          the a frame
 * @param prefEditors
 *          the a preference editors
 */
public UserPrefsManager(JFrame frame, List<ModelEditor> prefEditors) {
    this.frame = frame;
    this.prefEditors=prefEditors;
    dialog = null;
    initUI();
  }
  
  /**
   * Initializes the UI.
   */
  private void initUI() {
    title = "Edit Preferences";
    jTabbedPane = new JTabbedPane();
    jTabbedPane.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
    jTabbedPane.setTabPlacement(JTabbedPane.TOP);
    for(ModelEditor prefEditor: prefEditors) {
      JComponent editorGui = prefEditor.getUI();
      editorGui.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
      jTabbedPane.addTab(prefEditor.getTitle() , editorGui);
    }

  }
  
  /**
   * Show dialog.
   */
  public void showDialog() {
    backupData = backup();
    dialog = new JDialog(frame, title, true);
    dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
    dialog.setResizable(false);
//    System.out.println("showDialog title: " + title);
    
    JPanel layout = new JPanel();
    layout.setLayout(new BoxLayout(layout, BoxLayout.Y_AXIS));
    layout.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
    JPanel content = new JPanel();
    content.setLayout(new BoxLayout(content, BoxLayout.Y_AXIS));
    layout.add(jTabbedPane);

    layout.add(getButtons());

    dialog.getContentPane().add(layout);

    dialog.setPreferredSize(new Dimension(500,400));
    dialog.pack();
    dialog.setLocationRelativeTo(null);
    dialog.setVisible(true);
  }
  
  /**
   * Gets the button row.
   *
   * @return the button row
   */
  private JComponent getButtons() {
    JButton ok = new JButton("OK");
    ok.addActionListener( new ActionListener() {
      @Override public void actionPerformed(ActionEvent event) {
        okAction();
      }
    });
    dialog.getRootPane().setDefaultButton( ok );
    JButton cancel = new JButton("Cancel");
    cancel.addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent event) {
        closeDialog();
      }
    });
    JPanel panel = new JPanel();
    LayoutManager layout = new BoxLayout(panel, BoxLayout.X_AXIS);
    panel.setLayout(layout);
    panel.setBorder(BorderFactory.createEmptyBorder(15, 0, 0, 0));
    panel.add(Box.createHorizontalGlue());
    panel.add(ok);
    panel.add(Box.createHorizontalStrut(5));
    panel.add(cancel);
    return panel;
  }

  /**
   * Ok action.
   */
  public void okAction() {
    saveSettings();
    dialog.dispose();
  }
  
  /**
   * Close dialog.
   */
  private void closeDialog(){
    restore(backupData);
    dialog.dispose();
  }

  /**
   * Save settings.
   */
  //Called only when the user hits the OK button.
  private void saveSettings(){
    for(ModelEditor prefEditor: prefEditors) {
      prefEditor.savePreferences();
    }
  }

  /**
   * Backup.
   *
   * @return the <code>string</code> object
   */
  public String backup() {
    try {
      ByteArrayOutputStream baos = new ByteArrayOutputStream();
      ObjectOutputStream out = new ObjectOutputStream(baos);
      for (ModelEditor editor : prefEditors) {
        editor.getModel().writeModel(out);
      }
      out.close();
      baos.close();
      return Base64.getEncoder().encodeToString(baos.toByteArray());
    } catch (IOException e) {
      System.out.print("IOException occurred." + e.toString());
      e.printStackTrace();
      return "";
    }
  }

  /**
   * Restore.
   *
   * @param state
   *          the state
   */
  public void restore(String state) {
    try {
      byte[] data = Base64.getDecoder().decode(state);
      ObjectInputStream in = new ObjectInputStream(new ByteArrayInputStream(data));
      for (ModelEditor editor : prefEditors) {
        editor.getModel().readModel(in, null);
      }
      in.close();
    } catch (ClassNotFoundException e) {
      System.out.print("ClassNotFoundException occurred." + e.toString());
      e.printStackTrace();
    } catch (IOException e) {
      System.out.print("IOException occurred." + e.toString());
      e.printStackTrace();
    }
  }
}
