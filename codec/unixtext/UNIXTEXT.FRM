VERSION 5.00
Begin VB.Form frmUnixText 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Unix Text Generator"
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
   Begin VB.CommandButton btnLoad 
      Caption         =   "Load"
      Height          =   375
      Left            =   1320
      TabIndex        =   3
      Top             =   2760
      Width           =   855
   End
   Begin VB.CommandButton btnSave 
      Caption         =   "Save"
      Height          =   375
      Left            =   2400
      TabIndex        =   4
      Top             =   2760
      Width           =   855
   End
   Begin VB.CheckBox chkMacText 
      Caption         =   "Mac Text"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   2880
      Width           =   1215
   End
   Begin VB.TextBox txtText 
      Height          =   2535
      Left            =   120
      MultiLine       =   -1  'True
      OLEDropMode     =   1  'Manual
      ScrollBars      =   3  'Both
      TabIndex        =   0
      Top             =   120
      Width           =   4455
   End
   Begin VB.CheckBox chkClipboard 
      Caption         =   "Clipboard"
      Height          =   255
      Left            =   120
      TabIndex        =   1
      Top             =   2640
      Width           =   1215
   End
   Begin VB.CommandButton btnExit 
      Cancel          =   -1  'True
      Caption         =   "Exit"
      Height          =   375
      Left            =   3480
      TabIndex        =   5
      Top             =   2760
      Width           =   855
   End
End
Attribute VB_Name = "frmUnixText"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub ReadFile(ByVal szFileName As String)

Dim sz As String, szLine As String, nOpenFile As Integer
nOpenFile = FreeFile
On Error GoTo lnErrorOpen
Open szFileName For Input As #nOpenFile
On Error GoTo 0
sz = ""
Do While Not EOF(nOpenFile)
    Line Input #nOpenFile, szLine
    szLine = Replace(szLine, IIf(chkMacText.Value = 0, vbLf, vbCr), vbCrLf)
    sz = sz + szLine + vbCrLf
Loop
txtText.Text = sz
Close #nOpenFile

Exit Sub
lnErrorOpen:
MsgBox "Cannot Read File " + szFileName, vbExclamation
On Error GoTo 0

End Sub

Private Sub ReadClipboard(ByVal sz As String)

txtText.Text = Replace(sz, IIf(chkMacText.Value = 0, vbLf, vbCr), vbCrLf)

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

Private Sub btnLoad_Click()

Dim sz As String, szLine As String, nOpenFile As Integer
If chkClipboard.Value = 0 Then
    sz = OpenFileDialog("Load Text File", "Text File (*.*)|*.*")
    If sz <> "" Then
        ReadFile sz
    End If
Else
    sz = Clipboard.GetText
    If sz <> "" Then
        ReadClipboard sz
    End If
End If

End Sub

Private Sub btnSave_Click()

Dim nLen As Long, nOpenFile As Integer, sz As String
If chkClipboard.Value = 0 Then
    sz = SaveFileDialog("Save Text File", "Text File (*.*)|*.*", "")
    If sz <> "" Then
        nOpenFile = FreeFile
        On Error GoTo lnErrorOpen
        Open sz For Output As #nOpenFile
        On Error GoTo 0
        Print #nOpenFile, Replace(txtText.Text, IIf(chkMacText.Value = 0, vbCr, vbLf), "");
        Close #nOpenFile
    End If
Else
    Clipboard.Clear
    Clipboard.SetText Replace(txtText.Text, IIf(chkMacText.Value = 0, vbCr, vbLf), "")
End If

Exit Sub
lnErrorOpen:
MsgBox "Cannot Write File " + sz, vbExclamation
On Error GoTo 0

End Sub

Private Sub btnExit_Click()

Unload Me

End Sub

Private Sub txtText_OLEDragDrop(data As DataObject, effect As Long, button As Integer, shift As Integer, x As Single, y As Single)

If data.GetFormat(vbCFFiles) Then
    ReadFile data.Files(1)
End If

End Sub
