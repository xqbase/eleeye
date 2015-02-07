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

#include <string.h>
#include "../base/base.h"
#include "position.h"

#ifndef HASH_H
#define HASH_H

// 置换表标志，只用在"RecordHash()"函数中
const int HASH_BETA = 1;
const int HASH_ALPHA = 2;
const int HASH_PV = HASH_ALPHA | HASH_BETA;

const int HASH_LAYERS = 2;   // 置换表的层数
const int NULL_DEPTH = 2;    // 空着裁剪的深度

// 置换表结构，置换表信息夹在两个Zobrist校验锁中间，可以防止存取冲突
struct HashStruct {
  uint32_t dwZobristLock0;           // Zobrist校验锁，第一部分
  uint16_t wmv;                      // 最佳着法
  uint8_t ucAlphaDepth, ucBetaDepth; // 深度(上边界和下边界)
  int16_t svlAlpha, svlBeta;         // 分值(上边界和下边界)
  uint32_t dwZobristLock1;           // Zobrist校验锁，第二部分
}; // hsh

// 置换表信息
extern int nHashMask;              // 置换表的大小
extern HashStruct *hshItems;       // 置换表的指针，ElephantEye采用多层的置换表
#ifdef HASH_QUIESC
  extern HashStruct *hshItemsQ;
#endif

inline void ClearHash(void) {         // 清空置换表
  memset(hshItems, 0, (nHashMask + 1) * sizeof(HashStruct));
#ifdef HASH_QUIESC
  memset(hshItemsQ, 0, (nHashMask + 1) * sizeof(HashStruct));
#endif
}

inline void NewHash(int nHashScale) { // 分配置换表，大小是 2^nHashScale 字节
  nHashMask = ((1 << nHashScale) / sizeof(HashStruct)) - 1;
  hshItems = new HashStruct[nHashMask + 1];
#ifdef HASH_QUIESC
  hshItemsQ = new HashStruct[nHashMask + 1];
#endif
  ClearHash();
}

inline void DelHash(void) {           // 释放置换表
  delete[] hshItems;
#ifdef HASH_QUIESC
  delete[] hshItemsQ;
#endif
}

// 判断置换表是否符合局面(Zobrist锁是否相等)
inline bool HASH_POS_EQUAL(const HashStruct &hsh, const PositionStruct &pos) {
  return hsh.dwZobristLock0 == pos.zobr.dwLock0 && hsh.dwZobristLock1 == pos.zobr.dwLock1;
}

// 按局面和层数获取置换表项(返回一个引用，可以对其赋值)
inline HashStruct &HASH_ITEM(const PositionStruct &pos, int nLayer) {
  return hshItems[(pos.zobr.dwKey + nLayer) & nHashMask];
}

// 置换表的管理过程
void RecordHash(const PositionStruct &pos, int nFlag, int vl, int nDepth, int mv);                    // 存储置换表局面信息
int ProbeHash(const PositionStruct &pos, int vlAlpha, int vlBeta, int nDepth, bool bNoNull, int &mv); // 获取置换表局面信息
#ifdef HASH_QUIESC
  void RecordHashQ(const PositionStruct &pos, int vlBeta, int vlAlpha); // 存储置换表局面信息(静态搜索)
  int ProbeHashQ(const PositionStruct &pos, int vlAlpha, int vlBeta);   // 获取置换表局面信息(静态搜索)
#endif

#ifndef CCHESS_A3800
  // UCCI支持 - 输出Hash表中的局面信息
  bool PopHash(const PositionStruct &pos);
#endif

#endif
