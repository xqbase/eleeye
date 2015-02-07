/*
search.h/search.cpp - Source Code for ElephantEye, Part VIII

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.32, Last Modified: May 2012
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

#ifndef CCHESS_A3800
  #include <stdio.h>
#endif
#include "../base/base2.h"
#include "pregen.h"
#include "position.h"
#include "hash.h"
#ifndef CCHESS_A3800
  #include "ucci.h"
  #include "book.h"
#endif
#include "movesort.h"
#include "search.h"

const int IID_DEPTH = 2;         // 内部迭代加深的深度
const int SMP_DEPTH = 6;         // 并行搜索的深度
const int UNCHANGED_DEPTH = 4;   // 未改变最佳着法的深度

const int DROPDOWN_VALUE = 20;   // 落后的分值
const int RESIGN_VALUE = 300;    // 认输的分值
const int DRAW_OFFER_VALUE = 40; // 提和的分值

SearchStruct Search;

// 搜索信息，是封装在模块内部的
static struct {
  int64_t llTime;                     // 计时器
  bool bStop, bPonderStop;            // 中止信号和后台思考认为的中止信号
  bool bPopPv, bPopCurrMove;          // 是否输出pv和currmove
  int nPopDepth, vlPopValue;          // 输出的深度和分值
  int nAllNodes, nMainNodes;          // 总结点数和主搜索树的结点数
  int nUnchanged;                     // 未改变最佳着法的深度
  uint16_t wmvPvLine[MAX_MOVE_NUM];   // 主要变例路线上的着法列表
  uint16_t wmvKiller[LIMIT_DEPTH][2]; // 杀手着法表
  MoveSortStruct MoveSort;            // 根结点的着法序列
} Search2;

#ifndef CCHESS_A3800

void BuildPos(PositionStruct &pos, const UcciCommStruct &UcciComm) {
  int i, mv;
  pos.FromFen(UcciComm.szFenStr);
  for (i = 0; i < UcciComm.nMoveNum; i ++) {
    mv = COORD_MOVE(UcciComm.lpdwMovesCoord[i]);
    if (mv == 0) {
      break;
    }
    if (pos.LegalMove(mv) && pos.MakeMove(mv) && pos.LastMove().CptDrw > 0) {
      // 始终让pos.nMoveNum反映没吃子的步数
      pos.SetIrrev();
    }
  }
}

#endif

// 中断例程
static bool Interrupt(void) {
  if (Search.bIdle) {
    Idle();
  }
  if (Search.nGoMode == GO_MODE_NODES) {
    if (!Search.bPonder && Search2.nAllNodes > Search.nNodes * 4) {
      Search2.bStop = true;
      return true;
    }
  } else if (Search.nGoMode == GO_MODE_TIMER) {
    if (!Search.bPonder && (int) (GetTime() - Search2.llTime) > Search.nMaxTimer) {
      Search2.bStop = true;
      return true;
    }
  }
  if (Search.bBatch) {
    return false;
  }

#ifdef CCHESS_A3800
  return false;
#else
  UcciCommStruct UcciComm;
  PositionStruct posProbe;
  // 如果不是批处理模式，那么先调用UCCI解释程序，再判断是否中止
  switch (BusyLine(UcciComm, Search.bDebug)) {
  case UCCI_COMM_ISREADY:
    // "isready"指令实际上没有意义
    printf("readyok\n");
    fflush(stdout);
    return false;
  case UCCI_COMM_PONDERHIT:
    // "ponderhit"指令启动计时功能，如果"SearchMain()"例程认为已经搜索了足够的时间， 那么发出中止信号
    if (Search2.bPonderStop) {
      Search2.bStop = true;
      return true;
    } else {
      Search.bPonder = false;
      return false;
    }
  case UCCI_COMM_PONDERHIT_DRAW:
    // "ponderhit draw"指令启动计时功能，并设置提和标志
    Search.bDraw = true;
    if (Search2.bPonderStop) {
      Search2.bStop = true;
      return true;
    } else {
      Search.bPonder = false;
      return false;
    }
  case UCCI_COMM_STOP:
    // "stop"指令发送中止信号
    Search2.bStop = true;
    return true;
  case UCCI_COMM_PROBE:
    // "probe"指令输出Hash表信息
    BuildPos(posProbe, UcciComm);
    PopHash(posProbe);
    return false;
  case UCCI_COMM_QUIT:
    // "quit"指令发送退出信号
    Search.bQuit = Search2.bStop = true;
    return true;
  default:
    return false;
  }
#endif
}

#ifndef CCHESS_A3800

// 输出主要变例
static void PopPvLine(int nDepth = 0, int vl = 0) {
  uint16_t *lpwmv;
  uint32_t dwMoveStr;
  // 如果尚未达到需要输出的深度，那么记录该深度和分值，以后再输出
  if (nDepth > 0 && !Search2.bPopPv && !Search.bDebug) {
    Search2.nPopDepth = nDepth;
    Search2.vlPopValue = vl;
    return;
  }
  // 输出时间和搜索结点数
  printf("info time %d nodes %d\n", (int) (GetTime() - Search2.llTime), Search2.nAllNodes);
  fflush(stdout);
  if (nDepth == 0) {
    // 如果是搜索结束后的输出，并且已经输出过，那么不必再输出
    if (Search2.nPopDepth == 0) {
      return;
    }
    // 获取以前没有输出的深度和分值
    nDepth = Search2.nPopDepth;
    vl = Search2.vlPopValue;
  } else {
    // 达到需要输出的深度，那么以后不必再输出
    Search2.nPopDepth = Search2.vlPopValue = 0;
  }
  printf("info depth %d score %d pv", nDepth, vl);
  lpwmv = Search2.wmvPvLine;
  while (*lpwmv != 0) {
    dwMoveStr = MOVE_COORD(*lpwmv);
    printf(" %.4s", (const char *) &dwMoveStr);
    lpwmv ++;
  }
  printf("\n");
  fflush(stdout);
}

#endif

// 无害裁剪
static int HarmlessPruning(const PositionStruct &pos, int vlBeta) {
  int vl, vlRep;

  // 1. 杀棋步数裁剪；
  vl = pos.nDistance - MATE_VALUE;
  if (vl >= vlBeta) {
    return vl;
  }

  // 2. 和棋裁剪；
  if (pos.IsDraw()) {
    return 0; // 安全起见，这里不用"pos.DrawValue()";
  }

  // 3. 重复裁剪；
  vlRep = pos.RepStatus();
  if (vlRep > 0) {
    return pos.RepValue(vlRep);
  }

  return -MATE_VALUE;
}

// 调整型局面评价函数
inline int Evaluate(const PositionStruct &pos, int vlAlpha, int vlBeta) {
  int vl;
  vl = Search.bKnowledge ? pos.Evaluate(vlAlpha, vlBeta) : pos.Material();
  return vl == pos.DrawValue() ? vl - 1 : vl;
}

// 静态搜索例程
static int SearchQuiesc(PositionStruct &pos, int vlAlpha, int vlBeta) {
  int vlBest, vl, mv;
  bool bInCheck;
  MoveSortStruct MoveSort;  
  // 静态搜索例程包括以下几个步骤：
  Search2.nAllNodes ++;

  // 1. 无害裁剪；
  vl = HarmlessPruning(pos, vlBeta);
  if (vl > -MATE_VALUE) {
    return vl;
  }

#ifdef HASH_QUIESC
  // 3. 置换裁剪；
  vl = ProbeHashQ(pos, vlAlpha, vlBeta);
  if (Search.bUseHash && vl > -MATE_VALUE) {
    return vl;
  }
#endif

  // 4. 达到极限深度，直接返回评价值；
  if (pos.nDistance == LIMIT_DEPTH) {
    return Evaluate(pos, vlAlpha, vlBeta);
  }
  __ASSERT(Search.pos.nDistance < LIMIT_DEPTH);

  // 5. 初始化；
  vlBest = -MATE_VALUE;
  bInCheck = (pos.LastMove().ChkChs > 0);

  // 6. 对于被将军的局面，生成全部着法；
  if (bInCheck) {
    MoveSort.InitAll(pos);
  } else {

    // 7. 对于未被将军的局面，在生成着法前首先尝试空着(空着启发)，即对局面作评价；
    vl = Evaluate(pos, vlAlpha, vlBeta);
    __ASSERT_BOUND(1 - WIN_VALUE, vl, WIN_VALUE - 1);
    __ASSERT(vl > vlBest);
    if (vl >= vlBeta) {
#ifdef HASH_QUIESC
      RecordHashQ(pos, vl, MATE_VALUE);
#endif
      return vl;
    }
    vlBest = vl;
    vlAlpha = MAX(vl, vlAlpha);

    // 8. 对于未被将军的局面，生成并排序所有吃子着法(MVV(LVA)启发)；
    MoveSort.InitQuiesc(pos);
  }

  // 9. 用Alpha-Beta算法搜索这些着法；
  while ((mv = MoveSort.NextQuiesc(bInCheck)) != 0) {
    __ASSERT(bInCheck || pos.ucpcSquares[DST(mv)] > 0);
    if (pos.MakeMove(mv)) {
      vl = -SearchQuiesc(pos, -vlBeta, -vlAlpha);
      pos.UndoMakeMove();
      if (vl > vlBest) {
        if (vl >= vlBeta) {
#ifdef HASH_QUIESC
          if (vl > -WIN_VALUE && vl < WIN_VALUE) {
            RecordHashQ(pos, vl, MATE_VALUE);
          }
#endif
          return vl;
        }
        vlBest = vl;
        vlAlpha = MAX(vl, vlAlpha);
      }
    }
  }

  // 10. 返回分值。
  if (vlBest == -MATE_VALUE) {
    __ASSERT(pos.IsMate());
    return pos.nDistance - MATE_VALUE;
  } else {
#ifdef HASH_QUIESC
    if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE) {
      RecordHashQ(pos, vlBest > vlAlpha ? vlBest : -MATE_VALUE, vlBest);
    }
#endif
    return vlBest;
  }
}

#ifndef CCHESS_A3800

// UCCI支持 - 输出叶子结点的局面信息
void PopLeaf(PositionStruct &pos) {
  int vl;
  Search2.nAllNodes = 0;
  vl = SearchQuiesc(pos, -MATE_VALUE, MATE_VALUE);
  printf("pophash lowerbound %d depth 0 upperbound %d depth 0\n", vl, vl);
  fflush(stdout);
}

#endif

const bool NO_NULL = true; // "SearchCut()"的参数，是否禁止空着裁剪

// 零窗口完全搜索例程
static int SearchCut(int vlBeta, int nDepth, bool bNoNull = false) {
  int nNewDepth, vlBest, vl;
  int mvHash, mv, mvEvade;
  MoveSortStruct MoveSort;
  // 完全搜索例程包括以下几个步骤：

  // 1. 在叶子结点处调用静态搜索；
  if (nDepth <= 0) {
    __ASSERT(nDepth >= -NULL_DEPTH);
    return SearchQuiesc(Search.pos, vlBeta - 1, vlBeta);
  }
  Search2.nAllNodes ++;

  // 2. 无害裁剪；
  vl = HarmlessPruning(Search.pos, vlBeta);
  if (vl > -MATE_VALUE) {
    return vl;
  }

  // 3. 置换裁剪；
  vl = ProbeHash(Search.pos, vlBeta - 1, vlBeta, nDepth, bNoNull, mvHash);
  if (Search.bUseHash && vl > -MATE_VALUE) {
    return vl;
  }

  // 4. 达到极限深度，直接返回评价值；
  if (Search.pos.nDistance == LIMIT_DEPTH) {
    return Evaluate(Search.pos, vlBeta - 1, vlBeta);
  }
  __ASSERT(Search.pos.nDistance < LIMIT_DEPTH);

  // 5. 中断调用；
  Search2.nMainNodes ++;
  vlBest = -MATE_VALUE;
  if ((Search2.nMainNodes & Search.nCountMask) == 0 && Interrupt()) {
    return vlBest;
  }

  // 6. 尝试空着裁剪；
  if (Search.bNullMove && !bNoNull && Search.pos.LastMove().ChkChs <= 0 && Search.pos.NullOkay()) {
    Search.pos.NullMove();
    vl = -SearchCut(1 - vlBeta, nDepth - NULL_DEPTH - 1, NO_NULL);
    Search.pos.UndoNullMove();
    if (Search2.bStop) {
      return vlBest;
    }

    if (vl >= vlBeta) {
      if (Search.pos.NullSafe()) {
        // a. 如果空着裁剪不带检验，那么记录深度至少为(NULL_DEPTH + 1)；
        RecordHash(Search.pos, HASH_BETA, vl, MAX(nDepth, NULL_DEPTH + 1), 0);
        return vl;
      } else if (SearchCut(vlBeta, nDepth - NULL_DEPTH, NO_NULL) >= vlBeta) {
        // b. 如果空着裁剪带检验，那么记录深度至少为(NULL_DEPTH)；
        RecordHash(Search.pos, HASH_BETA, vl, MAX(nDepth, NULL_DEPTH), 0);
        return vl;
      }
    }
  }

  // 7. 初始化；
  if (Search.pos.LastMove().ChkChs > 0) {
    // 如果是将军局面，那么生成所有应将着法；
    mvEvade = MoveSort.InitEvade(Search.pos, mvHash, Search2.wmvKiller[Search.pos.nDistance]);
  } else {
    // 如果不是将军局面，那么使用正常的着法列表。
    MoveSort.InitFull(Search.pos, mvHash, Search2.wmvKiller[Search.pos.nDistance]);
    mvEvade = 0;
  }

  // 8. 按照"MoveSortStruct::NextFull()"例程的着法顺序逐一搜索；
  while ((mv = MoveSort.NextFull(Search.pos)) != 0) {
    if (Search.pos.MakeMove(mv)) {

      // 9. 尝试选择性延伸；
      nNewDepth = (Search.pos.LastMove().ChkChs > 0 || mvEvade != 0 ? nDepth : nDepth - 1);

      // 10. 零窗口搜索；
      vl = -SearchCut(1 - vlBeta, nNewDepth);
      Search.pos.UndoMakeMove();
      if (Search2.bStop) {
        return vlBest;
      }

      // 11. 截断判定；
      if (vl > vlBest) {
        vlBest = vl;
        if (vl >= vlBeta) {
          RecordHash(Search.pos, HASH_BETA, vlBest, nDepth, mv);
          if (!MoveSort.GoodCap(Search.pos, mv)) {
            SetBestMove(mv, nDepth, Search2.wmvKiller[Search.pos.nDistance]);
          }
          return vlBest;
        }
      }
    }
  }

  // 12. 不截断措施。
  if (vlBest == -MATE_VALUE) {
    __ASSERT(Search.pos.IsMate());
    return Search.pos.nDistance - MATE_VALUE;
  } else {
    RecordHash(Search.pos, HASH_ALPHA, vlBest, nDepth, mvEvade);
    return vlBest;
  }
}

// 连接主要变例
static void AppendPvLine(uint16_t *lpwmvDst, uint16_t mv, const uint16_t *lpwmvSrc) {
  *lpwmvDst = mv;
  lpwmvDst ++;
  while (*lpwmvSrc != 0) {
    *lpwmvDst = *lpwmvSrc;
    lpwmvSrc ++;
    lpwmvDst ++;
  }
  *lpwmvDst = 0;
}

/* 主要变例完全搜索例程，和零窗口完全搜索的区别有以下几点：
 *
 * 1. 启用内部迭代加深启发；
 * 2. 不使用有负面影响的裁剪；
 * 3. Alpha-Beta边界判定复杂；
 * 4. PV结点要获取主要变例；
 * 5. 考虑PV结点处理最佳着法的情况。
 */
static int SearchPV(int vlAlpha, int vlBeta, int nDepth, uint16_t *lpwmvPvLine) {
  int nNewDepth, nHashFlag, vlBest, vl;
  int mvBest, mvHash, mv, mvEvade;
  MoveSortStruct MoveSort;
  uint16_t wmvPvLine[LIMIT_DEPTH];
  // 完全搜索例程包括以下几个步骤：

  // 1. 在叶子结点处调用静态搜索；
  *lpwmvPvLine = 0;
  if (nDepth <= 0) {
    __ASSERT(nDepth >= -NULL_DEPTH);
    return SearchQuiesc(Search.pos, vlAlpha, vlBeta);
  }
  Search2.nAllNodes ++;

  // 2. 无害裁剪；
  vl = HarmlessPruning(Search.pos, vlBeta);
  if (vl > -MATE_VALUE) {
    return vl;
  }

  // 3. 置换裁剪；
  vl = ProbeHash(Search.pos, vlAlpha, vlBeta, nDepth, NO_NULL, mvHash);
  if (Search.bUseHash && vl > -MATE_VALUE) {
    // 由于PV结点不适用置换裁剪，所以不会发生PV路线中断的情况
    return vl;
  }

  // 4. 达到极限深度，直接返回评价值；
  __ASSERT(Search.pos.nDistance > 0);
  if (Search.pos.nDistance == LIMIT_DEPTH) {
    return Evaluate(Search.pos, vlAlpha, vlBeta);
  }
  __ASSERT(Search.pos.nDistance < LIMIT_DEPTH);

  // 5. 中断调用；
  Search2.nMainNodes ++;
  vlBest = -MATE_VALUE;
  if ((Search2.nMainNodes & Search.nCountMask) == 0 && Interrupt()) {
    return vlBest;
  }

  // 6. 内部迭代加深启发；
  if (nDepth > IID_DEPTH && mvHash == 0) {
    __ASSERT(nDepth / 2 <= nDepth - IID_DEPTH);
    vl = SearchPV(vlAlpha, vlBeta, nDepth / 2, wmvPvLine);
    if (vl <= vlAlpha) {
      vl = SearchPV(-MATE_VALUE, vlBeta, nDepth / 2, wmvPvLine);
    }
    if (Search2.bStop) {
      return vlBest;
    }
    mvHash = wmvPvLine[0];
  }

  // 7. 初始化；
  mvBest = 0;
  nHashFlag = HASH_ALPHA;
  if (Search.pos.LastMove().ChkChs > 0) {
    // 如果是将军局面，那么生成所有应将着法；
    mvEvade = MoveSort.InitEvade(Search.pos, mvHash, Search2.wmvKiller[Search.pos.nDistance]);
  } else {
    // 如果不是将军局面，那么使用正常的着法列表。
    MoveSort.InitFull(Search.pos, mvHash, Search2.wmvKiller[Search.pos.nDistance]);
    mvEvade = 0;
  }

  // 8. 按照"MoveSortStruct::NextFull()"例程的着法顺序逐一搜索；
  while ((mv = MoveSort.NextFull(Search.pos)) != 0) {
    if (Search.pos.MakeMove(mv)) {

      // 9. 尝试选择性延伸；
      nNewDepth = (Search.pos.LastMove().ChkChs > 0 || mvEvade != 0 ? nDepth : nDepth - 1);

      // 10. 主要变例搜索；
      if (vlBest == -MATE_VALUE) {
        vl = -SearchPV(-vlBeta, -vlAlpha, nNewDepth, wmvPvLine);
      } else {
        vl = -SearchCut(-vlAlpha, nNewDepth);
        if (vl > vlAlpha && vl < vlBeta) {
          vl = -SearchPV(-vlBeta, -vlAlpha, nNewDepth, wmvPvLine);
        }
      }
      Search.pos.UndoMakeMove();
      if (Search2.bStop) {
        return vlBest;
      }

      // 11. Alpha-Beta边界判定；
      if (vl > vlBest) {
        vlBest = vl;
        if (vl >= vlBeta) {
          mvBest = mv;
          nHashFlag = HASH_BETA;
          break;
        }
        if (vl > vlAlpha) {
          vlAlpha = vl;
          mvBest = mv;
          nHashFlag = HASH_PV;
          AppendPvLine(lpwmvPvLine, mv, wmvPvLine);
        }
      }
    }
  }

  // 12. 更新置换表、历史表和杀手着法表。
  if (vlBest == -MATE_VALUE) {
    __ASSERT(Search.pos.IsMate());
    return Search.pos.nDistance - MATE_VALUE;
  } else {
    RecordHash(Search.pos, nHashFlag, vlBest, nDepth, mvEvade == 0 ? mvBest : mvEvade);
    if (mvBest != 0 && !MoveSort.GoodCap(Search.pos, mvBest)) {
      SetBestMove(mvBest, nDepth, Search2.wmvKiller[Search.pos.nDistance]);
    }
    return vlBest;
  }
}

/* 根结点搜索例程，和完全搜索的区别有以下几点：
 *
 * 1. 省略无害裁剪(也不获取置换表着法)；
 * 2. 不使用空着裁剪；
 * 3. 选择性延伸只使用将军延伸；
 * 4. 过滤掉禁止着法；
 * 5. 搜索到最佳着法时要做很多处理(包括记录主要变例、结点排序等)；
 * 6. 不更新历史表和杀手着法表。
 */
static int SearchRoot(int nDepth) {
  int nNewDepth, vlBest, vl, mv, nCurrMove;
#ifndef CCHESS_A3800
  uint32_t dwMoveStr;
#endif
  uint16_t wmvPvLine[LIMIT_DEPTH];
  // 根结点搜索例程包括以下几个步骤：

  // 1. 初始化
  vlBest = -MATE_VALUE;
  Search2.MoveSort.ResetRoot();

  // 2. 逐一搜索每个着法(要过滤禁止着法)
  nCurrMove = 0;
  while ((mv = Search2.MoveSort.NextRoot()) != 0) {
    if (Search.pos.MakeMove(mv)) {
#ifndef CCHESS_A3800
      if (Search2.bPopCurrMove || Search.bDebug) {
        dwMoveStr = MOVE_COORD(mv);
        nCurrMove ++;
        printf("info currmove %.4s currmovenumber %d\n", (const char *) &dwMoveStr, nCurrMove);
        fflush(stdout);
      }
#endif

      // 3. 尝试选择性延伸(只考虑将军延伸)
      nNewDepth = (Search.pos.LastMove().ChkChs > 0 ? nDepth : nDepth - 1);

      // 4. 主要变例搜索
      if (vlBest == -MATE_VALUE) {
        vl = -SearchPV(-MATE_VALUE, MATE_VALUE, nNewDepth, wmvPvLine);
        __ASSERT(vl > vlBest);
      } else {
        vl = -SearchCut(-vlBest, nNewDepth);
        if (vl > vlBest) { // 这里不需要" && vl < MATE_VALUE"了
          vl = -SearchPV(-MATE_VALUE, -vlBest, nNewDepth, wmvPvLine);
        }
      }
      Search.pos.UndoMakeMove();
      if (Search2.bStop) {
        return vlBest;
      }

      // 5. Alpha-Beta边界判定("vlBest"代替了"SearchPV()"中的"vlAlpha")
      if (vl > vlBest) {

        // 6. 如果搜索到第一着法，那么"未改变最佳着法"的计数器加1，否则清零
        Search2.nUnchanged = (vlBest == -MATE_VALUE ? Search2.nUnchanged + 1 : 0);
        vlBest = vl;

        // 7. 搜索到最佳着法时记录主要变例
        AppendPvLine(Search2.wmvPvLine, mv, wmvPvLine);
#ifndef CCHESS_A3800
        PopPvLine(nDepth, vl);
#endif

        // 8. 如果要考虑随机性，则Alpha值要作随机浮动，但已搜索到杀棋时不作随机浮动
        if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE) {
          vlBest += (Search.rc4Random.NextLong() & Search.nRandomMask) -
              (Search.rc4Random.NextLong() & Search.nRandomMask);
          vlBest = (vlBest == Search.pos.DrawValue() ? vlBest - 1 : vlBest);
        }

        // 9. 更新根结点着法列表
        Search2.MoveSort.UpdateRoot(mv);
      }
    }
  }
  return vlBest;
}

// 唯一着法检验是ElephantEye在搜索上的一大特色，用来判断用以某种深度进行的搜索是否找到了唯一着法。
// 其原理是把找到的最佳着法设成禁止着法，然后以(-WIN_VALUE, 1 - WIN_VALUE)的窗口重新搜索，
// 如果低出边界则说明其他着法都将被杀。
static bool SearchUnique(int vlBeta, int nDepth) {
  int vl, mv;
  Search2.MoveSort.ResetRoot(ROOT_UNIQUE);
  // 跳过第一个着法
  while ((mv = Search2.MoveSort.NextRoot()) != 0) {
    if (Search.pos.MakeMove(mv)) {
      vl = -SearchCut(1 - vlBeta, Search.pos.LastMove().ChkChs > 0 ? nDepth : nDepth - 1);
      Search.pos.UndoMakeMove();
      if (Search2.bStop || vl >= vlBeta) {
        return false;
      }
    }
  }
  return true;
}

// 主搜索例程
void SearchMain(int nDepth) {
  int i, vl, vlLast, nDraw;
  int nCurrTimer, nLimitTimer, nLimitNodes;
  bool bUnique;
#ifndef CCHESS_A3800
  int nBookMoves;
  uint32_t dwMoveStr;
  BookStruct bks[MAX_GEN_MOVES];
#endif
  // 主搜索例程包括以下几个步骤：

  // 1. 遇到和棋则直接返回
  if (Search.pos.IsDraw() || Search.pos.RepStatus(3) > 0) {
#ifndef CCHESS_A3800
    printf("nobestmove\n");
    fflush(stdout);
#endif
    return;    
  }

#ifndef CCHESS_A3800
  // 2. 从开局库中搜索着法
  if (Search.bUseBook) {
    // a. 获取开局库中的所有走法
    nBookMoves = GetBookMoves(Search.pos, Search.szBookFile, bks);
    if (nBookMoves > 0) {
      vl = 0;
      for (i = 0; i < nBookMoves; i ++) {
        vl += bks[i].wvl;
        dwMoveStr = MOVE_COORD(bks[i].wmv);
        printf("info depth 0 score %d pv %.4s\n", bks[i].wvl, (const char *) &dwMoveStr);
        fflush(stdout);
      }
      // b. 根据权重随机选择一个走法
      vl = Search.rc4Random.NextLong() % (uint32_t) vl;
      for (i = 0; i < nBookMoves; i ++) {
        vl -= bks[i].wvl;
        if (vl < 0) {
          break;
        }
      }
      __ASSERT(vl < 0);
      __ASSERT(i < nBookMoves);
      // c. 如果开局库中的着法够成循环局面，那么不走这个着法
      Search.pos.MakeMove(bks[i].wmv);
      if (Search.pos.RepStatus(3) == 0) {
        dwMoveStr = MOVE_COORD(bks[i].wmv);
        printf("bestmove %.4s", (const char *) &dwMoveStr);
        // d. 给出后台思考的着法(开局库中第一个即权重最大的后续着法)
        nBookMoves = GetBookMoves(Search.pos, Search.szBookFile, bks);
        Search.pos.UndoMakeMove();
        if (nBookMoves > 0) {
          dwMoveStr = MOVE_COORD(bks[0].wmv);
          printf(" ponder %.4s", (const char *) &dwMoveStr);
        }
        printf("\n");
        fflush(stdout);
        return;
      }
      Search.pos.UndoMakeMove();
    }
  }
#endif

  // 3. 如果深度为零则返回静态搜索值
  if (nDepth == 0) {
#ifndef CCHESS_A3800
    printf("info depth 0 score %d\n", SearchQuiesc(Search.pos, -MATE_VALUE, MATE_VALUE));
    fflush(stdout);
    printf("nobestmove\n");
    fflush(stdout);
#endif
    return;
  }

  // 4. 生成根结点的每个着法
  Search2.MoveSort.InitRoot(Search.pos, Search.nBanMoves, Search.wmvBanList);

  // 5. 初始化时间和计数器
  Search2.bStop = Search2.bPonderStop = Search2.bPopPv = Search2.bPopCurrMove = false;
  Search2.nPopDepth = Search2.vlPopValue = 0;
  Search2.nAllNodes = Search2.nMainNodes = Search2.nUnchanged = 0;
  Search2.wmvPvLine[0] = 0;
  ClearKiller(Search2.wmvKiller);
  ClearHistory();
  ClearHash();
  // 由于 ClearHash() 需要消耗一定时间，所以计时从这以后开始比较合理
  Search2.llTime = GetTime();
  vlLast = 0;
  // 如果走了10回合无用着法，那么允许主动提和，以后每隔8回合提和一次
  nDraw = -Search.pos.LastMove().CptDrw;
  if (nDraw > 5 && ((nDraw - 4) / 2) % 8 == 0) {
    Search.bDraw = true;
  }
  bUnique = false;
  nCurrTimer = 0;

  // 6. 做迭代加深搜索
  for (i = 1; i <= nDepth; i ++) {
    // 需要输出主要变例时，第一个"info depth n"是不输出的
#ifndef CCHESS_A3800
    if (Search2.bPopPv || Search.bDebug) {
      printf("info depth %d\n", i);
      fflush(stdout);
    }

    // 7. 根据搜索的时间决定，是否需要输出主要变例和当前思考的着法
    Search2.bPopPv = (nCurrTimer > 300);
    Search2.bPopCurrMove = (nCurrTimer > 3000);
#endif

    // 8. 搜索根结点
    vl = SearchRoot(i);
    if (Search2.bStop) {
      if (vl > -MATE_VALUE) {
        vlLast = vl; // 跳出后，vlLast会用来判断认输或投降，所以需要给定最近一个值
      }
      break; // 没有跳出，则"vl"是可靠值
    }

    nCurrTimer = (int) (GetTime() - Search2.llTime);
    // 9. 如果搜索时间超过适当时限，则终止搜索
    if (Search.nGoMode == GO_MODE_TIMER) {
      // a. 如果没有使用空着裁剪，那么适当时限减半(因为分枝因子加倍了)
      nLimitTimer = (Search.bNullMove ? Search.nProperTimer : Search.nProperTimer / 2);
      // b. 如果当前搜索值没有落后前一层很多，那么适当时限减半
      nLimitTimer = (vl + DROPDOWN_VALUE >= vlLast ? nLimitTimer / 2 : nLimitTimer);
      // c. 如果最佳着法连续多层没有变化，那么适当时限减半
      nLimitTimer = (Search2.nUnchanged >= UNCHANGED_DEPTH ? nLimitTimer / 2 : nLimitTimer);
      if (nCurrTimer > nLimitTimer) {
        if (Search.bPonder) {
          Search2.bPonderStop = true; // 如果处于后台思考模式，那么只是在后台思考命中后立即中止搜索。
        } else {
          vlLast = vl;
          break; // 不管是否跳出，"vlLast"都已更新
        }
      }
    } else if (Search.nGoMode == GO_MODE_NODES) {
      // nLimitNodes的计算方法与nLimitTimer是一样的
      nLimitNodes = (Search.bNullMove ? Search.nNodes : Search.nNodes / 2);
      nLimitNodes = (vl + DROPDOWN_VALUE >= vlLast ? nLimitNodes / 2 : nLimitNodes);
      nLimitNodes = (Search2.nUnchanged >= UNCHANGED_DEPTH ? nLimitNodes / 2 : nLimitNodes);
      // GO_MODE_NODES下是不延长后台思考时间的
      if (Search2.nAllNodes > nLimitNodes) {
        vlLast = vl;
        break;
      }
    }
    vlLast = vl;

    // 10. 搜索到杀棋则终止搜索
    if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE) {
      break;
    }

    // 11. 是唯一着法，则终止搜索
    if (SearchUnique(1 - WIN_VALUE, i)) {
      bUnique = true;
      break;
    }
  }

#ifdef CCHESS_A3800
  Search.mvResult = Search2.wmvPvLine[0];
#else
  // 12. 输出最佳着法及其最佳应对(作为后台思考的猜测着法)
  if (Search2.wmvPvLine[0] != 0) {
    PopPvLine();
    dwMoveStr = MOVE_COORD(Search2.wmvPvLine[0]);
    printf("bestmove %.4s", (const char *) &dwMoveStr);
    if (Search2.wmvPvLine[1] != 0) {
      dwMoveStr = MOVE_COORD(Search2.wmvPvLine[1]);
      printf(" ponder %.4s", (const char *) &dwMoveStr);
    }

    // 13. 判断是否认输或提和，但是经过唯一着法检验的不适合认输或提和(因为搜索深度不够)
    if (!bUnique) {
      if (vlLast > -WIN_VALUE && vlLast < -RESIGN_VALUE) {
        printf(" resign");
      } else if (Search.bDraw && !Search.pos.NullSafe() && vlLast < DRAW_OFFER_VALUE * 2) {
        printf(" draw");
      }
    }
  } else {
    printf("nobestmove");
  }
  printf("\n");
  fflush(stdout);
#endif
}
