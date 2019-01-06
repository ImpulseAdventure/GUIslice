package builder.tables;

import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.Color;

import javax.swing.JComponent;
import javax.swing.JSlider;
import javax.swing.plaf.basic.BasicSliderUI;

/**
 * <p>
 * ColoredThumbSliderUI A Custom Java L{@literal &}F implementation 
 * of SliderUI with a Colored thumb control.
 * </p>
 * <p>
 * This code has circulated on various forums for at least 12 years.
 * I have no idea who is the original author.
 * </p>
 *
 */
public class ColoredThumbSliderUI extends BasicSliderUI {
  /** Slider <code>JSlider</code>. */
  private JSlider slider;

  /** thumb color <code>Color</code>. */
  private Color thumbColor;

  /**
   * Instantiates a new colored thumb slider UI.
   *
   * @param slider
   *          the slider
   * @param color
   *          the color
   */
  public ColoredThumbSliderUI(JSlider slider, Color color) {
    super(slider);
    this.slider = slider;
    thumbColor = color;
  }

  /**
   * paint
   *
   * @see javax.swing.plaf.basic.BasicSliderUI#paint(java.awt.Graphics, javax.swing.JComponent)
   */
  @Override
  public void paint(Graphics g, JComponent c) {
    recalculateIfInsetsChanged();
    recalculateIfOrientationChanged();
    Rectangle clip = g.getClipBounds();

    if (slider.getPaintTrack() && clip.intersects(trackRect)) {
      paintTrack(g);
    }
    if (slider.getPaintTicks() && clip.intersects(tickRect)) {
      paintTicks(g);
    }
    if (slider.getPaintLabels() && clip.intersects(labelRect)) {
      paintLabels(g);
    }
    if (slider.hasFocus() && clip.intersects(focusRect)) {
      paintFocus(g);
    }
    if (clip.intersects(thumbRect)) {
      Color savedColor = slider.getBackground();
      slider.setBackground(thumbColor);
      paintThumb(g);
      slider.setBackground(savedColor);
    }
  }

}
