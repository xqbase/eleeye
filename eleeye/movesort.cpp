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

#include "../base/base.h"
#include "position.h"
#include "movesort.h"

int nHistory[65536]; // 历史表

// 根据历史表对着法列表赋值
void MoveSortStruct::SetHistory(void) {
  int i, j, vl, nShift, nNewShift;
  nShift = 0;
  for (i = nMoveIndex; i < nMoveNum; i ++) {
    // 如果着法的分值超过65536，就必需对所有着法的分值作缩减，使它们都不超过65536
    vl = nHistory[mvs[i].wmv] >> nShift;
    if (vl > 65535) {
      nNewShift = Bsr(vl) - 15;
      for (j = nMoveIndex; j < i; j ++) {
        mvs[j].wvl >>= nNewShift;
      }
      vl >>= nNewShift;
      __ASSERT_BOUND(32768, vl, 65535);
      nShift += nNewShift;
    }
    mvs[i].wvl = vl;
  }
}

// Shell排序法，这里用"1, 4, 13, 40 ..."的序列，这样要比"1, 2, 4, 8, ..."好
static const int cnShellStep[8] = {0, 1, 4, 13, 40, 121, 364, 1093};

void MoveSortStruct::ShellSort(void) {
  int i, j, nStep, nStepLevel;
  MoveStruct mvsBest;
  nStepLevel = 1;
  while (cnShellStep[nStepLevel] < nMoveNum - nMoveIndex) {
    nStepLevel ++;
  }
  nStepLevel --;
  while (nStepLevel > 0) {
    nStep = cnShellStep[nStepLevel];
    for (i = nMoveIndex + nStep; i < nMoveNum; i ++) {
      mvsBest = mvs[i];
      j = i - nStep;
      while (j >= nMoveIndex && mvsBest.wvl > mvs[j].wvl) {
        mvs[j + nStep] = mvs[j];
        j -= nStep;
      }
      mvs[j + nStep] = mvsBest;
    }
    nStepLevel --;
  }
}

/* 生成解将着法，返回唯一应将着法(有多个应将着法则返回零)
 * 
 * 解将着法的顺序如下：
 * 1. 置换表着法(SORT_VALUE_MAX)；
 * 2. 两个杀手着法(SORT_VALUE_MAX - 1或2)；
 * 3. 其他着法，按历史表排序(从1到SORT_VALUE_MAX - 3)；
 * 4. 不能解将的着法(0)，这些着法会过滤掉。
 */
int MoveSortStruct::InitEvade(PositionStruct &pos, int mv, const uint16_t *lpwmvKiller) {
  int i, nLegal;
  nPhase = PHASE_REST;
  nMoveIndex = 0;
  nMoveNum = pos.GenAllMoves(mvs);
  SetHistory();
  nLegal = 0;
  for (i = nMoveIndex; i < nMoveNum; i ++) {
    if (mvs[i].wmv == mv) {
      nLegal ++;
      mvs[i].wvl = SORT_VALUE_MAX;
    } else if (pos.MakeMove(mvs[i].wmv)) {
      pos.UndoMakeMove();
      nLegal ++;
      if (mvs[i].wmv == lpwmvKiller[0]) {
        mvs[i].wvl = SORT_VALUE_MAX - 1;
      } else if (mvs[i].wmv == lpwmvKiller[1]) {
        mvs[i].wvl = SORT_VALUE_MAX - 2;
      } else {
        mvs[i].wvl = MIN(mvs[i].wvl + 1, SORT_VALUE_MAX - 3);
      }
    } else {
      mvs[i].wvl = 0;
    }
  }
  ShellSort();
  nMoveNum = nMoveIndex + nLegal;
  return (nLegal == 1 ? mvs[0].wmv : 0);
}

// 给出下一个即将搜索的着法
int MoveSortStruct::NextFull(const PositionStruct &pos) {
  switch (nPhase) {
  // "nPhase"表示着法启发的若干阶段，依次为：

  // 0. 置换表着法启发，完成后立即进入下一阶段；
  case PHASE_HASH:
    nPhase = PHASE_GEN_CAP;
    if (mvHash != 0) {
      __ASSERT(pos.LegalMove(mvHash));
      return mvHash;
    }
    // 技巧：这里没有"break"，表示"switch"的上一个"case"执行完后紧接着做下一个"case"，下同

  // 1. 生成所有吃子着法，完成后立即进入下一阶段；
  case PHASE_GEN_CAP:
    nPhase = PHASE_GOODCAP;
    nMoveIndex = 0;
    nMoveNum = pos.GenCapMoves(mvs);
    ShellSort();

  // 2. MVV(LVA)启发，可能要循环若干次；
  case PHASE_GOODCAP:
    if (nMoveIndex < nMoveNum && mvs[nMoveIndex].wvl > 1) {
      // 注意：MVV(LVA)值不超过1，则说明吃子不是直接能获得优势的，这些着法将留在以后搜索
      nMoveIndex ++;
      __ASSERT_PIECE(pos.ucpcSquares[DST(mvs[nMoveIndex - 1].wmv)]);
      return mvs[nMoveIndex - 1].wmv;
    }

  // 3. 杀手着法启发(第一个杀手着法)，完成后立即进入下一阶段；
  case PHASE_KILLER1:
    nPhase = PHASE_KILLER2;
    if (mvKiller1 != 0 && pos.LegalMove(mvKiller1)) {
      // 注意：杀手着法必须检验着法合理性，下同
      return mvKiller1;
    }

  // 4. 杀手着法启发(第二个杀手着法)，完成后立即进入下一阶段；
  case PHASE_KILLER2:
    nPhase = PHASE_GEN_NONCAP;
    if (mvKiller2 != 0 && pos.LegalMove(mvKiller2)) {
      return mvKiller2;
    }

  // 5. 生成所有不吃子着法，完成后立即进入下一阶段；
  case PHASE_GEN_NONCAP:
    nPhase = PHASE_REST;
    nMoveNum += pos.GenNonCapMoves(mvs + nMoveNum);
    SetHistory();
    ShellSort();

  // 6. 对剩余着法做历史表启发(包括返回解将着法)；
  case PHASE_REST:
    if (nMoveIndex < nMoveNum) {
      nMoveIndex ++;
      return mvs[nMoveIndex - 1].wmv;
    }

  // 7. 没有着法了，返回零。
  default:
    return 0;
  }
}

// 生成根结点的着法
void MoveSortStruct::InitRoot(const PositionStruct &pos, int nBanMoves, const uint16_t *lpwmvBanList) {
  int i, j, nBanned;
  nMoveIndex = 0;
  nMoveNum = pos.GenAllMoves(mvs);
  nBanned = 0;
  for (i = 0; i < nMoveNum; i ++) {
    mvs[i].wvl = 1;
    for (j = 0; j < nBanMoves; j ++) {
      if (mvs[i].wmv == lpwmvBanList[j]) {
        mvs[i].wvl = 0;
        nBanned ++;
        break;
      }
    }  
  }
  ShellSort();
  nMoveNum -= nBanned;
}

// 更新根结点的着法排序列表
void MoveSortStruct::UpdateRoot(int mv) {
  int i;
  for (i = 0; i < nMoveNum; i ++) {
    if (mvs[i].wmv == mv) {
      mvs[i].wvl = SORT_VALUE_MAX;
    } else if (mvs[i].wvl > 0) {
      mvs[i].wvl --;      
    }
  }
}
