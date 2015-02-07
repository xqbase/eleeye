VERSION 5.00
Begin VB.Form frmTestPipe 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Pipe Test"
   ClientHeight    =   3195
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4680
   MaxButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.ListBox lstOutput 
      Height          =   2595
      ItemData        =   "TESTPIPE.frx":0000
      Left            =   120
      List            =   "TESTPIPE.frx":0002
      TabIndex        =   2
      Top             =   480
      Width           =   4455
   End
   Begin VB.CommandButton btnSend 
      Caption         =   "Send"
      Height          =   375
      Left            =   3720
      TabIndex        =   1
      Top             =   0
      Width           =   855
   End
   Begin VB.TextBox txtSend 
      Height          =   285
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3495
   End
End
Attribute VB_Name = "frmTestPipe"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub btnSend_Click()

If txtSend.Text = vbNullString Then
    PipeLineOutput pipe, ""
Else
    PipeLineOutput pipe, txtSend.Text
End If
txtSend.Text = ""

End Sub

Private Sub Form_UnLoad(nCancel As Integer)

bRunning = False

End Sub
