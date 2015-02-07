VERSION 5.00
Begin VB.Form frmDumpHex 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "DumpHex"
   ClientHeight    =   3195
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4680
   BeginProperty Font 
      Name            =   "ËÎÌו"
      Size            =   9
      Charset         =   134
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   MaxButton       =   0   'False
   OLEDropMode     =   1  'Manual
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnClipboard 
      Caption         =   "Clipboard"
      Height          =   375
      Left            =   360
      TabIndex        =   1
      Top             =   2760
      Width           =   1095
   End
   Begin VB.CommandButton btnLoad 
      Caption         =   "Load"
      Height          =   375
      Left            =   1800
      TabIndex        =   2
      Top             =   2760
      Width           =   1095
   End
   Begin VB.TextBox txtCode 
      Height          =   2535
      Left            =   120
      Locked          =   -1  'True
      MultiLine       =   -1  'True
      OLEDropMode     =   1  'Manual
      ScrollBars      =   3  'Both
      TabIndex        =   0
      Top             =   120
      Width           =   4455
   End
   Begin VB.CommandButton btnExit 
      Cancel          =   -1  'True
      Caption         =   "Exit"
      Height          =   375
      Left            =   3240
      TabIndex        =   3
      Top             =   2760
      Width           =   1095
   End
End
Attribute VB_Name = "frmDumpHex"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Declare Sub DumpHex Lib "DUMPHEX.DLL" Alias "_DumpHexA@16" (ByVal lpText As Long, ByVal szData As String, ByVal nOffset As Long, ByVal nLength As Long)

Private Sub ReadFile(ByVal szFileName As String)

Dim i As Long, nFileLen As Long, nOpenFile As Integer
nOpenFile = FreeFile
On Error GoTo lnErrorOpen
Open szFileName For Binary Access Read As #nOpenFile
On Error GoTo 0
nFileLen = LOF(nOpenFile)
ReDim ucData(1 To nFileLen) As Byte
ReDim ucText(1 To ((nFileLen + 15) \ 16 + 1) * 79) As Byte
For i = 1 To nFileLen
    Get #nOpenFile, , ucData(i)
Next
DumpHex VarPtr(ucText(1)), VarPtr(ucData(1)), 0, nFileLen
txtCode.Text = AllocString(VarPtr(ucText(1)))
Erase ucText
Erase ucData
Close #nOpenFile

Exit Sub
lnErrorOpen:
MsgBox "Cannot Read File " + szFileName, vbExclamation
On Error GoTo 0

End Sub

Private Sub Form_Load()

Dim sz As String
sz = Command
If sz <> "" Then
    If Left(sz, 1) = """" And Right(sz, 1) = """" Then
        sz = Mid(sz, 2, Len(sz) - 2)
    End If
    ReadFile sz
End If

End Sub

Private Sub Form_OLEDragDrop(data As DataObject, effect As Long, button As Integer, shift As Integer, x As Single, y As Single)

If data.GetFormat(vbCFFiles) Then
    ReadFile data.Files(1)
End If

End Sub

Private Sub btnClipboard_Click()

Dim sz As String, nFileLen As Long
sz = Clipboard.GetText
If sz = "" Then
    Exit Sub
End If
nFileLen = lstrlenA(sz)
ReDim ucText(1 To ((nFileLen + 15) \ 16 + 1) * 79) As Byte
DumpHex VarPtr(ucText(1)), sz, 0, nFileLen
txtCode.Text = AllocString(VarPtr(ucText(1)))
Erase ucText

End Sub

Private Sub btnLoad_Click()

Dim sz As String
Dim i As Long, nFileLen As Long, nOpenFile As Integer
sz = OpenFileDialog("Load Binary File", "Binary File (*.*)|*.*")
If sz <> "" Then
    ReadFile sz
End If

End Sub

Private Sub btnExit_Click()

Unload Me

End Sub

Private Sub txtCode_OLEDragDrop(data As DataObject, effect As Long, button As Integer, shift As Integer, x As Single, y As Single)

If data.GetFormat(vbCFFiles) Then
    ReadFile data.Files(1)
End If

End Sub
