/*
XQF->PGN Convertor - a Chinese Chess Score Convertion Program
Designed by Morning Yellow, Version: 2.1, Last Modified: May 2007
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
#ifdef _WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif
#include "../base/base2.h"
#include "../eleeye/position.h"
#include "../cchess/cchess.h"
#include "../cchess/ecco.h"
#include "../cchess/pgnfile.h"
#include "xqffile.h"

static const int cnResultTrans[4] = {
  0, 1, 3, 2
};

static const unsigned char cucsqXqf2Square[96] = {
  0xc3, 0xb3, 0xa3, 0x93, 0x83, 0x73, 0x63, 0x53, 0x43, 0x33,
  0xc4, 0xb4, 0xa4, 0x94, 0x84, 0x74, 0x64, 0x54, 0x44, 0x34,
  0xc5, 0xb5, 0xa5, 0x95, 0x85, 0x75, 0x65, 0x55, 0x45, 0x35,
  0xc6, 0xb6, 0xa6, 0x96, 0x86, 0x76, 0x66, 0x56, 0x46, 0x36,
  0xc7, 0xb7, 0xa7, 0x97, 0x87, 0x77, 0x67, 0x57, 0x47, 0x37,
  0xc8, 0xb8, 0xa8, 0x98, 0x88, 0x78, 0x68, 0x58, 0x48, 0x38,
  0xc9, 0xb9, 0xa9, 0x99, 0x89, 0x79, 0x69, 0x59, 0x49, 0x39,
  0xca, 0xba, 0xaa, 0x9a, 0x8a, 0x7a, 0x6a, 0x5a, 0x4a, 0x3a,
  0xcb, 0xbb, 0xab, 0x9b, 0x8b, 0x7b, 0x6b, 0x5b, 0x4b, 0x3b,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const int cpcXqf2Piece[32] = {
  23, 21, 19, 17, 16, 18, 20, 22, 24, 25, 26, 27, 28, 29, 30, 31,
  39, 37, 35, 33, 32, 34, 36, 38, 40, 41, 42, 43, 44, 45, 46, 47
};

// 密钥流掩码
static const char *const cszEncStreamMask = "[(C) Copyright Mr. Dong Shiwei.]";

inline int Square54Plus221(int x) {
  return x * x * 54 + 221;
}

inline void ReadAndDecrypt(FILE *fp, void *lp, int nLen, const int *nEncStream, int &nEncIndex) {
  int i;
  fread(lp, nLen, 1, fp);
  for (i = 0; i < nLen; i ++) {
    ((uint8_t *) lp)[i] -= nEncStream[nEncIndex];
    nEncIndex = (nEncIndex + 1) % 32;
  }
}

inline void GetXqfString(char *szPgn, const char *szXqf) {
  strncpy(szPgn, szXqf + 1, szXqf[0]);
  szPgn[szXqf[0]] = '\0';
}

static const int XQF2PGN_ERROR_OPEN = -3;
static const int XQF2PGN_ERROR_FORMAT = -2;
static const int XQF2PGN_ERROR_CREATE = -1;
static const int XQF2PGN_OK = 0;

int Xqf2Pgn(const char *szXqfFile, const char *szPgnFile, const EccoApiStruct &EccoApi) {
  int i, nArg0, nArgs[4];
  int nCommentLen, mv, nStatus;
  bool bHasNext;
  PgnFileStruct pgn;
  PositionStruct pos;

  FILE *fp;
  XqfHeaderStruct xqfhd;  
  XqfMoveStruct xqfmv;
  // 版本号和加密偏移值
  int nXqfVer, nPieceOff, nSrcOff, nDstOff, nCommentOff;
  // 密钥流
  int nEncStream[32];
  // 密钥流索引号
  int nEncIndex;
  // 局面初始位置
  int nPiecePos[32];

  uint32_t dwEccoIndex, dwFileMove[20];

  fp = fopen(szXqfFile, "rb");
  if (fp == NULL) {
    return XQF2PGN_ERROR_OPEN;
  }
  fread(&xqfhd, sizeof(xqfhd), 1, fp);
  fseek(fp, sizeof(xqfhd), SEEK_CUR);
  if (xqfhd.szTag[0] == 'X' && xqfhd.szTag[1] == 'Q') {
    // PGN文件可以打开，现在正式解析XQF文件
    nXqfVer = xqfhd.szTag[2];
    if (nXqfVer < 11) {
      nPieceOff = nSrcOff = nDstOff = nCommentOff = 0;
      for (i = 0; i < 32; i ++) {
        nEncStream[i] = 0;
      }
    } else {
      // 局面初始位置的加密偏移值
      nPieceOff = (uint8_t) (Square54Plus221((uint8_t) xqfhd.szTag[13]) * (uint8_t) xqfhd.szTag[13]);
      // 着法起点的加密偏移值
      nSrcOff = (uint8_t) (Square54Plus221((uint8_t) xqfhd.szTag[14]) * nPieceOff);
      // 着法终点的加密偏移值
      nDstOff = (uint8_t) (Square54Plus221((uint8_t) xqfhd.szTag[15]) * nSrcOff);
      // 注释的加密偏移值
      nCommentOff = ((uint8_t) xqfhd.szTag[12] * 256 + (uint8_t) xqfhd.szTag[13]) % 32000 + 767;
      // 基本掩码
      nArg0 = xqfhd.szTag[3];
      // 密钥 = 前段密钥 | (后段密钥 & 基本掩码)
       for (i = 0; i < 4; i ++) {
        nArgs[i] = xqfhd.szTag[8 + i] | (xqfhd.szTag[12 + i] & nArg0);
      }
      // 密钥流 = 密钥 & 密钥流掩码
      for (i = 0; i < 32; i ++) {
        nEncStream[i] = (uint8_t) (nArgs[i % 4] & cszEncStreamMask[i]);
      }
    }
    nEncIndex = 0;

    // 记录棋谱信息
    if (xqfhd.szEvent[0] == 0) {
      GetXqfString(pgn.szEvent, xqfhd.szTitle);
    } else {
      GetXqfString(pgn.szEvent, xqfhd.szEvent);
    }
    GetXqfString(pgn.szDate, xqfhd.szDate);
    GetXqfString(pgn.szSite, xqfhd.szSite);
    GetXqfString(pgn.szRed, xqfhd.szRed);
    GetXqfString(pgn.szBlack, xqfhd.szBlack);
    pgn.nResult = cnResultTrans[(int) xqfhd.szResult[3]];

    if (xqfhd.szSetUp[0] < 2) {
      // 如果是开局或者全局，那么直接设置起始局面
      pgn.posStart.FromFen(cszStartFen);
    } else {
      // 如果是中局或者排局，那么根据"xqfhd.szPiecePos[32]"的内容摆放局面
      // 当版本号达到12时，还要进一步解密局面初始位置
      if (nXqfVer < 12) {
        for (i = 0; i < 32; i ++) {
          nPiecePos[i] = (uint8_t) (xqfhd.szPiecePos[i] - nPieceOff);
        }
      } else {
        for (i = 0; i < 32; i ++) {
          nPiecePos[(nPieceOff + 1 + i) % 32] = (uint8_t) (xqfhd.szPiecePos[i] - nPieceOff);
        }
      }
      // 把"nPiecePos[32]"的数据放到"PositionStruct"中
      pgn.posStart.ClearBoard();
      for (i = 0; i < 32; i ++) {
        if (nPiecePos[i] < 90) {
          pgn.posStart.AddPiece(cucsqXqf2Square[nPiecePos[i]], cpcXqf2Piece[i]);
        }
      }
      pgn.posStart.SetIrrev();
    }
    pos = pgn.posStart;

    bHasNext = true;
    while (bHasNext && pgn.nMaxMove < MAX_MOVE_LEN) {
      // 读取着法记录
      if (nXqfVer < 11) {
        fread(&xqfmv, sizeof(xqfmv), 1, fp);
        fread(&nCommentLen, sizeof(int), 1, fp);
        if ((xqfmv.ucTag & 0xf0) == 0) {
          bHasNext = false;
        }
      } else {
        ReadAndDecrypt(fp, &xqfmv, sizeof(xqfmv), nEncStream, nEncIndex);
        if ((xqfmv.ucTag & 0x20) != 0) {
          ReadAndDecrypt(fp, &nCommentLen, sizeof(int), nEncStream, nEncIndex);
          nCommentLen -= nCommentOff;
        } else {
          nCommentLen = 0;
        }
        if ((xqfmv.ucTag & 0x80) == 0) {
          bHasNext = false;
        }
      }
      if (pgn.nMaxMove > 0) {
        // 记录着法
        mv = MOVE(cucsqXqf2Square[(uint8_t) (xqfmv.ucSrc - 24 - nSrcOff)], cucsqXqf2Square[(uint8_t) (xqfmv.ucDst - 32 - nDstOff)]);
        if (pgn.nMaxMove == 1) {
          if ((pgn.posStart.ucpcSquares[SRC(mv)] & 32) != 0) {
            pgn.posStart.ChangeSide();
            pos.ChangeSide();
          }
        }
        if (xqfhd.szSetUp[0] < 2 && pgn.nMaxMove <= 20) {
          dwFileMove[pgn.nMaxMove - 1] = Move2File(mv, pos);
        }
        TryMove(pos, nStatus, mv);
        pgn.wmvMoveTable[pgn.nMaxMove] = mv;
        if (pos.nMoveNum == MAX_MOVE_NUM) {
          pos.SetIrrev();
        }
      }
      if (nCommentLen > 0) {
        pgn.szCommentTable[pgn.nMaxMove] = new char[nCommentLen + 1];
        ReadAndDecrypt(fp, pgn.szCommentTable[pgn.nMaxMove], nCommentLen, nEncStream, nEncIndex);
        pgn.szCommentTable[pgn.nMaxMove][nCommentLen] = '\0';
      }
      pgn.nMaxMove ++;
    }
    pgn.nMaxMove --;

    // 解析ECCO
    if (xqfhd.szSetUp[0] < 2) {
      if (pgn.nMaxMove < 20) {
        dwFileMove[pgn.nMaxMove] = 0;
      }
      if (EccoApi.Available()) {
        dwEccoIndex = EccoApi.EccoIndex((const char *) dwFileMove);
        strcpy(pgn.szEcco, (const char *) &dwEccoIndex);
        strcpy(pgn.szOpen, EccoApi.EccoOpening(dwEccoIndex));
        strcpy(pgn.szVar, EccoApi.EccoVariation(dwEccoIndex));
      }
    }

    fclose(fp);
    return (pgn.Write(szPgnFile) ? XQF2PGN_OK : XQF2PGN_ERROR_CREATE);
  } else {
    fclose(fp);
    return XQF2PGN_ERROR_FORMAT;
  }
}

#ifndef MXQFCONV_EXE

int main(int argc, char **argv) {
  EccoApiStruct EccoApi;
  char szLibEccoPath[1024];

  if (argc < 2) {
    printf("=== XQF->PGN Convertor ===\n");
    printf("Usage: XQF2PGN XQF-File [PGN-File]\n");
    return 0;
  }

  PreGenInit();
  ChineseInit();
  LocatePath(szLibEccoPath, cszLibEccoFile);
  EccoApi.Startup(szLibEccoPath);

  switch (Xqf2Pgn(argv[1], argc == 2 ? "XQF2PGN.PGN" : argv[2], EccoApi)) {
  case XQF2PGN_ERROR_OPEN:
    printf("%s: File Opening Error!\n", argv[1]);
    break;
  case XQF2PGN_ERROR_FORMAT:
    printf("%s: Not an XQF File!\n", argv[1]);
    break;
  case XQF2PGN_ERROR_CREATE:
    printf("File Creation Error!\n");
    break;
  case XQF2PGN_OK:
#ifdef _WIN32
    if (argc == 2) {
      ShellExecute(NULL, NULL, "XQF2PGN.PGN", NULL, NULL, SW_SHOW);
    }
#endif
    break;
  }
  EccoApi.Shutdown();
  return 0;
}

#endif
