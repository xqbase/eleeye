/*
UCCI2QH - a UCCI to Qianhong Protocol Adapter
Designed by Morning Yellow, Version: 2.0, Last Modified: Apr. 2007
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

const int MAX_CHAR = 1024;      // 配置文件的最大长度
const int MAX_IRREV_POS = 33;   // 不可逆局面的最大个数，整个棋局吃子数不会超过32个
const int MAX_IRREV_MOVE = 200; // 不可逆局面的最大着法数，不吃子着法必须限制在100回合以内
const int MAX_BAN_MOVE = 128;   // 最多的禁止着法数
const int MAX_INFO = 16;        // 版本信息的最大行数
const int MAX_OPTION = 16;      // 选项设置的最大行数
const int MAX_LEVEL = 16;       // 难度的最高级别数


/* 以下常量代表适配器的思考状态，后台思考的处理是一大难点：
 * (1) 不启用后台思考时，空闲状态是"IDLE_NONE"，正常思考状态是"BUSY_THINK"，提示思考状态是"BUSY_HINTS"；
 * (2) 启用后台思考时，正常思考结束后，就进入后台思考状态(BUSY_PONDER)，而"mvPonder"则是猜测着法；
 * (3) "BUSY_PONDER"状态下，如果对手给出的着法没有让后台思考命中，则后台思考中断；
 * (4) "BUSY_PONDER"状态下，如果对手给出的着法让后台思考命中(和"mvPonder"一致)，就进入后台思考命中状态(BUSY_PONDERHIT)；
 * (5) "BUSY_PONDER"状态下，如果后台思考结束(在对手给出着法之前)，则进入后台思考完成状态(IDLE_PONDER_FINISHED)，而"mvPonderFinished"则保存后台思考的结果；
 * (6) "BUSY_PONDERHIT"状态下，如果收到思考指令，就转入正常思考状态(BUSY_THINK)；
 * (7) "BUSY_PONDERHIT"状态下，如果后台思考结束(在对手给出着法之前)，就转入后台思考完成并且命中状态(IDLE_PONDERHIT_FINISHED)，而"mvPonderFinished"则保存后台思考的结果；
 * (8) "IDLE_PONDER_FINISHED"状态下，如果对手给出的着法没有让后台思考命中，则程序重新开始思考。
 * (9) "IDLE_PONDER_FINISHED"状态下，如果对手给出的着法让后台思考命中，就转入后台思考完成并且命中状态(IDLE_PONDERHIT_FINISHED)；
 * (10) "IDLE_PONDERHIT_FINISHED"状态下，如果收到思考指令，就立即给出"mvPonderFinished"着法；
 */
const int IDLE_NONE = 0;
const int IDLE_PONDER_FINISHED = 1;
const int IDLE_PONDERHIT_FINISHED = 2;
const int BUSY_WAIT = 3;
const int BUSY_THINK = 4;
const int BUSY_HINTS = 5;
const int BUSY_PONDER = 6;
const int BUSY_PONDERHIT = 7;

static struct {
  // 适配器状态选项
  bool bDebug, bUcciOkay, bBgThink;       // 是否调试模式，UCCI引擎是否启动，后台思考是否启用
  int nLevel, nStatus;                    // 级别和状态
  int mvPonder, mvPonderFinished;         // 后台思考的猜测着法和后台思考完成的着法
  int mvPonderFinishedPonder;             // 后台思考结束后，思考结果的后台思考猜测着法
  // 适配器局面信息
  int nIrrevPosNum;                       // 当前不可逆局面的个数
  PositionStruct posIrrev[MAX_IRREV_POS]; // 不可逆局面列表，组成了适配器的内部局面
  char szIrrevFen[MAX_IRREV_POS][128];    // 每组不可逆局面的起始局面的FEN串
  int nBanMoveNum;                        // 禁止着法个数，局面改动后就置零
  int wmvBanList[MAX_BAN_MOVE];           // 禁止着法列表
  // 适配器输入输出通道
  PipeStruct pipeStdin, pipeEngine;       // 标准输入(浅红象棋指令)和UCCI引擎管道，参阅"pipe.cpp"
  // UCCI引擎配置信息
  char szIniFile[MAX_CHAR];                            // 适配器配置文件"UCCI2QH.INI"的全路径
  int nInfoNum, nOptionNum, nLevelNum;                 // 版本信息行数、选项设置行数和难度级别数
  char szEngineName[MAX_CHAR], szEngineFile[MAX_CHAR]; // UCCI引擎名称和UCCI引擎程序文件的全路径
  char szInfoStrings[MAX_INFO][MAX_CHAR], szOptionStrings[MAX_OPTION][MAX_CHAR]; // 版本信息和选项设置
  char szLevelStrings[MAX_LEVEL][MAX_CHAR], szThinkModes[MAX_LEVEL][MAX_CHAR];   // 难度级别和各个难度级别下的思考模式
} Ucci2QH;

// ICCS格式转换为着法结构
inline int ICCS_MOVE(const char *szIccs) {
  int sqSrc, sqDst;
  sqSrc = COORD_XY(szIccs[0] - 'A' + FILE_LEFT, '9' + RANK_TOP - szIccs[1]);
  sqDst = COORD_XY(szIccs[3] - 'A' + FILE_LEFT, '9' + RANK_TOP - szIccs[4]);
  return MOVE(sqSrc, sqDst);
}

// 着法结构转换为ICCS格式
inline void MOVE_ICCS(char *szIccs, int mv) {
  szIccs[0] = (FILE_X(SRC(mv))) + 'A' - FILE_LEFT;
  szIccs[1] = '9' + RANK_TOP - (RANK_Y(SRC(mv)));
  szIccs[2] = '-';
  szIccs[3] = (FILE_X(DST(mv))) + 'A' - FILE_LEFT;
  szIccs[4] = '9' + RANK_TOP - (RANK_Y(DST(mv)));
  szIccs[5] = '\0';
}

// 设置适配器状态(在调试模式下，显示该状态)
static void SetStatus(int nArg) {
  Ucci2QH.nStatus = nArg;
  if (Ucci2QH.bDebug) {
    fprintf(stderr, "Adapter Info: Status = ");
    switch (nArg) {
    case IDLE_NONE:
      fprintf(stderr, "IDLE_NONE");
      break;
    case IDLE_PONDER_FINISHED:
      fprintf(stderr, "IDLE_PONDER_FINISHED");
      break;
    case IDLE_PONDERHIT_FINISHED:
      fprintf(stderr, "IDLE_PONDERHIT_FINISHED");
      break;
    case BUSY_WAIT:
      fprintf(stderr, "BUSY_WAIT");
      break;
    case BUSY_THINK:
      fprintf(stderr, "BUSY_THINK");
      break;
    case BUSY_HINTS:
      fprintf(stderr, "BUSY_HINTS");
      break;
    case BUSY_PONDER:
      fprintf(stderr, "BUSY_PONDER");
      break;    
    case BUSY_PONDERHIT:
      fprintf(stderr, "BUSY_PONDERHIT");
      break;    
    }
    fprintf(stderr, "\n");
    fflush(stderr);
  }
}

// 向“浅红象棋”发送反馈信息(在调试模式下显示该信息)
inline void Adapter2QH(const char *szLineStr) {
  printf("%s\n", szLineStr);
  fflush(stdout);
  if (Ucci2QH.bDebug) {
    fprintf(stderr, "Adapter->Qianhong: %s\n", szLineStr);
    fflush(stderr);
  }
}

// 向UCCI引擎发送指令(在调试模式下显示该信息)
inline void Adapter2UCCI(const char *szLineStr) {
  Ucci2QH.pipeEngine.LineOutput(szLineStr);
  if (Ucci2QH.bDebug) {
    fprintf(stderr, "Adapter->UCCI-Engine: %s\n", szLineStr);
    fflush(stderr);
  }
}

// 接收“浅红象棋”的指令
inline bool QH2Adapter(char *szLineStr) {
  if (Ucci2QH.pipeStdin.LineInput(szLineStr)) {
    if (Ucci2QH.bDebug) {
      fprintf(stderr, "Qianhong->Adapter: %s\n", szLineStr);
      fflush(stderr);
    }
    return true;
  } else {
    return false;
  }
}

// 接收UCCI引擎的反馈信息
inline bool UCCI2Adapter(char *szLineStr) {
  if (Ucci2QH.pipeEngine.LineInput(szLineStr)) {
    if (Ucci2QH.bDebug) {
      fprintf(stderr, "UCCI-Engine->Adapter: %s\n", szLineStr);
      fflush(stderr);
    }
    return true;
  } else {
    return false;
  }
}

// 浅红模式下更新内部局面的过程
static bool MakeMove(int mv) {
  if (mv == 0 || Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].ucpcSquares[SRC(mv)] == 0) {
    return false;
  }
  if (Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].ucpcSquares[DST(mv)] == 0) {
    // 如果不是吃子着法，那么在上一个可逆局面下执行着法
    if (Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].nMoveNum < MAX_IRREV_MOVE) {
      Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].MakeMove(mv);
      Ucci2QH.nBanMoveNum = 0;
      return true;
    } else {
      return false;
    }
  } else {
    // 如果是吃子着法，那么重新设立一个不可逆局面
    if (Ucci2QH.nIrrevPosNum < MAX_IRREV_POS - 1) {
      Ucci2QH.nIrrevPosNum ++;
      Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum] = Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum - 1];
      Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].MakeMove(mv);
      Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].SetIrrev();
      Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].ToFen(Ucci2QH.szIrrevFen[Ucci2QH.nIrrevPosNum]);
      Ucci2QH.nBanMoveNum = 0;
      return true;
    } else {
      return false;
    }
  }
}

inline int PieceChar(int pc) {
  if (pc < 16) {
    return '.';
  } else if (pc < 32) {
    return PIECE_BYTE(PIECE_TYPE(pc));
  } else {
    return PIECE_BYTE(PIECE_TYPE(pc)) - 'A' + 'a';
  }
}

// 把局面打印到屏幕上
static void PrintPosition(const PositionStruct &pos) {
  int i, j;
  for (i = 3; i <= 12; i ++) {
    for (j = 3; j <= 11; j ++) {
      printf("%c", PieceChar(pos.ucpcSquares[i * 16 + j]));
    }
    printf("\n");
    fflush(stdout);
  }
  if (Ucci2QH.bDebug) {
    for (i = 3; i <= 12; i ++) {
      fprintf(stderr, "Adapter->Qianhong: ");
      for (j = 3; j <= 11; j ++) {
        fprintf(stderr, "%c", PieceChar(pos.ucpcSquares[i * 16 + j]));
      }
      fprintf(stderr, "\n");
      fflush(stderr);
    }
  }
}

// 给UCCI引擎发送思考指令
static void RunEngine(void) {
  int i;
  uint32_t dwMoveStr;
  char *lp;
  char szLineStr[LINE_INPUT_MAX_CHAR];
  // 发送思考指令要分三个步骤：

  // 1. 发送局面信息，包括初始的不可逆FEN串和一系列后续着法(连同后台思考的猜测着法)；
  lp = szLineStr;
  lp += sprintf(lp, "position fen %s - - 0 1", Ucci2QH.szIrrevFen[Ucci2QH.nIrrevPosNum]);
  if (Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].nMoveNum > 1) {
    lp += sprintf(lp, " moves");
    for (i = 1; i < Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].nMoveNum; i ++) {
      dwMoveStr = MOVE_COORD(Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].rbsList[i].mvs.wmv);
      lp += sprintf(lp, " %.4s", (const char *) &dwMoveStr);
    }
  }
  if (Ucci2QH.nStatus == BUSY_PONDER) {
    if (Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].nMoveNum == 1) {
      lp += sprintf(lp, " moves");
    }
    dwMoveStr = MOVE_COORD(Ucci2QH.mvPonder);
    lp += sprintf(lp, " %.4s", (const char *) &dwMoveStr);
  }
  Adapter2UCCI(szLineStr);

  // 2. 发送禁止着法信息；
  if (Ucci2QH.nBanMoveNum > 0) {
    lp = szLineStr;
    lp += sprintf(lp, "banmoves");
    for (i = 0; i < Ucci2QH.nBanMoveNum; i ++) {
      dwMoveStr = MOVE_COORD(Ucci2QH.wmvBanList[i]);
      lp += sprintf(lp, " %.4s", (const char *) &dwMoveStr);
    }
    Adapter2UCCI(szLineStr);
  }

  // 3. 发送思考指令。
  sprintf(szLineStr, Ucci2QH.nStatus == BUSY_PONDER ? "go ponder %s" : "go %s", Ucci2QH.szThinkModes[Ucci2QH.nLevel]);
  Adapter2UCCI(szLineStr);
}

// UCCI反馈信息的接收过程
static bool ReceiveUCCI(void) {
  int mv;
  char *lp;
  char szIccs[8];
  char szLineStr[LINE_INPUT_MAX_CHAR];
  if (!UCCI2Adapter(szLineStr)) {
    return false;
  }
  lp = szLineStr;
  if (Ucci2QH.bUcciOkay) {
    if (StrEqvSkip(lp, "bestmove ")) {
      mv = COORD_MOVE(*(uint32_t *) lp);
      lp += sizeof(uint32_t);
      switch (Ucci2QH.nStatus) {
      // 一旦收到反馈着法，就根据"bStatus"决定相应的处理过程，并转入空闲状态：

      // 1. "BUSY_WAIT"状态，说明是由"StopEngine()"中断的，不作任何处理；
      case BUSY_WAIT:
        SetStatus(IDLE_NONE);
        break;

      // 2. "BUSY_THINK"状态，输出并执行最佳着法，并视有无后台思考猜测着法作出相应处理；
      case BUSY_THINK:
        MOVE_ICCS(szIccs, mv);
        Adapter2QH(szIccs);
        MakeMove(mv);
        if (Ucci2QH.bBgThink && StrEqvSkip(lp, " ponder ")) {
          Ucci2QH.mvPonder = COORD_MOVE(*(uint32_t *) lp);
          SetStatus(BUSY_PONDER);
          RunEngine();
        } else {
          SetStatus(IDLE_NONE);
        }
        break;

      // 3. "BUSY_HINTS"状态，只要输出最佳着法即可；
      case BUSY_HINTS:
        MOVE_ICCS(szIccs, mv);
        Adapter2QH(szIccs);
        Adapter2QH("ENDHINTS");
        SetStatus(IDLE_NONE);
        break;

      // 4. "BUSY_PONDER"和"BUSY_PONDERHIT"状态，只要将最佳着法记录为后台思考结果即可；
      case BUSY_PONDER:
      case BUSY_PONDERHIT:
        Ucci2QH.mvPonderFinished = mv;
        SetStatus(Ucci2QH.nStatus == BUSY_PONDER ? IDLE_PONDER_FINISHED : IDLE_PONDERHIT_FINISHED);
        if (Ucci2QH.bBgThink && StrEqvSkip(lp, " ponder ")) {
          Ucci2QH.mvPonderFinishedPonder = COORD_MOVE(*(uint32_t *) lp);
        } else {
          Ucci2QH.mvPonderFinishedPonder = 0;
        }
        break;
      default:
        break;
      };
    } else if (StrEqv(lp, "nobestmove")) {

      // 5. 最后考虑没有最佳着法的情况。
      switch (Ucci2QH.nStatus) {
      case BUSY_WAIT:
        break;
      case BUSY_HINTS:
      case BUSY_THINK:
        Adapter2QH("ERROR");
        break;
      case BUSY_PONDER:
      case BUSY_PONDERHIT:
        break;
      default:
        break;
      }
      SetStatus(IDLE_NONE);

    } else if (StrEqv(lp, "bye")) {
      Ucci2QH.bUcciOkay = false;
    }
  } else {
    if (StrEqv(lp, "ucciok")) {
      Ucci2QH.bUcciOkay = true;
    }
  }
  return true;
}

// 中止UCCI引擎的思考
static void StopEngine(void) {
  int64_t llTime;
  SetStatus(BUSY_WAIT);
  Adapter2UCCI("stop");
  llTime = GetTime();
  while (Ucci2QH.nStatus != IDLE_NONE && (int) (GetTime() - llTime) < 1000) {
    if (!ReceiveUCCI()) {
      Idle();
    }
  }
  Ucci2QH.nStatus = IDLE_NONE;
}

// 浅红象棋指令的接收过程
static bool ReceiveQH(void) {
  int i, j;
  int mv;
  int64_t llTime;
  char *lp;
  char szIccs[8];
  char szLineStr[LINE_INPUT_MAX_CHAR];

  if (!QH2Adapter(szLineStr)) {
    return false;
  }
  lp = szLineStr;
  if (false) {
  // 浅红象棋协议接收到的指令大致有以下几种：

  // 1. "SCR"指令(略)；
  } else if (StrEqv(lp, "SCR")) {
    PrintPosition(Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum]);

  // 2. "LEVEL"指令(略)；
  } else if (StrEqvSkip(lp, "LEVEL ")) {
    Ucci2QH.nLevel = Str2Digit(lp, 0, Ucci2QH.nLevelNum - 1);
    Adapter2QH("OK");
  // 注意：必须首先判断"LEVEL "，再判断"LEVEL"
  } else if (StrEqv(lp, "LEVEL")) {
    sprintf(szLineStr, "%d", Ucci2QH.nLevelNum);
    Adapter2QH(szLineStr);

  // 3. "FEN"指令，更新内部局面，并且清除后台思考状态；
  } else if (StrEqvSkip(lp, "FEN ")) {
    if (Ucci2QH.nStatus == BUSY_THINK || Ucci2QH.nStatus == BUSY_HINTS) {
      Adapter2QH("ERROR");
      return true;
    }
    Ucci2QH.nIrrevPosNum = 0;
    Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].FromFen(lp);
    Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].ToFen(Ucci2QH.szIrrevFen[Ucci2QH.nIrrevPosNum]);
    if (Ucci2QH.nStatus == BUSY_PONDER || Ucci2QH.nStatus == BUSY_PONDERHIT) {
      StopEngine();
    } else {
      SetStatus(IDLE_NONE);
    }
    Adapter2UCCI("setoption newgame");
    Adapter2QH("OK");

  // 4. "PLAY"指令；
  } else if (StrEqvSkip(lp, "PLAY ")) {
    if (Ucci2QH.nStatus == BUSY_THINK || Ucci2QH.nStatus == BUSY_HINTS) {
      Adapter2QH("ERROR");
      return true;
    }
    mv = ICCS_MOVE(lp);
    if (!MakeMove(mv)) {
      Adapter2QH("ERROR");
      return true;
    }
    // 至此着法执行完毕，以下是更改后台思考状态
    switch (Ucci2QH.nStatus) {
    case IDLE_PONDER_FINISHED:
      SetStatus(mv == Ucci2QH.mvPonder ? IDLE_PONDERHIT_FINISHED : IDLE_NONE);
      break;
    case IDLE_PONDERHIT_FINISHED:
      SetStatus(IDLE_NONE);
      break;
    case BUSY_PONDER:
      if (mv == Ucci2QH.mvPonder) {
        SetStatus(BUSY_PONDERHIT);
        Adapter2UCCI("ponderhit");
      } else {
        StopEngine();
      }
      break;
    case BUSY_PONDERHIT:
      StopEngine();
      break;
    default:
      break;
    }
    Adapter2QH("OK");

  // 5. "LOAD"指令，逐一载入着法，并且清除后台思考状态；
  } else if (StrEqvSkip(lp, "LOAD ")) {
    i = Str2Digit(lp, 0, 1998); // 一局棋最多有999个回合，即1998个着法
    if (Ucci2QH.nStatus == BUSY_THINK || Ucci2QH.nStatus == BUSY_HINTS) {
      for (j = 0; j < i; j ++) {
        while (!QH2Adapter(szLineStr)) {
          Idle();
        }
      }
      Adapter2QH("ERROR");
      return true;
    }
    for (j = 0; j < i; j ++) {
      while (!QH2Adapter(szLineStr)) {
        Idle();
      }
      mv = ICCS_MOVE(szLineStr);
      MakeMove(mv);
    }
    if (Ucci2QH.nStatus == BUSY_PONDER || Ucci2QH.nStatus == BUSY_PONDERHIT) {
      StopEngine();
    } else {
      SetStatus(IDLE_NONE);
    }
    Adapter2QH("OK");

  // 6. "AI"指令，进入思考状态；
  } else if (StrEqv(lp, "AI")) {
    if (Ucci2QH.nStatus == BUSY_THINK || Ucci2QH.nStatus == BUSY_HINTS) {
      Adapter2QH("ERROR");
      return true;
    }
    switch (Ucci2QH.nStatus) {
    case IDLE_NONE:
    case IDLE_PONDER_FINISHED:
      SetStatus(BUSY_THINK);
      RunEngine();
      break;
    case IDLE_PONDERHIT_FINISHED:
      MakeMove(Ucci2QH.mvPonderFinished);
      MOVE_ICCS(szIccs, Ucci2QH.mvPonderFinished);
      Adapter2QH(szIccs);
      if (Ucci2QH.mvPonderFinishedPonder == 0) {
        SetStatus(IDLE_NONE);
      } else {
        Ucci2QH.mvPonder = Ucci2QH.mvPonderFinishedPonder;
        SetStatus(BUSY_PONDER);
        RunEngine();
      }
      break;
    case BUSY_PONDER:
      StopEngine();
      SetStatus(BUSY_THINK);
      RunEngine();
      break;
    case BUSY_PONDERHIT:
      SetStatus(BUSY_THINK);
      break;
    default:
      break;
    }

  // 7. "ABORT"指令(略)；
  } else if (StrEqv(lp, "ABORT")) {
    StopEngine();
    Adapter2QH("ABORTED");

  // 8. "QUIT"指令(略)；
  } else if (StrEqv(lp, "QUIT")) {
    if (Ucci2QH.nStatus > BUSY_WAIT) {
      StopEngine();
    }
    Adapter2UCCI("quit");
    llTime = GetTime();
    while (Ucci2QH.bUcciOkay && (int) (GetTime() - llTime) < 1000) {
      if (!ReceiveUCCI()) {
        Idle();
      }
    }
    Ucci2QH.bUcciOkay = false;

  // 9. "UNDO"指令，撤消着法，并且清除后台思考状态；
  } else if (StrEqv(lp, "UNDO")) {
    if (Ucci2QH.nStatus == BUSY_THINK || Ucci2QH.nStatus == BUSY_HINTS) {
      Adapter2QH("ERROR");
      return true;
    }
    if (Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].nMoveNum == 1) {
      if (Ucci2QH.nIrrevPosNum == 0) {
        Adapter2QH("ERROR");
        return true;
      }
      Ucci2QH.nIrrevPosNum --;
    } else {
      Ucci2QH.posIrrev[Ucci2QH.nIrrevPosNum].UndoMakeMove();
    }
    if (Ucci2QH.nStatus == BUSY_PONDER || Ucci2QH.nStatus == BUSY_PONDERHIT) {
      StopEngine();
    } else {
      SetStatus(IDLE_NONE);
    }
    Adapter2QH("OK");

  // 10. "HINTS"指令，给出提示；
  } else if (StrEqv(lp, "HINTS")) {
    if (Ucci2QH.nStatus == BUSY_THINK || Ucci2QH.nStatus == BUSY_HINTS) {
      Adapter2QH("ERROR");
      return true;
    }
    if (Ucci2QH.nStatus == BUSY_PONDER || Ucci2QH.nStatus == BUSY_PONDERHIT) {
      // 如果正在后台思考，则输出后台思考的猜测着法，作为提示着法
      MOVE_ICCS(szIccs, Ucci2QH.mvPonder);
      Adapter2QH(szIccs);
      Adapter2QH("ENDHINTS");
    } else {
      // 其他情况，让引擎思考一个提示着法
      SetStatus(BUSY_HINTS);
      RunEngine();
    }

  // 11. "BAN"指令，读入禁止着法到"Ucci2QH.wmvBanList"就可以了；
  } else if (StrEqvSkip(lp, "BAN ")) {
    Ucci2QH.nBanMoveNum = Str2Digit(lp, 0, MAX_BAN_MOVE);
    for (i = 0; i < Ucci2QH.nBanMoveNum; i ++) {
      while (!QH2Adapter(szLineStr)) {
        Idle();
      }
      Ucci2QH.wmvBanList[i] = ICCS_MOVE(szLineStr);
    }
    if (Ucci2QH.nStatus == BUSY_PONDER || Ucci2QH.nStatus == BUSY_PONDERHIT) {
      StopEngine();
    } else {
      SetStatus(IDLE_NONE);
    }
    Adapter2QH("OK");

  // 12. "BGTHINK"指令(略)；
  } else if (StrEqv(lp, "BGTHINK ON")) {
    Ucci2QH.bBgThink = true;
    Adapter2QH("OK");
  } else if (StrEqv(lp, "BGTHINK OFF")) {
    Ucci2QH.bBgThink = false;
    Adapter2QH("OK");

  // 13. "TIMEOUT"指令(略)。
  } else if (StrEqv(lp, "TIMEOUT")) {
    Adapter2UCCI("stop");
  }
  return true;
}

// 主函数
int main(int argc, char **argv) {
  int64_t llTime;
  char szLineStr[MAX_CHAR];
  char *lp;
  FILE *fpIniFile;
  int i, nCurrLevel;

  if (argc < 2) {
    return 0;
  }
  LocatePath(Ucci2QH.szIniFile, "UCCI2QH.INI");
  nCurrLevel = Ucci2QH.nLevelNum = Ucci2QH.nInfoNum = 0;
  Ucci2QH.szEngineName[0] = Ucci2QH.szEngineFile[0] = '\0';
  fpIniFile = fopen(Ucci2QH.szIniFile, "rt");
  if (fpIniFile == NULL) {
    return 0;
  }
  while (fgets(szLineStr, MAX_CHAR, fpIniFile) != NULL) {
    StrCutCrLf(szLineStr);
    lp = szLineStr;
    if (false) {
    } else if (StrEqvSkip(lp, "Name=")) {
      strcpy(Ucci2QH.szEngineName, lp);
    } else if (StrEqvSkip(lp, "File=")) {
      LocatePath(Ucci2QH.szEngineFile, lp);
    } else if (StrEqvSkip(lp, "Info=")) {
      if (Ucci2QH.nLevelNum < MAX_INFO) {
        strcpy(Ucci2QH.szInfoStrings[Ucci2QH.nInfoNum], lp);
        Ucci2QH.nInfoNum ++;
      }
    } else if (StrEqvSkip(lp, "Option=")) {
      if (Ucci2QH.nOptionNum < MAX_OPTION) {
        strcpy(Ucci2QH.szOptionStrings[Ucci2QH.nOptionNum], lp);
        Ucci2QH.nOptionNum ++;
      }
    } else if (StrEqvSkip(lp, "Level=")) {
      if (Ucci2QH.nLevelNum < MAX_LEVEL) {
        strcpy(Ucci2QH.szLevelStrings[Ucci2QH.nLevelNum], lp);
        Ucci2QH.nLevelNum ++;
      }
    } else if (StrEqvSkip(lp, "ThinkMode=")) {
      if (nCurrLevel < Ucci2QH.nLevelNum) {
        strcpy(Ucci2QH.szThinkModes[nCurrLevel], lp);
        nCurrLevel ++;
      }
    }
  }
  fclose(fpIniFile);
  for (; nCurrLevel < Ucci2QH.nLevelNum; nCurrLevel ++) {
    Ucci2QH.szThinkModes[nCurrLevel][0] = '\0';
  }

  if (false) {
  // 浅红引擎有以下两种命令格式：

  // 1. 启动引擎：UCCI2QH -plugin [debug]
  } else if (StrEqv(argv[1], "-plugin")) {
    Ucci2QH.bDebug = Ucci2QH.bUcciOkay = Ucci2QH.bBgThink = false;
    Ucci2QH.nLevel = 0;
    SetStatus(IDLE_NONE);
    if (argc > 2) {
      if (StrEqv(argv[2], "debug")) {
        Ucci2QH.bDebug = true;
      }
    }
    Ucci2QH.pipeStdin.Open();
    Ucci2QH.pipeEngine.Open(Ucci2QH.szEngineFile);
    Adapter2UCCI("ucci");
    PreGenInit();
    Ucci2QH.nIrrevPosNum = 0;
    strcpy(Ucci2QH.szIrrevFen[0], cszStartFen);
    Ucci2QH.posIrrev[0].FromFen(Ucci2QH.szIrrevFen[0]);
    llTime = GetTime();
    // 等待10秒钟，如果引擎无法正常启动，就直接退出。
    while (!Ucci2QH.bUcciOkay && (int) (GetTime() - llTime) < 10000) {
      if (!ReceiveUCCI()) {
        Idle();
      }
    }
    if (Ucci2QH.bUcciOkay) {
      for (i = 0; i < Ucci2QH.nOptionNum; i ++) {
        Adapter2UCCI(Ucci2QH.szOptionStrings[i]);
      }
      Adapter2UCCI("setoption newgame");
    }
    while (Ucci2QH.bUcciOkay) {
      if (!(ReceiveUCCI() || ReceiveQH())) {
        Idle();
      }
    }
    Ucci2QH.pipeEngine.Close();
    Adapter2QH("BYE");

  // 2. 显示引擎信息：UCCI2QH -info
  } else if (StrEqv(argv[1], "-info")) {
    printf("QHPLUGIN V1.3\n");
    printf("%s\n", Ucci2QH.szEngineName);
    printf("LEVELS %d\n", Ucci2QH.nLevelNum);
    for (i = 0; i < Ucci2QH.nLevelNum; i ++) {
      printf("%d - %s\n", i, Ucci2QH.szLevelStrings[i]);
    }
    printf("UNDO 1\n");
    printf("HINTS 1\n");
    printf("RULES 1\n");
    printf("BGTHINK 1\n");
    printf("TIMEOUT 1\n");
    for (i = 0; i < Ucci2QH.nInfoNum; i ++) {
      printf("%s\n", Ucci2QH.szInfoStrings[i]);
    }
    printf("=== UCCI Engine Options ===\n");
    printf("Engine File: %s\n", Ucci2QH.szEngineFile);
    printf("Option List:\n");
    for (i = 0; i < Ucci2QH.nOptionNum; i ++) {
      printf("%s\n", Ucci2QH.szOptionStrings[i]);
    }
    printf("Level List:\n");
    for (i = 0; i < Ucci2QH.nLevelNum; i ++) {
      printf("%s=\"go [ponder] %s\"\n", Ucci2QH.szLevelStrings[i], Ucci2QH.szThinkModes[i]);
    }
    printf("ENDINFO\n");
    fflush(stdout);
  }
  return 0;
}
