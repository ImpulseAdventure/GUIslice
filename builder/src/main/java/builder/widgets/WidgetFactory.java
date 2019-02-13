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
package builder.widgets;

import builder.common.EnumFactory;
import builder.models.BoxModel;
import builder.models.CheckBoxModel;
import builder.models.GraphModel;
import builder.models.ImageModel;
import builder.models.ImgButtonModel;
import builder.models.ProgressBarModel;
import builder.models.RadioButtonModel;
import builder.models.SliderModel;
import builder.models.TextBoxModel;
import builder.models.TextModel;
import builder.models.TxtButtonModel;
import builder.models.WidgetModel;
import builder.widgets.Widget;

/**
 * A factory for creating Widget objects.
 * 
 * @author Paul Conti
 * 
 */
public class WidgetFactory  {
  
  /** The instance. */
  private static WidgetFactory instance = null;
  
  /**
   * Gets the single instance of WidgetFactory.
   *
   * @return single instance of WidgetFactory
   */
  public static synchronized WidgetFactory getInstance()  {
      if (instance == null) {
          instance = new WidgetFactory();
      }
      return instance;
  }  

  /**
   * Instantiates a new widget factory.
   */
  public WidgetFactory()
  {
  }

  /**
   * Creates a new Widget object.
   *
   * @param widgetType
   *          the widget type
   * @param x
   *          the x coordinate position
   * @param y
   *          the y coordinate position
   * @return the widget
   */
  public Widget createWidget(String widgetType, int x, int y)
  {
//    System.out.println("WF create: " + widgetType + " X=" + x +" Y=" + y);
    Widget widget=null;
    WidgetModel model=null;
    String strKey = "";
    int n = 0;
    String strCount = ""; 
    String ref = "";    
    switch(widgetType) {
    case "Box":
      widget = new BoxWidget(x, y);
      model = (BoxModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
      }
      break;
    case "CheckBox":
      widget = new CheckBoxWidget(x, y);
      model = (CheckBoxModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
      }
      break;
    case "Graph":
      widget = new GraphWidget(x, y);
      model = (GraphModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
        ref = GraphModel.ELEMENTREF_NAME;
        strKey = model.getKey();
        n = strKey.indexOf("$");
        strCount = strKey.substring(n+1, strKey.length());
        ref = ref + strCount;
        ((GraphModel) model).setElementRef(ref);
      }
      break;
    case "Image":
      widget = new ImageWidget();
      model = (ImageModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
      }
      break;
    case "ImageButton":
      widget = new ImgButtonWidget();
      model = (ImgButtonModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
      }
      break;
    case "ProgressBar":
      widget = new ProgressBarWidget(x, y);
      model = (ProgressBarModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
        ref = ProgressBarModel.ELEMENTREF_NAME;
        strKey = model.getKey();
        n = strKey.indexOf("$");
        strCount = strKey.substring(n+1, strKey.length());
        ref = ref + strCount;
        ((ProgressBarModel) model).setElementRef(ref);
      }
      break;
    case "RadioButton":
      widget = new RadioButtonWidget(x, y);
      model = (RadioButtonModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
      }
      break;
    case "Slider":
      widget = new SliderWidget(x, y);
      model = (SliderModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
        ref = SliderModel.ELEMENTREF_NAME;
        strKey = model.getKey();
        n = strKey.indexOf("$");
        strCount = strKey.substring(n+1, strKey.length());
        ref = ref + strCount;
        ((SliderModel) model).setElementRef(ref);
      }
      break;
    case "Text":
      widget = new TextWidget(x, y);
      model = (TextModel) widget.getModel();
      if (widget != null) {
//        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
      }
      break;
    case "TextBox":
      widget = new TextBoxWidget(x, y);
      model = (TextBoxModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
        ref = TextBoxModel.ELEMENTREF_NAME;
        strKey = model.getKey();
        n = strKey.indexOf("$");
        strCount = strKey.substring(n+1, strKey.length());
        ref = ref + strCount;
        ((TextBoxModel) model).setElementRef(ref);
      }
      break;
    case "TextButton":
      widget = new TxtButtonWidget(x, y);
      model = (TxtButtonModel) widget.getModel();
      if (widget != null) {
        model.setKey(EnumFactory.getInstance().createKey(widgetType));
        model.setEnum(EnumFactory.getInstance().createEnum(widgetType));
      }
      break;
    default:
      break;
    }
    if (widget != null) {
      model.TurnOnEvents();
    }
    return widget;
  }
}
