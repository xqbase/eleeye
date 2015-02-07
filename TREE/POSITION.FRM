VERSION 5.00
Begin VB.Form frmPosition 
   BorderStyle     =   4  'Fixed ToolWindow
   Caption         =   "局面比较"
   ClientHeight    =   3240
   ClientLeft      =   45
   ClientTop       =   270
   ClientWidth     =   5160
   ControlBox      =   0   'False
   BeginProperty Font 
      Name            =   "宋体"
      Size            =   9
      Charset         =   134
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3240
   ScaleWidth      =   5160
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnCopyFen2 
      Caption         =   "复制FEN"
      Height          =   375
      Left            =   3600
      TabIndex        =   3
      Top             =   0
      Width           =   1095
   End
   Begin VB.CommandButton btnCopyFen 
      Caption         =   "复制FEN"
      Height          =   375
      Left            =   1080
      TabIndex        =   2
      Top             =   0
      Width           =   1095
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   89
      Left            =   4680
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   88
      Left            =   4440
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   87
      Left            =   4200
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   86
      Left            =   3960
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   85
      Left            =   3720
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   84
      Left            =   3480
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   83
      Left            =   3240
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   82
      Left            =   3000
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   81
      Left            =   2760
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   80
      Left            =   4680
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   79
      Left            =   4440
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   78
      Left            =   4200
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   77
      Left            =   3960
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   76
      Left            =   3720
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   75
      Left            =   3480
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   74
      Left            =   3240
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   73
      Left            =   3000
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   72
      Left            =   2760
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   71
      Left            =   4680
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   70
      Left            =   4440
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   69
      Left            =   4200
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   68
      Left            =   3960
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   67
      Left            =   3720
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   66
      Left            =   3480
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   65
      Left            =   3240
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   64
      Left            =   3000
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   63
      Left            =   2760
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   62
      Left            =   4680
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   61
      Left            =   4440
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   60
      Left            =   4200
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   59
      Left            =   3960
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   58
      Left            =   3720
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   57
      Left            =   3480
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   56
      Left            =   3240
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   55
      Left            =   3000
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   54
      Left            =   2760
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   53
      Left            =   4680
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   52
      Left            =   4440
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   51
      Left            =   4200
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   50
      Left            =   3960
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   49
      Left            =   3720
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   48
      Left            =   3480
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   47
      Left            =   3240
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   46
      Left            =   3000
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   45
      Left            =   2760
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   44
      Left            =   4680
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   43
      Left            =   4440
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   42
      Left            =   4200
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   41
      Left            =   3960
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   40
      Left            =   3720
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   39
      Left            =   3480
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   38
      Left            =   3240
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   37
      Left            =   3000
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   36
      Left            =   2760
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   35
      Left            =   4680
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   34
      Left            =   4440
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   33
      Left            =   4200
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   32
      Left            =   3960
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   31
      Left            =   3720
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   30
      Left            =   3480
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   29
      Left            =   3240
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   28
      Left            =   3000
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   27
      Left            =   2760
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   26
      Left            =   4680
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   25
      Left            =   4440
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   24
      Left            =   4200
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   23
      Left            =   3960
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   22
      Left            =   3720
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   21
      Left            =   3480
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   20
      Left            =   3240
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   19
      Left            =   3000
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   18
      Left            =   2760
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   17
      Left            =   4680
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   16
      Left            =   4440
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   15
      Left            =   4200
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   14
      Left            =   3960
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   13
      Left            =   3720
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   12
      Left            =   3480
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   11
      Left            =   3240
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   10
      Left            =   3000
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   9
      Left            =   2760
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   8
      Left            =   4680
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   7
      Left            =   4440
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   6
      Left            =   4200
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   5
      Left            =   3960
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   4
      Left            =   3720
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   3
      Left            =   3480
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   2
      Left            =   3240
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   1
      Left            =   3000
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares2 
      Height          =   255
      Index           =   0
      Left            =   2760
      Top             =   600
      Width           =   255
   End
   Begin VB.Label lblPos2 
      Caption         =   "当前局面"
      Height          =   255
      Left            =   2760
      TabIndex        =   1
      Top             =   120
      Width           =   855
   End
   Begin VB.Label lblPos 
      Caption         =   "根局面"
      Height          =   255
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   615
   End
   Begin VB.Image imgBoard2 
      Height          =   2655
      Left            =   2640
      Picture         =   "POSITION.frx":0000
      Stretch         =   -1  'True
      Top             =   480
      Width           =   2415
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   0
      Left            =   240
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   1
      Left            =   480
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   2
      Left            =   720
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   3
      Left            =   960
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   4
      Left            =   1200
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   5
      Left            =   1440
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   6
      Left            =   1680
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   7
      Left            =   1920
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   8
      Left            =   2160
      Top             =   600
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   9
      Left            =   240
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   10
      Left            =   480
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   11
      Left            =   720
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   12
      Left            =   960
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   13
      Left            =   1200
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   14
      Left            =   1440
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   15
      Left            =   1680
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   16
      Left            =   1920
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   17
      Left            =   2160
      Top             =   840
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   18
      Left            =   240
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   19
      Left            =   480
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   20
      Left            =   720
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   21
      Left            =   960
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   22
      Left            =   1200
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   23
      Left            =   1440
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   24
      Left            =   1680
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   25
      Left            =   1920
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   26
      Left            =   2160
      Top             =   1080
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   27
      Left            =   240
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   28
      Left            =   480
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   29
      Left            =   720
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   30
      Left            =   960
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   31
      Left            =   1200
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   32
      Left            =   1440
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   33
      Left            =   1680
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   34
      Left            =   1920
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   35
      Left            =   2160
      Top             =   1320
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   36
      Left            =   240
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   37
      Left            =   480
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   38
      Left            =   720
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   39
      Left            =   960
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   40
      Left            =   1200
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   41
      Left            =   1440
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   42
      Left            =   1680
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   43
      Left            =   1920
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   44
      Left            =   2160
      Top             =   1560
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   45
      Left            =   240
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   46
      Left            =   480
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   47
      Left            =   720
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   48
      Left            =   960
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   49
      Left            =   1200
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   50
      Left            =   1440
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   51
      Left            =   1680
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   52
      Left            =   1920
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   53
      Left            =   2160
      Top             =   1800
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   54
      Left            =   240
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   55
      Left            =   480
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   56
      Left            =   720
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   57
      Left            =   960
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   58
      Left            =   1200
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   59
      Left            =   1440
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   60
      Left            =   1680
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   61
      Left            =   1920
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   62
      Left            =   2160
      Top             =   2040
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   63
      Left            =   240
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   64
      Left            =   480
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   65
      Left            =   720
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   66
      Left            =   960
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   67
      Left            =   1200
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   68
      Left            =   1440
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   69
      Left            =   1680
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   70
      Left            =   1920
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   71
      Left            =   2160
      Top             =   2280
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   72
      Left            =   240
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   73
      Left            =   480
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   74
      Left            =   720
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   75
      Left            =   960
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   76
      Left            =   1200
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   77
      Left            =   1440
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   78
      Left            =   1680
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   79
      Left            =   1920
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   80
      Left            =   2160
      Top             =   2520
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   81
      Left            =   240
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   82
      Left            =   480
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   83
      Left            =   720
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   84
      Left            =   960
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   85
      Left            =   1200
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   86
      Left            =   1440
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   87
      Left            =   1680
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   88
      Left            =   1920
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgSquares 
      Height          =   255
      Index           =   89
      Left            =   2160
      Top             =   2760
      Width           =   255
   End
   Begin VB.Image imgBoard 
      Height          =   2655
      Left            =   120
      Picture         =   "POSITION.frx":4359
      Stretch         =   -1  'True
      Top             =   480
      Width           =   2415
   End
End
Attribute VB_Name = "frmPosition"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' Search Tree Analyst - for UCCI Engines
' Designed by Morning Yellow, Version: 2.35, Last Modified: Oct. 2007
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
Private Sub btnCopyFen_Click()

Clipboard.Clear
Clipboard.SetText AllocString(CchessBoard2Fen(App_posRoot)) + " - - 0 1"

End Sub

Private Sub btnCopyFen2_Click()

Clipboard.Clear
Clipboard.SetText App_szCurrFen + " - - 0 1"

End Sub
