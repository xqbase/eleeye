VERSION 5.00
Object = "{FE0065C0-1B7B-11CF-9D53-00AA003C9CB6}#1.1#0"; "COMCT232.OCX"
Begin VB.Form frmMakeBook 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "ElephantEye 开局库管理工具"
   ClientHeight    =   3195
   ClientLeft      =   45
   ClientTop       =   330
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
   Icon            =   "MAKEBOOK.frx":0000
   MaxButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.Frame frmMain 
      Caption         =   "文件和棋谱"
      Height          =   1215
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   4455
      Begin VB.CommandButton btnPgnPath 
         Caption         =   "浏览"
         Height          =   375
         Left            =   3600
         TabIndex        =   6
         Top             =   720
         Width           =   735
      End
      Begin VB.TextBox txtPgnPath 
         BackColor       =   &H8000000F&
         Height          =   270
         Left            =   1080
         Locked          =   -1  'True
         TabIndex        =   5
         Top             =   720
         Width           =   2415
      End
      Begin VB.CommandButton btnBookFile 
         Caption         =   "浏览"
         Height          =   375
         Left            =   3600
         TabIndex        =   3
         Top             =   240
         Width           =   735
      End
      Begin VB.TextBox txtBookFile 
         BackColor       =   &H8000000F&
         Height          =   270
         Left            =   1080
         Locked          =   -1  'True
         TabIndex        =   2
         Top             =   240
         Width           =   2415
      End
      Begin VB.Label lblPgnPath 
         Caption         =   "棋谱文件夹"
         Height          =   255
         Left            =   120
         TabIndex        =   4
         Top             =   720
         Width           =   975
      End
      Begin VB.Label lblBookFile 
         Caption         =   "开局库文件"
         Height          =   255
         Left            =   120
         TabIndex        =   1
         Top             =   240
         Width           =   975
      End
   End
   Begin VB.Frame frmOptions 
      Caption         =   "开局库参数"
      Height          =   1095
      Left            =   120
      TabIndex        =   7
      Top             =   1440
      Width           =   4455
      Begin ComCtl2.UpDown updnRatio 
         Height          =   270
         Left            =   2400
         TabIndex        =   19
         Top             =   720
         Width           =   195
         _ExtentX        =   344
         _ExtentY        =   476
         _Version        =   327681
         Value           =   4
         AutoBuddy       =   -1  'True
         BuddyControl    =   "txtRatio"
         BuddyDispid     =   196617
         OrigLeft        =   2400
         OrigTop         =   720
         OrigRight       =   2595
         OrigBottom      =   975
         Max             =   9
         Min             =   1
         SyncBuddy       =   -1  'True
         BuddyProperty   =   0
         Enabled         =   -1  'True
      End
      Begin ComCtl2.UpDown updnWin 
         Height          =   270
         Left            =   1680
         TabIndex        =   11
         Top             =   240
         Width           =   195
         _ExtentX        =   344
         _ExtentY        =   476
         _Version        =   327681
         Value           =   3
         AutoBuddy       =   -1  'True
         BuddyControl    =   "txtWin"
         BuddyDispid     =   196620
         OrigLeft        =   1680
         OrigTop         =   240
         OrigRight       =   1875
         OrigBottom      =   495
         Max             =   5
         Min             =   -5
         SyncBuddy       =   -1  'True
         BuddyProperty   =   0
         Enabled         =   -1  'True
      End
      Begin ComCtl2.UpDown updnDraw 
         Height          =   270
         Left            =   2760
         TabIndex        =   14
         Top             =   240
         Width           =   195
         _ExtentX        =   344
         _ExtentY        =   476
         _Version        =   327681
         Value           =   1
         AutoBuddy       =   -1  'True
         BuddyControl    =   "txtDraw"
         BuddyDispid     =   196619
         OrigLeft        =   2760
         OrigTop         =   240
         OrigRight       =   2955
         OrigBottom      =   495
         Max             =   5
         Min             =   -5
         SyncBuddy       =   -1  'True
         BuddyProperty   =   0
         Enabled         =   -1  'True
      End
      Begin ComCtl2.UpDown updnLoss 
         Height          =   270
         Left            =   3840
         TabIndex        =   17
         Top             =   240
         Width           =   195
         _ExtentX        =   344
         _ExtentY        =   476
         _Version        =   327681
         Value           =   -1
         AutoBuddy       =   -1  'True
         BuddyControl    =   "txtLoss"
         BuddyDispid     =   196618
         OrigLeft        =   3840
         OrigTop         =   240
         OrigRight       =   4035
         OrigBottom      =   495
         Max             =   5
         Min             =   -5
         SyncBuddy       =   -1  'True
         BuddyProperty   =   0
         Enabled         =   -1  'True
      End
      Begin VB.TextBox txtRatio 
         BackColor       =   &H8000000F&
         Height          =   270
         Left            =   2160
         Locked          =   -1  'True
         TabIndex        =   18
         Text            =   "4"
         Top             =   720
         Width           =   270
      End
      Begin VB.TextBox txtLoss 
         BackColor       =   &H8000000F&
         Height          =   270
         Left            =   3600
         Locked          =   -1  'True
         TabIndex        =   16
         Text            =   "-1"
         Top             =   240
         Width           =   270
      End
      Begin VB.TextBox txtDraw 
         BackColor       =   &H8000000F&
         Height          =   270
         Left            =   2520
         Locked          =   -1  'True
         TabIndex        =   13
         Text            =   "1"
         Top             =   240
         Width           =   270
      End
      Begin VB.TextBox txtWin 
         BackColor       =   &H8000000F&
         Height          =   270
         Left            =   1440
         Locked          =   -1  'True
         TabIndex        =   10
         Text            =   "3"
         Top             =   240
         Width           =   270
      End
      Begin VB.Label lblScore 
         Caption         =   "分数 ="
         Height          =   255
         Left            =   240
         TabIndex        =   8
         Top             =   480
         Width           =   615
      End
      Begin VB.Line lnDivision 
         X1              =   840
         X2              =   4080
         Y1              =   600
         Y2              =   600
      End
      Begin VB.Label lblLoss 
         Caption         =   "+负局x"
         Height          =   255
         Left            =   3000
         TabIndex        =   15
         Top             =   240
         Width           =   615
      End
      Begin VB.Label lblDraw 
         Caption         =   "+和局x"
         Height          =   255
         Left            =   1920
         TabIndex        =   12
         Top             =   240
         Width           =   615
      End
      Begin VB.Label lblWin 
         Caption         =   "胜局x"
         Height          =   255
         Left            =   960
         TabIndex        =   9
         Top             =   240
         Width           =   495
      End
   End
   Begin VB.CommandButton btnExit 
      Cancel          =   -1  'True
      Caption         =   "退出"
      Height          =   375
      Left            =   3240
      TabIndex        =   22
      Top             =   2640
      Width           =   1095
   End
   Begin VB.CommandButton btnViewBook 
      Caption         =   "浏览开局库"
      Height          =   375
      Left            =   1800
      TabIndex        =   21
      Top             =   2640
      Width           =   1215
   End
   Begin VB.CommandButton btnMakeBook 
      Caption         =   "制作开局库"
      Height          =   375
      Left            =   360
      TabIndex        =   20
      Top             =   2640
      Width           =   1215
   End
End
Attribute VB_Name = "frmMakeBook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' ElephantEye Book Maker - a Chinese Chess Book Maker Program for ElephantEye
' Designed by Morning Yellow, Version: 3.13, Last Modified: Mar. 2008
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

Private Declare Function MakeBook Lib "MAKEBOOK.DLL" Alias "_MakeBook@24" (ByVal szPath As String, ByVal szFile As String, _
        ByVal nWin As Long, ByVal nDraw As Long, ByVal nLoss As Long, ByVal nRatio As Long) As Long

Private fs As Object

Private Sub Form_Load()

CchessInit
EccoInitOpenVar
txtBookFile.Text = App.Path & IIf(Right(App.Path, 1) = "\", "", "\") & "BOOK.DAT"
Set fs = CreateObject("Scripting.FileSystemObject")

End Sub

Private Sub btnBookFile_Click()

Dim sz As String
sz = SaveFileDialog("开局库文件", "开局库文件 (*.DAT)|*.DAT|所有文件 (*.*)|*.*", txtBookFile.Text, "DAT")
If sz <> "" Then
    txtBookFile.Text = sz
End If

End Sub

Private Sub btnPgnPath_Click()

Dim sz As String
sz = BrowseForFolder("请选择棋谱所在的文件夹")
If sz <> "" Then
    txtPgnPath.Text = sz
End If

End Sub

Private Sub btnMakeBook_Click()

If txtBookFile.Text = vbNullString Then
    MsgBox "没有指定开局库文件！", vbExclamation
    Exit Sub
End If
If txtPgnPath.Text = vbNullString Then
    MsgBox "没有指定棋谱文件夹！", vbExclamation
    Exit Sub
End If
If fs.FileExists(txtBookFile.Text) Then
    If MsgBox(txtBookFile.Text & " 已存在。" & vbCrLf & "要替换它吗?", vbExclamation + vbYesNo) <> vbYes Then
        Exit Sub
    End If
End If
If MakeBook(txtPgnPath.Text, txtBookFile.Text, Val(txtWin.Text), Val(txtDraw.Text), Val(txtLoss.Text), Val(txtRatio.Text)) Then
    MsgBox "开局库制作成功！", vbInformation
Else
    MsgBox "开局库制作失败！", vbExclamation
End If

End Sub

Private Sub btnViewBook_Click()

If txtBookFile.Text = vbNullString Then
    MsgBox "没有指定开局库文件！", vbExclamation
    Exit Sub
End If
If fs.FileExists(txtBookFile.Text) Then
    frmViewBook.Show vbModal, Me
Else
    MsgBox "开局库文件 " & frmMakeBook.txtBookFile.Text & " 不存在！", vbExclamation
End If

End Sub

Private Sub btnExit_Click()

Unload Me

End Sub
