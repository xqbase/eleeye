Attribute VB_Name = "mdlPipe"
Option Explicit

Public Type PipeStruct
    hInput      As Long
    hOutput     As Long
    bConsole    As Long
    nBytesLeft  As Long
    nEof        As Long
    nReadEnd    As Long
    szBuffer(0 To 8191) As Byte
End Type

Public Declare Sub PipeOpen Lib "PIPE.DLL" Alias "_PipeOpen@8" (ByRef pipe As PipeStruct, Optional ByVal szProcFile As String = vbNullString)
Public Declare Sub PipeClose Lib "PIPE.DLL" Alias "_PipeClose@4" (ByRef pipe As PipeStruct)
Public Declare Function PipeLineInput Lib "PIPE.DLL" Alias "_PipeLineInput@4" (ByRef pipe As PipeStruct) As Long
Public Declare Sub PipeLineOutput Lib "PIPE.DLL" Alias "_PipeLineOutput@8" (ByRef pipe As PipeStruct, ByVal szLineStr As String)
Public Declare Function PipeEof Lib "PIPE.DLL" Alias "_PipeEof@4" (ByRef pipe As PipeStruct) As Boolean
