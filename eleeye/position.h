/*
position.h/position.cpp - Source Code for ElephantEye, Part III

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com

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

#include <string.h>
#include "../base/base.h"
#include "pregen.h"

/* ElephantEye源程序使用的匈牙利记号约定：
 *
 * sq: 格子序号(整数，从0到255，参阅"pregen.cpp")
 * pc: 棋子序号(整数，从0到47，参阅"position.cpp")
 * pt: 棋子类型序号(整数，从0到6，参阅"position.cpp")
 * mv: 着法(整数，从0到65535，参阅"position.cpp")
 * sd: 走子方(整数，0代表红方，1代表黑方)
 * vl: 局面价值(整数，从"-MATE_VALUE"到"MATE_VALUE"，参阅"position.cpp")
 * (注：以上四个记号可与uc、dw等代表整数的记号配合使用)
 * pos: 局面(PositionStruct类型，参阅"position.h")
 * sms: 位行和位列的着法生成预置结构(参阅"pregen.h")
 * smv: 位行和位列的着法判断预置结构(参阅"pregen.h")
 */

#ifndef POSITION_H
#define POSITION_H

const int MAX_MOVE_NUM = 1024;  // 局面能容纳的回滚着法数
const int MAX_GEN_MOVES = 128;  // 搜索的最大着法数，中国象棋的任何局面都不会超过120个着法
const int DRAW_MOVES = 100;     // 默认的和棋着法数，ElephantEye设定在50回合即100步，但将军和应将不计入其中
const int REP_HASH_MASK = 4095; // 判断重复局面的迷你置换表长度，即4096个表项

const int MATE_VALUE = 10000;           // 最高分值，即将死的分值
const int BAN_VALUE = MATE_VALUE - 100; // 长将判负的分值，低于该值将不写入置换表(参阅"hash.cpp")
const int WIN_VALUE = MATE_VALUE - 200; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
const int NULLOKAY_MARGIN = 200;        // 空着裁剪可以不检验的子力价值边界
const int NULLSAFE_MARGIN = 400;        // 允许使用空着裁剪的条件的子力价值边界
const int DRAW_VALUE = 20;              // 和棋时返回的分数(取负值)

const bool CHECK_LAZY = true;   // 偷懒检测将军
const int CHECK_MULTI = 48;     // 被多个子将军

// 每种子力的类型编号
const int KING_TYPE = 0;
const int ADVISOR_TYPE = 1;
const int BISHOP_TYPE = 2;
const int KNIGHT_TYPE = 3;
const int ROOK_TYPE = 4;
const int CANNON_TYPE = 5;
const int PAWN_TYPE = 6;

// 每种子力的开始序号和结束序号
const int KING_FROM = 0;
const int ADVISOR_FROM = 1;
const int ADVISOR_TO = 2;
const int BISHOP_FROM = 3;
const int BISHOP_TO = 4;
const int KNIGHT_FROM = 5;
const int KNIGHT_TO = 6;
const int ROOK_FROM = 7;
const int ROOK_TO = 8;
const int CANNON_FROM = 9;
const int CANNON_TO = 10;
const int PAWN_FROM = 11;
const int PAWN_TO = 15;

// 各种子力的屏蔽位
const int KING_BITPIECE = 1 << KING_FROM;
const int ADVISOR_BITPIECE = (1 << ADVISOR_FROM) | (1 << ADVISOR_TO);
const int BISHOP_BITPIECE = (1 << BISHOP_FROM) | (1 << BISHOP_TO);
const int KNIGHT_BITPIECE = (1 << KNIGHT_FROM) | (1 << KNIGHT_TO);
const int ROOK_BITPIECE = (1 << ROOK_FROM) | (1 << ROOK_TO);
const int CANNON_BITPIECE = (1 << CANNON_FROM) | (1 << CANNON_TO);
const int PAWN_BITPIECE = (1 << PAWN_FROM) | (1 << (PAWN_FROM + 1)) |
    (1 << (PAWN_FROM + 2)) | (1 << (PAWN_FROM + 3)) | (1 << PAWN_TO);
const int ATTACK_BITPIECE = KNIGHT_BITPIECE | ROOK_BITPIECE | CANNON_BITPIECE | PAWN_BITPIECE;

inline uint32_t BIT_PIECE(int pc) {
  return 1 << (pc - 16);
}

inline uint32_t WHITE_BITPIECE(int nBitPiece) {
  return nBitPiece;
}

inline uint32_t BLACK_BITPIECE(int nBitPiece) {
  return nBitPiece << 16;
}

inline uint32_t BOTH_BITPIECE(int nBitPiece) {
  return nBitPiece + (nBitPiece << 16);
}

// "RepStatus()"返回的重复标记位
const int REP_NONE = 0;
const int REP_DRAW = 1;
const int REP_LOSS = 3;
const int REP_WIN = 5;

/* ElephantEye的很多代码中都用到"SIDE_TAG()"这个量，红方设为16，黑方设为32。
 * 用"SIDE_TAG() + i"可以方便地选择棋子的类型，"i"从0到15依次是：
 * 帅仕仕相相马马车车炮炮兵兵兵兵兵(将士士象象马马车车炮炮卒卒卒卒卒)
 * 例如"i"取"KNIGHT_FROM"到"KNIGHT_TO"，则表示依次检查两个马的位置
 */
inline int SIDE_TAG(int sd) {
  return 16 + (sd << 4);
}

inline int OPP_SIDE_TAG(int sd) {
  return 32 - (sd << 4);
}

inline int SIDE_VALUE(int sd, int vl) {
  return (sd == 0 ? vl : -vl);
}

inline int PIECE_INDEX(int pc) {
  return pc & 15;
}

extern const char *const cszStartFen;     // 起始局面的FEN串
extern const char *const cszPieceBytes;   // 棋子类型对应的棋子符号
extern const int cnPieceTypes[48];        // 棋子序号对应的棋子类型
extern const int cnSimpleValues[48];      // 棋子的简单分值
extern const uint8_t cucsqMirrorTab[256]; // 坐标的镜像(左右对称)数组

inline char PIECE_BYTE(int pt) {
  return cszPieceBytes[pt];
}

inline int PIECE_TYPE(int pc) {
  return cnPieceTypes[pc];
}

inline int SIMPLE_VALUE(int pc) {
  return cnSimpleValues[pc];
}

inline uint8_t SQUARE_MIRROR(int sq) {
  return cucsqMirrorTab[sq];
}

// FEN串中棋子标识
int FenPiece(int Arg);

// 复杂着法结构
union MoveStruct {
  uint32_t dwmv;           // 填满整个结构用
  struct {
    uint16_t wmv, wvl;     // 着法和分值
  };
  struct {
    uint8_t Src, Dst;      // 起始格和目标格
    int8_t CptDrw, ChkChs; // 被吃子(+)/和棋着法数(-)、将军子(+)/被捉子(-)
  };
}; // mvs

// 着法结构
inline int SRC(int mv) { // 得到着法的起点
  return mv & 255;
}

inline int DST(int mv) { // 得到着法的终点
  return mv >> 8;
}

inline int MOVE(int sqSrc, int sqDst) {   // 由起点和终点得到着法
  return sqSrc + (sqDst << 8);
}

inline uint32_t MOVE_COORD(int mv) {      // 把着法转换成字符串
  union {
    char c[4];
    uint32_t dw;
  } Ret;
  Ret.c[0] = FILE_X(SRC(mv)) - FILE_LEFT + 'a';
  Ret.c[1] = '9' - RANK_Y(SRC(mv)) + RANK_TOP;
  Ret.c[2] = FILE_X(DST(mv)) - FILE_LEFT + 'a';
  Ret.c[3] = '9' - RANK_Y(DST(mv)) + RANK_TOP;
  // 断言输出着法的合理性
  __ASSERT_BOUND('a', Ret.c[0], 'i');
  __ASSERT_BOUND('0', Ret.c[1], '9');
  __ASSERT_BOUND('a', Ret.c[2], 'i');
  __ASSERT_BOUND('0', Ret.c[3], '9');
  return Ret.dw;
}

inline int COORD_MOVE(uint32_t dwMoveStr) { // 把字符串转换成着法
  int sqSrc, sqDst;
  char *lpArgPtr;
  lpArgPtr = (char *) &dwMoveStr;
  sqSrc = COORD_XY(lpArgPtr[0] - 'a' + FILE_LEFT, '9' - lpArgPtr[1] + RANK_TOP);
  sqDst = COORD_XY(lpArgPtr[2] - 'a' + FILE_LEFT, '9' - lpArgPtr[3] + RANK_TOP);
  // 对输入着法的合理性不作断言
  // __ASSERT_SQUARE(sqSrc);
  // __ASSERT_SQUARE(sqDst);
  return (IN_BOARD(sqSrc) && IN_BOARD(sqDst) ? MOVE(sqSrc, sqDst) : 0);
}

inline int MOVE_MIRROR(int mv) {          // 对着法做镜像
  return MOVE(SQUARE_MIRROR(SRC(mv)), SQUARE_MIRROR(DST(mv)));
}

// 回滚结构
struct RollbackStruct {
  ZobristStruct zobr;   // Zobrist
  int vlWhite, vlBlack; // 红方和黑方的子力价值
  MoveStruct mvs;       // 着法
}; // rbs

const bool DEL_PIECE = true; // 函数"PositionStruct::AddPiece()"的选项

// 局面结构
struct PositionStruct {
  // 基本成员
  int sdPlayer;             // 轮到哪方走，0表示红方，1表示黑方
  uint8_t ucpcSquares[256]; // 每个格子放的棋子，0表示没有棋子
  uint8_t ucsqPieces[48];   // 每个棋子放的位置，0表示被吃
  ZobristStruct zobr;       // Zobrist

  // 位结构成员，用来增强棋盘的处理
  union {
    uint32_t dwBitPiece;    // 32位的棋子位，0到31位依次表示序号为16到47的棋子是否还在棋盘上
    uint16_t wBitPiece[2];  // 拆分成两个
  };
  uint16_t wBitRanks[16];   // 位行数组，注意用法是"wBitRanks[RANK_Y(sq)]"
  uint16_t wBitFiles[16];   // 位列数组，注意用法是"wBitFiles[FILE_X(sq)]"

  // 局面评价数据
  int vlWhite, vlBlack;   // 红方和黑方的子力价值

  // 回滚着法，用来检测循环局面
  int nMoveNum, nDistance;              // 回滚着法数和搜索深度
  RollbackStruct rbsList[MAX_MOVE_NUM]; // 回滚列表
  uint8_t ucRepHash[REP_HASH_MASK + 1]; // 判断重复局面的迷你置换表

  // 获取着法预生成信息
  SlideMoveStruct *RankMovePtr(int x, int y) const {
    return PreGen.smvRankMoveTab[x - FILE_LEFT] + wBitRanks[y];
  }
  SlideMoveStruct *FileMovePtr(int x, int y) const {
    return PreGen.smvFileMoveTab[y - RANK_TOP] + wBitFiles[x];
  }
  SlideMaskStruct *RankMaskPtr(int x, int y) const {
    return PreGen.smsRankMaskTab[x - FILE_LEFT] + wBitRanks[y];
  }
  SlideMaskStruct *FileMaskPtr(int x, int y) const {
    return PreGen.smsFileMaskTab[y - RANK_TOP] + wBitFiles[x];
  }

  // 棋盘处理过程
  void ClearBoard(void) { // 棋盘初始化
    sdPlayer = 0;
    memset(ucpcSquares, 0, 256);
    memset(ucsqPieces, 0, 48);
    zobr.InitZero();
    dwBitPiece = 0;
    memset(wBitRanks, 0, 16 * sizeof(uint16_t));
    memset(wBitFiles, 0, 16 * sizeof(uint16_t));
    vlWhite = vlBlack = 0;
    // "ClearBoard()"后面紧跟的是"SetIrrev()"，来初始化其它成员
  }
  void ChangeSide(void) { // 交换走棋方
    sdPlayer = OPP_SIDE(sdPlayer);
    zobr.Xor(PreGen.zobrPlayer);
  }
  void SaveStatus(void) { // 保存状态
    RollbackStruct *lprbs;
    lprbs = rbsList + nMoveNum;
    lprbs->zobr = zobr;
    lprbs->vlWhite = vlWhite;
    lprbs->vlBlack = vlBlack;
  }
  void Rollback(void) {   // 回滚
    RollbackStruct *lprbs;
    lprbs = rbsList + nMoveNum;
    zobr = lprbs->zobr;
    vlWhite = lprbs->vlWhite;
    vlBlack = lprbs->vlBlack;
  }
  void AddPiece(int sq, int pc, bool bDel = false); // 棋盘上增加棋子
  int MovePiece(int mv);                            // 移动棋子
  void UndoMovePiece(int mv, int pcCaptured);       // 撤消移动棋子
  int Promote(int sq);                              // 升变
  void UndoPromote(int sq, int pcCaptured);         // 撤消升变

  // 着法处理过程
  bool MakeMove(int mv);   // 执行一个着法
  void UndoMakeMove(void); // 撤消一个着法
  void NullMove(void);     // 执行一个空着
  void UndoNullMove(void); // 撤消一个空着
  void SetIrrev(void) {    // 把局面设成“不可逆”，即清除回滚着法
    rbsList[0].mvs.dwmv = 0; // wmv, Chk, CptDrw, ChkChs = 0
    rbsList[0].mvs.ChkChs = CheckedBy();
    nMoveNum = 1;
    nDistance = 0;
    memset(ucRepHash, 0, REP_HASH_MASK + 1);
  }

  // 局面处理过程
  void FromFen(const char *szFen); // FEN串识别
  void ToFen(char *szFen) const;   // 生成FEN串
  void Mirror(void);               // 局面镜像

  // 着法检测过程
  bool GoodCap(int mv) const {     // 好的吃子着法检测，这样的着法不记录到历史表和杀手着法表中
    int pcMoved, pcCaptured;
    pcCaptured = ucpcSquares[DST(mv)];
    if (pcCaptured == 0) {
      return false;
    }
    if (!Protected(OPP_SIDE(sdPlayer), DST(mv))) {
      return true;
    }
    pcMoved = ucpcSquares[SRC(mv)];
    return SIMPLE_VALUE(pcCaptured) > SIMPLE_VALUE(pcMoved);
  }
  bool LegalMove(int mv) const;            // 着法合理性检测，仅用在“杀手着法”的检测中
  int CheckedBy(bool bLazy = false) const; // 被哪个子将军
  bool IsMate(void);                       // 判断是已被将死
  MoveStruct LastMove(void) const {        // 前一步着法，该着法保存了局面的将军状态
    return rbsList[nMoveNum - 1].mvs;
  }
  bool CanPromote(void) const {            // 判断是否能升变
    return (wBitPiece[sdPlayer] & PAWN_BITPIECE) != PAWN_BITPIECE && LastMove().ChkChs <= 0;
  }
  bool NullOkay(void) const {              // 允许使用空着裁剪的条件
    return (sdPlayer == 0 ? vlWhite : vlBlack) > NULLOKAY_MARGIN;
  }
  bool NullSafe(void) const {              // 空着裁剪可以不检验的条件
    return (sdPlayer == 0 ? vlWhite : vlBlack) > NULLSAFE_MARGIN;
  }
  bool IsDraw(void) const {                // 和棋判断
    return (!PreEval.bPromotion && (dwBitPiece & BOTH_BITPIECE(ATTACK_BITPIECE)) == 0) ||
        -LastMove().CptDrw >= DRAW_MOVES || nMoveNum == MAX_MOVE_NUM;
  }
  int RepStatus(int nRecur = 1) const;     // 重复局面检测
  int DrawValue(void) const {              // 和棋的分值
    return (nDistance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
  }
  int RepValue(int vlRep) const {          // 重复局面的分值
    // return vlRep == REP_LOSS ? nDistance - MATE_VALUE : vlRep == REP_WIN ? MATE_VALUE - nDistance : DrawValue();
    // 长将判负的分值，低于BAN_VALUE将不写入置换表(参阅"hash.cpp")
    return vlRep == REP_LOSS ? nDistance - BAN_VALUE : vlRep == REP_WIN ? BAN_VALUE - nDistance : DrawValue();
  }
  int Material(void) const {               // 子力平衡，包括先行权因素
    return SIDE_VALUE(sdPlayer, vlWhite - vlBlack) + PreEval.vlAdvanced;
  }

  // 着法生成过程，由于这些过程代码量特别大，所以把他们都集中在"genmoves.cpp"中
  bool Protected(int sd, int sqSrc, int sqExcept = 0) const; // 棋子保护判断
  int ChasedBy(int mv) const;                                // 捉哪个子
  int MvvLva(int sqDst, int pcCaptured, int nLva) const;     // 计算MVV(LVA)值
  int GenCapMoves(MoveStruct *lpmvs) const;                  // 吃子着法生成器
  int GenNonCapMoves(MoveStruct *lpmvs) const;               // 不吃子着法生成器
  int GenAllMoves(MoveStruct *lpmvs) const {                 // 全部着法生成器
    int nCapNum;
    nCapNum = GenCapMoves(lpmvs);
    return nCapNum + GenNonCapMoves(lpmvs + nCapNum);
  }

  // 着法生成过程，由于这些过程代码量特别大，所以把他们都集中在"preeval.cpp"和"evaluate.cpp"中
  void PreEvaluate(void);
  int AdvisorShape(void) const;
  int StringHold(void) const;
  int RookMobility(void) const;
  int KnightTrap(void) const;
  int Evaluate(int vlAlpha, int vlBeta) const;
}; // pos

#endif
