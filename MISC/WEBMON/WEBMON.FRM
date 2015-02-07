VERSION 5.00
Object = "{48E59290-9880-11CF-9754-00AA00C00908}#1.0#0"; "MSINET.OCX"
Begin VB.Form frmWebMon 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "网站监控"
   ClientHeight    =   3195
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4680
   BeginProperty Font 
      Name            =   "宋体"
      Size            =   9
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   MaxButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnClear 
      Caption         =   "清空"
      Height          =   375
      Left            =   480
      TabIndex        =   2
      Top             =   2760
      Width           =   1095
   End
   Begin VB.CommandButton btnExit 
      Cancel          =   -1  'True
      Caption         =   "退出"
      Height          =   375
      Left            =   3120
      TabIndex        =   3
      Top             =   2760
      Width           =   1095
   End
   Begin VB.CommandButton btnStart 
      Caption         =   "开始"
      Default         =   -1  'True
      Height          =   375
      Left            =   1800
      TabIndex        =   0
      Top             =   2760
      Width           =   1095
   End
   Begin VB.TextBox txtLog 
      Height          =   2295
      Left            =   0
      Locked          =   -1  'True
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   1
      Top             =   360
      Width           =   4695
   End
   Begin VB.ComboBox txtUrl 
      Height          =   300
      ItemData        =   "WEBMON.frx":0000
      Left            =   0
      List            =   "WEBMON.frx":0013
      TabIndex        =   4
      Text            =   "http://www.baidu.com/"
      Top             =   0
      Width           =   4695
   End
   Begin VB.Timer tm 
      Enabled         =   0   'False
      Interval        =   10000
      Left            =   4200
      Top             =   2760
   End
   Begin InetCtlsObjects.Inet inet 
      Left            =   0
      Top             =   2640
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      RequestTimeout  =   10
   End
End
Attribute VB_Name = "frmWebMon"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Unload(nCancel As Integer)

If Not btnStart.Enabled Then
    nCancel = 1
End If

End Sub

Private Sub btnClear_Click()

txtLog.Text = ""

End Sub

Private Sub btnStart_Click()

If tm.Enabled Then
    tm.Enabled = False
    txtUrl.Enabled = True
    btnStart.Caption = "开始"
Else
    btnStart.Caption = "停止"
    txtUrl.Enabled = False
    tm_Timer
    tm.Enabled = True
End If

End Sub

Private Sub btnExit_Click()

Unload Me

End Sub

Private Sub tm_Timer()

Dim dfLast As Double, dfThis As Double, sz As String, nLen As Long

dfLast = Timer
Caption = App.Title & "[忙碌]"
btnExit.Enabled = False
btnStart.Enabled = False
On Error Resume Next
sz = inet.OpenURL(txtUrl.Text)
On Error GoTo 0
btnStart.Enabled = True
btnExit.Enabled = True
Caption = App.Title

If sz = "" Then
    sz = Time & " " & txtUrl.Text & " " & "Failed"
Else
    dfThis = Timer
    dfThis = IIf(dfThis < dfLast, dfThis + 86400#, dfThis)
    sz = Time & " " & txtUrl.Text & " (" & Len(sz) & ") " & FormatNumber(dfThis - dfLast, 3, True) & "ms"
End If

nLen = Len(txtLog.Text)
If txtLog.Text = "" Then
    txtLog.Text = sz
    txtLog.SelStart = 0
Else
    txtLog.Text = txtLog.Text & vbCrLf
    nLen = Len(txtLog.Text)
    txtLog.Text = txtLog.Text & sz
    txtLog.SelStart = nLen
End If
txtLog.SelStart = nLen

End Sub
