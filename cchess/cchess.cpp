/*
cchess.h/cchess.cpp - Source Code for ElephantEye, Additional Part

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.21, Last Modified: Sep. 2010
Copyright (C) 2004-2010 www.xqbase.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <string.h>
#include "../base/base.h"
#include "../eleeye/position.h"
#include "cchess.h"

/* 本程序是ElephantEye源程序的附加模块，作用是将ElephantEye的源程序应用到其他软件中。
 * 本程序的一个主要应用是中国象棋规则驱动程序，在编译时定义"CCHESS_DLL"后，即可编译成"CCHESS.DLL"。
 * 目前该驱动程序已经成为《象棋巫师》的一部分，这也使得《象棋巫师》在中国象棋规则处理上的核心代码公开化了。
 */

#ifdef CCHESS_DLL

#include <windows.h>

extern "C" __declspec(dllexport) LPCSTR WINAPI CchessVersion(VOID);
extern "C" __declspec(dllexport) VOID WINAPI CchessInit(BOOL bTraditional);
extern "C" __declspec(dllexport) VOID WINAPI CchessPromotion(BOOL bPromotion);
extern "C" __declspec(dllexport) VOID WINAPI CchessAddPiece(PositionStruct *lppos, LONG sq, LONG pc, BOOL bDel);
extern "C" __declspec(dllexport) BOOL WINAPI CchessCanPromote(PositionStruct *lppos, LONG sq);
extern "C" __declspec(dllexport) BOOL WINAPI CchessTryMove(PositionStruct *lppos, LPLONG lpStatus, LONG mv);
extern "C" __declspec(dllexport) VOID WINAPI CchessUndoMove(PositionStruct *lppos);
extern "C" __declspec(dllexport) BOOL WINAPI CchessTryNull(PositionStruct *lppos);
extern "C" __declspec(dllexport) VOID WINAPI CchessUndoNull(PositionStruct *lppos);
extern "C" __declspec(dllexport) LONG WINAPI CchessGenMoves(PositionStruct *lppos, LPLONG lpmv);
extern "C" __declspec(dllexport) VOID WINAPI CchessSetIrrev(PositionStruct *lppos);
extern "C" __declspec(dllexport) VOID WINAPI CchessClearBoard(PositionStruct *lppos);
extern "C" __declspec(dllexport) VOID WINAPI CchessBoardMirror(PositionStruct *lppos);
extern "C" __declspec(dllexport) VOID WINAPI CchessExchangeSide(PositionStruct *lppos);
extern "C" __declspec(dllexport) VOID WINAPI CchessFlipBoard(PositionStruct *lppos);
extern "C" __declspec(dllexport) LPSTR WINAPI CchessBoardText(const PositionStruct *lppos, BOOL bAnsi);
extern "C" __declspec(dllexport) LPSTR WINAPI CchessBoard2Fen(const PositionStruct *lppos);
extern "C" __declspec(dllexport) VOID WINAPI CchessFen2Board(PositionStruct *lppos, LPCSTR szFen);
extern "C" __declspec(dllexport) LPSTR WINAPI CchessFenMirror(LPCSTR szFenSrc);
extern "C" __declspec(dllexport) LONG WINAPI CchessFileMirror(LONG dwFileStr);
extern "C" __declspec(dllexport) LONG WINAPI CchessChin2File(LONGLONG qwChinStr);
extern "C" __declspec(dllexport) LONGLONG WINAPI CchessFile2Chin(LONG dwFileStr, LONG sd);
extern "C" __declspec(dllexport) LONG WINAPI CchessFile2Move(LONG dwFileStr, const PositionStruct *lppos);
extern "C" __declspec(dllexport) LONG WINAPI CchessMove2File(LONG mv, const PositionStruct *lppos);

// 驱动程序的版本号，在《象棋巫师》中使用“关于规则”功能可以看到。
static const char *const cszCchessVersion = "Chinese Chess Driver 3.21";

LPCSTR WINAPI CchessVersion(VOID) {
  return cszCchessVersion;
}

VOID WINAPI CchessInit(BOOL bTraditional) {
  PreGenInit();
  ChineseInit(bTraditional != FALSE);
}

VOID WINAPI CchessPromotion(BOOL bPromotion) {
  PreEval.bPromotion = bPromotion != FALSE;
}

VOID WINAPI CchessAddPiece(PositionStruct *lppos, LONG sq, LONG pc, BOOL bDel) {
  lppos->AddPiece(sq, pc, bDel != FALSE);
}

BOOL WINAPI CchessCanPromote(PositionStruct *lppos, LONG sq) {
  int pt;
  if (PreEval.bPromotion && lppos->CanPromote() && CAN_PROMOTE(sq)) {
    pt = PIECE_TYPE(lppos->ucpcSquares[sq]);
    return pt == ADVISOR_TYPE || pt == BISHOP_TYPE;
  }
  return FALSE;
}

BOOL WINAPI CchessTryMove(PositionStruct *lppos, LPLONG lpStatus, LONG mv) {
  return TryMove(*lppos, *(int *) lpStatus, mv);
}

VOID WINAPI CchessUndoMove(PositionStruct *lppos) {
  lppos->UndoMakeMove();
}

// 执行“空着”，该功能目前仅用在“搜索树分析器”中
BOOL WINAPI CchessTryNull(PositionStruct *lppos) {
  if (lppos->LastMove().ChkChs > 0) {
    return FALSE;
  } else {
    lppos->NullMove();
    return TRUE;
  }
}

// 撤消“空着”，该功能目前仅用在“搜索树分析器”中
VOID WINAPI CchessUndoNull(PositionStruct *lppos) {
  lppos->UndoNullMove();
}

// 生成全部合理着法
LONG WINAPI CchessGenMoves(PositionStruct *lppos, LPLONG lpmv) {
  int i, nTotal, nLegal;
  MoveStruct mvs[MAX_GEN_MOVES];
  nTotal = lppos->GenAllMoves(mvs);
  nLegal = 0;
  for (i = 0; i < nTotal; i ++) {
    if (lppos->MakeMove(mvs[i].wmv)) {
      lppos->UndoMakeMove();
      lpmv[nLegal] = mvs[i].wmv;
      nLegal ++;
    }
  }
  return nLegal;
}

VOID WINAPI CchessSetIrrev(PositionStruct *lppos) {
  lppos->SetIrrev();
}

VOID WINAPI CchessClearBoard(PositionStruct *lppos) {
  lppos->ClearBoard();
}

VOID WINAPI CchessStartBoard(PositionStruct *lppos) {
  lppos->FromFen(cszStartFen);
}

VOID WINAPI CchessBoardMirror(PositionStruct *lppos) {
  lppos->Mirror();
}

VOID WINAPI CchessExchangeSide(PositionStruct *lppos) {
  ExchangeSide(*lppos);
}

VOID WINAPI CchessFlipBoard(PositionStruct *lppos) {
  FlipBoard(*lppos);
}

LPSTR WINAPI CchessBoardText(const PositionStruct *lppos, BOOL bAnsi) {
  static char szBoard[2048];
  BoardText(szBoard, *lppos, bAnsi != FALSE);
  return szBoard;
}

LPSTR WINAPI CchessBoard2Fen(const PositionStruct *lppos) {
  static char szFen[128];
  lppos->ToFen(szFen);
  return szFen;
}

VOID WINAPI CchessFen2Board(PositionStruct *lppos, LPCSTR szFen) {
  lppos->FromFen(szFen);
}

LPSTR WINAPI CchessFenMirror(LPCSTR szFenSrc) {
  static char szFenDst[128];
  FenMirror(szFenDst, szFenSrc);
  return szFenDst;
}

LONG WINAPI CchessFileMirror(LONG dwFileStr) {
  return FileMirror(dwFileStr);
}

LONG WINAPI CchessChin2File(LONGLONG qwChinStr) {
  return Chin2File(qwChinStr);
}

LONGLONG WINAPI CchessFile2Chin(LONG dwFileStr, LONG sd) {
  return File2Chin(dwFileStr, sd);
}

LONG WINAPI CchessFile2Move(LONG dwFileStr, const PositionStruct *lppos) {
  return File2Move(dwFileStr, *lppos);
}

LONG WINAPI CchessMove2File(LONG mv, const PositionStruct *lppos) {
  return Move2File(mv, *lppos);
}

#endif

/* ElephantEye源程序使用的匈牙利记号约定：
 *
 * sq: 格子序号(整数，从0到255，参阅"pregen.cpp")
 * pc: 棋子序号(整数，从0到47，参阅"position.cpp")
 * pt: 棋子类型序号(整数，从0到6，参阅"position.cpp")
 * mv: 着法(整数，从0到65535，参阅"position.cpp")
 * sd: 走子方(整数，0代表红方，1代表黑方)
 * vl: 局面价值(整数，从"-MATE_VALUE"到"MATE_VALUE"，参阅"position.cpp")
 * (注：以上五个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(PositionStruct类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

/* 以下常量规定了着法表示使用的数字、棋子、方向(进平退)、位置(前后)等的最大个数。
 * 
 * 表示位置的符号共有8个，除了“前中后”以外还有“一二三四五”，参考
 * 《中国象棋电脑应用规范(二)：着法表示》(简称《规范》)，即以下网页：
 * 　　http://www.elephantbase.net/protocol/cchess_move.htm
 * 由于“前中后”被安排在“一二三四五”以后，但又和“进平退”在符号上一致，因此要加减"DIRECT_TO_POS"作转换。
 * 另外，由于仕(士)相(象)的着法表示的纵线形式和坐标形式有一一对应的关系(固定纵线表示)，
 * 因此可以使用数组"cdwFixFile"和"cucFixMove"对两者进行转换，总共有28种对应关系。
 */
const int MAX_DIGIT = 9;
const int MAX_PIECE = 7;
const int MAX_DIRECT = 3;
const int MAX_POS = 8;
const int DIRECT_TO_POS = 5;
const int MAX_FIX_FILE = 28;

/* 以下是数字、棋子、方向和位置编码对应的符号和汉字。
 *
 * 数组长度至少要比这些符号的个数多1，以"ccDirect2Byte"为例，当发现没有方向跟某个符号对应时，
 * 该方向编号为"MAX_DIRECT"，还原成符号时保证数组不越界，并以空格表示。
 * 汉字数组有简体(GBK码)和繁体(BIG5码)两套，以"cwDirect2Word..."为例，后缀"-Simp"表示简体，"-Trad"表示繁体。
 * 数组在使用前，必须用"lpcwDirect2Word"指针来定位，参阅函数"ChineseInit()"。
 */

static const char ccDirect2Byte[4] = {
  '+', '.', '-', ' '
};

static const char ccPos2Byte[12] = {
  'a', 'b', 'c', 'd', 'e', '+', '.', '-', ' ', ' ', ' ', ' '
};

static const uint16_t cwDigit2WordSimp[2][10] = {
  {
    0xbbd2/*一*/, 0xfeb6/*二*/, 0xfdc8/*三*/, 0xc4cb/*四*/, 0xe5ce/*五*/,
    0xf9c1/*六*/, 0xdfc6/*七*/, 0xcbb0/*八*/, 0xc5be/*九*/, 0xa1a1/*　*/
  }, {
    0xb1a3/*１*/, 0xb2a3/*２*/, 0xb3a3/*３*/, 0xb4a3/*４*/, 0xb5a3/*５*/,
    0xb6a3/*６*/, 0xb7a3/*７*/, 0xb8a3/*８*/, 0xb9a3/*９*/, 0xa1a1/*　*/
  }
};

static const uint16_t cwPiece2WordSimp[2][8] = {
  {
    0xa7cb/*帅*/, 0xcbca/*仕*/, 0xe0cf/*相*/, 0xedc2/*马*/, 0xb5b3/*车*/, 0xdac5/*炮*/, 0xf8b1/*兵*/, 0xa1a1/*　*/
  }, {
    0xabbd/*将*/, 0xbfca/*士*/, 0xf3cf/*象*/, 0xedc2/*马*/, 0xb5b3/*车*/, 0xdac5/*炮*/, 0xe4d7/*卒*/, 0xa1a1/*　*/
  }
};

static const uint16_t cwDirect2WordSimp[4] = {
  0xf8bd/*进*/, 0xbdc6/*平*/, 0xcbcd/*退*/, 0xa1a1/*　*/
};

static const uint16_t cwPos2WordSimp[10] = {
  0xbbd2/*一*/, 0xfeb6/*二*/, 0xfdc8/*三*/, 0xc4cb/*四*/, 0xe5ce/*五*/,
  0xb0c7/*前*/, 0xd0d6/*中*/, 0xf3ba/*后*/, 0xa1a1/*　*/, 0xa1a1/*　*/
};

static const uint16_t cwDigit2WordTrad[2][10] = {
  {
    0x40a4/*[一]*/, 0x47a4/*[二]*/, 0x54a4/*[三]*/, 0x7ca5/*[四]*/, 0xada4/*き[五]*/,
    0xbba4/*せ[六]*/, 0x43a4/*[七]*/, 0x4ba4/*[八]*/, 0x45a4/*[九]*/, 0x40a1/**/
  }, {
    0xb0a2/*[１]*/, 0xb1a2/*⒈[２]*/, 0xb2a2/*⒉[３]*/, 0xb3a2/*⒊[４]*/, 0xb4a2/*⒋[５]*/,
    0xb5a2/*⒌[６]*/, 0xb6a2/*⒍[７]*/, 0xb7a2/*⒎[８]*/, 0xb8a2/*⒏[９]*/, 0x40a1/**/
  }
};

static const uint16_t cwPiece2WordTrad[2][8] = {
  {
    0xd3ab/*[帥]*/, 0x4ba5/*[仕]*/, 0xdbac/*[相]*/, 0xa8b0/*皑[馬]*/,
    0xaea8/*ó[車]*/, 0xb6ac/*[炮]*/, 0x4ca7/*[兵]*/, 0x40a1/**/
  }, {
    0x4eb1/*盢[將]*/, 0x68a4/*[士]*/, 0x48b6/*禜[象]*/, 0xa8b0/*皑[馬]*/,
    0xaea8/*ó[車]*/, 0xb6ac/*[炮]*/, 0xf2a8/*[卒]*/, 0x40a1/**/
  }
};

static const uint16_t cwDirect2WordTrad[4] = {
  0x69b6/*秈[進]*/, 0xada5/*キ[平]*/, 0x68b0/*癶[退]*/, 0x40a1/**/
};

static const uint16_t cwPos2WordTrad[10] = {
  0x40a4/*[一]*/, 0x47a4/*[二]*/, 0x54a4/*[三]*/, 0x7ca5/*[四]*/, 0xada4/*き[五]*/,
  0x65ab/*玡[前]*/, 0xa4a4/*い[中]*/, 0xe1ab/*[後]*/, 0x40a1/**/, 0x40a1/**/
};

// 固定纵线表示的纵线数组
static const uint32_t cdwFixFile[28] = {
  0x352d3441/*A4-5*/, 0x352b3441/*A4+5*/, 0x342d3541/*A5-4*/, 0x342b3541/*A5+4*/,
  0x362d3541/*A5-6*/, 0x362b3541/*A5+6*/, 0x352d3641/*A6-5*/, 0x352b3641/*A6+5*/,
  0x332d3142/*B1-3*/, 0x332b3142/*B1+3*/, 0x312d3342/*B3-1*/, 0x312b3342/*B3+1*/,
  0x352d3342/*B3-5*/, 0x352b3342/*B3+5*/, 0x332d3542/*B5-3*/, 0x332b3542/*B5+3*/,
  0x372d3542/*B5-7*/, 0x372b3542/*B5+7*/, 0x352d3742/*B7-5*/, 0x352b3742/*B7+5*/,
  0x392d3742/*B7-9*/, 0x392b3742/*B7+9*/, 0x372d3942/*B9-7*/, 0x372b3942/*B9+7*/,
  0x503d3441/*A4=P*/, 0x503d3641/*A6=P*/, 0x503d3342/*B3=P*/, 0x503d3742/*B7=P*/
};

// 固定纵线表示的坐标数组
static const uint8_t cucFixMove[28][2] = {
  {0xa8, 0xb7}, {0xc8, 0xb7}, {0xb7, 0xc8}, {0xb7, 0xa8}, {0xb7, 0xc6}, {0xb7, 0xa6}, {0xa6, 0xb7}, {0xc6, 0xb7},
  {0xab, 0xc9}, {0xab, 0x89}, {0x89, 0xab}, {0xc9, 0xab}, {0x89, 0xa7}, {0xc9, 0xa7}, {0xa7, 0xc9}, {0xa7, 0x89},
  {0xa7, 0xc5}, {0xa7, 0x85}, {0x85, 0xa7}, {0xc5, 0xa7}, {0x85, 0xa3}, {0xc5, 0xa3}, {0xa3, 0xc5}, {0xa3, 0x85},
  {0xc8, 0xc8}, {0xc6, 0xc6}, {0xc9, 0xc9}, {0xc5, 0xc5}
};

// 简体文本棋盘的棋盘字符
static const char *cszBoardStrSimp[19] = {
  " ┌--┬--┬--┬--┬--┬--┬--┬--┐ ",
  " │  │  │  │＼│／│  │  │  │ ",
  " ├--┼--┼--┼--※--┼--┼--┼--┤ ",
  " │  │  │  │／│＼│  │  │  │ ",
  " ├--┼--┼--┼--┼--┼--┼--┼--┤ ",
  " │  │  │  │  │  │  │  │  │ ",
  " ├--┼--┼--┼--┼--┼--┼--┼--┤ ",
  " │  │  │  │  │  │  │  │  │ ",
  " ├--┴--┴--┴--┴--┴--┴--┴--┤ ",
  " │                              │ ",
  " ├--┬--┬--┬--┬--┬--┬--┬--┤ ",
  " │  │  │  │  │  │  │  │  │ ",
  " ├--┼--┼--┼--┼--┼--┼--┼--┤ ",
  " │  │  │  │  │  │  │  │  │ ",
  " ├--┼--┼--┼--┼--┼--┼--┼--┤ ",
  " │  │  │  │＼│／│  │  │  │ ",
  " ├--┼--┼--┼--※--┼--┼--┼--┤ ",
  " │  │  │  │／│＼│  │  │  │ ",
  " └--┴--┴--┴--┴--┴--┴--┴--┘ "
};

// 繁体文本棋盘的棋盘字符
static const char *cszBoardStrTrad[19] = {
  " ---------------- ",
  "       〓       ",
  " --------“-------- ",
  "       〓       ",
  " ---------------- ",
  "                  ",
  " ---------------- ",
  "                  ",
  " ---------------- ",
  "                                ",
  " ---------------- ",
  "                  ",
  " ---------------- ",
  "                  ",
  " ---------------- ",
  "       〓       ",
  " --------“-------- ",
  "       〓       ",
  " ---------------- "
};

/* 以下两个数组实现了内部棋盘坐标(Square)和纵线优先坐标(FileSq)的转换。
 *
 * 内部棋盘坐标是有3层边界的16x16冗余数组(参阅"pregen.cpp")，为方便转换成纵线格式，
 * 要对它们重新编号，即按列优先从右到左，相同的列再从前到后的顺序(参阅《规范》)。
 * 转换后的坐标仍然是16x16的冗余数组，整除16后就是列号(右边线是0)，对16取余就是行号(上边线是0)。
 */

static const uint8_t cucSquare2FileSq[256] = {
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0, 0x80, 0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00, 0, 0, 0, 0,
  0, 0, 0, 0x81, 0x71, 0x61, 0x51, 0x41, 0x31, 0x21, 0x11, 0x01, 0, 0, 0, 0,
  0, 0, 0, 0x82, 0x72, 0x62, 0x52, 0x42, 0x32, 0x22, 0x12, 0x02, 0, 0, 0, 0,
  0, 0, 0, 0x83, 0x73, 0x63, 0x53, 0x43, 0x33, 0x23, 0x13, 0x03, 0, 0, 0, 0,
  0, 0, 0, 0x84, 0x74, 0x64, 0x54, 0x44, 0x34, 0x24, 0x14, 0x04, 0, 0, 0, 0,
  0, 0, 0, 0x85, 0x75, 0x65, 0x55, 0x45, 0x35, 0x25, 0x15, 0x05, 0, 0, 0, 0,
  0, 0, 0, 0x86, 0x76, 0x66, 0x56, 0x46, 0x36, 0x26, 0x16, 0x06, 0, 0, 0, 0,
  0, 0, 0, 0x87, 0x77, 0x67, 0x57, 0x47, 0x37, 0x27, 0x17, 0x07, 0, 0, 0, 0,
  0, 0, 0, 0x88, 0x78, 0x68, 0x58, 0x48, 0x38, 0x28, 0x18, 0x08, 0, 0, 0, 0,
  0, 0, 0, 0x89, 0x79, 0x69, 0x59, 0x49, 0x39, 0x29, 0x19, 0x09, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
};

static const uint8_t cucFileSq2Square[256] = {
  0x3b, 0x4b, 0x5b, 0x6b, 0x7b, 0x8b, 0x9b, 0xab, 0xbb, 0xcb, 0, 0, 0, 0, 0, 0,
  0x3a, 0x4a, 0x5a, 0x6a, 0x7a, 0x8a, 0x9a, 0xaa, 0xba, 0xca, 0, 0, 0, 0, 0, 0,
  0x39, 0x49, 0x59, 0x69, 0x79, 0x89, 0x99, 0xa9, 0xb9, 0xc9, 0, 0, 0, 0, 0, 0,
  0x38, 0x48, 0x58, 0x68, 0x78, 0x88, 0x98, 0xa8, 0xb8, 0xc8, 0, 0, 0, 0, 0, 0,
  0x37, 0x47, 0x57, 0x67, 0x77, 0x87, 0x97, 0xa7, 0xb7, 0xc7, 0, 0, 0, 0, 0, 0,
  0x36, 0x46, 0x56, 0x66, 0x76, 0x86, 0x96, 0xa6, 0xb6, 0xc6, 0, 0, 0, 0, 0, 0,
  0x35, 0x45, 0x55, 0x65, 0x75, 0x85, 0x95, 0xa5, 0xb5, 0xc5, 0, 0, 0, 0, 0, 0,
  0x34, 0x44, 0x54, 0x64, 0x74, 0x84, 0x94, 0xa4, 0xb4, 0xc4, 0, 0, 0, 0, 0, 0,
  0x33, 0x43, 0x53, 0x63, 0x73, 0x83, 0x93, 0xa3, 0xb3, 0xc3, 0, 0, 0, 0, 0, 0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0
};

// 汉字符号的指针，即规定了简体还是繁体，由"ChineseInit()"进行赋值
static const uint16_t (*lpcwDigit2Word)[10], (*lpcwPiece2Word)[8], *lpcwDirect2Word, *lpcwPos2Word;
static const char **lpcszBoardStr;
static uint16_t wPromote;

inline uint8_t SQUARE_FILESQ(int sq) {
  return cucSquare2FileSq[sq];
}

inline uint8_t FILESQ_SQUARE(int sq) {
  return cucFileSq2Square[sq];
}

inline int FILESQ_RANK_Y(int sq) {
  return sq & 15;
}

inline int FILESQ_FILE_X(int sq) {
  return sq >> 4;
}

inline int FILESQ_COORD_XY(int x, int y) {
  return (x << 4) + y;
}

// 获得某个棋子对于本方视角的纵线优先坐标，棋子编号从0到15
inline int FILESQ_SIDE_PIECE(const PositionStruct &pos, int nPieceNum) {
  int sq;
  sq = pos.ucsqPieces[SIDE_TAG(pos.sdPlayer) + nPieceNum];
  return (sq == 0 ? -1 : pos.sdPlayer == 0 ? SQUARE_FILESQ(sq) : SQUARE_FILESQ(SQUARE_FLIP(sq)));
}

// 根据子力类型获得棋子的编号
inline int FIRST_PIECE(int pt, int pc) {
  return pt * 2 - 1 + pc;
}

/* 以下函数实现了数字、棋子、方向和位置的编码和符号、编码和汉字之间的转换
 * 
 * 部分符号编码转换的代码，利用了"position.cpp"中的"PIECE_BYTE"数组和"FenPiece()"函数。
 * 从汉字转换为编码是难点，无论处于简体状态还是繁体状态，转换时既考虑了简体、繁体和异体，也考虑了GBK码和BIG5码，
 * 因此除了依次比较汉字数组外，还增加了对GBK码繁体字和异体字的识别。
 */

inline int Digit2Byte(int nArg) {
  return nArg + '1';
}

inline int Byte2Digit(int nArg) {
  return (nArg >= '1' && nArg <= '9' ? nArg - '1' : MAX_DIGIT);
}

inline int Piece2Byte(int nArg) {
  return PIECE_BYTE(nArg);
}

inline int Byte2Piece(int nArg) {
  return (nArg >= '1' && nArg <= '7' ? nArg - '1' : nArg >= 'A' && nArg <= 'Z' ? FenPiece(nArg) :
      nArg >= 'a' && nArg <= 'z' ? FenPiece(nArg - 'a' + 'A') : MAX_PIECE);
}

inline int Byte2Direct(int nArg) {
  return (nArg == '+' ? 0 : nArg == '.' || nArg == '=' ? 1 : nArg == '-' ? 2 : 3);
}

inline int Byte2Pos(int nArg) {
  return (nArg >= 'a' && nArg <= 'e' ? nArg - 'a' : Byte2Direct(nArg) + DIRECT_TO_POS);
}

static int Word2Digit(int nArg) {
  int i;
  for (i = 0; i < MAX_DIGIT; i ++) {
    if (nArg == cwDigit2WordSimp[0][i] || nArg == cwDigit2WordSimp[1][i] ||
        nArg == cwDigit2WordTrad[0][i] || nArg == cwDigit2WordTrad[1][i]) {
      break;
    }
  }
  return i;
}

static int Word2Piece(int nArg) {
  int i;
  if (false) {
  } else if (nArg == 0x9b8e/*帥*/ || nArg == 0xa28c/*將*/) {
    return 0;
  } else if (nArg == 0x52f1/*馬*/ || nArg == 0xd882/*傌*/ || nArg == 0x58d8/*豖[傌]*/) {
    return 3;
  } else if (nArg == 0x87dc/*車*/ || nArg == 0x8cb3/*硨*/ || nArg == 0xcfda/*谙[硨]*/ || nArg == 0x6582 /*俥*/) {
    return 4;
  } else if (nArg == 0xfcb0/*包*/ || nArg == 0x5da5/*[包]*/ || nArg == 0x68b3/*砲*/ || nArg == 0xa5af/*[砲]*/) {
    return 5;
  } else {
    for (i = 0; i < MAX_PIECE; i ++) {
      if (nArg == cwPiece2WordSimp[0][i] || nArg == cwPiece2WordSimp[1][i] ||
          nArg == cwPiece2WordTrad[0][i] || nArg == cwPiece2WordTrad[1][i]) {
        break;
      }
    }
    return i;
  }
}

static int Word2Direct(int nArg) {
  int i;
  if (nArg == 0x4ddf/*進*/) {
    return 0;
  } else {
    for (i = 0; i < MAX_DIRECT; i ++) {
      if (nArg == cwDirect2WordSimp[i] || nArg == cwDirect2WordTrad[i]) {
        break;
      }
    }
    return i;
  }
}

static int Word2Pos(int nArg) {
  int i;
  if (nArg == 0xe1e1/*後*/ || nArg == 0x5aa6/*[后]*/) {
    return 2 + DIRECT_TO_POS;
  } else {
    for (i = 0; i < MAX_POS; i ++) {
      if (nArg == cwPos2WordSimp[i] || nArg == cwPos2WordTrad[i]) {
        break;
      }
    }
    return i;
  }
}

// 确定使用简体汉字和繁体汉字
void ChineseInit(bool bTraditional) {
  if (bTraditional) {
    lpcwDigit2Word = cwDigit2WordTrad;
    lpcwPiece2Word = cwPiece2WordTrad;
    lpcwDirect2Word = cwDirect2WordTrad;
    lpcwPos2Word = cwPos2WordTrad;
    lpcszBoardStr = cszBoardStrTrad;
    wPromote = 0xdcc5/*跑*/;
  } else {
    lpcwDigit2Word = cwDigit2WordSimp;
    lpcwPiece2Word = cwPiece2WordSimp;
    lpcwDirect2Word = cwDirect2WordSimp;
    lpcwPos2Word = cwPos2WordSimp;
    lpcszBoardStr = cszBoardStrSimp;
    wPromote = 0xe4b1/*变*/;
  }
}

// 尝试某个着法，并返回着法状态，参阅"cchess.h"
bool TryMove(PositionStruct &pos, int &nStatus, int mv) {
  if (!pos.LegalMove(mv)) {
    nStatus = MOVE_ILLEGAL;
    return false;
  }
  if (!pos.MakeMove(mv)) {
    nStatus = MOVE_INCHECK;
    return false;
  }
  nStatus = 0;
  nStatus += (pos.LastMove().CptDrw > 0 ? MOVE_CAPTURE : 0);
  nStatus += (pos.LastMove().ChkChs > 0 ? MOVE_CHECK : 0);
  nStatus += (pos.IsMate() ? MOVE_MATE : 0);
  nStatus += pos.RepStatus(3) * MOVE_PERPETUAL; // 提示：参阅"position.cpp"中的"IsRep()"函数
  nStatus += (pos.IsDraw() ? MOVE_DRAW : 0);
  return true;
}

// 局面镜像

// 红黑互换
void ExchangeSide(PositionStruct &pos) {
  int i, sq;
  uint8_t ucsqList[32];
  for (i = 16; i < 48; i ++) {
    sq = pos.ucsqPieces[i];
    ucsqList[i - 16] = sq;
    if (sq != 0) {
      pos.AddPiece(sq, i, DEL_PIECE);
    }
  }
  for (i = 16; i < 48; i ++) {
    sq = ucsqList[i < 32 ? i : i - 32]; // 这行不同于FlipBoard
    if (sq != 0) {
      pos.AddPiece(SQUARE_FLIP(sq), i);
    }
  }
  pos.ChangeSide(); // 这行不同于FlipBoard
}

// 翻转棋盘
void FlipBoard(PositionStruct &pos) {
  int i, sq;
  uint8_t ucsqList[32];
  for (i = 16; i < 48; i ++) {
    sq = pos.ucsqPieces[i];
    ucsqList[i - 16] = sq;
    if (sq != 0) {
      pos.AddPiece(sq, i, DEL_PIECE);
    }
  }
  for (i = 16; i < 48; i ++) {
    sq = ucsqList[i - 16]; // 这行不同于ExchangeSide
    if (sq != 0) {
      pos.AddPiece(SQUARE_FLIP(sq), i);
    }
  }
}

// 生成文本棋盘(红子用()表示，黑子用[]表示)
void BoardText(char *szBoard, const PositionStruct &pos, bool bAnsi) {
  char *lpBoard;
  int i, j, pc;

  lpBoard = szBoard;
  if (bAnsi) {
    lpBoard += sprintf(lpBoard, "\33[0m");
  }
  for (i = 0; i < 19; i ++) {
    if (i % 2 == 0) {
      for (j = FILE_LEFT; j <= FILE_RIGHT; j ++) {
        pc = pos.ucpcSquares[COORD_XY(j, i / 2 + RANK_TOP)];
        if ((pc & SIDE_TAG(0)) != 0) {
          lpBoard += sprintf(lpBoard, bAnsi ? "(\33[1;31m%.2s\33[0m)" :
              "(%.2s)", (const char *) &lpcwPiece2Word[0][PIECE_TYPE(pc)]);
        } else if ((pc & SIDE_TAG(1)) != 0) {
          lpBoard += sprintf(lpBoard, bAnsi ? "[\33[1;32m%.2s\33[0m]" :
              "[%.2s]", (const char *) &lpcwPiece2Word[1][PIECE_TYPE(pc)]);
        } else {
          lpBoard += sprintf(lpBoard, "%.4s", lpcszBoardStr[i] + (j - FILE_LEFT) * 4);
        }
      }
      lpBoard += sprintf(lpBoard, "\r\n");
    } else {
      lpBoard += sprintf(lpBoard, "%s\r\n", lpcszBoardStr[i]);
    }
  }
}

// 对FEN串作镜像(只要识别行分隔符"/"，行内字符串顺序颠倒即可)
void FenMirror(char *szFenDst, const char *szFenSrc) {
  int i, j;
  const char *lpSrc;
  char *lpDst, *lpDstLimit;
  char szTempStr[128];

  lpSrc = szFenSrc;
  lpDst = szFenDst;
  lpDstLimit = lpDst + 127;
  if (*lpSrc == '\0') {
    *lpDst = '\0';
    return;
  }
  while (*lpSrc == ' ') {
    lpSrc ++;
    if (*lpSrc == '\0') {
      *lpDst = '\0';
      return;
    }
  }
  i = 0;
  while(lpDst < lpDstLimit && i < 127) {
    if (*lpSrc == '/' || *lpSrc == ' ' || *lpSrc == '\0') {
      for (j = 0; j < i; j ++) {
        *lpDst = szTempStr[i - j - 1];
        lpDst ++;
        if (lpDst == lpDstLimit) {
          break;
        }
      }
      i = 0;
      if (*lpSrc == '/') {
        *lpDst = '/';
        lpDst ++;
      } else {
        break;
      }
    } else {
      szTempStr[i] = *lpSrc;
      i ++;
    }
    lpSrc ++;
  };
  while(lpSrc != '\0' && lpDst < lpDstLimit) {
    *lpDst = *lpSrc;
    lpSrc ++;
    lpDst ++;
  }
  *lpDst = '\0';
  return;
}

union C4dwStruct {
  char c[4];
  uint32_t dw;
};

/* 函数"FileMirror()"对着法的纵线表示作镜像。
 *
 * 纵线的符号表示基本类似于汉字表示，但当出现类似“前炮退二”这样的表示时，符号表示就会有不同的情况。
 * 按照《规范》的建议，表示成"C+-2"最容易被识别，但是也有表示成"+C-2"的，即符号和汉字完全对应，因此本函数也会考虑这种形式。
 * 对一般着法而言，纵线表示的镜像是唯一的，但是对于“两条的纵线上有多个兵(卒)”的罕见情况，
 * 本函数只能考虑最不罕见的一种特例，即两条纵线上各有两个兵(卒)，这样，"Paxx"和"Pbxx"分别跟"Pcxx"和"Pdxx"镜像，
 * 而对于其他情况则无法作出正确转换。
 * 注意：符号表示由4个字节构成，所以可以用一个"uint32_t"类型作快速传输(同理，汉字表示用"uint64_t")。
 */
uint32_t FileMirror(uint32_t dwFileStr) {
  int nPos, nFile, pt;
  C4dwStruct Ret;
  Ret.dw = dwFileStr;

  nPos = Byte2Direct(Ret.c[0]);
  if (nPos == MAX_DIRECT) {
    pt = Byte2Piece(Ret.c[0]);
    nFile = Byte2Digit(Ret.c[1]);
    if (nFile == MAX_DIGIT) {
      switch (Ret.c[1]) {
      case 'a':
        Ret.c[1] = 'c';
        break;
      case 'b':
        Ret.c[1] = 'd';
        break;
      case 'c':
        Ret.c[1] = 'a';
        break;
      case 'd':
        Ret.c[1] = 'b';
        break;
      default:
        break;
      }
    } else {
      Ret.c[1] = Digit2Byte(8 - nFile);
    }
  } else {
    pt = Byte2Piece(Ret.c[1]);
  }
  if ((pt >= ADVISOR_TYPE && pt <= KNIGHT_TYPE) || Byte2Direct(Ret.c[2]) == 1) {
    Ret.c[3] = Digit2Byte(8 - Byte2Digit(Ret.c[3]));
  }
  return Ret.dw;
}

// 将汉字表示转换为符号表示
uint32_t Chin2File(uint64_t qwChinStr) {
  int nPos;
  uint16_t *lpwArg;
  C4dwStruct Ret;

  lpwArg = (uint16_t *) (void *) &qwChinStr;
  nPos = Word2Pos(lpwArg[0]);
  Ret.c[0] = PIECE_BYTE(Word2Piece(nPos == MAX_POS ? lpwArg[0] : lpwArg[1]));
  Ret.c[1] = (nPos == MAX_POS ? Digit2Byte(Word2Digit(lpwArg[1])) : ccPos2Byte[nPos]);
  if ((lpwArg[2] == 0xe4b1/*变*/ || lpwArg[2] == 0xdcc5/*跑*/ || lpwArg[2] == 0x83d7/*變*/) &&
      Word2Piece(lpwArg[3]) == 6) {
    Ret.c[2] = '=';
    Ret.c[3] = 'P';
  } else {
    Ret.c[2] = ccDirect2Byte[Word2Direct(lpwArg[2])];
    Ret.c[3] = Digit2Byte(Word2Digit(lpwArg[3]));
  }
  return Ret.dw;
}

// 将符号表示转换为汉字表示
uint64_t File2Chin(uint32_t dwFileStr, int sdPlayer) {
  int nPos;
  char *lpArg;
  union {
    uint16_t w[4];
    uint64_t qw;
  } Ret;

  lpArg = (char *) &dwFileStr;
  nPos = Byte2Direct(lpArg[0]);
  if (nPos == MAX_DIRECT) {
    nPos = Byte2Pos(lpArg[1]);
    Ret.w[0] = (nPos == MAX_POS ? lpcwPiece2Word[sdPlayer][Byte2Piece(lpArg[0])] : lpcwPos2Word[nPos]);
    Ret.w[1] = (nPos == MAX_POS ? lpcwDigit2Word[sdPlayer][Byte2Digit(lpArg[1])] :
        lpcwPiece2Word[sdPlayer][Byte2Piece(lpArg[0])]);
  } else {
    Ret.w[0] = lpcwPos2Word[nPos + DIRECT_TO_POS];
    Ret.w[1] = lpcwPiece2Word[sdPlayer][Byte2Piece(lpArg[1])];
  }
  if (lpArg[2] == '=' && Byte2Piece(lpArg[3]) == 6) {
    Ret.w[2] = wPromote;
    Ret.w[3] = lpcwPiece2Word[sdPlayer][6];
  } else {
    Ret.w[2] = lpcwDirect2Word[Byte2Direct(lpArg[2])];
    Ret.w[3] = lpcwDigit2Word[sdPlayer][Byte2Digit(lpArg[3])];
  }
  return Ret.qw;
}

/* "File2Move()"函数将纵线符号表示转换为内部着法表示。
 *
 * 这个函数以及后面的"Move2File()"函数是本模块最难处理的两个函数，特别是在处理“两条的纵线上有多个兵(卒)”的问题上。
 * 在棋谱的快速时，允许只使用数字键盘，因此1到7依次代表帅(将)到兵(卒)这七种棋子，"File2Move()"函数也考虑到了这个问题。
 */
int File2Move(uint32_t dwFileStr, const PositionStruct &pos) {
  int i, j, nPos, pt, sq, nPieceNum;
  int xSrc, ySrc, xDst, yDst;
  C4dwStruct FileStr;
  int nFileList[9], nPieceList[5];
  // 纵线符号表示转换为内部着法表示，通常分为以下几个步骤：

  // 1. 检查纵线符号是否是仕(士)相(象)的28种固定纵线表示，在这之前首先必须把数字、小写等不统一的格式转换为统一格式；
  FileStr.dw = dwFileStr;
  switch (FileStr.c[0]) {
  case '2':
  case 'a':
    FileStr.c[0] = 'A';
    break;
  case '3':
  case 'b':
  case 'E':
  case 'e':
    FileStr.c[0] = 'B';
    break;
  default:
    break;
  }
  if (FileStr.c[3] == 'p') {
    FileStr.c[3] = 'P';
  }
  for (i = 0; i < MAX_FIX_FILE; i ++) {
    if (FileStr.dw == cdwFixFile[i]) {
      if (pos.sdPlayer == 0) {
        return MOVE(cucFixMove[i][0], cucFixMove[i][1]);
      } else {
        return MOVE(SQUARE_FLIP(cucFixMove[i][0]), SQUARE_FLIP(cucFixMove[i][1]));
      }
    }
  }

  // 2. 如果不是这28种固定纵线表示，那么把棋子、位置和纵线序号(列号)解析出来
  nPos = Byte2Direct(FileStr.c[0]);
  if (nPos == MAX_DIRECT) {
    pt = Byte2Piece(FileStr.c[0]);
    nPos = Byte2Pos(FileStr.c[1]);
  } else {
    pt = Byte2Piece(FileStr.c[1]);
    nPos += DIRECT_TO_POS;
  }
  if (nPos == MAX_POS) {

    // 3. 如果棋子是用列号表示的，那么可以直接根据纵线来找到棋子序号；
    xSrc = Byte2Digit(FileStr.c[1]);
    if (pt == KING_TYPE) {
      sq = FILESQ_SIDE_PIECE(pos, 0);
    } else if (pt >= KNIGHT_TYPE && pt <= PAWN_TYPE) {
      j = (pt == PAWN_TYPE ? 5 : 2);
      for (i = 0; i < j; i ++) {
        sq = FILESQ_SIDE_PIECE(pos, FIRST_PIECE(pt, i));
        if (sq != -1) {
          if (FILESQ_FILE_X(sq) == xSrc) {
            break;
          }
        }
      }
      sq = (i == j ? -1 : sq);
    } else {
      sq = -1;
    }
  } else {

    // 4. 如果棋子是用位置表示的，那么必须挑选出含有多个该种棋子的所有纵线，这是本函数最难处理的地方；
    if (pt >= KNIGHT_TYPE && pt <= PAWN_TYPE) {
      for (i = 0; i < 9; i ++) {
        nFileList[i] = 0;
      }
      j = (pt == PAWN_TYPE ? 5 : 2);
      for (i = 0; i < j; i ++) {
        sq = FILESQ_SIDE_PIECE(pos, FIRST_PIECE(pt, i));
        if (sq != -1) {
          nFileList[FILESQ_FILE_X(sq)] ++;
        }
      }
      nPieceNum = 0;
      for (i = 0; i < j; i ++) {
        sq = FILESQ_SIDE_PIECE(pos, FIRST_PIECE(pt, i));
        if (sq != -1) {
          if (nFileList[FILESQ_FILE_X(sq)] > 1) {
            nPieceList[nPieceNum] = FIRST_PIECE(pt, i);
            nPieceNum ++;
          }
        }
      }

      // 5. 找到这些纵线以后，对这些纵线上的棋子进行排序，然后根据位置来确定棋子序号；
      for (i = 0; i < nPieceNum - 1; i ++) {
        for (j = nPieceNum - 1; j > i; j --) {
          if (FILESQ_SIDE_PIECE(pos, nPieceList[j - 1]) > FILESQ_SIDE_PIECE(pos, nPieceList[j])) {
            SWAP(nPieceList[j - 1], nPieceList[j]);
          }
        }
      }
      // 提示：如果只有两个棋子，那么“后”表示第二个棋子，如果有多个棋子，
      // 那么“一二三四五”依次代表第一个到第五个棋子，“前中后”依次代表第一个到第三个棋子。
      if (nPieceNum == 2 && nPos == 2 + DIRECT_TO_POS) {
        sq = FILESQ_SIDE_PIECE(pos, nPieceList[1]);
      } else {
        nPos -= (nPos >= DIRECT_TO_POS ? DIRECT_TO_POS : 0);
        sq = (nPos >= nPieceNum ? -1 : FILESQ_SIDE_PIECE(pos, nPieceList[nPos]));
      }
    } else {
      sq = -1;
    }
  }
  if (sq == -1) {
    return 0;
  }

  // 6. 现在已知了着法的起点，就可以根据纵线表示的后两个符号来确定着法的终点；
  xSrc = FILESQ_FILE_X(sq);
  ySrc = FILESQ_RANK_Y(sq);
  if (pt == KNIGHT_TYPE) {
    // 提示：马的进退处理比较特殊。
    xDst = Byte2Digit(FileStr.c[3]);
    if (FileStr.c[2] == '+') {
      yDst = ySrc - 3 + ABS(xDst - xSrc);
    } else {
      yDst = ySrc + 3 - ABS(xDst - xSrc);
    }
  } else {
    if (FileStr.c[2] == '+') {
      xDst = xSrc;
      yDst = ySrc - Byte2Digit(FileStr.c[3]) - 1;
    } else if (FileStr.c[2] == '-') {
      xDst = xSrc;
      yDst = ySrc + Byte2Digit(FileStr.c[3]) + 1;
    } else {
      xDst = Byte2Digit(FileStr.c[3]);
      yDst = ySrc;
    }
  }
  // 注意：yDst有可能超过范围！
  if (yDst < 0 || yDst > 9) {
    return 0;
  }

  // 7. 把相对走子方的坐标转换为固定坐标，得到着法的起点和终点。
  if (pos.sdPlayer == 0) {
    return MOVE(FILESQ_SQUARE(FILESQ_COORD_XY(xSrc, ySrc)), FILESQ_SQUARE(FILESQ_COORD_XY(xDst, yDst)));
  } else {
    return MOVE(SQUARE_FLIP(FILESQ_SQUARE(FILESQ_COORD_XY(xSrc, ySrc))),
        SQUARE_FLIP(FILESQ_SQUARE(FILESQ_COORD_XY(xDst, yDst))));
  }
}

// 将内部着法表示转换为纵线符号
uint32_t Move2File(int mv, const PositionStruct &pos) {
  int i, j, sq, pc, pt, nPieceNum;
  int xSrc, ySrc, xDst, yDst;
  int nFileList[9], nPieceList[5];
  C4dwStruct Ret;

  if (SRC(mv) == 0 || DST(mv) == 0) {
    return 0x20202020;
  }
  pc = pos.ucpcSquares[SRC(mv)];
  if (pc == 0) {
    return 0x20202020;
  }
  pt = PIECE_TYPE(pc);
  Ret.c[0] = PIECE_BYTE(pt);
  if (pos.sdPlayer == 0) {
    xSrc = FILESQ_FILE_X(SQUARE_FILESQ(SRC(mv)));
    ySrc = FILESQ_RANK_Y(SQUARE_FILESQ(SRC(mv)));
    xDst = FILESQ_FILE_X(SQUARE_FILESQ(DST(mv)));
    yDst = FILESQ_RANK_Y(SQUARE_FILESQ(DST(mv)));
  } else {
    xSrc = FILESQ_FILE_X(SQUARE_FILESQ(SQUARE_FLIP(SRC(mv))));
    ySrc = FILESQ_RANK_Y(SQUARE_FILESQ(SQUARE_FLIP(SRC(mv))));
    xDst = FILESQ_FILE_X(SQUARE_FILESQ(SQUARE_FLIP(DST(mv))));
    yDst = FILESQ_RANK_Y(SQUARE_FILESQ(SQUARE_FLIP(DST(mv))));
  }
  if (pt >= KING_TYPE && pt <= BISHOP_TYPE) {
    Ret.c[1] = Digit2Byte(xSrc);
  } else {
    for (i = 0; i < 9; i ++) {
      nFileList[i] = 0;
    }
    j = (pt == PAWN_TYPE ? 5 : 2);
    for (i = 0; i < j; i ++) {
      sq = FILESQ_SIDE_PIECE(pos, FIRST_PIECE(pt, i));
      if (sq != -1) {
        nFileList[FILESQ_FILE_X(sq)] ++;
      }
    }
    // 提示：处理“两条的纵线上有多个兵(卒)”的问题上，可参阅"File2Move()"函数。
    if (nFileList[xSrc] > 1) {
      nPieceNum = 0;
      for (i = 0; i < j; i ++) {
        sq = FILESQ_SIDE_PIECE(pos, FIRST_PIECE(pt, i));
        if (sq != -1) {
          if (nFileList[FILESQ_FILE_X(sq)] > 1) {
            nPieceList[nPieceNum] = FIRST_PIECE(pt, i);
            nPieceNum ++;
          }
        }
      }
      for (i = 0; i < nPieceNum - 1; i ++) {
        for (j = nPieceNum - 1; j > i; j --) {
          if (FILESQ_SIDE_PIECE(pos, nPieceList[j - 1]) > FILESQ_SIDE_PIECE(pos, nPieceList[j])) {
            SWAP(nPieceList[j - 1], nPieceList[j]);
          }
        }
      }
      sq = FILESQ_COORD_XY(xSrc, ySrc);
      for (i = 0; i < nPieceNum; i ++) {
        if (FILESQ_SIDE_PIECE(pos, nPieceList[i]) == sq) {
          break;
        }
      }
      Ret.c[1] = (nPieceNum == 2 && i == 1 ? ccPos2Byte[2 + DIRECT_TO_POS] :
          ccPos2Byte[nPieceNum > 3 ? i : i + DIRECT_TO_POS]);
    } else {
      Ret.c[1] = Digit2Byte(xSrc);
    }
  }
  if (pt >= ADVISOR_TYPE && pt <= KNIGHT_TYPE) {
    if (SRC(mv) == DST(mv)) {
      Ret.c[2] = '=';
      Ret.c[3] = 'P';
    } else {
      Ret.c[2] = (yDst > ySrc ? '-' : '+');
      Ret.c[3] = Digit2Byte(xDst);
    }
  } else {
    Ret.c[2] = (yDst == ySrc ? '.' : yDst > ySrc ? '-' : '+');
    Ret.c[3] = (yDst == ySrc ? Digit2Byte(xDst) : Digit2Byte(ABS(ySrc - yDst) - 1));
  }
  return Ret.dw;
}
