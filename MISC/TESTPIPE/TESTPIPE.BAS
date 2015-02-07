Attribute VB_Name = "mdlTestPipe"
Option Explicit

Public pipe As PipeStruct, bRunning As Boolean

Public Sub Main()

Dim lpStr As Long
Dim szFileName As String
szFileName = OpenFileDialog("Load Executable File", "Executive Files (*.EXE)|*.EXE|All Files (*.*)|*.*")
If szFileName = "" Then
    Exit Sub
End If
PipeOpen pipe, szFileName
frmTestPipe.Show
bRunning = True
Do While bRunning
    Sleep 1
    DoEvents
    lpStr = PipeLineInput(pipe)
    If lpStr <> 0 Then
        frmTestPipe.lstOutput.AddItem AllocString(lpStr)
        frmTestPipe.lstOutput.ListIndex = frmTestPipe.lstOutput.ListCount - 1
    End If
    If PipeEof(pipe) Then
        MsgBox "=== Pipe Broken ===", vbExclamation
        Unload frmTestPipe
    End If
Loop
PipeClose pipe

End Sub
