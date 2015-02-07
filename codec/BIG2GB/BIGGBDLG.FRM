VERSION 5.00
Object = "{B69BA6E9-1281-11D2-8C37-0060089950F4}#3.0#0"; "BIG2GB.OCX"
Begin VB.Form frmBigGBDlg 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Big<=>GB Convertor"
   ClientHeight    =   3195
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   4680
   BeginProperty Font 
      Name            =   "ËÎÌו"
      Size            =   9
      Charset         =   134
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   Icon            =   "BIGGBDLG.frx":0000
   MaxButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnExit 
      Cancel          =   -1  'True
      Caption         =   "E&xit"
      Height          =   375
      Left            =   1920
      TabIndex        =   4
      Top             =   2760
      Width           =   855
   End
   Begin VB.CommandButton btnGBToBig 
      Caption         =   "Big<-&GB"
      Height          =   375
      Left            =   1920
      TabIndex        =   3
      Top             =   720
      Width           =   855
   End
   Begin VB.CommandButton btnBigToGB 
      Caption         =   "&Big->GB"
      Height          =   375
      Left            =   1920
      TabIndex        =   2
      Top             =   120
      Width           =   855
   End
   Begin BigToGB.Big2GB big2gb 
      Left            =   0
      Top             =   0
      _ExtentX        =   423
      _ExtentY        =   397
   End
   Begin VB.TextBox txtGB 
      Height          =   3015
      Left            =   2880
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   1
      Top             =   120
      Width           =   1695
   End
   Begin VB.TextBox txtBig 
      Height          =   3015
      Left            =   120
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   0
      Top             =   120
      Width           =   1695
   End
End
Attribute VB_Name = "frmBigGBDlg"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub btnBigToGB_Click()

txtGB.Text = Big2GB.BigToGB(txtBig.Text)

End Sub

Private Sub btnGBToBig_Click()

txtBig.Text = Big2GB.GBToBig(txtGB.Text)

End Sub

Private Sub btnExit_Click()

Unload frmBigGBDlg

End Sub
