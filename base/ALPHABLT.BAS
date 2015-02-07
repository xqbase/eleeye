Attribute VB_Name = "mdlAlphaBlt"
Option Explicit

Private Const WS_VISIBLE As Long = &H10000000
Private Const WS_POPUP As Long = &H80000000

Private Type WndClassEx
    cbSize As Long
    style As Long
    lpfnWndProc As Long
    cbClsExtra As Long
    cbWndExtra As Long
    hInstance As Long
    hIcon As Long
    hCursor As Long
    hbrBackground As Long
    lpszMenuName As String
    lpszClassName As String
    hIconSm As Long
End Type

Private Declare Sub AlphaBlt Lib "ALPHABLT.DLL" Alias "_AlphaBlt@44" (ByVal hdcDest As Long, ByVal xDest As Long, ByVal yDest As Long, ByVal nWidth As Long, ByVal nHeight As Long, _
        ByVal hdcSrc As Long, ByVal xSrc As Long, ByVal ySrc As Long, ByVal hdcAlpha As Long, ByVal xAlpha As Long, ByVal yAlpha As Long)
Private Declare Function DefWindowProcA Lib "USER32.DLL" (ByVal hWnd As Long, ByVal uMsg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Private Declare Sub RegisterClassExA Lib "USER32.DLL" (ByRef wcx As WndClassEx)
Private Declare Function CreateWindowExA Lib "USER32.DLL" (ByVal dwExStyle As Long, ByVal lpClassName As String, ByVal lpWindowName As String, ByVal dwStyle As Long, _
        ByVal x As Long, ByVal y As Long, ByVal nWidth As Long, ByVal nHeight As Long, ByVal hWndParent As Long, ByVal hMenu As Long, ByVal hInstance As Long, ByVal lpParam As Long) As Long
Private Declare Function GetDC Lib "USER32.DLL" (ByVal hWnd As Long) As Long
Private Declare Sub ReleaseDC Lib "USER32.DLL" (ByVal hWnd As Long, ByVal hdc As Long)
Private Declare Function CreateCompatibleDC Lib "GDI32.DLL" (ByVal hdc As Long) As Long
Private Declare Sub SelectObject Lib "GDI32.DLL" (ByVal hdc As Long, ByVal hgdiobj As Long)
Private Declare Sub DeleteDC Lib "GDI32.DLL" (ByVal hdc As Long)
Private Declare Function Assign Lib "MSVBVM60.DLL" Alias "VarPtr" (ByVal dw As Long) As Long

Private Function WndProc(ByVal hWnd As Long, ByVal uMsg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long

WndProc = DefWindowProcA(hWnd, uMsg, wParam, lParam)

End Function

Public Function TransparentWindow(ByVal hInstance As Long, ByVal hbmpSrc As Long, ByVal hbmpAlpha As Long, ByVal nWidth As Integer, ByVal nHeight As Integer)

Dim hWnd As Long, hdcDest As Long, hdcSrc As Long, hdcAlpha As Long
Dim nWidthScreen As Integer, nHeightScreen As Integer, nWidthBitmap As Integer, nHeightBitmap As Integer
Dim wcx As WndClassEx

nWidthScreen = Fix(Screen.Width / Screen.TwipsPerPixelX)
nHeightScreen = Fix(Screen.Height / Screen.TwipsPerPixelY)
nWidthBitmap = Fix(nWidth / Screen.TwipsPerPixelX)
nHeightBitmap = Fix(nHeight / Screen.TwipsPerPixelY)
wcx.cbSize = Len(wcx)
wcx.style = 0
wcx.lpfnWndProc = Assign(AddressOf WndProc)
wcx.cbClsExtra = 0
wcx.cbWndExtra = 0
wcx.hInstance = hInstance
wcx.hIcon = 0
wcx.hCursor = 0
wcx.hbrBackground = 0
wcx.lpszMenuName = vbNullString
wcx.lpszClassName = "TransparentWindow"
wcx.hIconSm = 0
RegisterClassExA wcx
hWnd = CreateWindowExA(0, "TransparentWindow", vbNullString, WS_VISIBLE Or WS_POPUP, (nWidthScreen - nWidthBitmap) \ 2, (nHeightScreen - nHeightBitmap) \ 2, nWidthBitmap, nHeightBitmap, 0, 0, hInstance, 0)
hdcDest = GetDC(hWnd)
hdcSrc = CreateCompatibleDC(hdcDest)
hdcAlpha = CreateCompatibleDC(hdcDest)
SelectObject hdcSrc, hbmpSrc
SelectObject hdcAlpha, hbmpAlpha
AlphaBlt hdcDest, 0, 0, nWidthBitmap, nHeightBitmap, hdcSrc, 0, 0, hdcAlpha, 0, 0
DeleteDC hdcSrc
DeleteDC hdcAlpha
ReleaseDC hWnd, hdcDest
TransparentWindow = hWnd

End Function
