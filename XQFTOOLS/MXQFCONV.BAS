Attribute VB_Name = "mdlConv"
Option Explicit

Public Declare Sub ConvInit Lib "MXQFCONV.DLL" Alias "_ConvInit@4" (Optional ByVal bTraditional As Long = 0)
Public Declare Sub ConvXqf2Pgn Lib "MXQFCONV.DLL" Alias "_ConvXqf2Pgn@8" (ByVal szXqfFile As String, ByVal szPgnFile As String)
Public Declare Sub ConvMxq2Pgn Lib "MXQFCONV.DLL" Alias "_ConvMxq2Pgn@8" (ByVal szMxqFile As String, ByVal szPgnFile As String)
Public Declare Sub ConvChn2Pgn Lib "MXQFCONV.DLL" Alias "_ConvChn2Pgn@8" (ByVal szChnFile As String, ByVal szPgnFile As String)
Public Declare Sub ConvChe2Pgn Lib "MXQFCONV.DLL" Alias "_ConvChe2Pgn@8" (ByVal szCheFile As String, ByVal szPgnFile As String)
Public Declare Sub ConvCcm2Pgn Lib "MXQFCONV.DLL" Alias "_ConvCcm2Pgn@8" (ByVal szCcmFile As String, ByVal szPgnFile As String)
Public Declare Sub ConvPgn2Xqf Lib "MXQFCONV.DLL" Alias "_ConvPgn2Xqf@8" (ByVal szPgnFile As String, ByVal szXqfFile As String)
