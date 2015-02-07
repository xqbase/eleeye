/*
movesort.h/movesort.cpp - Source Code for ElephantEye, Part VII

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.11, Last Modified: Dec. 2007
Copyright (C) 2004-2007 www.elephantbase.net

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

#ifndef MOVESORT_H
#define MOVESORT_H

#include <string.h>
#include "../base/base.h"
#include "position.h"

const int LIMIT_DEPTH = 64;       // 搜索的极限深度
const int SORT_VALUE_MAX = 65535; // 着法序列最大值

extern const int FIBONACCI_LIST[32];

// "nHistory"只在"movesort.cpp"一个模块中使用
extern int nHistory[65536]; // 历史表

// 着法顺序的若干阶段(参阅"NextFull()"函数)
const int PHASE_HASH = 0;
const int PHASE_GEN_CAP = 1;
const int PHASE_GOODCAP = 2;
const int PHASE_KILLER1 = 3;
const int PHASE_KILLER2 = 4;
const int PHASE_GEN_NONCAP = 5;
const int PHASE_REST = 6;

const bool NEXT_ALL = true;    // 着法顺序函数"MoveSortStruct::NextQuiesc()"选项
const bool ROOT_UNIQUE = true; // 着法顺序函数"MoveSortStruct::ResetRoot()"选项

// 着法序列结构
struct MoveSortStruct {
  int nPhase, nMoveIndex, nMoveNum;
  int mvHash, mvKiller1, mvKiller2;
  MoveStruct mvs[MAX_GEN_MOVES];

  void SetHistory(void); // 根据历史表对着法列表赋值
  void ShellSort(void);  // 着法排序过程
  // 好的吃子着法(包括没有着法，都不更新历史表和杀手着法表)
  bool GoodCap(const PositionStruct &pos, int mv) {
    return mv == 0 || nPhase == PHASE_GOODCAP || (nPhase < PHASE_GOODCAP && pos.GoodCap(mv));
  }

  // 静态搜索的着法顺序控制
  void InitAll(const PositionStruct &pos) {
    nMoveIndex = 0;
    nMoveNum = pos.GenAllMoves(mvs);
    SetHistory();
    ShellSort();
  }
  void InitQuiesc(const PositionStruct &pos) {
    nMoveIndex = 0;
    nMoveNum = pos.GenCapMoves(mvs);
    ShellSort();
  }
  void InitQuiesc2(const PositionStruct &pos) {
    nMoveNum += pos.GenNonCapMoves(mvs);
    SetHistory();
    ShellSort();
  }
  int NextQuiesc(bool bNextAll = false) {
    if (nMoveIndex < nMoveNum && (bNextAll || mvs[nMoveIndex].wvl > 0)) {
      nMoveIndex ++;
      return mvs[nMoveIndex - 1].wmv;
    } else {
      return 0;
    }
  }

  // 完全搜索的着法顺序控制
  void InitFull(const PositionStruct &pos, int mv, const uint16_t *lpwmvKiller) {
    nPhase = PHASE_HASH;
    mvHash = mv;
    mvKiller1 = lpwmvKiller[0];
    mvKiller2 = lpwmvKiller[1];
  }
  int InitEvade(PositionStruct &pos, int mv, const uint16_t *lpwmvKiller);
  int NextFull(const PositionStruct &pos);

  // 根结点着法顺序控制
  void InitRoot(const PositionStruct &pos, int nBanMoves, const uint16_t *lpwmvBanList);
  void ResetRoot(bool bUnique = false) {
    nMoveIndex = 0;
    ShellSort();
    nMoveIndex = (bUnique ? 1 : 0);
  }
  int NextRoot(void) {
    if (nMoveIndex < nMoveNum) {
      nMoveIndex ++;
      return mvs[nMoveIndex - 1].wmv;
    } else {
      return 0;
    }
  }
  void UpdateRoot(int mv);
};

// 清空历史表
inline void ClearHistory(void) {
  memset(nHistory, 0, sizeof(int[65536]));
}

// 清空杀手着法表
inline void ClearKiller(uint16_t (*lpwmvKiller)[2]) {
  memset(lpwmvKiller, 0, LIMIT_DEPTH * sizeof(uint16_t[2]));
}

// 复制杀手着法表
inline void CopyKiller(uint16_t (*lpwmvDst)[2], const uint16_t (*lpwmvSrc)[2]) {
  memcpy(lpwmvDst, lpwmvSrc, LIMIT_DEPTH * sizeof(uint16_t[2]));
}
     
/* 找到最佳着法时采取的措施
 *
 * 历史表的深度相关增量有以下几种选择：
 * 1. 平方关系(n^2)；
 * 2. 指数关系(2^n)；
 * 3. Fibonacci数列；
 * 4. 以上几种情况的组合，例如：n^2 + 2^n，等等。
 * ElephantEye使用最传统的平方关系。
 */
inline void SetBestMove(int mv, int nDepth, uint16_t *lpwmvKiller) {
  nHistory[mv] += SQR(nDepth);
  if (lpwmvKiller[0] != mv) {
    lpwmvKiller[1] = lpwmvKiller[0];
    lpwmvKiller[0] = mv;
  }
}

#endif
