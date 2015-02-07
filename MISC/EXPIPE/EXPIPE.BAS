Attribute VB_Name = "mdlExPipe"
' Run "LINK /EDIT /SUBSYSTEM:CONSOLE EXPIPE.EXE" After Make

Option Explicit

Sub Main()

Dim lpsz As Long, sz As String, pipe As PipeStruct
PipeOpen pipe
Do
    lpsz = PipeLineInput(pipe)
    If lpsz = 0 Then
        Sleep 1
    Else
        sz = AllocString(lpsz)
        If sz = "" Then
            Exit Do
        Else
            PipeLineOutput pipe, sz
        End If
    End If
Loop

End Sub
