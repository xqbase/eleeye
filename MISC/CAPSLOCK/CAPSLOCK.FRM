VERSION 5.00
Begin VB.Form frmCapsLock 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   0  'None
   ClientHeight    =   480
   ClientLeft      =   120
   ClientTop       =   120
   ClientWidth     =   480
   ControlBox      =   0   'False
   Icon            =   "CAPSLOCK.frx":0000
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   480
   ScaleWidth      =   480
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.Image imgCapsOff 
      Height          =   480
      Left            =   0
      Picture         =   "CAPSLOCK.frx":0442
      Top             =   0
      Width           =   480
   End
   Begin VB.Image imgCapsOn 
      Height          =   480
      Left            =   0
      Picture         =   "CAPSLOCK.frx":0884
      Top             =   0
      Width           =   480
   End
   Begin VB.Menu mnTray 
      Caption         =   ""
      Visible         =   0   'False
      Begin VB.Menu mnTrayTooltip 
         Caption         =   "Tooltip"
      End
      Begin VB.Menu mnTrayExit 
         Caption         =   "Exit"
      End
   End
End
Attribute VB_Name = "frmCapsLock"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_Click()

Hide
App_bShow = False

End Sub

Private Sub imgCapsOff_Click()

Form_Click

End Sub

Private Sub imgCapsOn_Click()

Form_Click

End Sub

Private Sub mnTrayTooltip_Click()

mnTrayTooltip.Checked = Not mnTrayTooltip.Checked

End Sub

Private Sub mnTrayExit_Click()

App_bRunning = False

End Sub
