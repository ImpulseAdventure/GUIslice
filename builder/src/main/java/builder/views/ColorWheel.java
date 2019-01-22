package builder.views;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JPanel;


public class ColorWheel extends JPanel {
  private static final long serialVersionUID = 1L;

  protected void paintComponent(Graphics g)
  {
    super.paintComponent(g);

    float hueDegree = 1 / 360.0f;

    for (int i = 0; i < 360; i++)
    {
        Color color = Color.getHSBColor(i * hueDegree, 1.0f, 1.0f);
        g.setColor( color );
        g.fillArc( 30, 15, 250, 250, i, 1);
    }
  }
  
  @Override
  public Dimension getPreferredSize()
  {
      return new Dimension(250, 250);
  }
}
