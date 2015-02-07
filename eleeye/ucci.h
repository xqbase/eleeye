/*
ucci.h/ucci.cpp - Source Code for ElephantEye, Part I

ElephantEye - a Chinese Chess Program (UCCI Engine)
Designed by Morning Yellow, Version: 3.2, Last Modified: Sep. 2010
Copyright (C) 2004-2010 www.xqbase.com

This part (ucci.h/ucci.cpp only) of codes is NOT published under LGPL, and
can be used without restriction.
*/

#include "../base/base.h"

#ifndef UCCI_H
#define UCCI_H

const int UCCI_MAX_DEPTH = 32; // UCCI引擎思考的极限深度

// 和UCCI指令中关键字有关的选项
enum UcciOptionEnum {
  UCCI_OPTION_UNKNOWN, UCCI_OPTION_BATCH, UCCI_OPTION_DEBUG, UCCI_OPTION_PONDER, UCCI_OPTION_USEHASH, UCCI_OPTION_USEBOOK, UCCI_OPTION_USEEGTB,
  UCCI_OPTION_BOOKFILES, UCCI_OPTION_EGTBPATHS, UCCI_OPTION_HASHSIZE, UCCI_OPTION_THREADS, UCCI_OPTION_PROMOTION,
  UCCI_OPTION_IDLE, UCCI_OPTION_PRUNING, UCCI_OPTION_KNOWLEDGE, UCCI_OPTION_RANDOMNESS, UCCI_OPTION_STYLE, UCCI_OPTION_NEWGAME
}; // 由"setoption"指定的选项
enum UcciRepetEnum {
  UCCI_REPET_ALWAYSDRAW, UCCI_REPET_CHECKBAN, UCCI_REPET_ASIANRULE, UCCI_REPET_CHINESERULE
}; // 选项"repetition"的设定值
enum UcciGradeEnum {
  UCCI_GRADE_NONE, UCCI_GRADE_TINY, UCCI_GRADE_SMALL, UCCI_GRADE_MEDIUM, UCCI_GRADE_LARGE, UCCI_GRADE_HUGE
}; // 选项"idle"、"pruning"、"knowledge"、"selectivity"的设定值
enum UcciStyleEnum {
  UCCI_STYLE_SOLID, UCCI_STYLE_NORMAL, UCCI_STYLE_RISKY
}; // 选项"style"的设定值
enum UcciGoEnum {
  UCCI_GO_DEPTH, UCCI_GO_NODES, UCCI_GO_TIME_MOVESTOGO, UCCI_GO_TIME_INCREMENT
}; // 由"go"指令指定的时间模式，分别是限定深度、限定结点数、时段制和加时制
enum UcciCommEnum {
  UCCI_COMM_UNKNOWN, UCCI_COMM_UCCI, UCCI_COMM_ISREADY, UCCI_COMM_PONDERHIT, UCCI_COMM_PONDERHIT_DRAW, UCCI_COMM_STOP,
  UCCI_COMM_SETOPTION, UCCI_COMM_POSITION, UCCI_COMM_BANMOVES, UCCI_COMM_GO, UCCI_COMM_PROBE, UCCI_COMM_QUIT
}; // UCCI指令类型

// UCCI指令可以解释成以下这个抽象的结构
union UcciCommStruct {

  /* 可得到具体信息的UCCI指令只有以下4种类型
   *
   * 1. "setoption"指令传递的信息，适合于"UCCI_COMM_SETOPTION"指令类型
   *    "setoption"指令用来设定选项，因此引擎接受到的信息有“选项类型”和“选项值”
   *    例如，"setoption batch on"，选项类型就是"UCCI_OPTION_DEBUG"，值(Value.bCheck)就是"true"
   */
  struct {
    UcciOptionEnum Option; // 选项类型
    union {                // 选项值
      int nSpin;           // "spin"类型的选项的值
      bool bCheck;         // "check"类型的选项的值
      UcciRepetEnum Repet; // "combo"类型的选项"repetition"的值
      UcciGradeEnum Grade; // "combo"类型的选项"pruning"、"knowledge"和"selectivity"的值
      UcciStyleEnum Style; // "combo"类型的选项"style"的值
      char *szOption;      // "string"类型的选项的值
    };                     // "button"类型的选项没有值
  };

  /* 2. "position"指令传递的信息，适合于"e_CommPosition"指令类型
   *    "position"指令用来设置局面，包括初始局面连同后续着法构成的局面
   *    例如，position startpos moves h2e2 h9g8，FEN串就是"startpos"代表的FEN串，着法数(MoveNum)就是2
   */
  struct {
    const char *szFenStr;     // FEN串，特殊局面(如"startpos"等)也由解释器最终转换成FEN串
    int nMoveNum;             // 后续着法数
    uint32_t *lpdwMovesCoord; // 后续着法，指向程序"IdleLine()"中的一个静态数组，但可以把"CoordList"本身看成数组
  };

  /* 3. "banmoves"指令传递的信息，适合于"e_CommBanMoves"指令类型
   *    "banmoves"指令用来设置禁止着法，数据结构时类似于"position"指令的后续着法，但没有FEN串
   */
  struct {
    int nBanMoveNum;
    uint32_t *lpdwBanMovesCoord;
  };

  /* 4. "go"指令传递的信息，适合于"UCCI_COMM_GO指令类型
   *    "go"指令让引擎思考(搜索)，同时设定思考模式，即固定深度、时段制还是加时制
   */
  struct {
    UcciGoEnum Go; // 思考模式
    bool bPonder;  // 后台思考
    bool bDraw;    // 提和
    union {
      int nDepth, nNodes, nTime;
    }; // 深度、结点数或时间
    union {
      int nMovesToGo, nIncrement;
    }; // 限定时间内要走多少步棋(加时制)或走完该步后限定时间加多少(时段制)
  };
};

// 下面三个函数用来解释UCCI指令，但适用于不同场合
UcciCommEnum BootLine(void);                                  // UCCI引擎启动的第一条指令，只接收"ucci"
UcciCommEnum IdleLine(UcciCommStruct &UcciComm, bool bDebug); // 引擎空闲时接收指令
UcciCommEnum BusyLine(UcciCommStruct &UcciComm, bool bDebug); // 引擎思考时接收指令，只允许接收"stop"、"ponderhit"和"probe"

#endif
