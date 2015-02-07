Attribute VB_Name = "mdlBIG2GB"
Option Explicit

Public Sub Main()

Dim nFileIn As Integer, nFileOut As Integer, szFileName As String, szLineStr As String
If Command = "" Then
    szFileName = OpenFileDialog("Open BIG File", "Text Files (*.TXT)|*.TXT|All Files (*.*)|*.*")
Else
    If Left(Command, 1) = """" And Right(Command, 1) = """" Then
        szFileName = Mid(Command, 2, Len(Command) - 2)
    Else
        szFileName = Command
    End If
End If
If szFileName <> "" Then
    nFileIn = FreeFile
    On Error GoTo lnErrorOpen
    Open szFileName For Input As #nFileIn
    On Error GoTo 0
    nFileOut = FreeFile
    Open szFileName + ".tmp" For Output As #nFileOut
    Do While Not EOF(nFileIn)
        Line Input #nFileIn, szLineStr
        Print #nFileOut, frmHide.big2gb.BigToGB(szLineStr)
    Loop
    Close #nFileIn
    Close #nFileOut
    On Error GoTo lnErrorRename
    Name szFileName As szFileName + ".bak"
    On Error GoTo 0
    Name szFileName + ".tmp" As szFileName
End If
Unload frmHide
Exit Sub

lnErrorOpen:
MsgBox "Cannot Open " + szFileName, vbExclamation
Unload frmHide
Exit Sub

lnErrorRename:
Kill szFileName + ".bak"
Resume 0

End Sub
