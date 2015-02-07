VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{FE0065C0-1B7B-11CF-9D53-00AA003C9CB6}#1.1#0"; "COMCT232.OCX"
Begin VB.Form frmViewBook 
   Caption         =   "浏览开局库"
   ClientHeight    =   4080
   ClientLeft      =   60
   ClientTop       =   345
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
   Icon            =   "VIEWBOOK.frx":0000
   MinButton       =   0   'False
   ScaleHeight     =   4080
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin ComCtl2.UpDown updnScore 
      Height          =   270
      Left            =   1680
      TabIndex        =   3
      Top             =   2760
      Width           =   195
      _ExtentX        =   344
      _ExtentY        =   476
      _Version        =   327681
      AutoBuddy       =   -1  'True
      BuddyControl    =   "txtScore"
      BuddyDispid     =   196615
      OrigLeft        =   1680
      OrigTop         =   2760
      OrigRight       =   1875
      OrigBottom      =   3015
      Max             =   65535
      SyncBuddy       =   -1  'True
      BuddyProperty   =   0
      Enabled         =   0   'False
   End
   Begin VB.CommandButton btnModify 
      Caption         =   "修改"
      Enabled         =   0   'False
      Height          =   375
      Left            =   600
      TabIndex        =   4
      Top             =   3120
      Width           =   1215
   End
   Begin VB.TextBox txtScore 
      Enabled         =   0   'False
      Height          =   270
      Left            =   1080
      TabIndex        =   2
      Top             =   2760
      Width           =   615
   End
   Begin VB.CommandButton btnExit 
      Cancel          =   -1  'True
      Caption         =   "关闭"
      Default         =   -1  'True
      Height          =   375
      Left            =   600
      TabIndex        =   5
      Top             =   3600
      Width           =   1215
   End
   Begin MSComctlLib.ImageList imglst 
      Left            =   0
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   15
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":0ABA
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":0C14
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":0D6E
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":0EC8
            Key             =   ""
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":1022
            Key             =   ""
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":117C
            Key             =   ""
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":12D6
            Key             =   ""
         EndProperty
         BeginProperty ListImage8 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":1430
            Key             =   ""
         EndProperty
         BeginProperty ListImage9 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":158A
            Key             =   ""
         EndProperty
         BeginProperty ListImage10 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":16E4
            Key             =   ""
         EndProperty
         BeginProperty ListImage11 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":183E
            Key             =   ""
         EndProperty
         BeginProperty ListImage12 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":1998
            Key             =   ""
         EndProperty
         BeginProperty ListImage13 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":1AF2
            Key             =   ""
         EndProperty
         BeginProperty ListImage14 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":1C4C
            Key             =   ""
         EndProperty
         BeginProperty ListImage15 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "VIEWBOOK.frx":1DA6
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.TreeView tree 
      Height          =   3975
      Left            =   2400
      TabIndex        =   0
      Top             =   0
      Width           =   2175
      _ExtentX        =   3836
      _ExtentY        =   7011
      _Version        =   393217
      Indentation     =   212
      LabelEdit       =   1
      LineStyle       =   1
      Style           =   7
      ImageList       =   "imglst"
      Appearance      =   1
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "宋体"
         Size            =   9
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin VB.Label lblScore 
      Caption         =   "评分"
      Height          =   255
      Left            =   600
      TabIndex        =   1
      Top             =   2760
      Width           =   495
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   89
      Left            =   2040
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   88
      Left            =   1800
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   87
      Left            =   1560
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   86
      Left            =   1320
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   85
      Left            =   1080
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   84
      Left            =   840
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   83
      Left            =   600
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   82
      Left            =   360
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   81
      Left            =   120
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   80
      Left            =   2040
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   79
      Left            =   1800
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   78
      Left            =   1560
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   77
      Left            =   1320
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   76
      Left            =   1080
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   75
      Left            =   840
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   74
      Left            =   600
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   73
      Left            =   360
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   72
      Left            =   120
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   71
      Left            =   2040
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   70
      Left            =   1800
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   69
      Left            =   1560
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   68
      Left            =   1320
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   67
      Left            =   1080
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   66
      Left            =   840
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   65
      Left            =   600
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   64
      Left            =   360
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   63
      Left            =   120
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   62
      Left            =   2040
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   61
      Left            =   1800
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   60
      Left            =   1560
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   59
      Left            =   1320
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   58
      Left            =   1080
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   57
      Left            =   840
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   56
      Left            =   600
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   55
      Left            =   360
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   54
      Left            =   120
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   53
      Left            =   2040
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   52
      Left            =   1800
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   51
      Left            =   1560
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   50
      Left            =   1320
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   49
      Left            =   1080
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   48
      Left            =   840
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   47
      Left            =   600
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   46
      Left            =   360
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   45
      Left            =   120
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   44
      Left            =   2040
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   43
      Left            =   1800
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   42
      Left            =   1560
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   41
      Left            =   1320
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   40
      Left            =   1080
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   39
      Left            =   840
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   38
      Left            =   600
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   37
      Left            =   360
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   36
      Left            =   120
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   35
      Left            =   2040
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   34
      Left            =   1800
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   33
      Left            =   1560
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   32
      Left            =   1320
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   31
      Left            =   1080
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   30
      Left            =   840
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   29
      Left            =   600
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   28
      Left            =   360
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   27
      Left            =   120
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   26
      Left            =   2040
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   25
      Left            =   1800
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   24
      Left            =   1560
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   23
      Left            =   1320
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   22
      Left            =   1080
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   21
      Left            =   840
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   20
      Left            =   600
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   19
      Left            =   360
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   18
      Left            =   120
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   17
      Left            =   2040
      Top             =   360
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   16
      Left            =   1800
      Top             =   360
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   15
      Left            =   1560
      Top             =   360
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   14
      Left            =   1320
      Top             =   360
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   13
      Left            =   1080
      Top             =   360
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   12
      Left            =   840
      Top             =   360
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   11
      Left            =   600
      Top             =   360
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   10
      Left            =   360
      Top             =   360
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   9
      Left            =   120
      Top             =   360
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   8
      Left            =   2040
      Top             =   120
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   7
      Left            =   1800
      Top             =   120
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   6
      Left            =   1560
      Top             =   120
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   5
      Left            =   1320
      Top             =   120
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   4
      Left            =   1080
      Top             =   120
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   3
      Left            =   840
      Top             =   120
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   2
      Left            =   600
      Top             =   120
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   1
      Left            =   360
      Top             =   120
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   0
      Left            =   120
      Top             =   120
      Width           =   255
   End
   Begin VB.Image imgBoard 
      Height          =   2655
      Left            =   0
      Picture         =   "VIEWBOOK.frx":1F00
      Stretch         =   -1  'True
      Top             =   0
      Width           =   2415
   End
End
Attribute VB_Name = "frmViewBook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ElephantEye Book Maker - a Chinese Chess Book Maker Program for ElephantEye
' Designed by Morning Yellow, Version: 3.25, Last Modified: Apr. 2011
' Copyright (C) 2004-2011 www.xqbase.com
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

Private Type BookStruct
    nPtr As Long
    wmv As Integer
    wvl As Integer
End Type

Private Declare Function GetBookMoves Lib "MAKEBOOK.DLL" Alias "_GetBookMoves@12" (ByRef pos As PositionStruct, _
        ByVal szFile As String, ByRef bks As BookStruct) As Long
Private Declare Function PutBookMove Lib "MAKEBOOK.DLL" Alias "_PutBookMove@8" (ByVal szFile As String, ByRef bks As BookStruct) As Long

Private Const MAX_NODE_NUM As Integer = 32767
Private szBookFile As String
Private nCurr As Integer, bkCurr As BookStruct
Private Nodes_szFen(0 To MAX_NODE_NUM) As String
Private Nodes_szFile(0 To MAX_NODE_NUM) As String
Private Nodes_bExpanded(0 To MAX_NODE_NUM) As Boolean
Private Nodes_bks(0 To MAX_NODE_NUM) As BookStruct
Private Nodes_nIndex As Integer

Private Sub AddNode(ByRef pos As PositionStruct, ByRef bk As BookStruct, Optional ByVal nIndex As Integer = -1)

Dim nStatus As Long, sz As String, dwFile As Long, mv As Long
Dim pc As Integer, nPicId As Integer

If Nodes_nIndex = MAX_NODE_NUM Then
    Exit Sub
End If
mv = bk.wmv And &HFFFF&
pc = pos.ucpcSquares(Src(mv))
If pc = 0 Then
    nPicId = 1
Else
    nPicId = PieceType(pc) + IIf(pc < 32, 2, 9)
End If
dwFile = CchessMove2File(mv, pos)
sz = Mid(MkC(CchessFile2Chin(dwFile, pos.sdPlayer)), 2) & "(" & (bk.wvl And &HFFFF&) & ")"
If nIndex = -1 Then
    tree.Nodes.Add , , , sz, nPicId
Else
    tree.Nodes.Add nIndex + 1, tvwChild, , sz, nPicId
End If
CchessTryMove pos, nStatus, mv
Nodes_szFen(Nodes_nIndex) = AllocString(CchessBoard2Fen(pos))
If nIndex = -1 Then
    Nodes_szFile(Nodes_nIndex) = MkL(dwFile)
Else
    Nodes_szFile(Nodes_nIndex) = Nodes_szFile(nIndex) & MkL(dwFile)
End If
CchessUndoMove pos
Nodes_bExpanded(Nodes_nIndex) = False
Nodes_bks(Nodes_nIndex) = bk
Nodes_nIndex = Nodes_nIndex + 1

End Sub

Private Sub DrawPos(ByRef pos As PositionStruct)

Dim i As Integer, j As Integer, pc As Integer, nPicId As Integer
For i = 3 To 12
    For j = 3 To 11
        pc = pos.ucpcSquares(i * 16 + j)
        If pc = 0 Then
            nPicId = 1
        Else
            nPicId = PieceType(pc) + IIf(pc < 32, 2, 9)
        End If
        imgSquares((i - 3) * 9 + j - 3).Picture = imglst.ListImages.Item(nPicId).Picture
    Next
Next

End Sub

Private Sub Form_Load()

Dim i As Integer, nMoveNum As Integer
Dim bks(1 To 128) As BookStruct
Dim pos As PositionStruct

tree.Nodes.Clear
Nodes_nIndex = 0
nCurr = -1
CchessFen2Board pos, CCHESS_START_FEN
DrawPos pos
szBookFile = frmMakeBook.txtBookFile.Text
nMoveNum = GetBookMoves(pos, szBookFile, bks(1))
If nMoveNum = 0 Then
    MsgBox "开局库文件 " & szBookFile & " 格式不正确！", vbExclamation
End If
For i = 1 To nMoveNum
    AddNode pos, bks(i)
Next

End Sub

Private Sub Form_Resize()

If Height < 4485 Then
    Height = 4485
End If
If Width < 4800 Then
    Width = 4800
End If
tree.Height = Height - 405
tree.Width = Width - 2520

End Sub

Private Sub tree_NodeClick(ByVal n As Node)

Dim i As Integer, nMoveNum As Integer
Dim dwEcco As Long, szEcco As String, szOpen As String, szVar As String
Dim bks(1 To 128) As BookStruct
Dim pos As PositionStruct

nCurr = n.Index - 1
CchessFen2Board pos, Nodes_szFen(nCurr)
DrawPos pos
If Not Nodes_bExpanded(nCurr) Then
    Nodes_bExpanded(nCurr) = True
    nMoveNum = GetBookMoves(pos, szBookFile, bks(1))
    For i = 1 To nMoveNum
        AddNode pos, bks(i), nCurr
    Next
End If

txtScore.Enabled = True
updnScore.Enabled = True
btnModify.Enabled = True
bkCurr = Nodes_bks(nCurr)
txtScore.Text = bkCurr.wvl And &HFFFF&

dwEcco = EccoIndex(Nodes_szFile(nCurr))
szEcco = Left(MkL(dwEcco), 3)
szOpen = AllocString(EccoOpening(dwEcco))
szVar = AllocString(EccoVariation(dwEcco))
Caption = "浏览开局库 - " & szOpen & IIf(szVar = "", "", "——") & szVar & "(" & szEcco & ")"

End Sub

Private Sub txtScore_Change()

Dim n As Long
If nCurr < 0 Then
    Exit Sub
End If
If (bkCurr.wvl And &HFFFF&) <> Str2Lng(txtScore.Text) Then
    n = Str2Lng(txtScore.Text, 0, 65535)
    bkCurr.wvl = IIf(n < 32768, n, n - 65536)
    txtScore.Text = (bkCurr.wvl And &HFFFF&)
End If

End Sub

Private Sub btnModify_Click()

If nCurr < 0 Then
    Exit Sub
End If
Nodes_bks(nCurr) = bkCurr
If PutBookMove(szBookFile, bkCurr) = 0 Then
    MsgBox "无法修改开局库文件 " & szBookFile & " ！", vbExclamation
Else
    MsgBox "开局库文件 " & szBookFile & " 已修改。", vbInformation
End If

End Sub

Private Sub btnExit_Click()

Unload Me

End Sub
