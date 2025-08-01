
# ex45_bld_smoothfont demonstration

<p>
The smooth font is stored in flash based memory.  The header file describing the font is
ex45_bld_smoothfont\fonts\NotoSansJPThin28.h
</p>

Steps required to run the smooth font demo.

1. Copy ex45_bld_smoothfont\fonts\NotoSansJP-VariableFont_wght.ttf to GUIsliceBuilder\fonts\vlw

2. Copy ex45_bld_smoothfont\fonts\builder_fonts.json to GUIsliceBuilder\templates

3. Make sure your GUIslice\src\GUIslice_config.h has been edited to support the TFT_eSPI 
driver such as esp-tftespi-default-stmpe610 etc...

<p>
Creating your own flash based smooth fonts is explained inside the GUIsliceBuilder's user_guide.pdf 
version 0.17.b41 dated 8/1/2025 Appendix G - Creating Smooth Fonts. 
Also you can read over lines 2215 to 2227 of ex45_bld_smoothfont\fonts\builder_fonts.json for an example
of whats required.
</p>

<p>
The sample is set for Arduino using TFT_eSPI library and a standard
320x240 pixel display but you can use the GUIsliceBuilder to set the
your display to another size. 
Simply open the use the ex45_bld_smoothfont.prj file within the GUIsliceBuilder, select the 
PROJECT_OPTIONS tab and make your changes before doing a new code generation.
</p> 
