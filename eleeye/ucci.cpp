/*
ucci.h/ucci.cpp - Source Code for ElephantEye, Part I

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.3, Last Modified: Mar. 2012
Copyright (C) 2004-2012 www.xqbase.com

This part (ucci.h/ucci.cpp only) of codes is NOT published under LGPL, and
can be used without restriction.
*/

#include <stdio.h>
#include "../base/base2.h"
#include "../base/parse.h"
#include "../base/pipe.h"
#include "ucci.h"  

/* UCCI指令分析模块由三各UCCI指令解释器组成。
 *
 * 其中第一个解释器"BootLine()"最简单，只用来接收引擎启动后的第一行指令
 * 输入"ucci"时就返回"UCCI_COMM_UCCI"，否则一律返回"UCCI_COMM_UNKNOWN"
 * 前两个解释器都等待是否有输入，如果没有输入则执行待机指令"Idle()"
 * 而第三个解释器("BusyLine()"，只用在引擎思考时)则在没有输入时直接返回"UCCI_COMM_UNKNOWN"
 */
static PipeStruct pipeStd;

const int MAX_MOVE_NUM = 1024;

static char szFen[LINE_INPUT_MAX_CHAR];
static uint32_t dwCoordList[MAX_MOVE_NUM];

static bool ParsePos(UcciCommStruct &UcciComm, char *lp) {
  int i;
  // 首先判断是否指定了FEN串
  if (StrEqvSkip(lp, "fen ")) {
    strcpy(szFen, lp);
    UcciComm.szFenStr = szFen;
  // 然后判断是否是startpos
  } else if (StrEqv(lp, "startpos")) {
    UcciComm.szFenStr = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w";
  // 如果两者都不是，就立即返回
  } else {
    return false;
  }
  // 然后寻找是否指定了后续着法，即是否有"moves"关键字
  UcciComm.nMoveNum = 0;
  if (StrScanSkip(lp, " moves ")) {
    *(lp - strlen(" moves ")) = '\0';
    UcciComm.nMoveNum = MIN((int) (strlen(lp) + 1) / 5, MAX_MOVE_NUM); // 提示："moves"后面的每个着法都是1个空格和4个字符
    for (i = 0; i < UcciComm.nMoveNum; i ++) {
      dwCoordList[i] = *(uint32_t *) lp; // 4个字符可转换为一个"uint32_t"，存储和处理起来方便
      lp += sizeof(uint32_t) + 1;
    }
    UcciComm.lpdwMovesCoord = dwCoordList;
  }
  return true;
}

UcciCommEnum BootLine(void) {
  char szLineStr[LINE_INPUT_MAX_CHAR];
  pipeStd.Open();
  while (!pipeStd.LineInput(szLineStr)) {
    Idle();
  }
  if (StrEqv(szLineStr, "ucci")) {
    return UCCI_COMM_UCCI;
  } else {
    return UCCI_COMM_UNKNOWN;
  }
}

UcciCommEnum IdleLine(UcciCommStruct &UcciComm, bool bDebug) {
  char szLineStr[LINE_INPUT_MAX_CHAR];
  char *lp;
  int i;
  bool bGoTime;

  while (!pipeStd.LineInput(szLineStr)) {
    Idle();
  }
  lp = szLineStr;
  if (bDebug) {
    printf("info idleline [%s]\n", lp);
    fflush(stdout);
  }
  if (false) {
  // "IdleLine()"是最复杂的UCCI指令解释器，大多数的UCCI指令都由它来解释，包括：

  // 1. "isready"指令
  } else if (StrEqv(lp, "isready")) {
    return UCCI_COMM_ISREADY;

  // 2. "setoption <option> [<arguments>]"指令
  } else if (StrEqvSkip(lp, "setoption ")) {
    if (false) {

    // (1) "batch"选项
    } else if (StrEqvSkip(lp, "batch ")) {
      UcciComm.Option = UCCI_OPTION_BATCH;
      if (StrEqv(lp, "on")) {
        UcciComm.bCheck = true;
      } else if (StrEqv(lp, "true")) {
        UcciComm.bCheck = true;
      } else {
        UcciComm.bCheck = false;
      } // 由于"batch"选项默认是关闭的，所以只有设定"on"或"true"时才打开，下同

    // (2) "debug"选项
    } else if (StrEqvSkip(lp, "debug ")) {
      UcciComm.Option = UCCI_OPTION_DEBUG;
      if (StrEqv(lp, "on")) {
        UcciComm.bCheck = true;
      } else if (StrEqv(lp, "true")) {
        UcciComm.bCheck = true;
      } else {
        UcciComm.bCheck = false;
      }

    // (3) "ponder"选项
    } else if (StrEqvSkip(lp, "ponder ")) {
      UcciComm.Option = UCCI_OPTION_PONDER;
      if (StrEqv(lp, "on")) {
        UcciComm.bCheck = true;
      } else if (StrEqv(lp, "true")) {
        UcciComm.bCheck = true;
      } else {
        UcciComm.bCheck = false;
      }

    // (4) "usehash"选项
    } else if (StrEqvSkip(lp, "usehash ")) {
      UcciComm.Option = UCCI_OPTION_USEHASH;
      if (StrEqv(lp, "off")) {
        UcciComm.bCheck = false;
      } else if (StrEqv(lp, "false")) {
        UcciComm.bCheck = false;
      } else {
        UcciComm.bCheck = true;
      }

    // (5) "usebook"选项
    } else if (StrEqvSkip(lp, "usebook ")) {
      UcciComm.Option = UCCI_OPTION_USEBOOK;
      if (StrEqv(lp, "off")) {
        UcciComm.bCheck = false;
      } else if (StrEqv(lp, "false")) {
        UcciComm.bCheck = false;
      } else {
        UcciComm.bCheck = true;
      }

    // (6) "useegtb"选项
    } else if (StrEqvSkip(lp, "useegtb ")) {
      UcciComm.Option = UCCI_OPTION_USEEGTB;
      if (StrEqv(lp, "off")) {
        UcciComm.bCheck = false;
      } else if (StrEqv(lp, "false")) {
        UcciComm.bCheck = false;
      } else {
        UcciComm.bCheck = true;
      }

    // (7) "bookfiles"选项
    } else if (StrEqvSkip(lp, "bookfiles ")) {
      UcciComm.Option = UCCI_OPTION_BOOKFILES;
      UcciComm.szOption = lp;

    // (8) "egtbpaths"选项
    } else if (StrEqvSkip(lp, "egtbpaths ")) {
      UcciComm.Option = UCCI_OPTION_EGTBPATHS;
      UcciComm.szOption = lp;

    // (9) "evalapi"选项，3.3以后不再支持

    // (10) "hashsize"选项
    } else if (StrEqvSkip(lp, "hashsize ")) {
      UcciComm.Option = UCCI_OPTION_HASHSIZE;
      UcciComm.nSpin = Str2Digit(lp, 0, 1024);

    // (11) "threads"选项
    } else if (StrEqvSkip(lp, "threads ")) {
      UcciComm.Option = UCCI_OPTION_THREADS;
      UcciComm.nSpin = Str2Digit(lp, 0, 32);

    // (12) "promotion"选项
    } else if (StrEqvSkip(lp, "promotion ")) {
      UcciComm.Option = UCCI_OPTION_PROMOTION;
      if (StrEqv(lp, "on")) {
        UcciComm.bCheck = true;
      } else if (StrEqv(lp, "true")) {
        UcciComm.bCheck = true;
      } else {
        UcciComm.bCheck = false;
      }

    // (13) "idle"选项
    } else if (StrEqvSkip(lp, "idle ")) {
      UcciComm.Option = UCCI_OPTION_IDLE;
      if (false) {
      } else if (StrEqv(lp, "none")) {
        UcciComm.Grade = UCCI_GRADE_NONE;
      } else if (StrEqv(lp, "small")) {
        UcciComm.Grade = UCCI_GRADE_SMALL;
      } else if (StrEqv(lp, "medium")) {
        UcciComm.Grade = UCCI_GRADE_MEDIUM;
      } else if (StrEqv(lp, "large")) {
        UcciComm.Grade = UCCI_GRADE_LARGE;
      } else {
        UcciComm.Grade = UCCI_GRADE_NONE;
      }

    // (14) "pruning"选项
    } else if (StrEqvSkip(lp, "pruning ")) {
      UcciComm.Option = UCCI_OPTION_PRUNING;
      if (false) {
      } else if (StrEqv(lp, "none")) {
        UcciComm.Grade = UCCI_GRADE_NONE;
      } else if (StrEqv(lp, "small")) {
        UcciComm.Grade = UCCI_GRADE_SMALL;
      } else if (StrEqv(lp, "medium")) {
        UcciComm.Grade = UCCI_GRADE_MEDIUM;
      } else if (StrEqv(lp, "large")) {
        UcciComm.Grade = UCCI_GRADE_LARGE;
      } else {
        UcciComm.Grade = UCCI_GRADE_LARGE;
      }

    // (15) "knowledge"选项
    } else if (StrEqvSkip(lp, "knowledge ")) {
      UcciComm.Option = UCCI_OPTION_KNOWLEDGE;
      if (false) {
      } else if (StrEqv(lp, "none")) {
        UcciComm.Grade = UCCI_GRADE_NONE;
      } else if (StrEqv(lp, "small")) {
        UcciComm.Grade = UCCI_GRADE_SMALL;
      } else if (StrEqv(lp, "medium")) {
        UcciComm.Grade = UCCI_GRADE_MEDIUM;
      } else if (StrEqv(lp, "large")) {
        UcciComm.Grade = UCCI_GRADE_LARGE;
      } else {
        UcciComm.Grade = UCCI_GRADE_LARGE;
      }

    // (16) "randomness"选项
    } else if (StrEqvSkip(lp, "randomness ")) {
      UcciComm.Option = UCCI_OPTION_RANDOMNESS;
      if (false) {
      } else if (StrEqv(lp, "none")) {
        UcciComm.Grade = UCCI_GRADE_NONE;
      } else if (StrEqv(lp, "tiny")) {
        UcciComm.Grade = UCCI_GRADE_TINY;
      } else if (StrEqv(lp, "small")) {
        UcciComm.Grade = UCCI_GRADE_SMALL;
      } else if (StrEqv(lp, "medium")) {
        UcciComm.Grade = UCCI_GRADE_MEDIUM;
      } else if (StrEqv(lp, "large")) {
        UcciComm.Grade = UCCI_GRADE_LARGE;
      } else if (StrEqv(lp, "huge")) {
        UcciComm.Grade = UCCI_GRADE_HUGE;
      } else {
        UcciComm.Grade = UCCI_GRADE_NONE;
      }

    // (17) "style"选项
    } else if (StrEqvSkip(lp, "style ")) {
      UcciComm.Option = UCCI_OPTION_STYLE;
      if (false) {
      } else if (StrEqv(lp, "solid")) {
        UcciComm.Style = UCCI_STYLE_SOLID;
      } else if (StrEqv(lp, "normal")) {
        UcciComm.Style = UCCI_STYLE_NORMAL;
      } else if (StrEqv(lp, "risky")) {
        UcciComm.Style = UCCI_STYLE_RISKY;
      } else {
        UcciComm.Style = UCCI_STYLE_NORMAL;
      }

    // (18) "newgame"选项
    } else if (StrEqv(lp, "newgame")) {
      UcciComm.Option = UCCI_OPTION_NEWGAME;

    // (19) 无法识别的选项，有扩充的余地
    } else {
      UcciComm.Option = UCCI_OPTION_UNKNOWN;
    }
    return UCCI_COMM_SETOPTION;

  // 3. "position {<special_position> | fen <fen_string>} [moves <move_list>]"指令
  } else if (StrEqvSkip(lp, "position ")) {
    return ParsePos(UcciComm, lp) ? UCCI_COMM_POSITION : UCCI_COMM_UNKNOWN;

  // 4. "banmoves <move_list>"指令，处理起来和"position ... moves ..."是一样的
  } else if (StrEqvSkip(lp, "banmoves ")) {
    UcciComm.nBanMoveNum = MIN((int) (strlen(lp) + 1) / 5, MAX_MOVE_NUM);
    for (i = 0; i < UcciComm.nBanMoveNum; i ++) {
      dwCoordList[i] = *(uint32_t *) lp;
      lp += sizeof(uint32_t) + 1;
    }
    UcciComm.lpdwBanMovesCoord = dwCoordList;
    return UCCI_COMM_BANMOVES;

  // 5. "go [ponder | draw] <mode>"指令
  } else if (StrEqvSkip(lp, "go ")) {
    UcciComm.bPonder = UcciComm.bDraw = false;
    // 首先判断到底是"go"、"go ponder"还是"go draw"
    if (StrEqvSkip(lp, "ponder ")) {
      UcciComm.bPonder = true;
    } else if (StrEqvSkip(lp, "draw ")) {
      UcciComm.bDraw = true;
    }
    // 然后判断思考模式
    bGoTime = false;
    if (false) {
    } else if (StrEqvSkip(lp, "depth ")) {
      UcciComm.Go = UCCI_GO_DEPTH;
      UcciComm.nDepth = Str2Digit(lp, 0, UCCI_MAX_DEPTH);
    } else if (StrEqvSkip(lp, "nodes ")) {
      UcciComm.Go = UCCI_GO_NODES;
      UcciComm.nDepth = Str2Digit(lp, 0, 2000000000);
    } else if (StrEqvSkip(lp, "time ")) {
      UcciComm.nTime = Str2Digit(lp, 0, 2000000000);
      bGoTime = true;
    // 如果没有说明是固定深度还是设定时限，就固定深度为"UCCI_MAX_DEPTH"
    } else {
      UcciComm.Go = UCCI_GO_DEPTH;
      UcciComm.nDepth = UCCI_MAX_DEPTH;
    }
    // 如果是设定时限，就要判断是时段制还是加时制
    if (bGoTime) {
      if (false) {
      } else if (StrScanSkip(lp, " movestogo ")) {
        UcciComm.Go = UCCI_GO_TIME_MOVESTOGO;
        UcciComm.nMovesToGo = Str2Digit(lp, 1, 999);
      } else if (StrScanSkip(lp, " increment ")) {
        UcciComm.Go = UCCI_GO_TIME_INCREMENT;
        UcciComm.nIncrement = Str2Digit(lp, 0, 999999);
      // 如果没有说明是时段制还是加时制，就设定为步数是1的时段制
      } else {
        UcciComm.Go = UCCI_GO_TIME_MOVESTOGO;
        UcciComm.nMovesToGo = 1;
      }
    }
    return UCCI_COMM_GO;

  // 6. "stop"指令
  } else if (StrEqv(lp, "stop")) {
    return UCCI_COMM_STOP;

  // 7. "probe {<special_position> | fen <fen_string>} [moves <move_list>]"指令
  } else if (StrEqvSkip(lp, "probe ")) {
    return ParsePos(UcciComm, lp) ? UCCI_COMM_PROBE : UCCI_COMM_UNKNOWN;

  // 8. "quit"指令
  } else if (StrEqv(lp, "quit")) {
    return UCCI_COMM_QUIT;

  // 9. 无法识别的指令
  } else {
    return UCCI_COMM_UNKNOWN;
  }
}

UcciCommEnum BusyLine(UcciCommStruct &UcciComm, bool bDebug) {
  char szLineStr[LINE_INPUT_MAX_CHAR];
  char *lp;
  if (pipeStd.LineInput(szLineStr)) {
    if (bDebug) {
      printf("info busyline [%s]\n", szLineStr);
      fflush(stdout);
    }
    // "BusyLine"只能接收"isready"、"ponderhit"和"stop"这三条指令
    if (false) {
    } else if (StrEqv(szLineStr, "isready")) {
      return UCCI_COMM_ISREADY;
    } else if (StrEqv(szLineStr, "ponderhit draw")) {
      return UCCI_COMM_PONDERHIT_DRAW;
    // 注意：必须首先判断"ponderhit draw"，再判断"ponderhit"
    } else if (StrEqv(szLineStr, "ponderhit")) {
      return UCCI_COMM_PONDERHIT;
    } else if (StrEqv(szLineStr, "stop")) {
      return UCCI_COMM_STOP;
    } else if (StrEqv(szLineStr, "quit")) {
      return UCCI_COMM_QUIT;
    } else {
      lp = szLineStr;
      if (StrEqvSkip(lp, "probe ")) {
        return ParsePos(UcciComm, lp) ? UCCI_COMM_PROBE : UCCI_COMM_UNKNOWN;
      } else {
        return UCCI_COMM_UNKNOWN;
      }
    }
  } else {
    return UCCI_COMM_UNKNOWN;
  }
}
