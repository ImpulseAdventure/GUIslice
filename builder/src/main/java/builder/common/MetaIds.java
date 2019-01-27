/**
 *
 * The MIT License
 *
 * Copyright 2019 Paul Conti
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
package builder.common;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import builder.Builder;

/**
 * A mapping class for GUIslice Widget properties
 * 
 * @author Paul Conti
 * 
 */
public class MetaIds {
  
  /** The Constant ARDUINO_FONT_TEMPLATE. */
  public  final static String METAIDS_TEMPLATE  = "/resources/templates/meta_ids.csv";
  
  /** The instance. */
  private static MetaIds instance = null;
  
  /** The meta id list. */
  private static List<String> metaidList = new ArrayList<String>();
  
  /** The meta id map. */
  private static HashMap<String, Integer> metaidMap = new HashMap<String, Integer>();
  
  /**
   * Gets the single instance of MetaIds.
   *
   * @return single instance of MetaIds
   */
  public static synchronized MetaIds getInstance() {
    if (instance == null) {
      instance = new MetaIds();
      metaidMap = new HashMap<String, Integer>(128);
      instance.readMetaIds();

    }
    return instance;
  }

  /**
   * Instantiates a new MetaIds object.
   */
  public MetaIds() {
  }

  /**
   * Gets the display name of the meta id
   *
   * @param metaid is the id to lookup
   * @return the String name 
   */
  public String getName(String metaid) {
    Integer idx = Integer.valueOf(0);  // always return something...
      if (metaidMap.containsKey(metaid)) {
        idx = metaidMap.get(metaid);
        return metaidList.get(idx.intValue());
      }
      return new String("UnKnown<" + metaid +">");
  }

  /**
   * Read Arduino fonts.
   *
   */
  public void readMetaIds() {
    String line = "";
    String cvsSplitBy = ",";
    String metaid = null;
    String name = null;
    int i=0;
    try {
      InputStream in = Builder.class.getResourceAsStream(METAIDS_TEMPLATE);
      InputStreamReader ir = new InputStreamReader(in);
      if (in == null) {
        System.out.println("missing resource: " + METAIDS_TEMPLATE);
        return;
      }
      BufferedReader br = new BufferedReader(ir);
      while ((line = br.readLine()) != null) {
        if (!line.startsWith("#")) {
          // use comma as separator
          String[] f = line.split(cvsSplitBy);
          metaid = f[0];
          name = f[1];  // display name
          metaidMap.put(metaid, Integer.valueOf(i));
          metaidList.add(name);
          i++;
        }
      }
      br.close();
      ir.close();
      in.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
  
}
