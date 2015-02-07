/* 
UCCI-Engine Test Driver - for UCCI Engines
Designed by Morning Yellow, Version: 3.1, Last Modified: Nov. 2007
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

#include <stdio.h>
#include <string.h>
#include "../base/base2.h"
#include "../base/parse.h"
#include "../base/pipe.h"
#include "../eleeye/position.h"

const int MAX_CHAR = 1024; // 配置文件的最大长度
const int MAX_INIT = 16;   // Init配置项最大行数

inline void GetMoveNodes(int &mvTest, int &nNodes, PipeStruct &pipe) {
  char szLineStr[MAX_CHAR];
  char *lp;
  if (pipe.LineInput(szLineStr)) {
    lp = szLineStr;
    if (StrEqvSkip(lp, "bestmove ")) {
      mvTest = COORD_MOVE(*(uint32_t *) lp);
    } else if (StrScanSkip(lp, " nodes ")) {
      nNodes = Str2Digit(lp, 0, 2000000000);
    }
  } else {
    Idle();
  }
}

int main(void) {
  int nInitNum, nTimeout;
  bool bNodes, bReset;
  char szIniFile[MAX_CHAR], szPosFile[MAX_CHAR], szEngineFile[MAX_CHAR];
  char szCommand[MAX_CHAR], szOutput[MAX_CHAR];
  char szInit[MAX_INIT][MAX_CHAR];

  FILE *fpIniFile, *fpPosFile, *fpOutput;
  char szLineStr[MAX_CHAR];
  char szFen[128];
  char *lp;

  PipeStruct pipe;
  bool bUcciOkay;
  int i, nHitNum, nNodes, nNodesTotal, mvBase, mvTest;
  uint32_t dwMoveBase, dwMoveTest;
  PositionStruct pos;
  int64_t llTime;

  // 读取配置文件
  nInitNum = nTimeout = 0;
  bNodes = bReset = false;
  szPosFile[0] = szEngineFile[0] = szCommand[0] = szOutput[0] = '\0';
  LocatePath(szIniFile, "UCCITEST.INI");
  fpIniFile = fopen(szIniFile, "rt");
  if (fpIniFile == NULL) {
    printf("%s: File Opening Error!\n", szIniFile);
    return 0;
  }
  while (fgets(szLineStr, MAX_CHAR, fpIniFile) != NULL) {
    StrCutCrLf(szLineStr);
    lp = szLineStr;
    if (false) {
    } else if (StrEqvSkip(lp, "Positions=")) {
      LocatePath(szPosFile, lp);
    } else if (StrEqvSkip(lp, "Engine=")) {
      LocatePath(szEngineFile, lp);
    } else if (StrEqvSkip(lp, "Nodes=On")) {
      bNodes = true;
    } else if (StrEqvSkip(lp, "Nodes=True")) {
      bNodes = true;
    } else if (StrEqvSkip(lp, "Init=")) {
      if (nInitNum < MAX_INIT) {
        strcpy(szInit[nInitNum], lp);
        nInitNum ++;
      }
    } else if (StrEqvSkip(lp, "Command=")) {
      strcpy(szCommand, lp);
    } else if (StrEqvSkip(lp, "Output=")) {
      LocatePath(szOutput, lp);
    } else if (StrEqvSkip(lp, "Reset=On")) {
      bReset = true;
    } else if (StrEqvSkip(lp, "Reset=True")) {
      bReset = true;
    } else if (StrEqvSkip(lp, "Timeout=")) {
      nTimeout = Str2Digit(lp, 0, 3600);
    }
  }
  fclose(fpIniFile);
  nTimeout *= 1000;

  // 初始化引擎
  pipe.Open(szEngineFile);
  pipe.LineOutput("ucci");
  llTime = GetTime();
  bUcciOkay = false;
  while (!bUcciOkay && (int) (GetTime() - llTime) < 10000) {
    if (pipe.LineInput(szLineStr)) {
      if (StrEqv(szLineStr, "ucciok")) {
        bUcciOkay = true;
      }
    } else {
      Idle();
    }
  }
  if (!bUcciOkay) {
    pipe.LineOutput("quit");
    printf("%s: Not a UCCI-Engine!\n", szEngineFile);
    return 0;
  }
  for (i = 0; i < nInitNum; i ++) {
    pipe.LineOutput(szInit[i]);
  }

  // 读取测试局面文件
  fpPosFile = fopen(szPosFile, "rt");
  if (fpPosFile == NULL) {
    printf("%s: File Opening Error!\n", szPosFile);
    return 0;
  }

  // 打开输出文件
  fpOutput = stdout;
  if (szOutput[0] != '\0') {
    fpOutput = fopen(szOutput, "wt");
    if (fpOutput == NULL) {
      fpOutput = stdout;
    }
  }
  PreGenInit();
  nNodesTotal = 0;
  nHitNum = 0;
  if (bNodes) {
    fprintf(fpOutput, "    No Base Test Result    Nodes Position (FEN)\n");
  } else {
    fprintf(fpOutput, "    No Base Test Result Position (FEN)\n");
  }
  fprintf(fpOutput, "===================================================\n");
  fflush(fpOutput);
  i = 0;
  while (fgets(szLineStr, MAX_CHAR, fpPosFile) != NULL) {
    StrCutCrLf(szLineStr);
    if (szLineStr[0] == ';') {
      continue;
    }
    i ++;
    // 读取局面
    dwMoveBase = *(uint32_t *) szLineStr;
    mvBase = COORD_MOVE(dwMoveBase);
    pos.FromFen(szLineStr + 5);
    // 向引擎发送指令
    if (bReset) {
      pipe.LineOutput("setoption newgame");
    }
    pos.ToFen(szFen);
    sprintf(szLineStr, "position fen %s - - 0 1", szFen);
    pipe.LineOutput(szLineStr);
    pipe.LineOutput(szCommand);
    // 等待引擎返回结果
    mvTest = 0;
    llTime = GetTime();
    while (mvTest == 0 && (nTimeout == 0 || (int) (GetTime() - llTime) < nTimeout)) {
      GetMoveNodes(mvTest, nNodes, pipe);
    }
    // 如果超时仍然没有结果，则强行让引擎给出着法
    if (mvTest == 0) {
      pipe.LineOutput("stop");
      llTime = GetTime();
      while (mvTest == 0 && (int) (GetTime() - llTime) < 1000) {
        GetMoveNodes(mvTest, nNodes, pipe);
      }
    }
    // 统计结果
    nNodesTotal += nNodes;
    if (mvTest == 0) {
      if (bNodes) {
        fprintf(fpOutput, "%6d %.4s ---- Miss %10d %s\n", i, (const char *) &dwMoveBase, nNodes, szFen);
      } else {
        fprintf(fpOutput, "%6d %.4s ---- Miss %s\n", i, (const char *) &dwMoveBase, szFen);
      }
    } else {
      dwMoveTest = MOVE_COORD(mvTest);
      if (bNodes) {
        fprintf(fpOutput, "%6d %.4s %.4s %s %10d %s\n", i, (const char *) &dwMoveBase, (const char *) &dwMoveTest,
            mvTest == mvBase ? "Hit " : "Miss", nNodes, szFen);
      } else {
        fprintf(fpOutput, "%6d %.4s %.4s %s %s\n", i, (const char *) &dwMoveBase, (const char *) &dwMoveTest,
            mvTest == mvBase ? "Hit " : "Miss", szFen);
      }
      nHitNum += (mvTest == mvBase ? 1 : 0);
    }
    fflush(fpOutput);
  }
  fprintf(fpOutput, "==================================================\n");
  if (bNodes) {
    fprintf(fpOutput, " Total    %6d Hits %10d\n", nHitNum, nNodesTotal);
  } else {
    fprintf(fpOutput, " Total    %6d Hits\n", nHitNum);
  }
  fflush(fpOutput);
  // 关闭输出文件
  if (fpOutput != stdout) {
    fclose(fpOutput);
  }
  fclose(fpPosFile);

  // 关闭引擎
  pipe.LineOutput("quit");
  while (bUcciOkay && (int) (GetTime() - llTime) < 10000) {
    if (pipe.LineInput(szLineStr)) {
      if (StrEqv(szLineStr, "bye")) {
        bUcciOkay = false;
      }
    } else {
      Idle();
    }
  }
  return 0;
}
