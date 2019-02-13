; GUIsliceBuilder Inno Setup SKELETON Script
;
; PLEASE NOTE:
;
; 1. This script is a SKELETON and is meant to be parsed by the Gradle 
;    task "innosetup" before handing it to the Inno Setup compiler (ISCC)
;
; 2. All VARIABLES with a dollar sign and curly brackets are replaced
;    by Gradle, e.g. "applicationVersion" below
;
; 3. The script is COPIED to build/innosetup before its run,
;    so all relative paths refer to this path!
;
; 4. All BACKSLASHES must be escaped 
;

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
; Right now we are not setting 64 bit mode 
;ArchitecturesInstallIn64BitMode=x64 ia64
AppId={{D2C2D2DF-35CC-4921-9F51-2F31B8C0E3C5}
AppName=GUIsliceBuilder
AppVersion=${applicationVersion}
AppVerName=GUIsliceBuilder ${applicationVersion}
AppPublisher=impulseadventure.com
AppPublisherURL=https://www.impulseadventure.com
AppSupportURL=https://github.com/ImpulseAdventure/GUIslice/issues
AppUpdatesURL=https://github.com/ImpulseAdventure/GUIslice/releases
DefaultGroupName=GUIslice
DefaultDirName={pf}\\GUIsliceBuilder
DisableDirPage=no
DisableWelcomePage=no
DisableProgramGroupPage=yes
LicenseFile=..\\..\\docs\\LICENSE.txt
OutputDir=.
OutputBaseFilename=builder-win-${applicationVersion}
SetupIconFile=..\\tmp\\windows\\GUIsliceBuilder\\guislicebuilder.ico
Compression=lzma
SolidCompression=yes

[Setup]
; Tell Windows Explorer to reload the environment
ChangesEnvironment=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Dirs]
Name: "{app}"; 
Name: "{app}\\projects"; Permissions: everyone-full
Name: "{app}\\logs"; Permissions: everyone-full

[Files]
Source: "..\\tmp\\windows\\GUIsliceBuilder\\GUIslice.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\\tmp\\windows\\GUIsliceBuilder\\guislicebuilder.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\\tmp\\windows\\GUIsliceBuilder\\lib\\*"; DestDir: "{app}\\lib"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\\..\\templates\\*"; DestDir: "{app}\\templates"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\\..\\arduino_res\\*"; DestDir: "{app}\\arduino_res"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\\..\\linux_res\\*"; DestDir: "{app}\\linix_res"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\\GUIsliceBuilder"; Filename: "{app}\\GUIslice.bat"
Name: "{commondesktop}\\GUIsliceBuilder"; Filename: "{app}\\GUIslice.bat"; IconFilename: "{app}\\guislicebuilder.ico"; Tasks: desktopicon

[Run]
Filename: "{app}\\GUIslice.bat"; Description: "{cm:LaunchProgram,GUIsliceBuilder}"; Flags: shellexec postinstall skipifsilent

[Registry]
; set JAVA_HOME
Root: HKCU; Subkey: "Environment"; ValueType:string; ValueName:"JAVA_HOME"; ValueData:"{code:GetJavaHome}"; Flags: preservestringtype
; set PROJECT DIRECTORY
Root: HKCU; Subkey: "Software\\JavaSoft\\Prefs\\com\\impulseadventure\\builder\\general"; ValueType:string; ValueName:"/Project /Directory"; ValueData:"{code:GetProjectDir}"

[Code]
var
  javaPath: String;
  JavaDirPage: TInputDirWizardPage;
  ProjectDirPage: TInputDirWizardPage;
  S: String;

function InitializeSetup(): Boolean;
begin
  javaPath := GetEnv('JAVA_HOME')   
  
  if javaPath = '' then begin
    javaPath := ExpandConstant('C:\\Program Files (x86)\\Arduino\\java');
  end
  Result := True;
end;

function GetJavaHome(Value: string): string;
begin
  Result := JavaDirPage.Values[0];
end;

function GetProjectDir(Value: string): string;
begin
  S := ProjectDirPage.Values[0];
  S := AnsiLowercase(S);
  StringChangeEx(S, '\\', '//', True);
  Result := S;
end;

procedure InitializeWizard;
begin
  JavaDirPage := CreateInputDirPage(wpLicense, 'Set JAVE_HOME to the folder where Java is installed', 
    'JAVA_HOME can be set to either Arduino IDE Java or Oracle JRE', 
    'If the defaults are acceptable, then click Next.', False, '');
  JavaDirPage.Add('Java Home:');
  JavaDirPage.Values[0] := javaPath;

  ProjectDirPage := CreateInputDirPage(wpLicense, 'Set your Project Folder to where all projects will be kept.', 
    'Project Folder should be your Arduino Sketchbook Folder or another writable Folder', 
    'If the defaults are acceptable, then click Next.', False, '');
  ProjectDirPage.Add('Project Folder:');
  ProjectDirPage.Values[0] := ExpandConstant('{userdocs}\\Arduino');
end;
