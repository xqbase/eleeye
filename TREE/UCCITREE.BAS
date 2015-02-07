Attribute VB_Name = "mdlUcciTree"
' Search Tree Analyst - for UCCI Engines
' Designed by Morning Yellow, Version: 3.1, Last Modified: Nov. 2007
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

Public Const MAX_NODE_NUM As Integer = 32767

Public Const NODE_NONE As Integer = 1
Public Const NODE_LEAF As Integer = 2
Public Const NODE_ALPHA As Integer = 3
Public Const NODE_BETA As Integer = 4
Public Const NODE_PV As Integer = 5

Public Const VALUE_NONE As Long = 0
Public Const VALUE_LEAF_1 As Long = 1000000
Public Const VALUE_LEAF_2 As Long = 2000000
Public Const VALUE_ALPHA As Long = 3000000
Public Const VALUE_BETA As Long = 4000000
Public Const VALUE_PV_1 As Long = 5000000
Public Const VALUE_PV_2 As Long = 6000000
Public Const VALUE_BEST As Long = 7000000

Public Type NodeStruct
    szFen   As String
    mvBest  As Long
    bExp    As Boolean
End Type

Public Type InfoStruct
    szFen   As String
    mvBest  As Long
    bExp    As Boolean
    nIcon   As Integer
    vl      As Long
    szInfo  As String
End Type

Public App_bRunning As Boolean
Public App_pipe As PipeStruct
Public App_posRoot As PositionStruct
Public App_szCurrFen As String
Public App_nNodeNum As Integer
Public App_Nodes(0 To MAX_NODE_NUM) As NodeStruct

Public Function Int2Str(ByVal n As Integer)

Int2Str = LTrim(Str(n))

End Function

Public Sub PipeOut(ByVal sz As String)

PipeLineOutput App_pipe, sz

End Sub

Public Function PipeIn() As String

Dim lpStr As Long, sz As String
lpStr = PipeLineInput(App_pipe)
If lpStr = 0 Then
    PipeIn = ""
Else
    sz = AllocString(lpStr)
    PipeIn = sz
End If

End Function

Public Sub ProbeHash(ByRef mvBest As Long, ByRef nAlphaDepth As Integer, ByRef nBetaDepth As Integer, ByRef vlAlpha As Integer, ByRef vlBeta As Integer, ByVal szFen As String)

Dim szRecv As String, i As Integer
Dim dfThisTime As Double, dfLastTime As Double

mvBest = 0
nAlphaDepth = -1
nBetaDepth = -1
PipeOut "probe fen " + szFen + " - - 0 1"
dfLastTime = Timer
Do
    szRecv = PipeIn
    If szRecv = "" Then
        Sleep 1
        dfThisTime = Timer
        dfThisTime = dfThisTime + IIf(dfThisTime < dfLastTime, 86400#, 0#)
        If dfThisTime > dfLastTime + 1# Then
            Exit Do
        End If
    Else
        If Left(szRecv, 7) = "pophash" Then
            ' 获取mvBest
            i = InStr(szRecv, " bestmove ")
            If i > 0 Then
                mvBest = Coord2Move(Mid(szRecv, i + 10, 4))
            End If
            ' 获取nAlphaDepth和vlAlpha
            i = InStr(szRecv, " lowerbound ")
            If i > 0 Then
                vlAlpha = Str2Int(Mid(szRecv, i + 12))
                i = InStr(i + 12, szRecv, " depth ")
                If i > 0 Then
                    nAlphaDepth = Str2Int(Mid(szRecv, i + 7))
                End If
            End If
            ' 获取nBetaDepth和vlBeta
            i = InStr(szRecv, " upperbound ")
            If i > 0 Then
                vlBeta = Str2Int(Mid(szRecv, i + 12))
                i = InStr(i + 12, szRecv, " depth ")
                If i > 0 Then
                    nBetaDepth = Str2Int(Mid(szRecv, i + 7))
                End If
            End If
            Exit Do
        End If
    End If
Loop

End Sub

Public Sub AddNode(ByRef pos As PositionStruct, ByVal mvBest As Long, Optional ByVal nIndex As Integer = -1)

Dim mvs(0 To 127) As Long
Dim Infos(0 To 127) As InfoStruct, InfoTemp As InfoStruct
Dim nTotal As Integer, nLegal As Integer, i As Integer, j As Integer
Dim nAlphaDepth As Integer, nBetaDepth As Integer, vlAlpha As Integer, vlBeta As Integer
Dim nStatus As Long, szMoveStr As String

nTotal = CchessGenMoves(pos, mvs(0))
nLegal = 0
' 首先检查空着裁剪
If CchessTryNull(pos) <> 0 Then
    Infos(nLegal).szFen = AllocString(CchessBoard2Fen(pos))
    CchessUndoNull pos
    ProbeHash Infos(nLegal).mvBest, nAlphaDepth, nBetaDepth, vlAlpha, vlBeta, Infos(nLegal).szFen
    Infos(nLegal).bExp = False
    ' Infos(nLegal) 是可以用的，如果 nLegal ++ 不执行，那么它将被以后的结点覆盖
    If nBetaDepth > 0 Then
        Infos(nLegal).nIcon = NODE_BETA
        Infos(nLegal).vl = VALUE_BETA - vlBeta
        Infos(nLegal).szInfo = "空着裁剪 " + Int2Str(nBetaDepth) + "：≥" + Int2Str(-vlBeta)
        nLegal = nLegal + 1
    End If
End If
For i = 0 To nTotal - 1
    If CchessTryMove(pos, nStatus, mvs(i)) <> 0 Then
        Infos(nLegal).szFen = AllocString(CchessBoard2Fen(pos))
        Infos(nLegal).mvBest = 0
        CchessUndoMove pos
        szMoveStr = MkC(CchessFile2Chin(CchessMove2File(mvs(i), pos), pos.sdPlayer))
        ProbeHash Infos(nLegal).mvBest, nAlphaDepth, nBetaDepth, vlAlpha, vlBeta, Infos(nLegal).szFen
        Infos(nLegal).bExp = False
        If nAlphaDepth > nBetaDepth Then
            Infos(nLegal).nIcon = NODE_ALPHA
            Infos(nLegal).vl = VALUE_ALPHA - vlAlpha
            Infos(nLegal).szInfo = szMoveStr + " " + Int2Str(nAlphaDepth) + "：≤" + Int2Str(-vlAlpha)
        ElseIf nBetaDepth > nAlphaDepth Then
            Infos(nLegal).nIcon = NODE_BETA
            Infos(nLegal).vl = VALUE_BETA - vlBeta
            Infos(nLegal).szInfo = szMoveStr + " " + Int2Str(nBetaDepth) + "：≥" + Int2Str(-vlBeta)
        Else
            If nAlphaDepth = -1 Then
                Infos(nLegal).bExp = True
                Infos(nLegal).nIcon = NODE_NONE
                Infos(nLegal).vl = VALUE_NONE
                Infos(nLegal).szInfo = szMoveStr
            Else
                Infos(nLegal).bExp = (nAlphaDepth = 0)
                Infos(nLegal).nIcon = IIf(nAlphaDepth = 0, NODE_LEAF, NODE_PV)
                If vlAlpha < vlBeta Then
                    Infos(nLegal).szInfo = szMoveStr + " " + Int2Str(nAlphaDepth) + "：[" + Int2Str(-vlBeta) + "," + Int2Str(-vlAlpha) + "]"
                    ' 不是彻底的PV结点
                    Infos(nLegal).vl = IIf(nAlphaDepth = 0, VALUE_LEAF_1, VALUE_PV_1) - vlAlpha
                Else
                    Infos(nLegal).szInfo = szMoveStr + " " + Int2Str(nAlphaDepth) + "：＝" + Int2Str(-vlAlpha)
                    Infos(nLegal).vl = IIf(nAlphaDepth = 0, VALUE_LEAF_2, VALUE_PV_2) - vlAlpha
                End If
            End If
        End If
        ' 最佳着法放在第一位
        If mvs(i) = mvBest Then
            Infos(nLegal).vl = VALUE_BEST
        End If
        nLegal = nLegal + 1
    End If
Next
' 对结点做冒泡排序
For i = nLegal - 1 To 1 Step -1
    For j = 0 To i - 1
        If Infos(j).vl < Infos(j + 1).vl Then
            InfoTemp = Infos(j)
            Infos(j) = Infos(j + 1)
            Infos(j + 1) = InfoTemp
        End If
    Next
Next
' 输出结点
For i = 0 To nLegal - 1
    If App_nNodeNum < MAX_NODE_NUM Then
        App_Nodes(App_nNodeNum).szFen = Infos(i).szFen
        App_Nodes(App_nNodeNum).mvBest = Infos(i).mvBest
        App_Nodes(App_nNodeNum).bExp = Infos(i).bExp
        App_nNodeNum = App_nNodeNum + 1
        If nIndex < 0 Then
            frmUcciTree.tree.Nodes.Add , , , Infos(i).szInfo, Infos(i).nIcon
        Else
            frmUcciTree.tree.Nodes.Add nIndex + 1, tvwChild, , Infos(i).szInfo, Infos(i).nIcon
        End If
    End If
Next

End Sub

Public Sub DrawRoot()

Dim i As Integer, j As Integer, pc As Integer, nPicId As Integer

For i = 3 To 12
    For j = 3 To 11
        pc = App_posRoot.ucpcSquares(i * 16 + j)
        If pc = 0 Then
            nPicId = 0
        Else
            nPicId = PieceType(pc) + IIf(pc < 32, 1, 8)
        End If
        frmPosition.imgSquares((i - 3) * 9 + j - 3).Picture = frmUcciTree.imgPiecesMini(nPicId).Picture
    Next
Next

End Sub

Public Sub DrawCurr(ByRef pos As PositionStruct)

Dim i As Integer, j As Integer, pc As Integer, nPicId As Integer
For i = 3 To 12
    For j = 3 To 11
        pc = pos.ucpcSquares(i * 16 + j)
        If pc = 0 Then
            nPicId = 0
        Else
            nPicId = PieceType(pc) + IIf(pc < 32, 1, 8)
        End If
        frmPosition.imgSquares2((i - 3) * 9 + j - 3).Picture = frmUcciTree.imgPiecesMini(nPicId).Picture
    Next
Next

End Sub

Public Sub Main()

Dim lpStr As Long, szRecv As String, szEngine As String

szEngine = OpenFileDialog("加载引擎", "引擎文件 (*.EXE)|*.EXE|所有文件 (*.*)|*.*")
If szEngine <> "" Then
    CchessInit
    CchessFen2Board App_posRoot, CCHESS_START_FEN
    DrawRoot
    App_szCurrFen = CCHESS_START_FEN
    DrawCurr App_posRoot
    PipeOpen App_pipe, szEngine
    PipeOut "ucci"
    PipeOut "setoption promotion false"
    PipeOut "setoption usebook false"
    frmPosition.Show , frmUcciTree
    frmUcciTree.Show
    frmUcciTree.cmbSend.List(0) = "position fen " + CCHESS_START_FEN + " - - 0 1"
    App_nNodeNum = 0
    App_bRunning = True
    Do While App_bRunning
        Sleep 1
        DoEvents
        szRecv = PipeIn
        If szRecv <> "" Then
            frmUcciTree.stb.Panels("Info").Text = szRecv
            If Left(szRecv, 9) = "bestmove " Then
                AddNode App_posRoot, Coord2Move(Mid(szRecv, 10, 4))
            End If
        End If
    Loop
    PipeOut "quit"
    PipeClose App_pipe
End If

End Sub
