/*
search.h/search.cpp - Source Code for ElephantEye, Part VIII

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

#include "../base/base.h"
#include "../base/rc4prng.h"
#ifndef CCHESS_A3800
  #include "ucci.h"
#endif
#include "pregen.h"
#include "position.h"

#ifndef SEARCH_H
#define SEARCH_H

// 搜索模式
const int GO_MODE_INFINITY = 0;
const int GO_MODE_NODES = 1;
const int GO_MODE_TIMER = 2;

// 搜索前可设置的全局变量，指定搜索参数
struct SearchStruct {
  PositionStruct pos;                // 有待搜索的局面
  bool bQuit, bPonder, bDraw;        // 是否收到退出指令、后台思考模式和提和模式
  bool bBatch, bDebug;               // 是否批处理模式和调试模式
  bool bUseHash, bUseBook;           // 是否使用置换表裁剪和开局库
  bool bNullMove, bKnowledge;        // 是否空着裁剪和使用局面评价知识
  bool bIdle;                        // 是否空闲
  RC4Struct rc4Random;               // 随机数
  int nGoMode, nNodes, nCountMask;   // 搜索模式、结点数和
  int nProperTimer, nMaxTimer;       // 计划使用时间
  int nRandomMask, nBanMoves;        // 随机性屏蔽位和禁着数
  uint16_t wmvBanList[MAX_MOVE_NUM]; // 禁着列表
  char szBookFile[1024];             // 开局库
#ifdef CCHESS_A3800
  int mvResult;                      // 返回着法
#endif
};

extern SearchStruct Search;

#ifndef CCHESS_A3800

// UCCI局面构造过程
void BuildPos(PositionStruct &pos, const UcciCommStruct &UcciComm);

// UCCI支持 - 输出叶子结点的局面信息
void PopLeaf(PositionStruct &pos);

#endif

// 搜索的启动过程
void SearchMain(int nDepth);

#endif
