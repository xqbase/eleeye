VERSION 5.00
Begin VB.Form frmBbsChess 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "BBS Chess"
   ClientHeight    =   4425
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   7050
   BeginProperty Font 
      Name            =   "宋体"
      Size            =   9
      Charset         =   134
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Icon            =   "BBSCHESS.frx":0000
   MaxButton       =   0   'False
   ScaleHeight     =   4425
   ScaleWidth      =   7050
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnExit 
      Cancel          =   -1  'True
      Caption         =   "退出"
      Height          =   375
      Left            =   6000
      TabIndex        =   22
      Top             =   3600
      Width           =   975
   End
   Begin VB.CommandButton btnCopy 
      Caption         =   "复制棋盘"
      Height          =   375
      Left            =   6000
      TabIndex        =   21
      Top             =   3120
      Width           =   975
   End
   Begin VB.CommandButton btnLeftRight 
      Caption         =   "左右翻转"
      Height          =   375
      Left            =   6000
      TabIndex        =   20
      Top             =   2520
      Width           =   975
   End
   Begin VB.CommandButton btnUpDown 
      Caption         =   "上下翻转"
      Height          =   375
      Left            =   6000
      TabIndex        =   19
      Top             =   2040
      Width           =   975
   End
   Begin VB.Frame frmPieces 
      Caption         =   "放置棋子"
      Height          =   3735
      Left            =   120
      TabIndex        =   1
      Top             =   240
      Width           =   1815
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   12
         Left            =   960
         TabIndex        =   14
         Top             =   3240
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   11
         Left            =   960
         TabIndex        =   13
         Top             =   2760
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   10
         Left            =   960
         TabIndex        =   12
         Top             =   2280
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   9
         Left            =   960
         TabIndex        =   11
         Top             =   1800
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   8
         Left            =   960
         TabIndex        =   10
         Top             =   1320
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   7
         Left            =   960
         TabIndex        =   9
         Top             =   840
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   6
         Left            =   120
         TabIndex        =   8
         Top             =   3240
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   5
         Left            =   120
         TabIndex        =   7
         Top             =   2760
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   4
         Left            =   120
         TabIndex        =   6
         Top             =   2280
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   3
         Left            =   120
         TabIndex        =   5
         Top             =   1800
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   2
         Left            =   120
         TabIndex        =   4
         Top             =   1320
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   1
         Left            =   120
         TabIndex        =   3
         Top             =   840
         Width           =   255
      End
      Begin VB.OptionButton optPieces 
         Height          =   255
         Index           =   0
         Left            =   480
         TabIndex        =   2
         Top             =   360
         Value           =   -1  'True
         Width           =   255
      End
      Begin VB.Image imgPieces 
         Appearance      =   0  'Flat
         BorderStyle     =   1  'Fixed Single
         Height          =   510
         Index           =   0
         Left            =   840
         Picture         =   "BBSCHESS.frx":0ABA
         Top             =   240
         Width           =   510
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   12
         Left            =   1200
         Picture         =   "BBSCHESS.frx":0DC4
         Top             =   3120
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   11
         Left            =   1200
         Picture         =   "BBSCHESS.frx":10CE
         Top             =   2640
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   10
         Left            =   1200
         Picture         =   "BBSCHESS.frx":13D8
         Top             =   2160
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   9
         Left            =   1200
         Picture         =   "BBSCHESS.frx":16E2
         Top             =   1680
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   8
         Left            =   1200
         Picture         =   "BBSCHESS.frx":19EC
         Top             =   1200
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   7
         Left            =   1200
         Picture         =   "BBSCHESS.frx":1CF6
         Top             =   720
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   6
         Left            =   360
         Picture         =   "BBSCHESS.frx":2000
         Top             =   3120
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   5
         Left            =   360
         Picture         =   "BBSCHESS.frx":230A
         Top             =   2640
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   4
         Left            =   360
         Picture         =   "BBSCHESS.frx":2614
         Top             =   2160
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   3
         Left            =   360
         Picture         =   "BBSCHESS.frx":291E
         Top             =   1680
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   2
         Left            =   360
         Picture         =   "BBSCHESS.frx":2C28
         Top             =   1200
         Width           =   480
      End
      Begin VB.Image imgPieces 
         Height          =   480
         Index           =   1
         Left            =   360
         Picture         =   "BBSCHESS.frx":2F32
         Top             =   720
         Width           =   480
      End
   End
   Begin VB.CommandButton btnCounter 
      Caption         =   "逆时针转"
      Height          =   375
      Left            =   6000
      TabIndex        =   18
      Top             =   1560
      Width           =   975
   End
   Begin VB.CommandButton btnClockwise 
      Caption         =   "顺时针转"
      Height          =   375
      Left            =   6000
      TabIndex        =   17
      Top             =   1080
      Width           =   975
   End
   Begin VB.CommandButton btnClear 
      Caption         =   "清空棋盘"
      Height          =   375
      Left            =   6000
      TabIndex        =   16
      Top             =   600
      Width           =   975
   End
   Begin VB.CommandButton btnStart 
      Caption         =   "起始局面"
      Height          =   375
      Left            =   6000
      TabIndex        =   15
      Top             =   120
      Width           =   975
   End
   Begin VB.TextBox txtFen 
      Height          =   270
      Left            =   120
      TabIndex        =   0
      Top             =   4080
      Width           =   6855
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   63
      Left            =   5400
      Top             =   3480
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   62
      Left            =   4920
      Top             =   3480
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   61
      Left            =   4440
      Top             =   3480
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   60
      Left            =   3960
      Top             =   3480
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   59
      Left            =   3480
      Top             =   3480
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   58
      Left            =   3000
      Top             =   3480
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   57
      Left            =   2520
      Top             =   3480
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   56
      Left            =   2040
      Top             =   3480
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   55
      Left            =   5400
      Top             =   3000
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   54
      Left            =   4920
      Top             =   3000
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   53
      Left            =   4440
      Top             =   3000
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   52
      Left            =   3960
      Top             =   3000
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   51
      Left            =   3480
      Top             =   3000
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   50
      Left            =   3000
      Top             =   3000
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   49
      Left            =   2520
      Top             =   3000
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   48
      Left            =   2040
      Top             =   3000
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   47
      Left            =   5400
      Top             =   2520
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   46
      Left            =   4920
      Top             =   2520
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   45
      Left            =   4440
      Top             =   2520
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   44
      Left            =   3960
      Top             =   2520
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   43
      Left            =   3480
      Top             =   2520
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   42
      Left            =   3000
      Top             =   2520
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   41
      Left            =   2520
      Top             =   2520
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   40
      Left            =   2040
      Top             =   2520
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   39
      Left            =   5400
      Top             =   2040
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   38
      Left            =   4920
      Top             =   2040
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   37
      Left            =   4440
      Top             =   2040
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   36
      Left            =   3960
      Top             =   2040
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   35
      Left            =   3480
      Top             =   2040
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   34
      Left            =   3000
      Top             =   2040
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   33
      Left            =   2520
      Top             =   2040
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   32
      Left            =   2040
      Top             =   2040
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   31
      Left            =   5400
      Top             =   1560
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   30
      Left            =   4920
      Top             =   1560
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   29
      Left            =   4440
      Top             =   1560
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   28
      Left            =   3960
      Top             =   1560
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   27
      Left            =   3480
      Top             =   1560
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   26
      Left            =   3000
      Top             =   1560
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   25
      Left            =   2520
      Top             =   1560
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   24
      Left            =   2040
      Top             =   1560
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   23
      Left            =   5400
      Top             =   1080
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   22
      Left            =   4920
      Top             =   1080
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   21
      Left            =   4440
      Top             =   1080
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   20
      Left            =   3960
      Top             =   1080
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   19
      Left            =   3480
      Top             =   1080
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   18
      Left            =   3000
      Top             =   1080
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   17
      Left            =   2520
      Top             =   1080
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   16
      Left            =   2040
      Top             =   1080
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   15
      Left            =   5400
      Top             =   600
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   14
      Left            =   4920
      Top             =   600
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   13
      Left            =   4440
      Top             =   600
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   12
      Left            =   3960
      Top             =   600
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   11
      Left            =   3480
      Top             =   600
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   10
      Left            =   3000
      Top             =   600
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   9
      Left            =   2520
      Top             =   600
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   8
      Left            =   2040
      Top             =   600
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   7
      Left            =   5400
      Top             =   120
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   6
      Left            =   4920
      Top             =   120
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   5
      Left            =   4440
      Top             =   120
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   4
      Left            =   3960
      Top             =   120
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   3
      Left            =   3480
      Top             =   120
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   2
      Left            =   3000
      Top             =   120
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   1
      Left            =   2520
      Top             =   120
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   495
      Index           =   0
      Left            =   2040
      Top             =   120
      Width           =   495
   End
   Begin VB.Image imgBoard 
      Height          =   3840
      Left            =   2040
      Picture         =   "BBSCHESS.frx":323C
      Top             =   120
      Width           =   3840
   End
End
Attribute VB_Name = "frmBbsChess"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' BBS Chess - A Chess Position Edit Program
' Designed by Morning Yellow, Version: 1.1, Last Modified: May. 2008
' Copyright (C) 2004-2008 www.elephantbase.net
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

Private szFen As String, szTail As String, nSelected As Integer
Private nBoard(1 To 8, 1 To 8) As Integer

Private Sub Swap(ByRef dst As Variant, ByRef src As Variant)

Dim tmp As Variant
tmp = dst
dst = src
src = tmp

End Sub

Private Function Piece2Char(ByVal nPiece As Integer) As String

If nPiece > 6 Then
    Piece2Char = LCase(Piece2Char(nPiece - 6))
Else
    Piece2Char = Choose(nPiece, "K", "Q", "B", "N", "R", "P")
End If

End Function

Private Sub Board2Fen()

Dim i As Integer, j As Integer, k As Integer, l As Integer
k = 0
szFen = ""
For i = 1 To 8
    For j = 1 To 8
        l = nBoard(i, j)
        If l = 0 Then
            k = k + 1
        Else
            If k > 0 Then
                szFen = szFen + Chr(k + 48)
                k = 0
            End If
            szFen = szFen + Piece2Char(l)
        End If
    Next
    If k > 0 Then
        szFen = szFen + Chr(k + 48)
        k = 0
    End If
    If i < 8 Then
        szFen = szFen + "/"
    End If
Next
szFen = szFen + " " + szTail

End Sub

Private Function Char2Piece(ByVal Char As String) As Integer

Select Case Char
Case "K"
    Char2Piece = 1
Case "Q"
    Char2Piece = 2
Case "B"
    Char2Piece = 3
Case "N"
    Char2Piece = 4
Case "R"
    Char2Piece = 5
Case "P"
    Char2Piece = 6
Case Else
    Char2Piece = 0
End Select

End Function

Private Sub Fen2Board()

Dim i As Integer, j As Integer, k As Integer, l As Integer, szChar As String
i = InStr(szFen, " ")
szTail = Right(szFen, Len(szFen) - i)
j = 1
k = 1
For i = 1 To Len(szFen)
    szChar = Mid(szFen, i, 1)
    Select Case szChar
    Case "/"
        j = j + 1
        If j > 8 Then
            Exit For
        End If
        k = 1
    Case "1" To "9"
        For l = 1 To Asc(szChar) - 48
            nBoard(j, k) = 0
            k = k + 1
            If k > 8 Then
                Exit For
            End If
        Next
    Case "A" To "Z"
        If k <= 8 Then
            nBoard(j, k) = Char2Piece(szChar)
            k = k + 1
        End If
    Case "a" To "z"
        If k <= 8 Then
            nBoard(j, k) = 6 + Char2Piece(UCase(szChar))
            k = k + 1
        End If
    Case Else
        Exit For
    End Select
Next

End Sub

Private Sub DrawSquare(ByVal x As Integer, ByVal y As Integer)

imgSquares((x - 1) * 8 + (y - 1)).Picture = imgPieces(nBoard(x, y)).Picture

End Sub

Private Sub BoardFlush()

Dim i As Integer, j As Integer
For i = 1 To 8
    For j = 1 To 8
        DrawSquare i, j
    Next
Next

End Sub

Private Function Piece2Chin(ByVal nPiece As Integer, ByVal bBlackSq As Boolean) As String

Piece2Chin = Chr(27) + Chr(27) + IIf(nPiece > 6, "[0;", "[1;") + IIf(bBlackSq, "43;", "46;") + IIf(nPiece > 6, "30m", "37m") + Choose(IIf(nPiece > 6, nPiece - 6, nPiece) + 1, "　", "王", "后", "象", "马", "车", "兵")

End Function

Private Sub Form_Load()

nSelected = 0
btnStart_Click

End Sub

Private Sub btnStart_Click()

szFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
Fen2Board
txtFen.Text = szFen
BoardFlush

End Sub

Private Sub btnClear_Click()

szFen = "8/8/8/8/8/8/8/8 w - - 0 1"
Fen2Board
txtFen.Text = szFen
BoardFlush

End Sub

Private Sub btnClockwise_Click()

Dim i As Integer, j As Integer
Dim nTempBoard(1 To 8, 1 To 8) As Integer
For i = 1 To 8
    For j = 1 To 8
        nTempBoard(i, j) = nBoard(9 - j, i)
    Next
Next
For i = 1 To 8
    For j = 1 To 8
        nBoard(i, j) = nTempBoard(i, j)
    Next
Next
Board2Fen
txtFen.Text = szFen
BoardFlush

End Sub

Private Sub btnCounter_Click()

Dim i As Integer, j As Integer
Dim nTempBoard(1 To 8, 1 To 8) As Integer
For i = 1 To 8
    For j = 1 To 8
        nTempBoard(i, j) = nBoard(j, 9 - i)
    Next
Next
For i = 1 To 8
    For j = 1 To 8
        nBoard(i, j) = nTempBoard(i, j)
    Next
Next
Board2Fen
txtFen.Text = szFen
BoardFlush

End Sub

Private Sub btnUpDown_Click()

Dim i As Integer, j As Integer
For i = 1 To 4
    For j = 1 To 8
        Swap nBoard(i, j), nBoard(9 - i, j)
    Next
Next
Board2Fen
txtFen.Text = szFen
BoardFlush

End Sub

Private Sub btnLeftRight_Click()

Dim i As Integer, j As Integer
For i = 1 To 8
    For j = 1 To 4
        Swap nBoard(i, j), nBoard(i, 9 - j)
    Next
Next
Board2Fen
txtFen.Text = szFen
BoardFlush

End Sub

Private Sub btnCopy_Click()

Dim i As Integer, j As Integer, sz As String
sz = ""
For i = 1 To 8
    For j = 1 To 8
        sz = sz + Piece2Chin(nBoard(i, j), (i + j) Mod 2 = 0)
    Next
    sz = sz + Chr(27) + Chr(27) + "[0m" + vbCrLf
Next
Clipboard.Clear
Clipboard.SetText sz + szFen + vbCrLf + "欢迎访问www.elephantbase.net" + vbCrLf
MsgBox "文本棋盘已复制到剪贴板。", vbInformation

End Sub

Private Sub btnExit_Click()

Unload Me

End Sub

Private Sub txtFen_Change()

If szFen <> txtFen.Text Then
    szFen = txtFen.Text
    Fen2Board
    BoardFlush
End If

End Sub

Private Sub optPieces_Click(nIndex As Integer)

nSelected = nIndex
optPieces(nIndex).Value = True

End Sub

Private Sub imgPieces_Click(nIndex As Integer)

nSelected = nIndex
optPieces(nIndex).Value = True

End Sub

Private Sub imgSquares_MouseDown(nIndex As Integer, nButton As Integer, nShift As Integer, x As Single, y As Single)

If nButton = vbLeftButton Then
    nBoard(nIndex \ 8 + 1, nIndex Mod 8 + 1) = nSelected
ElseIf nButton = vbRightButton Then
    nBoard(nIndex \ 8 + 1, nIndex Mod 8 + 1) = 0
End If
DrawSquare nIndex \ 8 + 1, nIndex Mod 8 + 1
Board2Fen
txtFen.Text = szFen

End Sub
