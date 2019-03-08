# GUIslice Builder

Please refer to the wiki page for installation and usage details:

[GUIslice Builder - Documentation](https://github.com/ImpulseAdventure/GUIslice/wiki/GUIslice-Builder)

### Builder Contents
Note that the Builder executables are attached to the latest [Release Notes](https://github.com/ImpulseAdventure/GUIslice/releases):
- `Builder_user_guide.pdf`: Builder User Guide (in PDF format)
- `builder-win-x.y.z.zip`: Builder Executable (in Windows ZIP format)
- `builder-linux-x.y.z.tar.gz`: Builder Executable (in LINUX tar gzip format)
- `builder-osx-x.y.z.zip` Builder Executable (in Mac OS/X zip format)

### Builder Source Code
The Builder source code is located in this directory (`/builder`)
- Build instructions can be found in `BUILD.txt`

### Brief Overview
The GUIslice Builder is a standalone desktop application that is designed to help generate layouts for GUIslice.

The cross-platform utility includes a graphical editor that enables drag & drop placement of UI elements. Once a GUI has been laid out, the Builder can then generate the functional GUIslice skeleton framework code, for both Arduino and LINUX targets.

The generated output code (.c, .ino) includes all of the necessary defines, UI storage elements and initialization in addition to the placement of the UI elements. This should greatly improve the ease in creating a new GUI.