Attribute VB_Name = "mdlCChess"
' CCHESS.BAS - Source Code for ElephantEye, Additional Part
'
' ElephantEye - a Chinese Chess Program (UCCI Engine)
' Designed by Morning Yellow, Version: 4.5, Last Modified: Jan. 2010
' Copyright (C) 2004-2010 www.xqbase.com
'
' This library is free software; you can redistribute it and/or
' modify it under the terms of the GNU Lesser General Public
' License as published by the Free Software Foundation; either
' version 2.1 of the License, or (at your option) any later version.
'
' This library is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
' Lesser General Public License for more details.
'
' You should have received a copy of the GNU Lesser General Public
' License along with this library; if not, write to the Free Software
' Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Option Explicit

Public Type RollbackStruct
    dwZobristKey    As Long
    dwZobristLock0  As Long
    dwZobristLock1  As Long
    vlWhiteValue    As Long
    vlBlackValue    As Long
    mvs             As Long
End Type

Public Type PositionStruct
    sdPlayer                As Long
    ucpcSquares(0 To 255)   As Byte
    ucsqPieces(0 To 47)     As Byte
    dwZobristKey            As Long
    dwZobristLock0          As Long
    dwZobristLock1          As Long
    dwBitPiece              As Long
    wBitRanks(0 To 15)      As Integer
    wBitFiles(0 To 15)      As Integer
    vlWhiteValue            As Long
    vlBlackValue            As Long
    nMoveNum                As Long
    nDistance               As Long
    rbsList(0 To 1023)      As RollbackStruct
    ucRepHash(0 To 4095)    As Byte
End Type

Public Declare Function CchessVersion Lib "CCHESS.DLL" Alias "_CchessVersion@0" () As Long
Public Declare Sub CchessInit Lib "CCHESS.DLL" Alias "_CchessInit@4" (Optional ByVal bTraditional As Long = 0)
Public Declare Sub CchessPromotion Lib "CCHESS.DLL" Alias "_CchessPromotion@4" (Optional ByVal bPromotion As Long = 0)
Public Declare Sub CchessAddPiece Lib "CCHESS.DLL" Alias "_CchessAddPiece@16" (ByRef pos As PositionStruct, ByVal sq As Long, ByVal pc As Long, Optional ByVal bDel As Long = 0)
Public Declare Function CchessCanPromote Lib "CCHESS.DLL" Alias "_CchessCanPromote@8" (ByRef pos As PositionStruct, ByVal sq As Long) As Long
Public Declare Function CchessTryMove Lib "CCHESS.DLL" Alias "_CchessTryMove@12" (ByRef pos As PositionStruct, ByRef nStatus As Long, ByVal mv As Long) As Long
Public Declare Sub CchessUndoMove Lib "CCHESS.DLL" Alias "_CchessUndoMove@4" (ByRef pos As PositionStruct)
Public Declare Function CchessTryNull Lib "CCHESS.DLL" Alias "_CchessTryNull@4" (ByRef pos As PositionStruct) As Long
Public Declare Sub CchessUndoNull Lib "CCHESS.DLL" Alias "_CchessUndoNull@4" (ByRef pos As PositionStruct)
Public Declare Function CchessGenMoves Lib "CCHESS.DLL" Alias "_CchessGenMoves@8" (ByRef pos As PositionStruct, ByRef lpmv As Long) As Long
Public Declare Sub CchessSetIrrev Lib "CCHESS.DLL" Alias "_CchessSetIrrev@4" (ByRef pos As PositionStruct)
Public Declare Sub CchessClearBoard Lib "CCHESS.DLL" Alias "_CchessClearBoard@4" (ByRef pos As PositionStruct)
Public Declare Sub CchessBoardMirror Lib "CCHESS.DLL" Alias "_CchessBoardMirror@4" (ByRef pos As PositionStruct)
Public Declare Sub CchessExchangeSide Lib "CCHESS.DLL" Alias "_CchessExchangeSide@4" (ByRef pos As PositionStruct)
Public Declare Sub CchessFlipBoard Lib "CCHESS.DLL" Alias "_CchessFlipBoard@4" (ByRef pos As PositionStruct)
Public Declare Function CchessBoardText Lib "CCHESS.DLL" Alias "_CchessBoardText@8" (ByRef pos As PositionStruct, Optional ByVal bAnsi As Long = 0) As Long
Public Declare Function CchessBoard2Fen Lib "CCHESS.DLL" Alias "_CchessBoard2Fen@4" (ByRef pos As PositionStruct) As Long
Public Declare Sub CchessFen2Board Lib "CCHESS.DLL" Alias "_CchessFen2Board@8" (ByRef pos As PositionStruct, ByVal szFen As String)
Public Declare Function CchessFenMirror Lib "CCHESS.DLL" Alias "_CchessFenMirror@4" (ByVal szFenSrc As String) As Long
Public Declare Function CchessFileMirror Lib "CCHESS.DLL" Alias "_CchessFileMirror@4" (ByVal dwFileStr As Long) As Long
Public Declare Function CchessChin2File Lib "CCHESS.DLL" Alias "_CchessChin2File@8" (ByVal qwChinStr As Currency) As Long
Public Declare Function CchessFile2Chin Lib "CCHESS.DLL" Alias "_CchessFile2Chin@8" (ByVal dwFileStr As Long, ByVal sd As Long) As Currency
Public Declare Function CchessFile2Move Lib "CCHESS.DLL" Alias "_CchessFile2Move@8" (ByVal dwFileStr As Long, ByRef pos As PositionStruct) As Long
Public Declare Function CchessMove2File Lib "CCHESS.DLL" Alias "_CchessMove2File@8" (ByVal mv As Long, ByRef pos As PositionStruct) As Long

Public Declare Function EccoVersion Lib "ECCO.DLL" Alias "_EccoVersion@0" () As Long
Public Declare Sub EccoInitOpenVar Lib "ECCO.DLL" Alias "_EccoInitOpenVar@4" (Optional ByVal bTraditional As Long = 0)
Public Declare Function EccoOpening Lib "ECCO.DLL" Alias "_EccoOpening@4" (ByVal dwEccoIndex As Long) As Long
Public Declare Function EccoVariation Lib "ECCO.DLL" Alias "_EccoVariation@4" (ByVal dwEccoIndex As Long) As Long
Public Declare Function EccoIndex Lib "ECCO.DLL" Alias "_EccoIndex@4" (ByVal szFileMove As String) As Long

Public Const MOVE_ILLEGAL           As Long = 256
Public Const MOVE_INCHECK           As Long = 128
Public Const MOVE_DRAW              As Long = 64
Public Const MOVE_PERPETUAL_LOSS    As Long = 32
Public Const MOVE_PERPETUAL_WIN     As Long = 16
Public Const MOVE_PERPETUAL         As Long = 8
Public Const MOVE_MATE              As Long = 4
Public Const MOVE_CHECK             As Long = 2
Public Const MOVE_CAPTURE           As Long = 1

Public Const CCHESS_TRADITIONAL     As Long = 1
Public Const CCHESS_DELETE_PIECE    As Long = 1
Public Const CCHESS_ANSI_TEXT       As Long = 1

Public Const CCHESS_START_FEN       As String = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w"

Private Declare Sub RtlMoveMemory Lib "KERNEL32.DLL" (ByVal lpDst As Long, ByVal szSrc As String, ByVal dwCount As Long)
Private Declare Function SysAllocStringByteLen Lib "OLEAUT32.DLL" (ByVal lpsz As Long, ByVal dwLen As Long) As String

Public Function PieceType(ByVal nArg As Integer) As Integer

PieceType = Choose(nArg - 15, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6)

End Function

Public Function PieceByte(ByVal nArg As Integer) As String

PieceByte = Choose(nArg + 1, "K", "A", "B", "N", "R", "C", "P")

End Function

Public Function Src(ByVal mv As Long) As Integer

Src = mv Mod 256

End Function

Public Function Dst(ByVal mv As Long) As Integer

Dst = mv \ 256

End Function

Public Function Move(ByVal sqSrc As Integer, ByVal sqDst As Integer) As Long

Move = sqSrc + sqDst * 256&

End Function

Public Function MoveMirror(ByVal mv As Long) As Long

Dim sqSrc As Integer, sqDst As Integer
sqSrc = (Src(mv) \ 16) * 16 + 14 - Src(mv) Mod 16
sqDst = (Dst(mv) \ 16) * 16 + 14 - Dst(mv) Mod 16
If sqSrc >= 0 And sqSrc <= 255 And sqDst >= 0 And sqDst <= 255 Then
    MoveMirror = Move(sqSrc, sqDst)
Else
    MoveMirror = 0
End If

End Function

Public Function Coord2Move(ByVal szCoord As String) As Long

Dim sqSrc As Integer, sqDst As Integer
If Len(szCoord) < 4 Then
    Coord2Move = 0
Else
    sqSrc = (60 - Asc(Mid(szCoord, 2, 1))) * 16 + Asc(Mid(szCoord, 1, 1)) - 94
    sqDst = (60 - Asc(Mid(szCoord, 4, 1))) * 16 + Asc(Mid(szCoord, 3, 1)) - 94
    If sqSrc >= 0 And sqSrc <= 255 And sqDst >= 0 And sqDst <= 255 Then
        Coord2Move = Move(sqSrc, sqDst)
    Else
        Coord2Move = 0
    End If
End If

End Function

Public Function Move2Coord(ByVal mv As Long) As String

Move2Coord = Chr(Src(mv) Mod 16 + 94) & Chr(60 - Src(mv) \ 16) & Chr(Dst(mv) Mod 16 + 94) & Chr(60 - Dst(mv) \ 16)

End Function

Public Function Iccs2Move(ByVal szIccs As String) As Long

Dim sqSrc As Integer, sqDst As Integer
If Len(szIccs) < 5 Then
    Iccs2Move = 0
Else
    sqSrc = (60 - Asc(Mid(szIccs, 2, 1))) * 16 + Asc(Mid(szIccs, 1, 1)) - 62
    sqDst = (60 - Asc(Mid(szIccs, 5, 1))) * 16 + Asc(Mid(szIccs, 4, 1)) - 62
    If sqSrc >= 0 And sqSrc <= 255 And sqDst >= 0 And sqDst <= 255 Then
        Iccs2Move = Move(sqSrc, sqDst)
    Else
        Iccs2Move = 0
    End If
End If

End Function

Public Function Move2Iccs(ByVal mv As Long) As String

Move2Iccs = Chr(Src(mv) Mod 16 + 62) & Chr(60 - Src(mv) \ 16) & "-" & Chr(Dst(mv) Mod 16 + 62) & Chr(60 - Dst(mv) \ 16)

End Function

Public Function CvL(ByVal sz As String) As Long

Dim dw As Long
RtlMoveMemory VarPtr(dw), sz, 4
CvL = dw

End Function

Public Function CvC(ByVal sz As String) As Currency

Dim qw As Currency
RtlMoveMemory VarPtr(qw), sz, 8
CvC = qw

End Function

Public Function MkL(ByVal dw As Long) As String

MkL = SysAllocStringByteLen(VarPtr(dw), 4)

End Function

Public Function MkC(ByVal qw As Currency) As String

MkC = SysAllocStringByteLen(VarPtr(qw), 8)

End Function
