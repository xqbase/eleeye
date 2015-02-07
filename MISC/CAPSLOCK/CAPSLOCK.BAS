Attribute VB_Name = "mdlCapsLock"
Option Explicit

Private Const WM_LBUTTONUP As Long = &H202
Private Const WM_RBUTTONUP As Long = &H205
Private Const VK_CAPITAL As Long = &H14

Private Declare Function GetKeyState Lib "USER32.DLL" (ByVal nVirtKey As Long) As Integer
Private Declare Function LoadIconA Lib "USER32.DLL" (ByVal hInstance As Long, ByVal lpIconName As Long) As Long

Public App_bRunning As Boolean, App_bShow As Boolean, app_bShowTooltip As Boolean

Private lpPrevWndFunc As Long, hIconOn As Long, hIconOff As Long, dfLastTime As Double

Private Function NewWndFunc(ByVal hWnd As Long, ByVal uMsg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long

If uMsg = WM_TRAY Then
    If lParam = WM_LBUTTONUP Then
        ShowTooltip
    ElseIf lParam = WM_RBUTTONUP Then
        frmCapsLock.PopupMenu frmCapsLock.mnTray, , , , frmCapsLock.mnTrayExit
    End If
End If
NewWndFunc = CallWindowProcA(lpPrevWndFunc, hWnd, uMsg, wParam, lParam)

End Function

Private Function CapsLockOn() As Boolean

Dim iKeyState As Integer
iKeyState = GetKeyState(VK_CAPITAL)
CapsLockOn = iKeyState = 1 Or iKeyState = -127

End Function

Private Sub ShowTray(bCapsOn As Boolean)

frmCapsLock.imgCapsOn.Visible = bCapsOn
frmCapsLock.imgCapsOff.Visible = Not bCapsOn
ModifyTrayIcon frmCapsLock, IIf(bCapsOn, hIconOn, hIconOff), IIf(bCapsOn, "Caps Lock On", "Caps Lock Off")

End Sub

Private Sub ShowTooltip()

If frmCapsLock.mnTrayTooltip.Checked Then
    frmCapsLock.Show
    App_bShow = True
    dfLastTime = Timer
End If

End Sub

Public Sub Main()

Dim i As Integer, bCapsOn As Boolean, dfTime As Double

App_bRunning = True
App_bShow = False
dfLastTime = Timer
hIconOn = LoadIconA(App.hInstance, 1)
hIconOff = LoadIconA(App.hInstance, 2)
i = 0

frmCapsLock.mnTrayTooltip.Checked = (GetSetting("CapsLock", "CapsLock", "Tooltip", "1") = "1")
frmCapsLock.Move Screen.Width - frmCapsLock.Width, Screen.Height - frmCapsLock.Height
SetWindowPos frmCapsLock.hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE + SWP_NOSIZE
lpPrevWndFunc = SetWindowLongA(frmCapsLock.hWnd, GWL_WNDPROC, AddressOf NewWndFunc)
AddTrayIcon frmCapsLock
bCapsOn = CapsLockOn
ShowTray bCapsOn

Do While App_bRunning
    i = i + 1
    If i = 10 Then
        i = 0
        If App_bShow Then
            dfTime = Timer
            dfTime = IIf(dfTime >= 86400, dfTime + 86400, dfTime)
            If dfTime - dfLastTime > 5 Then
                frmCapsLock.Hide
                App_bShow = False
            End If
        End If
        If CapsLockOn Xor bCapsOn Then
            bCapsOn = Not bCapsOn
            ShowTray bCapsOn
            ShowTooltip
        End If
    End If
    Sleep 1
    DoEvents
Loop

DeleteTrayIcon frmCapsLock
SaveSetting "CapsLock", "CapsLock", "Tooltip", IIf(frmCapsLock.mnTrayTooltip.Checked, "1", "0")
Unload frmCapsLock

End Sub
