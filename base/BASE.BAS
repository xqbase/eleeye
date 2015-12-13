Attribute VB_Name = "mdlBase"
Option Explicit

' === Begin Windows APIs ===

Public Const SND_ASYNC          As Long = 1         ' PlaySoundA
Public Const SND_NOWAIT         As Long = 8192      ' PlaySoundA
Public Const SND_FILENAME       As Long = 131072    ' PlaySoundA
Public Const HWND_TOPMOST       As Long = -1        ' SetWindowPos
Public Const HWND_NOTOPMOST     As Long = -2        ' SetWindowPos
Public Const SWP_NOSIZE         As Long = 1         ' SetWindowPos
Public Const SWP_NOMOVE         As Long = 2         ' SetWindowPos
Public Const GWL_WNDPROC        As Long = -4        ' SetWindowLongA

Public Declare Sub Sleep Lib "KERNEL32.DLL" (ByVal dwMilliseconds As Long)
Public Declare Function lstrlenA Lib "KERNEL32.DLL" (ByVal sz As String) As Long
Public Declare Sub MessageBeep Lib "USER32.DLL" (ByVal uType As Long)
Public Declare Sub SetWindowPos Lib "USER32.DLL" (ByVal hWnd As Long, ByVal hWndInsertAfter As Long, _
        ByVal x As Long, ByVal y As Long, ByVal cx As Long, ByVal cy As Long, ByVal wFlags As Long)
Public Declare Function SetWindowLongA Lib "USER32.DLL" (ByVal hWnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Public Declare Function CallWindowProcA Lib "USER32.DLL" (ByVal lpPrevWndFunc As Long, _
        ByVal hWnd As Long, ByVal uMsg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Public Declare Sub PlaySoundA Lib "WINMM.DLL" (ByVal szSound As String, ByVal hMod As Long, ByVal fdwSound As Long)

' === End Windows APIs ===

' For AllocString
Private Declare Function lstrlenB Lib "KERNEL32.DLL" Alias "lstrlenA" (ByVal lpsz As Long) As Long
Private Declare Function SysAllocStringByteLen Lib "OLEAUT32.DLL" (ByVal lpsz As Long, ByVal dwLen As Long) As String

' For MsgBoxIcon
Private Type MsgBoxParams
    cbSize As Long
    hwndOwner As Long
    hInstance As Long
    lpszText As String
    lpszCaption As String
    dwStyle As Long
    lpszIcon As Long
    dwContextHelpId As Long
    lpfnMsgBoxCallback As Long
    dwLanguageId As Long
End Type

Private Const MB_USERICON As Long = 128

Private Declare Function LoadLibraryA Lib "KERNEL32.DLL" (ByVal lpLibFileName As String) As Long
Private Declare Sub FreeLibrary Lib "KERNEL32.DLL" (ByVal hLibModule As Long)
Private Declare Function LoadIconA Lib "USER32.DLL" (ByVal hInstance As Long, ByVal lpIconName As Long) As Long
Private Declare Function MessageBoxIndirectA Lib "USER32.DLL" (ByRef lpMsgBoxParams As MsgBoxParams) As Long

' For MsgBoxCheck
Private Const WH_CBT            As Long = 5             ' SetWindowsHookExA
Private Const HCBT_CREATEWND    As Long = 3             ' HookProc
Private Const WM_SETFONT        As Long = 48            ' SendMessageA/WndProc
Private Const BM_SETCHECK       As Long = 241           ' SendMessageA/WndProc
Private Const WM_INITDIALOG     As Long = 272           ' SendMessageA/WndProc
Private Const WM_COMMAND        As Long = 273           ' SendMessageA/WndProc
Private Const BN_CLICKED        As Long = 0             ' SendMessageA/WndProc
Private Const BS_CHECKBOX       As Long = 2             ' CreateWindowExA
Private Const WS_TABSTOP        As Long = &H10000       ' CreateWindowExA
Private Const WS_VISIBLE        As Long = &H10000000    ' CreateWindowExA
Private Const WS_CHILD          As Long = &H40000000    ' CreateWindowExA
Private Const CW_USEDEFAULT     As Long = &H80000000    ' CreateWindowExA
Private Const DEFAULT_GUI_FONT  As Long = 17            ' GetStockObject

Private Declare Function SetWindowsHookExA Lib "USER32.DLL" (ByVal idHook As Long, ByVal lpfn As Long, ByVal hMod As Long, ByVal dwThreadId As Long) As Long
Private Declare Function CallNextHookEx Lib "USER32.DLL" (ByVal hhk As Long, ByVal iCode As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Private Declare Sub UnhookWindowsHookEx Lib "USER32.DLL" (ByVal hhk As Long)
Private Declare Sub GetClassNameA Lib "USER32.DLL" (ByVal hWnd As Long, ByVal lpClassName As Long, ByVal nMaxCount As Long)
Private Declare Sub GetWindowRect Lib "USER32.DLL" (ByVal hWnd As Long, ByVal lpRect As Long)
Private Declare Sub GetClientRect Lib "USER32.DLL" (ByVal hWnd As Long, ByVal lpRect As Long)
Private Declare Sub SendMessageA Lib "USER32.DLL" (ByVal hWnd As Long, ByVal uMsg As Long, ByVal wParam As Long, ByVal lParam As Long)
Private Declare Sub MoveWindow Lib "USER32.DLL" (ByVal hWnd As Long, ByVal x As Long, ByVal y As Long, ByVal nWidth As Long, ByVal nHeight As Long, ByVal bRepaint As Long)
Private Declare Function FindWindowExA Lib "USER32.DLL" (Optional ByVal hWndParent As Long = 0, Optional ByVal hwndChildAfter As Long = 0, _
        Optional ByVal szClassName As String = vbNullString, Optional ByVal szWindowName As String = vbNullString) As Long
Private Declare Function CreateWindowExA Lib "USER32.DLL" (ByVal dwExStyle As Long, ByVal lpClassName As String, ByVal lpWindowName As String, ByVal dwStyle As Long, _
        ByVal x As Long, ByVal y As Long, ByVal nWidth As Long, ByVal nHeight As Long, ByVal hWndParent As Long, ByVal hMenu As Long, ByVal hInstance As Long, ByVal lpParam As Long) As Long
Private Declare Function GetStockObject Lib "GDI32.DLL" (ByVal fnObject As Long) As Long

Private mbc_hhkPrev As Long, mbc_WndProcPrev As Long, mbc_hCheck As Long
Private mbc_szCheck As String, mbc_bChecked As Boolean

' For Open/Save
Private Type OpenFileName
    lStructSize As Long
    hwndOwner As Long
    hInstance As Long
    lpstrFilter As String
    lpstrCustomFilter As String
    nMaxCustFilter As Long
    nFilterIndex As Long
    lpstrFile As String
    nMaxFile As Long
    lpstrFileTitle As String
    nMaxFileTitle As Long
    lpstrInitialDir As String
    lpstrTitle As String
    flags As Long
    nFileOffset As Integer
    nFileExtension As Integer
    lpstrDefExt As String
    lCustData As Long
    lpfnHook As Long
    lpTemplateName As String
End Type

Private Const OFN_OVERWRITEPROMPT As Long = 2
Private Const OFN_HIDEREADONLY As Long = 4
Private Const OFN_PATHMUSTEXIST As Long = 2048
Private Const OFN_FILEMUSTEXIST As Long = 4096

Private Declare Function GetOpenFileNameA Lib "COMDLG32.DLL" (ByRef lpofn As OpenFileName) As Long
Private Declare Function GetSaveFileNameA Lib "COMDLG32.DLL" (ByRef lpofn As OpenFileName) As Long

' For BrowseForFolder
Private Type BrowseInfo
    hwndOwner As Long
    pidlRoot As Long
    pszDisplayName As String
    lpszTitle As String
    ulFlags As Long
    lpfn As Long
    lParam As Long
    iImage As Long
End Type

Private Const BIF_RETURNONLYFSDIRS As Long = 1

Private Declare Function SHBrowseForFolderA Lib "SHELL32.DLL" (ByRef lpbi As BrowseInfo) As Long
Private Declare Function SHGetPathFromIDListA Lib "SHELL32.DLL" (ByVal pidl As Long, ByVal pszPath As Long) As Long

' For TrayIcon
Private Type NotifyIconData
    cbSize As Long
    hWnd As Long
    uID As Long
    uFlags As Long
    uCallbackMessage As Long
    hIcon As Long
    szTip As String * 64
End Type

Private Const NIF_MESSAGE As Long = 1
Private Const NIF_ICON As Long = 2
Private Const NIF_TIP As Long = 4

Private Const NIM_ADD As Long = 0
Private Const NIM_MODIFY As Long = 1
Private Const NIM_DELETE As Long = 2

Public Const WM_TRAY As Long = &H401

Private Declare Sub Shell_NotifyIconA Lib "SHELL32.DLL" (ByVal dwMessage As Long, ByRef lpData As NotifyIconData)

' For GetSelfExe & FindProcess
Private Const TH32CS_SNAPPROCESS As Long = 2

Private Type PROCESSENTRY32
    dwSize As Long
    cntUsage As Long
    th32ProcessID As Long
    th32DefaultHeapID As Long
    th32ModuleID As Long
    cntThreads As Long
    th32ParentProcessID As Long
    pcPriClassBase As Long
    dwFlags As Long
    szExeFile As String * 1024
End Type

Private Declare Function GetModuleFileNameA Lib "KERNEL32.DLL" (ByVal hModule As Long, ByVal lpFileName As Long, ByVal nSize As Long) As Long
Private Declare Function CreateToolhelp32Snapshot Lib "KERNEL32.DLL" (ByVal dwFlags As Long, ByVal th32ProcessID As Long) As Long
Private Declare Function Process32First Lib "KERNEL32.DLL" (ByVal hSnapshot As Long, ByRef lppe As PROCESSENTRY32) As Long
Private Declare Function Process32Next Lib "KERNEL32.DLL" (ByVal hSnapshot As Long, ByRef lppe As PROCESSENTRY32) As Long
Private Declare Sub CloseHandle Lib "KERNEL32.DLL" (ByVal hObject As Long)

' For GetExeType
Public Const IMAGE_SUBSYSTEM_UNKNOWN        As Integer = 0  ' Unknown subsystem.
Public Const IMAGE_SUBSYSTEM_NATIVE         As Integer = 1  ' Image doesn't require a subsystem.
Public Const IMAGE_SUBSYSTEM_WINDOWS_GUI    As Integer = 2  ' Image runs in the Windows GUI subsystem.
Public Const IMAGE_SUBSYSTEM_WINDOWS_CUI    As Integer = 3  ' Image runs in the Windows character subsystem.
Public Const IMAGE_SUBSYSTEM_OS2_CUI        As Integer = 5  ' image runs in the OS/2 character subsystem.
Public Const IMAGE_SUBSYSTEM_POSIX_CUI      As Integer = 7  ' image runs in the Posix character subsystem.
Public Const IMAGE_SUBSYSTEM_NATIVE_WINDOWS As Integer = 8  ' image is a native Win9x driver.
Public Const IMAGE_SUBSYSTEM_WINDOWS_CE_GUI As Integer = 9  ' Image runs in the Windows CE subsystem.

Private Const IMAGE_DOS_SIGNATURE           As Integer = &H5A4D
Private Const IMAGE_NT_SIGNATURE            As Long = &H4550

' For UnixTimestamp
Private Type SystemTime
    wYear As Integer
    wMonth As Integer
    wDayOfWeek As Integer
    wDay As Integer
    wHour As Integer
    wMinute As Integer
    wSecond As Integer
    wMilliseconds As Integer
End Type

Private Type TimeZoneInformation
    Bias As Long
    StandardName(32) As Integer
    StandardDate As SystemTime
    StandardBias As Long
    DaylightName(32) As Integer
    DaylightDate As SystemTime
    DaylightBias As Long
End Type

Private Declare Sub GetTimeZoneInformation Lib "KERNEL32.DLL" (ByRef lpTimeZoneInformation As TimeZoneInformation)

' === Begin AllocString ===

Public Function AllocString(ByVal lpsz As Long) As String

AllocString = SysAllocStringByteLen(lpsz, lstrlenB(lpsz))

End Function

' === End AllocString ===

' === Begin Str2Int/Lng ===

Public Function Dbl2Int(ByVal df As Double, Optional ByVal nMin As Integer = -32767, Optional ByVal nMax As Integer = 32767) As Integer

If df < nMin Then
    Dbl2Int = nMin
ElseIf df > nMax Then
    Dbl2Int = nMax
Else
    Dbl2Int = df
End If

End Function

Public Function Str2Int(ByVal sz As String, Optional ByVal nMin As Integer = -32767, Optional ByVal nMax As Integer = 32767) As Integer

Str2Int = Dbl2Int(Val(sz), nMin, nMax)

End Function

Public Function Dbl2Lng(ByVal df As Double, Optional ByVal nMin As Long = -2147483647, Optional ByVal nMax As Long = 2147483647) As Long

If df < nMin Then
    Dbl2Lng = nMin
ElseIf df > nMax Then
    Dbl2Lng = nMax
Else
    Dbl2Lng = df
End If

End Function

Public Function Str2Lng(ByVal sz As String, Optional ByVal nMin As Long = -2147483647, Optional ByVal nMax As Long = 2147483647) As Long

Str2Lng = Dbl2Lng(Val(sz), nMin, nMax)

End Function

' === End Str2Int/Lng ===

' === Begin URLs ===

Public Function UrlEncode(ByVal sz As String) As String

Dim szReturn As String, szChar As String
Dim i As Integer, nAscii As Integer

szReturn = ""
For i = 1 To Len(sz)
    szChar = Mid(sz, i, 1)
    Select Case szChar
    Case " "
        szReturn = szReturn & "+"
    Case "*", ".", "-", "0" To "9", "@", "A" To "Z", "_", "a" To "z"
        szReturn = szReturn & szChar
    Case Else
        nAscii = Asc(szChar)
        If nAscii < 0 Then
            szReturn = szReturn & "%" & Left(Hex(nAscii), 2) & "%" & Right(Hex(nAscii), 2)
        Else
            szReturn = szReturn & "%" & Mid(Hex(&H100 + nAscii), 2, 2)
        End If
    End Select
Next
UrlEncode = szReturn

End Function

Public Function UrlDecode(ByVal sz As String) As String

Dim szReturn As String, szChar As String
Dim i As Integer

szReturn = ""
For i = 1 To Len(sz)
    szChar = Mid(sz, i, 1)
    Select Case szChar
    Case "+"
        szReturn = szReturn & " "
    Case "%"
        szChar = Mid(sz, i + 1, 2)
        If szChar < "80" Then
            szReturn = szReturn & Chr(Val("&H" & szChar))
            i = i + 2
        Else
            szChar = szChar & Mid(sz, i + 4, 2)
            szReturn = szReturn & Chr(Val("&H" & szChar))
            i = i + 5
        End If
    Case Else
        szReturn = szReturn & szChar
    End Select
Next
UrlDecode = szReturn

End Function

' === End URLs ===

' === Begin MsgBoxIcon ===

Public Function MsgBoxIcon(ByVal szPrompt As String, Optional ByVal dwStyle As Long = 0, Optional ByVal szTitle As String = vbNullString, _
        Optional ByVal nIcon As Integer = 1, Optional ByVal szIconFile As String = vbNullString) As Long

Dim mbp As MsgBoxParams
mbp.cbSize = Len(mbp)
mbp.hwndOwner = 0
On Error Resume Next
mbp.hwndOwner = Screen.ActiveForm.hWnd
On Error GoTo 0
If StrPtr(szIconFile) = 0 Then
    mbp.hInstance = App.hInstance
Else
    mbp.hInstance = LoadLibraryA(szIconFile)
    If mbp.hInstance = 0 Then
        mbp.hInstance = App.hInstance
        nIcon = 1
    End If
End If
mbp.lpszText = szPrompt
mbp.lpszCaption = IIf(StrPtr(szTitle) = 0, App.Title, szTitle)
mbp.dwStyle = MB_USERICON + dwStyle
mbp.lpszIcon = nIcon
mbp.dwContextHelpId = 0
mbp.lpfnMsgBoxCallback = 0
mbp.dwLanguageId = 0
MsgBoxIcon = MessageBoxIndirectA(mbp)
If mbp.hInstance <> App.hInstance Then
    FreeLibrary mbp.hInstance
End If

End Function

' === End MsgBoxIcon ===

' === Begin MsgBoxCheck ===

Private Function mbc_NewWndProc(ByVal hWnd As Long, ByVal uMsg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long

Dim dw(0 To 3) As Long, nWidth As Long, nHeight As Long

If uMsg = WM_INITDIALOG Then
    GetClientRect hWnd, VarPtr(dw(0))
    nWidth = dw(2)
    nHeight = dw(3)
    GetWindowRect hWnd, VarPtr(dw(0))
    MoveWindow hWnd, dw(0), dw(1), dw(2) - dw(0), dw(3) - dw(1) + 30, 0
    mbc_hCheck = CreateWindowExA(0, "BUTTON", mbc_szCheck, WS_CHILD + WS_VISIBLE + WS_TABSTOP + BS_CHECKBOX, _
            10, nHeight, nWidth - 20, 20, hWnd, 0, App.hInstance, 0)
    SendMessageA mbc_hCheck, WM_SETFONT, GetStockObject(DEFAULT_GUI_FONT), 1
    SendMessageA mbc_hCheck, BM_SETCHECK, IIf(mbc_bChecked, 1, 0), 0
ElseIf uMsg = WM_COMMAND Then
    If wParam \ 65536 = BN_CLICKED And lParam = mbc_hCheck Then
        mbc_bChecked = Not mbc_bChecked
        SendMessageA mbc_hCheck, BM_SETCHECK, IIf(mbc_bChecked, 1, 0), 0
    End If
End If
mbc_NewWndProc = CallWindowProcA(mbc_WndProcPrev, hWnd, uMsg, wParam, lParam)

End Function

Public Function mbc_NewCbtProc(ByVal iCode As Long, ByVal wParam As Long, ByVal lParam As Long) As Long

Dim ucClass(0 To 7) As Byte, bDone As Boolean
bDone = False
If iCode = HCBT_CREATEWND Then
    GetClassNameA wParam, VarPtr(ucClass(0)), 7
    If AllocString(VarPtr(ucClass(0))) = "#32770" Then
        mbc_WndProcPrev = SetWindowLongA(wParam, GWL_WNDPROC, AddressOf mbc_NewWndProc)
        bDone = True
    End If
End If
mbc_NewCbtProc = CallNextHookEx(mbc_hhkPrev, iCode, wParam, lParam)
If bDone Then
    UnhookWindowsHookEx mbc_hhkPrev
End If

End Function

Public Function MsgBoxCheck(ByRef bChecked As Boolean, ByVal szCheck As String, ByVal szPrompt As String, _
        Optional ByVal dwStyle As Long = 0, Optional ByVal szTitle As String = vbNullString, _
        Optional ByVal nIcon As Integer = 0, Optional ByVal szIconFile As String = vbNullString) As Long

mbc_szCheck = szCheck
mbc_bChecked = bChecked
mbc_hhkPrev = SetWindowsHookExA(WH_CBT, AddressOf mbc_NewCbtProc, App.hInstance, App.ThreadID)
If nIcon > 0 Then
    MsgBoxCheck = MsgBoxIcon(szPrompt, dwStyle, szTitle, nIcon, szIconFile)
ElseIf StrPtr(szTitle) = 0 Then
    MsgBoxCheck = MsgBox(szPrompt, dwStyle)
Else
    MsgBoxCheck = MsgBox(szPrompt, dwStyle, szTitle)
End If
bChecked = mbc_bChecked

End Function

' === End MsgBoxCheck ===

' === Begin Open/Save ===

Private Function TrimNull(ByVal sz As String) As String

Dim i As Integer
i = InStr(sz, vbNullChar)
If i > 0 Then
    TrimNull = Left(sz, i - 1)
Else
    TrimNull = sz
End If

End Function

Public Function OpenFileDialog(ByVal szTitle As String, ByVal szFilter As String) As String

Dim ofn As OpenFileName
szFilter = Replace(szFilter, "|", vbNullChar)
ofn.lStructSize = Len(ofn)
ofn.hwndOwner = 0
On Error Resume Next
ofn.hwndOwner = Screen.ActiveForm.hWnd
On Error GoTo 0
ofn.lpstrCustomFilter = vbNullString
ofn.lpstrFileTitle = vbNullString
ofn.lpstrInitialDir = vbNullString
ofn.lpstrFilter = szFilter & vbNullChar
ofn.lpstrFile = String(260, 0)
ofn.nMaxFile = 260
ofn.lpstrTitle = szTitle
ofn.flags = OFN_PATHMUSTEXIST + OFN_FILEMUSTEXIST + OFN_HIDEREADONLY
ofn.nFileOffset = 0
ofn.nFileExtension = 0
ofn.lpstrDefExt = vbNullString
ofn.lCustData = 0
If GetOpenFileNameA(ofn) <> 0 Then
    OpenFileDialog = TrimNull(ofn.lpstrFile)
Else
    OpenFileDialog = ""
End If

End Function

Public Function SaveFileDialog(ByVal szTitle As String, ByVal szFilter As String, _
        ByVal szFileName As String, Optional ByVal szDefExt As String = vbNullString) As String

Dim ofn As OpenFileName, sz As String
szFilter = Replace(szFilter, "|", vbNullChar)
ofn.lStructSize = Len(ofn)
ofn.hwndOwner = 0
On Error Resume Next
ofn.hwndOwner = Screen.ActiveForm.hWnd
On Error GoTo 0
ofn.lpstrCustomFilter = vbNullString
ofn.lpstrFileTitle = vbNullString
ofn.lpstrInitialDir = vbNullString
ofn.lpstrFilter = szFilter & vbNullChar
ofn.lpstrFile = szFileName & String(260, 0)
ofn.nMaxFile = 260
ofn.lpstrTitle = szTitle
ofn.flags = OFN_PATHMUSTEXIST + OFN_HIDEREADONLY + OFN_OVERWRITEPROMPT
ofn.nFileOffset = 0
ofn.nFileExtension = 0
ofn.lpstrDefExt = szDefExt
ofn.lCustData = 0
If GetSaveFileNameA(ofn) <> 0 Then
   SaveFileDialog = TrimNull(ofn.lpstrFile)
Else
   SaveFileDialog = ""
End If

End Function

' === End OpenFile ===

' === Begin BrowseForFolder ===

Public Function BrowseForFolder(ByVal szTitle As String) As String

Dim bi As BrowseInfo
Dim szDisplayName(0 To 259) As Byte
Dim pidl As Long

bi.hwndOwner = 0
On Error Resume Next
bi.hwndOwner = Screen.ActiveForm.hWnd
On Error GoTo 0
bi.pidlRoot = 0
bi.pszDisplayName = 0
bi.lpszTitle = szTitle
bi.ulFlags = BIF_RETURNONLYFSDIRS
bi.lpfn = 0
bi.lParam = 0
bi.iImage = 0
pidl = SHBrowseForFolderA(bi)
If SHGetPathFromIDListA(pidl, VarPtr(szDisplayName(0))) = 0 Then
    BrowseForFolder = ""
Else
    BrowseForFolder = AllocString(VarPtr(szDisplayName(0)))
End If

End Function

' === End BrowseForFolder ===

' === Begin TrayIcon ===

Private Sub NotifyTray(ByVal frm As Form, ByVal hIcon As Long, ByVal szTip As String, ByVal dwMessage As Long)

Dim nid As NotifyIconData
nid.cbSize = Len(nid)
nid.hWnd = frm.hWnd
nid.uID = 0
nid.uFlags = NIF_MESSAGE + NIF_ICON + NIF_TIP
nid.uCallbackMessage = WM_TRAY
nid.hIcon = IIf(hIcon = 0, frm.Icon.Handle, hIcon)
nid.szTip = IIf(szTip = "", frm.Caption, szTip) & vbNullChar
Shell_NotifyIconA dwMessage, nid

End Sub

Public Sub AddTrayIcon(ByVal frm As Form, Optional ByVal hIcon As Long = 0, Optional ByVal szTip As String = "")

NotifyTray frm, hIcon, szTip, NIM_ADD

End Sub

Public Sub ModifyTrayIcon(ByVal frm As Form, Optional ByVal hIcon As Long = 0, Optional ByVal szTip As String = "")

NotifyTray frm, hIcon, szTip, NIM_MODIFY

End Sub

Public Sub DeleteTrayIcon(ByVal frm As Form)

NotifyTray frm, 0, "", NIM_DELETE

End Sub

' === End TrayIcon ===

' === Begin GetSelfExe & FindProcess ===

Public Function GetSelfExe() As String

Dim ucFileName(0 To 255) As Byte, lpsz As Long, dwLen As Long
lpsz = VarPtr(ucFileName(0))
dwLen = GetModuleFileNameA(0, lpsz, 256)
GetSelfExe = SysAllocStringByteLen(lpsz, dwLen)

End Function

Public Function FindProcess(ByVal szName As String) As Boolean

Dim hSnapshot As Long, pe As PROCESSENTRY32
Dim i As Integer

FindProcess = False
hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
If hSnapshot = 0 Then
    Exit Function
End If

szName = UCase(szName)
pe.dwSize = Len(pe)
If Process32First(hSnapshot, pe) <> 0 Then
    Do
        If InStr(UCase(pe.szExeFile), szName) > 0 Then
            FindProcess = True
            Exit Do
        End If
    Loop Until Process32Next(hSnapshot, pe) = 0
End If
CloseHandle hSnapshot

End Function

' === End GetSelfExe & FindProcess ===

' === Begin GetExeType ===

Public Function GetExeType(ByVal szFile As String) As Integer

Dim nFileNo As Integer, wMagic As Integer, wSubsystem As Integer
Dim dwLfanew As Long, dwNTSignature As Long

nFileNo = FreeFile
On Error GoTo lnErrorOpen
Open szFile For Input As #nFileNo
Close #nFileNo
Open szFile For Binary Access Read As #nFileNo
On Error GoTo 0

Get #nFileNo, 1, wMagic
If wMagic <> IMAGE_DOS_SIGNATURE Then
    Close #nFileNo
    GetExeType = -1
    Exit Function
End If

Get #nFileNo, 61, dwLfanew
Get #nFileNo, dwLfanew + 1, dwNTSignature
If dwNTSignature <> IMAGE_NT_SIGNATURE Then
    Close #nFileNo
    GetExeType = -1
    Exit Function
End If
Get #nFileNo, dwLfanew + 93, wSubsystem
Close #nFileNo
GetExeType = wSubsystem

Exit Function
lnErrorOpen:
GetExeType = -2

End Function

' === End GetExeType ===

' === Begin UnixTimestamp ===

Public Function UnixTimestamp() As Double

Dim tzi As TimeZoneInformation
GetTimeZoneInformation tzi
UnixTimestamp = (Now - DateSerial(1970, 1, 1)) * 86400# + tzi.Bias * 60#

End Function

' === End UnixTimestamp ===
