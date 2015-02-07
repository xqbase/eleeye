VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmUcciTree 
   Caption         =   "搜索树分析器"
   ClientHeight    =   3195
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
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.ComboBox cmbSend 
      Height          =   300
      ItemData        =   "UCCITREE.frx":0000
      Left            =   0
      List            =   "UCCITREE.frx":000D
      TabIndex        =   0
      Top             =   120
      Width           =   2775
   End
   Begin VB.CommandButton btnExit 
      Cancel          =   -1  'True
      Caption         =   "退出"
      Height          =   375
      Left            =   3840
      TabIndex        =   2
      Top             =   120
      Width           =   735
   End
   Begin VB.CommandButton btnSend 
      Caption         =   "发送"
      Height          =   375
      Left            =   3000
      TabIndex        =   1
      Top             =   120
      Width           =   735
   End
   Begin MSComctlLib.StatusBar stb 
      Align           =   2  'Align Bottom
      Height          =   255
      Left            =   0
      TabIndex        =   4
      Top             =   2940
      Width           =   4680
      _ExtentX        =   8255
      _ExtentY        =   450
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Object.Width           =   16933
            MinWidth        =   16933
            Key             =   "Info"
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.TreeView tree 
      Height          =   2295
      Left            =   0
      TabIndex        =   3
      Top             =   600
      Width           =   4575
      _ExtentX        =   8070
      _ExtentY        =   4048
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
         NumListImages   =   5
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "UCCITREE.frx":0044
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "UCCITREE.frx":019E
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "UCCITREE.frx":02F8
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "UCCITREE.frx":0452
            Key             =   ""
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "UCCITREE.frx":05AC
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   0
      Left            =   0
      Picture         =   "UCCITREE.frx":0706
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   1
      Left            =   240
      Picture         =   "UCCITREE.frx":0A52
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   2
      Left            =   480
      Picture         =   "UCCITREE.frx":0DD8
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   3
      Left            =   720
      Picture         =   "UCCITREE.frx":1156
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   4
      Left            =   960
      Picture         =   "UCCITREE.frx":14D9
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   5
      Left            =   1200
      Picture         =   "UCCITREE.frx":185B
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   6
      Left            =   1440
      Picture         =   "UCCITREE.frx":1BD8
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   7
      Left            =   1680
      Picture         =   "UCCITREE.frx":1F5E
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   8
      Left            =   1920
      Picture         =   "UCCITREE.frx":22DC
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   9
      Left            =   2160
      Picture         =   "UCCITREE.frx":2660
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   10
      Left            =   2400
      Picture         =   "UCCITREE.frx":29D6
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   11
      Left            =   2640
      Picture         =   "UCCITREE.frx":2D5B
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   12
      Left            =   2880
      Picture         =   "UCCITREE.frx":30DD
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   13
      Left            =   3120
      Picture         =   "UCCITREE.frx":345A
      Top             =   600
      Width           =   240
   End
   Begin VB.Image imgPiecesMini 
      Height          =   240
      Index           =   14
      Left            =   3360
      Picture         =   "UCCITREE.frx":37E0
      Top             =   600
      Width           =   240
   End
End
Attribute VB_Name = "frmUcciTree"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' Search Tree Analyst - for UCCI Engines
' Designed by Morning Yellow, Version: 3.01, Last Modified: Nov. 2007
' Copyright (C) 2004-2007 www.elephantbase.net
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

Private Sub Form_UnLoad(nCancel As Integer)

App_bRunning = False

End Sub

Private Sub Form_Resize()

If WindowState = vbMinimized Then
    Exit Sub
End If
If Height < 3600 Then
    Height = 3600
End If
If Width < 4800 Then
    Width = 4800
End If
cmbSend.Width = Width - 1920
btnSend.Left = Width - 1800
btnExit.Left = Width - 960
tree.Height = Height - 1320
tree.Width = Width - 120

End Sub

Private Sub btnSend_Click()

Dim sz As String, i As Integer, j As Integer
Dim mv As Long, nStatus As Long
If cmbSend.Text <> vbNullString Then
    sz = cmbSend.Text
    If UCase(Left(sz, 4)) = "QUIT" Then
        MsgBox "请按“退出”按钮！", vbExclamation
        Exit Sub
    End If
    PipeOut sz
    If Left(sz, 9) = "position " Then
        i = InStr(sz, " moves ")
        If Mid(sz, 10, 4) = "fen " Then
            If i > 0 Then
                CchessFen2Board App_posRoot, Mid(sz, 14, i - 14)
            Else
                CchessFen2Board App_posRoot, Mid(sz, 14)
            End If
        ElseIf Mid(sz, 10, 8) = "startpos" Then
            CchessFen2Board App_posRoot, CCHESS_START_FEN
        End If
        If i > 0 Then
            i = i + 7
            Do While i < Len(sz)
                mv = Coord2Move(Mid(sz, i))
                CchessTryMove App_posRoot, nStatus, mv
                i = i + 5
            Loop
        End If
        DrawRoot
    ElseIf Left(sz, 3) = "go " Then
        tree.Nodes.Clear
        App_nNodeNum = 0
    ElseIf Left(sz, 20) = "setoption promotion " Then
        If Mid(sz, 21) = "on" Or Mid(sz, 21) = "true" Then
            CchessPromotion 1
        Else
            CchessPromotion 0
        End If
    End If
End If
cmbSend.Text = ""

End Sub

Private Sub btnExit_Click()

Unload Me

End Sub

Private Sub tree_NodeClick(ByVal n As Node)

Dim pos As PositionStruct
Dim nIndex As Integer

nIndex = n.Index - 1
cmbSend.List(0) = "position fen " + App_Nodes(nIndex).szFen + " - - 0 1"
CchessFen2Board pos, App_Nodes(nIndex).szFen
App_szCurrFen = App_Nodes(nIndex).szFen
DrawCurr pos
If Not App_Nodes(nIndex).bExp Then
    App_Nodes(nIndex).bExp = True
    AddNode pos, App_Nodes(nIndex).mvBest, nIndex
End If

End Sub
