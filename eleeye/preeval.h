/*
preeval.h/preeval.cpp - Source Code for ElephantEye, Part X

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

#ifndef PREEVAL_H
#define PREEVAL_H

// 扩展的局面预评价结构
extern struct PreEvalStructEx {
  int vlBlackAdvisorLeakage, vlWhiteAdvisorLeakage;
  int vlHollowThreat[16], vlCentralThreat[16];
  int vlWhiteBottomThreat[16], vlBlackBottomThreat[16];
  char cPopCnt16[65536]; // 加速PopCnt16的数组，只需要初始化一次
} PreEvalEx;

#endif
