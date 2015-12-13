; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Languages]
Name: "zh"; MessagesFile: "compiler:Languages\ChineseSimp.isl"; InfoBeforeFile: "..\LGPL.TXT"

[Setup]
AppName=ElephantEye
AppVerName=ElephantEye 3.1 (��������ʦ���͡�ǳ�����塷����)
AppPublisher=www.elephantbase.net
AppPublisherURL=http://www.elephantbase.net/
AppSupportURL=http://www.elephantbase.net/
AppUpdatesURL=http://www.elephantbase.net/
DefaultDirName={pf}\Qianhong\Plugins\ElephantEye
DefaultGroupName=������ʦ
OutputBaseFilename=eleeye
OutputDir=.
Compression=lzma
SolidCompression=yes
Uninstallable=no
SetupIconFile=..\RES\XQWSETUP.ICO
AppCopyright=(C) 2004-2007 www.elephantbase.net

[Files]
Source: "UCCI2QH.INI"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\ELEBASE.URL"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\README.TXT"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\LGPL.TXT"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\BIN\UCCI2QH.EXE"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\BIN\ELEEYE.EXE"; DestDir: "{app}\BIN"; Flags: ignoreversion
Source: "..\BIN\EVALUATE.DLL"; DestDir: "{app}\BIN"; Flags: ignoreversion
Source: "..\BIN\BOOK.DAT"; DestDir: "{app}\BIN"; Flags: ignoreversion
Source: "..\BIN\MAKEBOOK.EXE"; DestDir: "{app}\BIN"; Flags: ignoreversion
Source: "..\BIN\MAKEBOOK.DLL"; DestDir: "{app}\BIN"; Flags: ignoreversion
Source: "..\BIN\CCHESS.DLL"; DestDir: "{app}\BIN"; Flags: ignoreversion

[Registry]
Root: HKCU; Subkey: "Software\VB and VBA Program Settings\XQWizard\Engine"; ValueType: string; ValueName: "FileName"; ValueData: "{app}\BIN\ELEEYE.EXE"
Root: HKCU; Subkey: "Software\VB and VBA Program Settings\XQWizard\Engine"; ValueType: string; ValueName: "BookFiles"; ValueData: "{app}\BIN\BOOK.DAT"

[Icons]
Name: "{group}\������ʦ����վ"; Filename: "{app}\ELEBASE.URL"
Name: "{group}\ElephantEye ���ֿ��������"; Filename: "{app}\BIN\MAKEBOOK.EXE"
Name: "{group}\ElephantEye ʹ��˵��"; Filename: "{app}\README.TXT"

[Run]
Filename: "{app}\README.TXT"; Description: "�Ķ� ElephantEye ʹ��˵��"; Flags: nowait postinstall shellexec skipifsilent