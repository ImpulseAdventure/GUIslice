package builder.controller;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JOptionPane;

//import builder.common.MetaIds;
import builder.models.WidgetModel;
import builder.parser.ApiNode;
import builder.parser.Parser;
import builder.views.PagePane;
import builder.widgets.Widget;
import builder.widgets.WidgetFactory;

public class Importer {

  /** The instance. */
  private static Importer instance = null;
  
  /**
   * Gets the single instance of Importer.
   *
   * @return single instance of Importer
   */
  public static synchronized Importer getInstance() {
    if (instance == null) {
      instance = new Importer();
    }
    return instance;
  }
  
  /**
   * Instantiates a new Importer.
   */
  public Importer() {
  }
  
  public void doImport(File file) {
    String fileName = file.getAbsolutePath();
    String logName = fileName + ".log";
    List<ApiNode> nodeList = null;
    List<String> errorList = new ArrayList<String>();
    List<ApiNode> pageList = new ArrayList<ApiNode>();
    // do the parsing of our import file
    try {
      // Acquire the GUIslice API parser for importing a file
      Parser parser = Parser.getInstance();
      nodeList = parser.parse(fileName, errorList);
    } catch (IOException e) {
      errorList.add(e.getMessage());
    }
    // output a log file detailing successes and failures
    try {
      File newFile = new File(fileName + ".orig");
      file.renameTo(newFile);
      PrintStream writer = new PrintStream(logName, "UTF-8");
      writer.println("---------------------------------------------\n");
      writer.println("Successful import of these UI Widgets\n");
      for (ApiNode api : nodeList) {
        // save our page nodes for the actual import
        if (api.getType() == ApiNode.PAGE)
          pageList.add(api);
        writer.println(api.toString());
      }
      writer.println("\n---------------------------------------------\n");
      if (errorList.size() > 0) {
        writer.println("Number of Parsing Errors: " + errorList.size());
        writer.println("\n");
        for (String s : errorList) {
          writer.println(s);
        }
      } else {
        writer.println("No Parsing Errors");
      }
      writer.close();
    } catch (IOException e1) {
      e1.printStackTrace();
    }
    // first check to see if we found anything
    if (pageList.size() > 0) {
      // now do the actual import, one page at a time
      // we assume here that the previous project was closed and 
      // a new unnamed one has been created with Page$1
      Controller controller = Controller.getInstance();
      PagePane p = null;
      Widget w = null;
      boolean bFirst=true;
      for (ApiNode pn : pageList) {
        if (bFirst) {
          p = controller.getFirstPage(pn.getEnum());
          bFirst = false;
        } else {
          p = controller.createPage(pn.getEnum());
        }
        for (ApiNode wn : nodeList) {
          if (wn.getType() != ApiNode.PAGE) {
            if (wn.getPageEnum().equals(pn.getEnum())) {
              w = WidgetFactory.getInstance().createWidget(wn.getWidgetType(),0,0);
              // now loop through the node and add any properties we have found
              addProperties(wn, w);
              p.addWidget(w);
            }
          }        
        }
      }
      controller.changePage(pageList.get(0).getEnum());
    }
    // give the user the good or bad news
    String msg = String.format("File '%s'\nRenamed To '%s'\nImport status logged to:\n '%s'\nNumber Of Errors Found %d", 
        fileName, fileName+".orig",logName,errorList.size());
    JOptionPane.showMessageDialog(null, msg, "Information", JOptionPane.INFORMATION_MESSAGE);
  }
  
  /**
   *
   */
  public void addProperties(ApiNode n, Widget w) {
    Object[][] nData = n.getData();
    Object[][] wData = w.getModel().getData();
    int nRows = n.getRowCount();
    String metaID = null;
    Object objectData = null;
    int mapped_row;
//    System.out.println("WM rows: " + nRows);
    // loop through the node and copy properties
    for (int i = 0; i < nRows; i++) {
      metaID = n.getMetaId(i);
      objectData = nData[i][ApiNode.PROP_VAL_VALUE];
      // did we find a property value?
      if (objectData != null) {
        mapped_row = mapMetaIDtoProperty(w, metaID);
        // check for obsolete property
        if (mapped_row >= 0) {
          wData[mapped_row][WidgetModel.PROP_VAL_VALUE] = objectData;
/*
          System.out.println(MetaIds.getInstance().getName(metaID) + ": " +
                    wData[mapped_row][WidgetModel.PROP_VAL_VALUE].toString() +
                    " mapped to row " + mapped_row);
*/
        }
      }        
    } 
  }
  
  /**
   * mapMetaIDtoProperty
   * 
   * @param metaID the id assigned that must never change over the life time of the builder
   * @return row that matches metaID, otherwise a -1 on no matching ID
   */
  public int mapMetaIDtoProperty(Widget w, String metaID) {
    Object[][] data = w.getModel().getData();
    for (int i=0; i<data.length; i++) {
      if (metaID.equals((String)data[i][WidgetModel.PROP_VAL_ID])) {
        return i;
      }
    }
    return -1;
  }
}
