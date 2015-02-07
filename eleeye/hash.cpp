/*
hash.h/hash.cpp - Source Code for ElephantEye, Part V

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.31, Last Modified: May 2012
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
#include "../base/base.h"
#include "position.h"
#include "hash.h"

int nHashMask;
HashStruct *hshItems;
#ifdef HASH_QUIESC
  HashStruct *hshItemsQ;
#endif

// 存储置换表局面信息
void RecordHash(const PositionStruct &pos, int nFlag, int vl, int nDepth, int mv) {
  HashStruct hsh;
  int i, nHashDepth, nMinDepth, nMinLayer;
  // 存储置换表局面信息的过程包括以下几个步骤：

  // 1. 对分值做杀棋步数调整；
  __ASSERT_BOUND(1 - MATE_VALUE, vl, MATE_VALUE - 1);
  __ASSERT(mv == 0 || pos.LegalMove(mv));
  if (vl > WIN_VALUE) {
    if (mv == 0 && vl <= BAN_VALUE) {
      return; // 导致长将的局面(不进行置换裁剪)如果连最佳着法也没有，那么没有必要写入置换表
    }
    vl += pos.nDistance;
  } else if (vl < -WIN_VALUE) {
    if (mv == 0 && vl >= -BAN_VALUE) {
      return; // 同理
    }
    vl -= pos.nDistance;
  } else if (vl == pos.DrawValue() && mv == 0) {
    return;   // 同理
  }

  // 2. 逐层试探置换表；
  nMinDepth = 512;
  nMinLayer = 0;
  for (i = 0; i < HASH_LAYERS; i ++) {
    hsh = HASH_ITEM(pos, i);

    // 3. 如果试探到一样的局面，那么更新置换表信息即可；
    if (HASH_POS_EQUAL(hsh, pos)) {
      // 如果深度更深，或者边界缩小，都可更新置换表的值
      if ((nFlag & HASH_ALPHA) != 0 && (hsh.ucAlphaDepth <= nDepth || hsh.svlAlpha >= vl)) {
        hsh.ucAlphaDepth = nDepth;
        hsh.svlAlpha = vl;
      }
      // Beta结点要注意：不要用Null-Move的结点覆盖正常的结点
      if ((nFlag & HASH_BETA) != 0 && (hsh.ucBetaDepth <= nDepth || hsh.svlBeta <= vl) && (mv != 0 || hsh.wmv == 0)) {
        hsh.ucBetaDepth = nDepth;
        hsh.svlBeta = vl;
      }
      // 最佳着法是始终覆盖的
      if (mv != 0) {
        hsh.wmv = mv;
      }
      HASH_ITEM(pos, i) = hsh;
      return;
    }

    // 4. 如果不是一样的局面，那么获得深度最小的置换表项；
    nHashDepth = MAX((hsh.ucAlphaDepth == 0 ? 0 : hsh.ucAlphaDepth + 256),
        (hsh.wmv == 0 ? hsh.ucBetaDepth : hsh.ucBetaDepth + 256));
    __ASSERT(nHashDepth < 512);
    if (nHashDepth < nMinDepth) {
      nMinDepth = nHashDepth;
      nMinLayer = i;
    }
  }

  // 5. 记录置换表。
  hsh.dwZobristLock0 = pos.zobr.dwLock0;
  hsh.dwZobristLock1 = pos.zobr.dwLock1;
  hsh.wmv = mv;
  hsh.ucAlphaDepth = hsh.ucBetaDepth = 0;
  hsh.svlAlpha = hsh.svlBeta = 0;
  if ((nFlag & HASH_ALPHA) != 0) {
    hsh.ucAlphaDepth = nDepth;
    hsh.svlAlpha = vl;
  }
  if ((nFlag & HASH_BETA) != 0) {
    hsh.ucBetaDepth = nDepth;
    hsh.svlBeta = vl;
  }
  HASH_ITEM(pos, nMinLayer) = hsh;
}

/* 判断获取置换表要符合哪些条件，置换表的分值针对四个不同的区间有不同的处理：
 * 一、如果分值在"WIN_VALUE"以内(即介于"-WIN_VALUE"到"WIN_VALUE"之间，下同)，则只获取满足搜索深度要求的局面；
 * 二、如果分值在"WIN_VALUE"和"BAN_VALUE"之间，则不能获取置换表中的值(只能获取最佳着法仅供参考)，目的是防止由于长将而导致的“置换表的不稳定性”；
 * 三、如果分值在"BAN_VALUE"以外，则获取局面时不必考虑搜索深度要求，因为这些局面已经被证明是杀棋了；
 * 四、如果分值是"DrawValue()"(是第一种情况的特殊情况)，则不能获取置换表中的值(原因与第二种情况相同)。
 * 注意：对于第三种情况，要对杀棋步数进行调整！
 */
inline int ValueAdjust(const PositionStruct &pos, bool &bBanNode, bool &bMateNode, int vl) {
  bBanNode = bMateNode = false;
  if (vl > WIN_VALUE) {
    if (vl <= BAN_VALUE) {
      bBanNode = true;
    } else {
      bMateNode = true;
      vl -= pos.nDistance;
    }
  } else if (vl < -WIN_VALUE) {
    if (vl >= -BAN_VALUE) {
      bBanNode = true;
    } else {
      bMateNode = true;
      vl += pos.nDistance;
    }
  } else if (vl == pos.DrawValue()) {
    bBanNode = true;
  }
  return vl;
}

// 检测下一个着法是否稳定，有助于减少置换表的不稳定性
inline bool MoveStable(PositionStruct &pos, int mv) {
  // 判断下一个着法是否稳定的依据是：
  // 1. 没有后续着法，则假定是稳定的；
  if (mv == 0) {
    return true;
  }
  // 2. 吃子着法是稳定的；
  __ASSERT(pos.LegalMove(mv));
  if (pos.ucpcSquares[DST(mv)] != 0) {
    return true;
  }
  // 3. 可能因置换表引起路线迁移，使得路线超过"MAX_MOVE_NUM"，此时应立刻终止路线，并假定是稳定的。
  if (!pos.MakeMove(mv)) {
    return true;
  }
  return false;
}

// 检测后续路线是否稳定(不是循环路线)，有助于减少置换表的不稳定性
static bool PosStable(const PositionStruct &pos, int mv) {
  HashStruct hsh;
  int i, nMoveNum;
  bool bStable;
  // pos会沿着路线变化，但最终会还原，所以被视为"const"，而让"posMutable"承担非"const"的角色
  PositionStruct &posMutable = (PositionStruct &) pos;

  __ASSERT(mv != 0);
  nMoveNum = 0;
  bStable = true;
  while (!MoveStable(posMutable, mv)) {
    nMoveNum ++; // "!MoveStable()"表明已经执行了一个着法，以后需要撤消
    // 执行这个着法，如果产生循环，那么终止后续路线，并确认该路线不稳定
    if (posMutable.RepStatus() > 0) {
      bStable = false;
      break;
    }
    // 逐层获取置换表项，方法同"ProbeHash()"
    for (i = 0; i < HASH_LAYERS; i ++) {
      hsh = HASH_ITEM(posMutable, i);
      if (HASH_POS_EQUAL(hsh, posMutable)) {
        break;
      }
    }
    mv = (i == HASH_LAYERS ? 0 : hsh.wmv);
  }
  // 撤消前面执行过的所有着法
  for (i = 0; i < nMoveNum; i ++) {
    posMutable.UndoMakeMove();
  }
  return bStable;
}

// 获取置换表局面信息(没有命中时，返回"-MATE_VALUE")
int ProbeHash(const PositionStruct &pos, int vlAlpha, int vlBeta, int nDepth, bool bNoNull, int &mv) {
  HashStruct hsh;
  int i, vl;
  bool bBanNode, bMateNode;
  // 获取置换表局面信息的过程包括以下几个步骤：

  // 1. 逐层获取置换表项
  mv = 0;
  for (i = 0; i < HASH_LAYERS; i ++) {
    hsh = HASH_ITEM(pos, i);
    if (HASH_POS_EQUAL(hsh, pos)) {
      mv = hsh.wmv;
      __ASSERT(mv == 0 || pos.LegalMove(mv));
      break;
    }
  }
  if (i == HASH_LAYERS) {
    return -MATE_VALUE;
  }

  // 2. 判断是否符合Beta边界
  if (hsh.ucBetaDepth > 0) {
    vl = ValueAdjust(pos, bBanNode, bMateNode, hsh.svlBeta);
    if (!bBanNode && !(hsh.wmv == 0 && bNoNull) && (hsh.ucBetaDepth >= nDepth || bMateNode) && vl >= vlBeta) {
      __ASSERT_BOUND(1 - MATE_VALUE, vl, MATE_VALUE - 1);
      if (hsh.wmv == 0 || PosStable(pos, hsh.wmv)) {
        return vl;
      }
    }
  }

  // 3. 判断是否符合Alpha边界
  if (hsh.ucAlphaDepth > 0) {
    vl = ValueAdjust(pos, bBanNode, bMateNode, hsh.svlAlpha);
    if (!bBanNode && (hsh.ucAlphaDepth >= nDepth || bMateNode) && vl <= vlAlpha) {
      __ASSERT_BOUND(1 - MATE_VALUE, vl, MATE_VALUE - 1);
      if (hsh.wmv == 0 || PosStable(pos, hsh.wmv)) {
        return vl;
      }
    }
  }
  return -MATE_VALUE;
}

#ifdef HASH_QUIESC

// 存储置换表局面信息(静态搜索)
void RecordHashQ(const PositionStruct &pos, int vlBeta, int vlAlpha) {
  volatile HashStruct *lphsh;
  __ASSERT((vlBeta > -WIN_VALUE && vlBeta < WIN_VALUE) || (vlAlpha > -WIN_VALUE && vlAlpha < WIN_VALUE));
  lphsh = hshItemsQ + (pos.zobr.dwKey & nHashMask);
  lphsh->dwZobristLock0 = pos.zobr.dwLock0;
  lphsh->svlAlpha = vlAlpha;
  lphsh->svlBeta = vlBeta;
  lphsh->dwZobristLock1 = pos.zobr.dwLock1;
}

// 获取置换表局面信息(静态搜索)
int ProbeHashQ(const PositionStruct &pos, int vlAlpha, int vlBeta) {
  volatile HashStruct *lphsh;
  int vlHashAlpha, vlHashBeta;

  lphsh = hshItemsQ + (pos.zobr.dwKey & nHashMask);
  if (lphsh->dwZobristLock0 == pos.zobr.dwLock0) {
    vlHashAlpha = lphsh->svlAlpha;
    vlHashBeta = lphsh->svlBeta;
    if (lphsh->dwZobristLock1 == pos.zobr.dwLock1) {
      if (vlHashBeta >= vlBeta) {
        __ASSERT(vlHashBeta > -WIN_VALUE && vlHashBeta < WIN_VALUE);
        return vlHashBeta;
      }
      if (vlHashAlpha <= vlAlpha) {
        __ASSERT(vlHashAlpha > -WIN_VALUE && vlHashAlpha < WIN_VALUE);
        return vlHashAlpha;
      }
    }
  }
  return -MATE_VALUE;
}

#endif

#ifndef CCHESS_A3800

// UCCI支持 - 输出Hash表中的局面信息
bool PopHash(const PositionStruct &pos) {
  HashStruct hsh;
  uint32_t dwMoveStr;
  int i;

  for (i = 0; i < HASH_LAYERS; i ++) {
    hsh = HASH_ITEM(pos, i);
    if (HASH_POS_EQUAL(hsh, pos)) {
      printf("pophash");
      if (hsh.wmv != 0) {
        __ASSERT(pos.LegalMove(hsh.wmv));
        dwMoveStr = MOVE_COORD(hsh.wmv);
        printf(" bestmove %.4s", (const char *) &dwMoveStr);
      }
      if (hsh.ucBetaDepth > 0) {
        printf(" lowerbound %d depth %d", hsh.svlBeta, hsh.ucBetaDepth);
      }
      if (hsh.ucAlphaDepth > 0) {
        printf(" upperbound %d depth %d", hsh.svlAlpha, hsh.ucAlphaDepth);
      }
      printf("\n");
      fflush(stdout);
      return true;
    }
  }
  return false;
}

#endif