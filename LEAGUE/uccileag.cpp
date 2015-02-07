/*
UCCILEAG - a Computer Chinese Chess League (UCCI Engine League) Emulator
Designed by Morning Yellow, Version: 3.8, Last Modified: Dec. 2011
Copyright (C) 2004-2011 www.xqbase.com

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

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif
#include "../base/base2.h"
#include "../base/parse.h"
#include "../base/pipe.h"
#include "../base/wsockbas.h"
#include "../codec/base64/base64.h"
#include "../eleeye/position.h"
#include "../cchess/cchess.h"
#include "../cchess/ecco.h"
#include "../cchess/pgnfile.h"

const int MAX_CHAR = LINE_INPUT_MAX_CHAR; // 输入报告的最大行长度，同时也是引擎发送和接收信息的最大行长度
const int MAX_ROBIN = 36;                 // 最多的循环
const int MAX_TEAM = 32;                  // 最多的参赛队数
const int MAX_PROCESSORS = 32;            // 最多的处理器数
const int QUEUE_LEN = 64;                 // 处理器队列长度(最好是处理器数的两倍)

const char *const cszRobinChar = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// 进程文件的记录结构
struct CheckStruct {
  int mv, nTimer;
}; // chk

// 进程文件的控制结构
struct CheckFileStruct {
  FILE *fp;
  int nLen, nPtr;
  bool Eof(void) {                    // 判断进程文件是否读完
    return nPtr == nLen;
  }
  void Open(const char *szFileName);  // 打开进程文件
  void Close(void) {                  // 关闭进程文件
    fclose(fp);
  }
  CheckStruct Read(void) {            // 读进程文件的记录
    CheckStruct chkRecord;
    fseek(fp, nPtr * sizeof(CheckStruct), SEEK_SET);
    fread(&chkRecord, sizeof(CheckStruct), 1, fp);
    nPtr ++;
    return chkRecord;
  }
  void Write(CheckStruct chkRecord) { // 写进程文件的记录
    fseek(fp, nPtr * sizeof(CheckStruct), SEEK_SET);
    fwrite(&chkRecord, sizeof(CheckStruct), 1, fp);
    fflush(fp);
    nPtr ++;
    nLen ++;
  }
};

// 打开进程文件，如果文件不存在，那么要新建一个空文件并重新打开
void CheckFileStruct::Open(const char *szFileName) {
  fp = fopen(szFileName, "r+b");
  if (fp == NULL) {
    fp = fopen(szFileName, "wb");
    if (fp == NULL) {
      printf("错误：无法建立比赛过程文件\"%s\"!\n", szFileName);
      exit(EXIT_FAILURE);
    }
    fclose(fp);
    fp = fopen(szFileName, "r+b");
    if (fp == NULL) {
      printf("错误：无法打开比赛过程文件\"%s\"!\n", szFileName);
      exit(EXIT_FAILURE);
    }
    nLen = nPtr = 0;
  } else {
    fseek(fp, 0, SEEK_END);
    nLen = ftell(fp) / sizeof(CheckStruct);
    nPtr = 0;
  }
}

// 参赛队结构
struct TeamStruct {
  uint32_t dwAbbr;
  int nEloValue, nKValue;
  char szEngineName[MAX_CHAR], szEngineFile[MAX_CHAR];
  char szOptionFile[MAX_CHAR], szUrl[MAX_CHAR], szGoParam[MAX_CHAR];
  int nWin, nDraw, nLoss, nScore;
};

// 参赛队列表
static TeamStruct TeamList[MAX_TEAM];

// 联赛全局变量
static struct {
  volatile bool bRunning;
  int nTeamNum, nRobinNum, nRoundNum, nGameNum, nRemainProcs;
  int nInitTime, nIncrTime, nStopTime, nStandardCpuTime, nNameLen;
  bool bPromotion;
  char szEvent[MAX_CHAR], szSite[MAX_CHAR];
  char szRobinFens[MAX_ROBIN][MAX_CHAR];
  EccoApiStruct EccoApi;
} League;

// 循环赛对阵图
static char RobinTable[2 * MAX_TEAM - 2][MAX_TEAM / 2][2];

// 直播全局变量
static struct {
  int8_t cResult[MAX_ROBIN][2 * MAX_TEAM - 2][MAX_TEAM / 2];
  char szHost[MAX_CHAR], szPath[MAX_CHAR], szPassword[MAX_CHAR];
  char szExt[MAX_CHAR], szCounter[MAX_CHAR], szHeader[MAX_CHAR], szFooter[MAX_CHAR];
  char szProxyHost[MAX_CHAR], szProxyUser[MAX_CHAR], szProxyPassword[MAX_CHAR];
  int nPort, nRefresh, nInterval, nProxyPort;
  int64_t llTime;
} Live;

static const char *const cszContent1 =
    "--[UCCI-LIVE-UPLOAD-BOUNDARY]" "\r\n"
    "Content-Disposition: form-data; name=\"upload\"; filename=\"upload.txt\"" "\r\n"
    "Content-Type: text/plain" "\r\n"
    "\r\n";
static const char *const cszContentFormat2 =
    "\r\n"
    "--[UCCI-LIVE-UPLOAD-BOUNDARY]" "\r\n"
    "Content-Disposition: form-data; name=\"filename\"" "\r\n"
    "\r\n"
    "%s" "\r\n"
    "--[UCCI-LIVE-UPLOAD-BOUNDARY]" "\r\n"
    "Content-Disposition: form-data; name=\"password\"" "\r\n"
    "\r\n"
    "%s" "\r\n"
    "--[UCCI-LIVE-UPLOAD-BOUNDARY]--" "\r\n";
static const char *const cszPostFormat =
    "POST %s HTTP/1.1" "\r\n"
    "Content-Type: multipart/form-data; boundary=[UCCI-LIVE-UPLOAD-BOUNDARY]" "\r\n"
    "Host: %s:%d" "\r\n"
    "Content-Length: %d" "\r\n"
    "\r\n";
static const char *const cszProxyFormat =
    "POST http://%s:%d%s HTTP/1.1" "\r\n"
    "Content-Type: multipart/form-data; boundary=[UCCI-LIVE-UPLOAD-BOUNDARY]" "\r\n"
    "Host: %s:%d" "\r\n"
    "Content-Length: %d" "\r\n"
    "\r\n";
static const char *const cszAuthFormat =
    "POST http://%s:%d%s HTTP/1.1" "\r\n"
    "Content-Type: multipart/form-data; boundary=[UCCI-LIVE-UPLOAD-BOUNDARY]" "\r\n"
    "Host: %s:%d" "\r\n"
    "Content-Length: %d" "\r\n"
    "Proxy-Authorization: Basic %s" "\r\n"
    "\r\n";

static void BlockSend(int nSocket, const char *lpBuffer, int nLen, int nTimeOut) {
  int nBytesWritten, nOffset;
  int64_t llTime;

  nOffset = 0;
  llTime = GetTime();
  while (nLen > 0 && (int) (GetTime() - llTime) < nTimeOut) {
    nBytesWritten = WSBSend(nSocket, lpBuffer + nOffset, nLen);
    if (nBytesWritten == 0) {
      Idle();
    } else if (nBytesWritten < 0) {
      return;
    }
    nOffset += nBytesWritten;
    nLen -= nBytesWritten;
  }
}

const bool FORCE_PUBLISH = true;

static void HttpUpload(const char *szFileName) {
  FILE *fpUpload;
  int nSocket, nContentLen1, nFileLen, nContentLen2, nPostLen;
  char szPost[MAX_CHAR * 4], szContent2[MAX_CHAR * 4], szAuth[MAX_CHAR], szAuthB64[MAX_CHAR];

  fpUpload = fopen(szFileName, "rb");
  if (fpUpload == NULL) {
    return;
  }
  if (Live.nProxyPort == 0) {
    nSocket = WSBConnect(Live.szHost, Live.nPort);
  } else {
    nSocket = WSBConnect(Live.szProxyHost, Live.nProxyPort);
  }
  if (nSocket == INVALID_SOCKET) {
    fclose(fpUpload);
    return;
  }
  fseek(fpUpload, 0, SEEK_END);
  nContentLen1 = strlen(cszContent1);
  nFileLen = ftell(fpUpload);
  nContentLen2 = sprintf(szContent2, cszContentFormat2, szFileName, Live.szPassword);
  if (Live.nProxyPort == 0) {
    nPostLen = sprintf(szPost, cszPostFormat, Live.szPath, Live.szHost, Live.nPort, nContentLen1 + nFileLen + nContentLen2);
  } else {
    if (Live.szProxyUser[0] == '\0') {
      nPostLen = sprintf(szPost, cszProxyFormat, Live.szHost, Live.nPort,
          Live.szPath, Live.szHost, Live.nPort, nContentLen1 + nFileLen + nContentLen2);
    } else {
      nPostLen = sprintf(szAuth, "%s:%s", Live.szProxyUser, Live.szProxyPassword);
      B64Enc(szAuthB64, szAuth, nPostLen, 0);
      nPostLen = sprintf(szPost, cszAuthFormat, Live.szHost, Live.nPort,
          Live.szPath, Live.szHost, Live.nPort, nContentLen1 + nFileLen + nContentLen2, szAuthB64);
    }
  }
  // 以阻塞方式发送数据，超时为10秒，这里缓冲区最大是16K，所以网速小于1.6KB/s时就容易出错
  BlockSend(nSocket, szPost, nPostLen, 10000);
  BlockSend(nSocket, cszContent1, nContentLen1, 10000);
  fseek(fpUpload, 0, SEEK_SET);
  while (nFileLen > 0) {
    nPostLen = MIN(nFileLen, MAX_CHAR * 4);
    fread(szPost, nPostLen, 1, fpUpload);
    BlockSend(nSocket, szPost, nPostLen, 10000);
    nFileLen -= nPostLen;
  }
  BlockSend(nSocket, szContent2, nContentLen2, 10000);
  WSBDisconnect(nSocket);
  fclose(fpUpload);
}

static const char *const cszResultDigit[4] = {
  "-", "(1-0)", "(1/2-1/2)", "(0-1)"
};

static bool SkipUpload(bool bForce) {
  if ((int) (GetTime() - Live.llTime) < Live.nInterval) {
    // 如果与上次上传间隔太近，那么暂缓上传
    if (!bForce) {
      return true;
    }
    // 如果强制上传，那么必须等待
    while ((int) (GetTime() - Live.llTime) < Live.nInterval) {
      Idle();
    }
  }
  Live.llTime = GetTime();
  return false;
}

static void PrintFile(FILE *fp, const char *szFileName) {
  char szLineStr[MAX_CHAR];
  char *lp;
  FILE *fpEmbedded;
  fpEmbedded = fopen(szFileName, "rt");
  if (fpEmbedded != NULL) {
    while (fgets(szLineStr, MAX_CHAR, fpEmbedded) != NULL) {
      lp = strchr(szLineStr, '\n');
      if (lp != NULL) {
        *lp = '\0';
      }
      fprintf(fp, "%s\n", szLineStr);
    }
    fclose(fpEmbedded);
  }
}

static void PublishLeague(void) {
  int nSortList[MAX_TEAM];
  int i, j, k, nLastRank ,nLastScore, nResult;
  uint32_t dwHome, dwAway;
  TeamStruct *lpTeam;
  char szEmbeddedFile[MAX_CHAR];
  char szUploadFile[16];
  FILE *fp;

  SkipUpload(FORCE_PUBLISH); // 始终返回 false
  if (Live.nPort == 0) {
    return;
  }
  strcpy(szUploadFile, "index.");
  strncpy(szUploadFile + 6, Live.szExt, 6);
  szUploadFile[12] = '\0';
  fp = fopen(szUploadFile, "wt");
  if (fp == NULL) {
    return;
  }

  // 显示页眉
  fprintf(fp, "<html>\n");
  fprintf(fp, "  <head>\n");
  fprintf(fp, "    <meta name=\"GENERATOR\" content=\"UCCI引擎联赛在线直播系统\">\n");
  fprintf(fp, "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb_2312-80\">\n");
  fprintf(fp, "    <title>%s 在线直播</title>\n", League.szEvent);
  fprintf(fp, "  </head>\n");
  fprintf(fp, "  <body background=\"background.gif\">\n");
  fprintf(fp, "    <p align=\"center\">\n");
  fprintf(fp, "      <font size=\"6\" face=\"隶书\">%s 在线直播</font>\n", League.szEvent);
  fprintf(fp, "    </p>\n");
  if (Live.szHeader[0] != '\0') {
    LocatePath(szEmbeddedFile, Live.szHeader);
    PrintFile(fp, szEmbeddedFile);
  }
  fprintf(fp, "    <p align=\"center\">\n");
  fprintf(fp, "      <font size=\"4\" face=\"楷体_GB2312\">\n");
  fprintf(fp, "        <strong>排名</strong>\n");
  fprintf(fp, "      </font>\n");
  fprintf(fp, "    </p>\n");
  fprintf(fp, "    <table align=\"center\" border=\"1\">\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <th>排名</th>\n");
  fprintf(fp, "        <th>缩写</th>\n");
  fprintf(fp, "        <th>引擎名称</th>\n");
  fprintf(fp, "        <th>等级分</th>\n");
  fprintf(fp, "        <th>K值</th>\n");
  fprintf(fp, "        <th>局数</th>\n");
  fprintf(fp, "        <th>胜</th>\n");
  fprintf(fp, "        <th>和</th>\n");
  fprintf(fp, "        <th>负</th>\n");
  fprintf(fp, "        <th>积分</th>\n");
  fprintf(fp, "      </tr>\n");

  // 显示排名，可参阅"PrintRankList()"
  for (i = 0; i < League.nTeamNum; i ++) {
    nSortList[i] = i;
  }
  for (i = 0; i < League.nTeamNum - 1; i ++) {
    for (j = League.nTeamNum - 1; j > i; j --) {
      if (TeamList[nSortList[j - 1]].nScore < TeamList[nSortList[j]].nScore) {
        SWAP(nSortList[j - 1], nSortList[j]);
      }
    }
  }
  nLastRank = nLastScore = 0;
  for (i = 0; i < League.nTeamNum; i ++) {
    lpTeam = TeamList + nSortList[i];
    if (lpTeam->nScore != nLastScore) {
      nLastRank = i;
      nLastScore = lpTeam->nScore;
    }
    fprintf(fp, "      <tr>\n");
    fprintf(fp, "        <td align=\"center\">%d</td>\n", nLastRank + 1);
    fprintf(fp, "        <td align=\"center\">%.3s</td>\n", (const char *) &lpTeam->dwAbbr);
    fprintf(fp, "        <td align=\"center\">\n");
    if (lpTeam->szUrl[0] == '\0') {
      fprintf(fp, "          %s\n", lpTeam->szEngineName);
    } else {
      fprintf(fp, "          <a href=\"%s\" target=\"_blank\">%s</a>\n", lpTeam->szUrl, lpTeam->szEngineName);
    }
    fprintf(fp, "        </td>\n");
    fprintf(fp, "        <td align=\"center\">%d</td>\n", lpTeam->nEloValue);
    fprintf(fp, "        <td align=\"center\">%d</td>\n", lpTeam->nKValue);
    fprintf(fp, "        <td align=\"center\">%d</td>\n", lpTeam->nWin + lpTeam->nDraw + lpTeam->nLoss);
    fprintf(fp, "        <td align=\"center\">%d</td>\n", lpTeam->nWin);
    fprintf(fp, "        <td align=\"center\">%d</td>\n", lpTeam->nDraw);
    fprintf(fp, "        <td align=\"center\">%d</td>\n", lpTeam->nLoss);
    fprintf(fp, "        <td align=\"center\">%d%s</td>\n", lpTeam->nScore / 2, lpTeam->nScore % 2 == 0 ? "" : ".5");
    fprintf(fp, "      </tr>\n");
  }

  // 显示内容
  fprintf(fp, "    </table>\n");
  fprintf(fp, "    <p align=\"center\">\n");
  fprintf(fp, "      <font size=\"4\" face=\"楷体_GB2312\">\n");
  fprintf(fp, "        <strong>赛程</strong>\n");
  fprintf(fp, "      </font>\n");
  fprintf(fp, "    </p>\n");
  fprintf(fp, "    <table align=\"center\" border=\"1\">\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <th>轮次</th>\n");
  fprintf(fp, "        <th colspan=\"%d\">对局</th>\n", League.nGameNum);
  fprintf(fp, "      </tr>\n");

  // 显示对局
  for (i = 0; i < League.nRobinNum; i ++) {
    for (j = 0; j < League.nRoundNum; j ++) {
      fprintf(fp, "      <tr>\n");
      fprintf(fp, "        <td align=\"center\">%d</td>\n", i * League.nRoundNum + j + 1);
      for (k = 0; k < League.nGameNum; k ++) {
        fprintf(fp, "        <td align=\"center\">\n");
        nResult = Live.cResult[i][j][k];
        dwHome = TeamList[(int) RobinTable[j][k][0]].dwAbbr;
        dwAway = TeamList[(int) RobinTable[j][k][1]].dwAbbr;
        if (nResult == -1) {
          fprintf(fp, "          %.3s-%.3s\n", (const char *) &dwHome, (const char *) &dwAway);
        } else {
          fprintf(fp, "          <a href=\"%.3s-%.3s%c.%s\" target=\"_blank\">\n",
              (const char *) &dwHome, (const char *) &dwAway, cszRobinChar[i], Live.szExt);
          if (nResult == 0) {
            fprintf(fp, "            <font color=\"#FF0000\">\n");
            fprintf(fp, "              <strong>\n");
          }
          fprintf(fp, "                %.3s%s%.3s\n", (const char *) &dwHome,
              cszResultDigit[nResult], (const char *) &dwAway);
          if (nResult == 0) {
            fprintf(fp, "              </strong>\n");
            fprintf(fp, "            </font>\n");
          }
          fprintf(fp, "          </a>\n");
        }
        fprintf(fp, "        </td>\n");
      }
      fprintf(fp, "      </tr>\n");
    }
  }

  // 显示页脚
  fprintf(fp, "    </table>\n");
  fprintf(fp, "    <table>\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">　　</td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "    </table>\n");
  if (Live.szFooter[0] != '\0') {
    LocatePath(szEmbeddedFile, Live.szFooter);
    PrintFile(fp, szEmbeddedFile);
  }
  fprintf(fp, "    <table align=\"center\">\n");
  if (Live.szCounter[0] != '\0') {
    fprintf(fp, "      <tr>\n");
    fprintf(fp, "        <td align=\"center\">\n");
    fprintf(fp, "          <font face=\"楷体_GB2312\">\n");
    fprintf(fp, "            <strong>您是第</strong>\n");
    fprintf(fp, "          </font>\n");
    fprintf(fp, "          <font face=\"Arial\">\n");
    fprintf(fp, "            <strong>\n");
    fprintf(fp, "              <script language=\"JavaScript\" src=\"%s\"></script>\n", Live.szCounter);
    fprintf(fp, "            </strong>\n");
    fprintf(fp, "          </font>\n");
    fprintf(fp, "          <font face=\"楷体_GB2312\">\n");
    fprintf(fp, "            <strong>位观众</strong>\n");
    fprintf(fp, "          </font>\n");
    fprintf(fp, "        </td>\n");
    fprintf(fp, "      </tr>\n");
    fprintf(fp, "      <tr>\n");
    fprintf(fp, "        <td align=\"center\">　　</td>\n");
    fprintf(fp, "      </tr>\n");
  }
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <font size=\"2\">\n");
  fprintf(fp, "            本页面由“<a href=\"http://www.xqbase.com/league/emulator.htm\" target=\"_blank\">"
      "UCCI引擎联赛在线直播系统</a>”生成\n");
  fprintf(fp, "          </font>\n");
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <a href=\"http://www.xqbase.com/\" target=\"_blank\">\n");
  fprintf(fp, "            <img src=\"xqbase.gif\" border=\"0\">\n");
  fprintf(fp, "          </a>\n");
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <a href=\"http://www.xqbase.com/\" target=\"_blank\">\n");
  fprintf(fp, "            <font size=\"2\" face=\"Arial\">\n");
  fprintf(fp, "              <strong>www.xqbase.com</strong>\n");
  fprintf(fp, "            </font>\n");
  fprintf(fp, "          </a>\n");
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "    </table>\n");
  fprintf(fp, "  </body>\n");
  fprintf(fp, "</html>\n");
  fclose(fp);
  HttpUpload(szUploadFile);
}

static const char *const cszResultChin[4] = {
  "对", "先胜", "先和", "先负"
};

inline void MOVE_ICCS(char *szIccs, int mv) {
  szIccs[0] = (FILE_X(SRC(mv))) + 'A' - FILE_LEFT;
  szIccs[1] = '9' + RANK_TOP - (RANK_Y(SRC(mv)));
  szIccs[2] = '%';
  szIccs[3] = '2';
  szIccs[4] = 'D';
  szIccs[5] = (FILE_X(DST(mv))) + 'A' - FILE_LEFT;
  szIccs[6] = '9' + RANK_TOP - (RANK_Y(DST(mv)));
  szIccs[7] = '\0';
}

static void PublishGame(PgnFileStruct *lppgn, const char *szGameFile, bool bForce = false) {
  int i, nStatus, nCounter;
  uint64_t dqChinMove;
  char szEmbeddedFile[MAX_CHAR], szStartFen[MAX_CHAR];
  char szUploadFile[16], szIccs[8];
  char *lp;
  FILE *fp;
  PositionStruct pos;

  if (SkipUpload(bForce)) {
    return;
  }
  if (Live.nPort == 0) {
    return;
  }
  strcpy(szUploadFile, szGameFile);
  lp = strchr(szUploadFile, '.') + 1;
  strncpy(lp, Live.szExt, 6);
  lp[6] = '\0';
  fp = fopen(szUploadFile, "wt");
  if (fp == NULL) {
    return;
  }

  // 显示页眉
  fprintf(fp, "<html>\n");
  fprintf(fp, "  <head>\n");
  fprintf(fp, "    <meta name=\"GENERATOR\" content=\"UCCI引擎联赛在线直播系统\">\n");
  fprintf(fp, "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb_2312-80\">\n");
  if (lppgn->nResult == 0 && Live.nRefresh != 0) {
    fprintf(fp, "    <meta http-equiv=\"Refresh\" content=\"%d;url=%s\">\n", Live.nRefresh, szUploadFile);
  }
  fprintf(fp, "    <title>%s (%s) %s - %s 在线直播</title>\n",
      lppgn->szRed, cszResultChin[lppgn->nResult], lppgn->szBlack, League.szEvent);
  fprintf(fp, "  </head>\n");
  fprintf(fp, "  <body background=\"background.gif\">\n");
  fprintf(fp, "    <p align=\"center\">\n");
  fprintf(fp, "      <font size=\"6\" face=\"隶书\">%s 在线直播</font>\n", League.szEvent);
  fprintf(fp, "    </p>\n");
  if (Live.szHeader[0] != '\0') {
    LocatePath(szEmbeddedFile, Live.szHeader);
    PrintFile(fp, szEmbeddedFile);
  }
  fprintf(fp, "    <p align=\"center\">\n");
  fprintf(fp, "      <font size=\"4\" face=\"楷体_GB2312\">\n");
  fprintf(fp, "        <strong>%s (%s) %s</strong>\n", lppgn->szRed, cszResultChin[lppgn->nResult], lppgn->szBlack);
  fprintf(fp, "      </font>\n");
  fprintf(fp, "    </p>\n");
  if (lppgn->nResult == 0) {
    fprintf(fp, "    <p align=\"center\">\n");
    fprintf(fp, "      <font size=\"2\">\n");
    fprintf(fp, "        <a href=\"%s\">\n", szUploadFile);
    fprintf(fp, "          <strong>对局进行中，如果您的浏览器没有自动跳转，请点击这里</strong>\n");
    fprintf(fp, "        </a>\n");
    fprintf(fp, "      </font>\n");
    fprintf(fp, "    </p>\n");
  }

  // 显示Flash棋盘
  fprintf(fp, "    <table align=\"center\">\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <font size=\"2\">黑方 %s (%s)</font>\n", lppgn->szBlack, lppgn->szBlackElo);
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <embed src=\"http://www.xqbase.com/flashxq.swf\" width=\"216\" height=\"264\""
      "allowScriptAccess=\"sameDomain\" quality=\"high\" wmode=\"transparent\" flashvars=\"MoveList=");
  for (i = 1; i <= lppgn->nMaxMove; i ++) {
    MOVE_ICCS(szIccs, lppgn->wmvMoveTable[i]);
    fprintf(fp, "%s+", szIccs);
  }
  if (lppgn->nResult == 0) {
    fprintf(fp, "&Step=%d", lppgn->nMaxMove);
  }
  lppgn->posStart.ToFen(szStartFen);
  if (strcmp(szStartFen, cszStartFen) != 0) {
    fprintf(fp, "&Position=%s", szStartFen);
  }
  fprintf(fp, "\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\" />\n");
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <font size=\"2\">红方 %s (%s)</font>\n", lppgn->szRed, lppgn->szRedElo);
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "    </table>\n");

  // 显示开局信息
  if (lppgn->szEcco[0] == '\0') {
    fprintf(fp, "    <table align=\"center\">\n");
    fprintf(fp, "      <tr>\n");
    fprintf(fp, "        <td align=\"center\">　　</td>\n");
    fprintf(fp, "      </tr>\n");
    fprintf(fp, "    </table>\n");
  } else {
    fprintf(fp, "    <p align=\"center\">\n");
    fprintf(fp, "      <font size=\"4\">\n");
    if (lppgn->szVar[0] == '\0') {
      fprintf(fp, "        <strong>%s(%s)</strong>\n", lppgn->szOpen, lppgn->szEcco);
    } else {
      fprintf(fp, "        <strong>%s——%s(%s)</strong>\n", lppgn->szOpen, lppgn->szVar, lppgn->szEcco);
    }
    fprintf(fp, "      </font>\n");
    fprintf(fp, "    </p>\n");
  }
  fprintf(fp, "    <table align=\"center\">\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td>\n");
  fprintf(fp, "          <dl>\n");

  // 显示着法
  pos = lppgn->posStart;
  nCounter = 1;
  for (i = 1; i <= lppgn->nMaxMove; i ++) {
    dqChinMove = File2Chin(Move2File(lppgn->wmvMoveTable[i], pos), pos.sdPlayer);
    if (pos.sdPlayer == 0) {
      fprintf(fp, "            <dt>%d. %.8s", nCounter, (const char *) &dqChinMove);
    } else {
      fprintf(fp, " %.8s</dt>\n", (const char *) &dqChinMove);
      nCounter ++;
    }
    TryMove(pos, nStatus, lppgn->wmvMoveTable[i]);
    if (pos.nMoveNum == MAX_MOVE_NUM) {
      pos.SetIrrev();
    }
  }
  if (pos.sdPlayer == 1) {
    fprintf(fp, "</dt>\n");
  }
  if (lppgn->szCommentTable[lppgn->nMaxMove] != NULL) {
    fprintf(fp, "            <dt>　　%s</dt>\n", lppgn->szCommentTable[lppgn->nMaxMove]);
  }

  // 显示页脚
  fprintf(fp, "          </dl>\n");
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <a href=\"%s\"><img src=\"pgn.gif\" border=\"0\">下载棋局</a>", szGameFile);
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "    </table>\n");
  fprintf(fp, "    <dl>\n");
  fprintf(fp, "      <dt>　　如果您已经安装《象棋巫师》软件，那么点击上面链接，《象棋巫师》就会自动打开棋局。</dt>\n");
  fprintf(fp, "      <dt>　　《象棋巫师》是免费软件，您可以访问以下页面，获得速度最快的下载链接：</dt>\n");
  fprintf(fp, "      <dt>　　　　<a href=\"http://www.skycn.com/soft/24665.html\" target=\"_blank\">"
      "http://www.skycn.com/soft/24665.html</a>(天空软件站)</dt>\n");
  fprintf(fp, "      <dt>　　　　<a href=\"http://www.onlinedown.net/soft/38287.htm\" target=\"_blank\">"
      "http://www.onlinedown.net/soft/38287.htm</a>(华军软件园)</dt>\n");
  fprintf(fp, "    </dl>\n");
  fprintf(fp, "    <ul>\n");
  fprintf(fp, "      <li>返回　<a href=\"index.%s\">%s 在线直播</a></li>\n", Live.szExt, League.szEvent);
  fprintf(fp, "    </ul>\n");
  if (Live.szFooter[0] != '\0') {
    LocatePath(szEmbeddedFile, Live.szFooter);
    PrintFile(fp, szEmbeddedFile);
  }
  fprintf(fp, "    <table align=\"center\">\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <font size=\"2\">\n");
  fprintf(fp, "            本页面由“<a href=\"http://www.xqbase.com/league/emulator.htm\" target=\"_blank\">"
      "UCCI引擎联赛在线直播系统</a>”生成\n");
  fprintf(fp, "          </font>\n");
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <a href=\"http://www.xqbase.com/\" target=\"_blank\">\n");
  fprintf(fp, "            <img src=\"xqbase.gif\" border=\"0\">\n");
  fprintf(fp, "          </a>\n");
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "      <tr>\n");
  fprintf(fp, "        <td align=\"center\">\n");
  fprintf(fp, "          <a href=\"http://www.xqbase.com/\" target=\"_blank\">\n");
  fprintf(fp, "            <font size=\"2\" face=\"Arial\">\n");
  fprintf(fp, "              <strong>www.xqbase.com</strong>\n");
  fprintf(fp, "            </font>\n");
  fprintf(fp, "          </a>\n");
  fprintf(fp, "        </td>\n");
  fprintf(fp, "      </tr>\n");
  fprintf(fp, "    </table>\n");
  fprintf(fp, "  </body>\n");
  fprintf(fp, "</html>\n");
  fclose(fp);
  HttpUpload(szUploadFile);
  // 由于紧接着就上传，不能保证棋局文件一定上传成功
  HttpUpload(szGameFile);
}

// 比赛结构，0代表主队(先行方)，1代表客队(后行方)
struct GameStruct {
  int sd, nCounter, nResult, nTimer[2];
  bool bTimeout, bStarted[2], bUseMilliSec[2], bDraw;
  int64_t llTime;
  TeamStruct *lpTeam[2];
  PipeStruct pipe[2];
  PositionStruct posIrrev;
  char szIrrevFen[MAX_CHAR];
  char szGameFile[16];
  PgnFileStruct *lppgn;
  uint32_t dwFileMove[20];
  FILE *fpLogFile;
  CheckFileStruct CheckFile;

  void Send(const char *szLineStr) {
    pipe[sd].LineOutput(szLineStr);
    fprintf(fpLogFile, "Emu->%.3s(%08d):%s\n", (const char *) &lpTeam[sd]->dwAbbr,
        nTimer[sd] - (int) (GetTime() - llTime), szLineStr);
    fflush(fpLogFile);
  }
  bool Receive(char *szLineStr) {
    if (pipe[sd].LineInput(szLineStr)) {
      fprintf(fpLogFile, "%.3s->Emu(%08d):%s\n", (const char *) &lpTeam[sd]->dwAbbr,
          nTimer[sd] - (int) (GetTime() - llTime), szLineStr);
      fflush(fpLogFile);
      return true;
    } else {
      return false;
    }
  }
  void AddMove(int mv);  // 走一个着法
  void RunEngine(void);  // 让引擎运行
  void BeginGame(int nRobin, int nRound, int nGame); // 开始一个棋局
  void QuitEngine(void); // 让引擎退出
  void ResumeGame(void); // 继续上次挂起的棋局
  bool EndGame(int nRobin, int nRound, int nGame);   // 终止一个棋局
  void TerminateGame(void); // 中断一个棋局
};

static const char *const cszColorStr[2] = {
  "红方", "黑方"
};

const int BESTMOVE_THINKING = 0; // 引擎正在思考，没有反馈值
const int BESTMOVE_DRAW = -1;    // 引擎接受提和的反馈值
const int BESTMOVE_RESIGN = -2;  // 引擎认输的反馈值
const int BESTMOVE_TIMEOUT = -3; // 引擎超时的反馈值

// 走一个着法
void GameStruct::AddMove(int mv) {
  int nStatus;
  uint32_t dwEccoIndex;
  char *szComment;
  char szStartFen[MAX_CHAR];

  if (mv < BESTMOVE_THINKING) {
    szComment = new char[MAX_CHAR];
    lppgn->szCommentTable[lppgn->nMaxMove] = szComment;
    if (mv == BESTMOVE_DRAW) {
      strcpy(szComment, "双方议和");
      nResult = 2;
    } else {
      sprintf(szComment, mv == BESTMOVE_RESIGN ? "%s认输" : "%s超时作负", cszColorStr[sd]);
      nResult = 3 - sd * 2;
    }
  } else {
    // 首先把该着法记录到棋谱上
    lppgn->nMaxMove ++;
    lppgn->wmvMoveTable[lppgn->nMaxMove] = mv;
    // 解析ECCO
    lppgn->posStart.ToFen(szStartFen);
    if (strcmp(szStartFen, cszStartFen) == 0 && League.EccoApi.Available()) {
      if (lppgn->nMaxMove <= 20) {
        dwFileMove[lppgn->nMaxMove - 1] = Move2File(mv, posIrrev);
      }
      dwEccoIndex = League.EccoApi.EccoIndex((const char *) dwFileMove);
      strcpy(lppgn->szEcco, (const char *) &dwEccoIndex);
      strcpy(lppgn->szOpen, League.EccoApi.EccoOpening(dwEccoIndex));
      strcpy(lppgn->szVar, League.EccoApi.EccoVariation(dwEccoIndex));
    }
    // 然后走这个着法，并判断状态
    TryMove(posIrrev, nStatus, mv);
    if ((nStatus & MOVE_CAPTURE) != 0) {
      posIrrev.ToFen(szIrrevFen);
      posIrrev.SetIrrev();
    }
    nTimer[sd] += League.nIncrTime * League.nStandardCpuTime;
    if (nStatus < MOVE_MATE) {
      // 如果是正常着法，那么结果设为“进行中”
      nResult = 0;
    } else {
      // 如果是终止着法，那么根据状态判定结果
      szComment = new char[MAX_CHAR];
      lppgn->szCommentTable[lppgn->nMaxMove] = szComment;
      if (false) {
      } else if ((nStatus & MOVE_ILLEGAL) != 0 || (nStatus & MOVE_INCHECK) != 0) {
        sprintf(szComment, "%s走法违例", cszColorStr[sd]);
        nResult = 3 - sd * 2;
      } else if ((nStatus & MOVE_DRAW) != 0) {
        strcpy(szComment, "超过自然限着作和");
        nResult = 2;
      } else if ((nStatus & MOVE_PERPETUAL) != 0) {
        if ((nStatus & MOVE_PERPETUAL_WIN) != 0) {
          if ((nStatus & MOVE_PERPETUAL_LOSS) != 0) {
            strcpy(szComment, "双方不变作和");
            nResult = 2;
          } else {
            sprintf(szComment, "%s长打作负", cszColorStr[1 - sd]);
            nResult = 1 + sd * 2;
          }
        } else {
          if ((nStatus & MOVE_PERPETUAL_LOSS) != 0) {
            sprintf(szComment, "%s长打作负", cszColorStr[sd]);
            nResult = 3 - sd * 2;
          } else {
            strcpy(szComment, "双方不变作和");
            nResult = 2;
          }
        }
      } else { // MOVE_MATE
        sprintf(szComment, "%s胜", cszColorStr[sd]);
        nResult = 1 + sd * 2;
      }
    }
  }
  lppgn->nResult = nResult;
  // 交换走子方，其实"sd"和"posIrrev.sdPlayer"是同步的
  sd = 1 - sd;
}

const char *const cszGo = "go time %d increment %d opptime %d oppincrement %d";
const char *const cszGoDraw = "go draw time %d increment %d opptime %d oppincrement %d";

// 让引擎运行
void GameStruct::RunEngine(void) {
  char szLineStr[MAX_CHAR], szFileName[MAX_CHAR];
  char *lpLineChar;
  int i, nMoveNum, nBanMoves;
  int mvBanList[MAX_GEN_MOVES];
  MoveStruct mvs[MAX_GEN_MOVES];
  uint32_t dwMoveStr;
  FILE *fpOptionFile;

  if (!bStarted[sd]) {
    // 如果引擎尚未启动，那么启动引擎
    llTime = GetTime();
    LocatePath(szFileName, lpTeam[sd]->szEngineFile);
    pipe[sd].Open(szFileName);
    Send("ucci");
    // 发送"ucci"指令后，在10秒钟内等待"ucciok"反馈信息
    while ((int) (GetTime() - llTime) < 10000) {
      if (Receive(szLineStr)) {
        if (StrEqv(szLineStr, "option usemillisec ")) {
          bUseMilliSec[sd] = true;
        }
        if (StrEqv(szLineStr, "ucciok")) {
          break;
        }
      } else {
        Idle();
      }
    }
    // 设置必要的初始化选项
    if (League.bPromotion) {
      Send("setoption promotion true");
    } else {
      Send("setoption promotion false");
    }
    Send("setoption ponder false");
    Send("setoption newgame");
    if (bUseMilliSec[sd]) {
      Send("setoption usemillisec true");
    }
    // 把引擎选项文件的内容发送给引擎
    LocatePath(szFileName, lpTeam[sd]->szOptionFile);
    fpOptionFile = fopen(szFileName, "rt");
    if (fpOptionFile != NULL) {
      while (fgets(szLineStr, MAX_CHAR, fpOptionFile) != NULL) {
        lpLineChar = strchr(szLineStr, '\n');
        if (lpLineChar != NULL) {
          *lpLineChar = '\0';
        }
        Send(szLineStr);
      }
      fclose(fpOptionFile);
    }
    bStarted[sd] = true;
  }

  // 向引擎发送当前局面
  llTime = GetTime();
  lpLineChar = szLineStr;
  lpLineChar += sprintf(lpLineChar, "position fen %s - - 0 1", szIrrevFen);
  if (posIrrev.nMoveNum > 1) {
    lpLineChar += sprintf(lpLineChar, " moves");
    for (i = 1; i < posIrrev.nMoveNum; i ++) {
      dwMoveStr = MOVE_COORD(posIrrev.rbsList[i].mvs.wmv);
      lpLineChar += sprintf(lpLineChar, " %.4s", (const char *) &dwMoveStr);
    }
  }
  Send(szLineStr);

  // 向引擎发送禁着指令
  nBanMoves = 0;
  nMoveNum = posIrrev.GenAllMoves(mvs);
  for (i = 0; i < nMoveNum; i ++) {
    if (posIrrev.MakeMove(mvs[i].wmv)) {
      // 如果走了合理着法，但构成长打并达到两次重复，则该着法必须设为禁着
      // 注意：现在已经轮到对方走子了，所以"REP_WIN"才表示长打
      if (posIrrev.RepStatus(2) == REP_WIN) {
        mvBanList[nBanMoves] = mvs[i].wmv;
        nBanMoves ++;
      }
      posIrrev.UndoMakeMove();
    }
  }
  if (nBanMoves > 0) {
    lpLineChar = szLineStr;
    lpLineChar += sprintf(lpLineChar, "banmoves");
    for (i = 0; i < nBanMoves; i ++) {
      dwMoveStr = MOVE_COORD(mvBanList[i]);
      lpLineChar += sprintf(lpLineChar, " %.4s", (const char *) &dwMoveStr);
    }
    Send(szLineStr);
  }

  // 向引擎发送走棋指令："go [draw] time %d increment %d opptime %d oppincrement %d";
  if (lpTeam[sd]->szGoParam[0] != '\0') {
    strcpy(szLineStr, bDraw ? "go draw " : "go ");
    strcat(szLineStr, lpTeam[sd]->szGoParam);
  } else if (bUseMilliSec[sd]) {
    sprintf(szLineStr, bDraw ? cszGoDraw : cszGo, nTimer[sd],
        League.nIncrTime * League.nStandardCpuTime, nTimer[1 - sd], League.nIncrTime * League.nStandardCpuTime);
  } else {
    sprintf(szLineStr, bDraw ? cszGoDraw : cszGo, nTimer[sd] / 1000,
        League.nIncrTime * League.nStandardCpuTime / 1000, nTimer[1 - sd] / 1000,
        League.nIncrTime * League.nStandardCpuTime / 1000);
  }
  Send(szLineStr);
  bTimeout = false;
}

// 开始一个棋局
void GameStruct::BeginGame(int nRobin, int nRound, int nGame) {
  int i;
  const char *szStartFen;
  char szFileName[16];
  CheckStruct chkRecord;
  time_t dwTime;
  tm *lptm;

  Live.cResult[nRobin][nRound][nGame] = 0;
  PublishLeague();
  League.nRemainProcs --; // 将剩余可用处理器数量减一
  time(&dwTime);
  lptm = localtime(&dwTime); // 获得时间
  lpTeam[0] = TeamList + RobinTable[nRound][nGame][0];
  lpTeam[1] = TeamList + RobinTable[nRound][nGame][1];
  sd = nCounter = nResult = 0;
  nTimer[0] = nTimer[1] = League.nInitTime * League.nStandardCpuTime * 60;
  bStarted[0] = bStarted[1] = bUseMilliSec[0] = bUseMilliSec[1] = bDraw = false;
  szStartFen = League.szRobinFens[nRobin];
  strcpy(szIrrevFen, szStartFen[0] == '\0' ? cszStartFen : szStartFen);
  posIrrev.FromFen(szIrrevFen);
  sd = posIrrev.sdPlayer; // 让sd与posIrrev.sdPlayer同步

  // 合成棋谱文件
  lppgn = new PgnFileStruct();
  lppgn->posStart = posIrrev;
  sprintf(szGameFile, "%.3s-%.3s%c.PGN", (const char *) &lpTeam[0]->dwAbbr,
      (const char *) &lpTeam[1]->dwAbbr, cszRobinChar[nRobin]);
  strcpy(lppgn->szEvent, League.szEvent);
  sprintf(lppgn->szRound, "%d", nRobin * League.nRoundNum + nRound + 1);
  sprintf(lppgn->szDate, "%04d.%02d.%02d", lptm->tm_year + 1900, lptm->tm_mon + 1, lptm->tm_mday);
  strcpy(lppgn->szSite, League.szSite);
  strcpy(lppgn->szRed, lpTeam[0]->szEngineName);
  sprintf(lppgn->szRedElo, "%d", lpTeam[0]->nEloValue);
  strcpy(lppgn->szBlack, lpTeam[1]->szEngineName);
  sprintf(lppgn->szBlackElo, "%d", lpTeam[1]->nEloValue);
  for (i = 0; i < 20; i ++) {
    dwFileMove[i] = 0;
  }

  // 打开日志文件和进程文件
  sprintf(szFileName, "%.3s-%.3s%c.LOG", (const char *) &lpTeam[0]->dwAbbr,
      (const char *) &lpTeam[1]->dwAbbr, cszRobinChar[nRobin]);
  fpLogFile = fopen(szFileName, "at");
  if (fpLogFile == NULL) {
    printf("错误：无法建立日志文件\"%s\"!\n", szFileName);
    exit(EXIT_FAILURE);
  }
  sprintf(szFileName, "%.3s-%.3s%c.CHK", (const char *) &lpTeam[0]->dwAbbr,
      (const char *) &lpTeam[1]->dwAbbr, cszRobinChar[nRobin]);
  CheckFile.Open(szFileName);

  // 如果进程文件有记录，那么先解析进程记录的着法
  while (!CheckFile.Eof()) {
    chkRecord = CheckFile.Read();
    nTimer[sd] = chkRecord.nTimer;
    AddMove(chkRecord.mv);
    if (nResult > 0) {
      lppgn->Write(szGameFile);
      PublishGame(lppgn, szGameFile, FORCE_PUBLISH);
      League.nRemainProcs ++; // 在EndGame()之前就释放处理器资源，提高处理器利用率
      return; // 如果棋局结束(进程文件是完整的)，那么就不必启动引擎了
    }
  }
  lppgn->Write(szGameFile);
  PublishGame(lppgn, szGameFile);
  RunEngine(); // 进程文件读完后(进程文件不完整)，就需要调用引擎了
  // 向引擎发送指令后，棋局就挂起，等待下次调用"ResumeGame()"以继续进行
}

// 退出引擎
void GameStruct::QuitEngine(void) {
  char szLineStr[MAX_CHAR];
  for (sd = 0; sd < 2; sd ++) {
    if (bStarted[sd]) {
      llTime = GetTime();
      Send("quit");
      while ((int) (GetTime() - llTime) < 1000) {
        if (Receive(szLineStr)) {
          if (StrEqv(szLineStr, "bye")) {
            break;
          }
        } else {
          Idle();
        }
      }
      pipe[sd].Close();
    }
  }
  League.nRemainProcs ++; // 在EndGame()之前就释放处理器资源，提高处理器利用率
}

// 继续上次挂起的棋局
void GameStruct::ResumeGame(void) {
  char szLineStr[MAX_CHAR];
  CheckStruct chkRecord;
  char *lp;

  // 棋局尚未结束时才有操作
  if (nResult > 0) {
    return;
  }
  // 首先读取引擎反馈信息
  chkRecord.mv = BESTMOVE_THINKING;
  while (Receive(szLineStr)) {
    lp = szLineStr;
    if (StrEqvSkip(lp, "bestmove ")) {
      chkRecord.mv = COORD_MOVE(*(uint32_t *) lp);
      lp += 4;
      if (StrScan(lp, " resign")) {
        chkRecord.mv = BESTMOVE_RESIGN;
      } else {
        if (StrScan(lp, " draw")) {
          if (bDraw) {
            chkRecord.mv = BESTMOVE_DRAW;
          } else {
            bDraw = true;
          }
        } else {
          bDraw = false;
        };
      };
      break;
    } else if (StrEqv(lp, "nobestmove")) {
      chkRecord.mv = BESTMOVE_RESIGN;
      break;
    }
  }
  // 如果没有读到反馈着法，就判断引擎是否超时
  if (chkRecord.mv == BESTMOVE_THINKING) {
    if (bTimeout) {
      if ((int) (GetTime() - llTime) > nTimer[sd] + League.nStopTime) {
        chkRecord.mv = BESTMOVE_TIMEOUT; // 只有时间超出停止时间后，才给空着以表示超时
      }
    } else {
      if ((int) (GetTime() - llTime) > nTimer[sd]) {
        Send("stop");
        bTimeout = true;
      }
    }
  }
  // 如果有反馈着法(包括超时返回的空着)，就走这个着法
  if (chkRecord.mv != BESTMOVE_THINKING) {
    nTimer[sd] -= (int) (GetTime() - llTime);
    if (nTimer[sd] < 0) {
      nTimer[sd] = 0;
    }
    chkRecord.nTimer = nTimer[sd];
    CheckFile.Write(chkRecord);
    AddMove(chkRecord.mv);
    lppgn->Write(szGameFile);
    PublishGame(lppgn, szGameFile, nResult > 0);
    if (nResult == 0) {
      RunEngine(); // 如果棋局尚未结束，那么让引擎思考下一步棋
    } else {
      // 如果棋局已经结束，那么终止两个引擎
      QuitEngine();
    }
  }
}

const struct ResultStruct {
  int nHomeWin, nHomeDraw, nHomeLoss, nHomeScore, nAwayWin, nAwayDraw, nAwayLoss, nAwayScore;
  double dfWHome;
  const char *szResultStr;
} ResultList[4] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0.0, "       "},
  {1, 0, 0, 2, 0, 0, 1, 0, 1.0, "  1-0  "},
  {0, 1, 0, 1, 0, 1, 0, 1, 0.5, "1/2-1/2"},
  {0, 0, 1, 0, 1, 0, 0, 2, 0.0, "  0-1  "}
};

inline void PrintDup(int nChar, int nDup) {
  int i;
  for (i = 0; i < nDup; i ++) {
    putchar(nChar);
  }
}

// 终止一个棋局
bool GameStruct::EndGame(int nRobin, int nRound, int nGame) {
  double dfWeHome;
  const ResultStruct *lpResult;

  if (nResult == 0) {
    return false;
  }
  delete lppgn;
  fclose(fpLogFile);
  CheckFile.Close();
  // 如果棋局已经完成，那么计算成绩
  dfWeHome = 1.0 / (1.0 + pow(10.0, (double) (lpTeam[1]->nEloValue - lpTeam[0]->nEloValue) / 400.0));
  lpResult = ResultList + nResult;
  lpTeam[0]->nWin += lpResult->nHomeWin;
  lpTeam[0]->nDraw += lpResult->nHomeDraw;
  lpTeam[0]->nLoss += lpResult->nHomeLoss;
  lpTeam[0]->nScore += lpResult->nHomeScore;
  lpTeam[1]->nWin += lpResult->nAwayWin;
  lpTeam[1]->nDraw += lpResult->nAwayDraw;
  lpTeam[1]->nLoss += lpResult->nAwayLoss;
  lpTeam[1]->nScore += lpResult->nAwayScore;
  lpTeam[0]->nEloValue += (int) ((lpResult->dfWHome - dfWeHome) * lpTeam[0]->nKValue);
  lpTeam[1]->nEloValue += (int) ((dfWeHome - lpResult->dfWHome) * lpTeam[1]->nKValue);
  // 每轮的第一局棋显示轮次
  if (nGame == 0) {
    printf("第 %d 轮：\n\n", nRobin * League.nRoundNum + nRound + 1);
  }
  // 输出棋局结果
  printf("%s", lpTeam[0]->szEngineName);
  PrintDup(' ', League.nNameLen - strlen(lpTeam[0]->szEngineName));
  printf(" %s %s", lpResult->szResultStr, lpTeam[1]->szEngineName);
  PrintDup(' ', League.nNameLen - strlen(lpTeam[1]->szEngineName));
  printf(" (%.3s-%.3s%c.PGN)\n", (const char *) &lpTeam[0]->dwAbbr,
      (const char *) &lpTeam[1]->dwAbbr, cszRobinChar[nRobin]);
  fflush(stdout);
  // 整理直播
  Live.cResult[nRobin][nRound][nGame] = nResult;
  PublishLeague();
  return true;
}

// 中断一个棋局
void GameStruct::TerminateGame(void) {
  if (nResult == 0) {
    if (!bTimeout) {
      Send("stop");
    }
    QuitEngine();
  }
  delete lppgn;
  fclose(fpLogFile);
  CheckFile.Close();
}

// 输出排名表
static void PrintRankList(void) {
  int i, j, nLastRank, nLastScore;
  int nSortList[MAX_TEAM];
  TeamStruct *lpTeam;
  // 输出表头
  printf("   缩写 引擎名称");
  PrintDup(' ', League.nNameLen - 8);
  printf(" ELO  K   胜  和  负 积分\n");
  PrintDup('=', League.nNameLen - 8);
  printf("================" "==========================\n");
  for (i = 0; i < League.nTeamNum; i ++) {
    nSortList[i] = i;
  }
  // 用冒泡排序法按积分排序
  for (i = 0; i < League.nTeamNum - 1; i ++) {
    for (j = League.nTeamNum - 1; j > i; j --) {
      if (TeamList[nSortList[j - 1]].nScore < TeamList[nSortList[j]].nScore) {
        SWAP(nSortList[j - 1], nSortList[j]);
      }
    }
  }
  // 依次显示名次，如果积分和前一个队相同，那么名次也和前一个队相同
  nLastRank = nLastScore = 0;
  for (i = 0; i < League.nTeamNum; i ++) {
    lpTeam = TeamList + nSortList[i];
    if (lpTeam->nScore != nLastScore) {
      nLastRank = i;
      nLastScore = lpTeam->nScore;
    }
    printf("%2d %.3s  %s", nLastRank + 1, (const char *) &lpTeam->dwAbbr, lpTeam->szEngineName);
    PrintDup(' ', League.nNameLen - strlen(lpTeam->szEngineName));
    printf(" %4d %2d %3d %3d %3d %3d%s\n", lpTeam->nEloValue, lpTeam->nKValue, lpTeam->nWin,
        lpTeam->nDraw, lpTeam->nLoss, lpTeam->nScore / 2, lpTeam->nScore % 2 == 0 ? "" : ".5");
  }
  PrintDup('=', League.nNameLen - 8);      
  printf("================" "==========================\n\n");
  //     "   缩写 引擎名称" " ELO  K   胜  和  负 积分"
}

// 棋局队列占用大量空间，所以必须用全局变量
static GameStruct GameList[QUEUE_LEN];

// 捕获Ctrl-C和Ctrl-Break
static void signal_handler(int sig) {
  signal(sig, signal_handler);
  League.bRunning = false;
}

// 主例程
int main(void) {
  // 以下变量牵涉输入报告的读取
  char szLineStr[MAX_CHAR];
  char *lp;
  FILE *fpIniFile;
  TeamStruct *lpTeam;
  int i, j, k, nRobinFen, nSocket;
  int nEngineFileLen; // 引擎文件的最大长度
  // 以下变量牵涉到棋局队列的控制
  int nRobinPush, nRoundPush, nGamePush;
  int nRobinPop, nRoundPop, nGamePop;
  int nQueueBegin, nQueueEnd, nQueueIndex;

  // 首先是读取输入报告
  League.nTeamNum = League.nInitTime = League.nIncrTime = League.nStopTime = 0;
  League.nRemainProcs = League.nRobinNum = 1;
  League.nStandardCpuTime = 1000;
  League.nNameLen = nEngineFileLen = 8; // 引擎名称和引擎文件的最小长度
  League.bPromotion = false;
  League.szEvent[0] = League.szSite[0] = '\0';
  Live.szHost[0] = Live.szPath[0] = Live.szPassword[0] = Live.szCounter[0] = '\0';
  Live.szProxyHost[0] = Live.szProxyUser[0] = Live.szProxyPassword[0] = '\0';
  strcpy(Live.szExt, "htm");
  Live.nPort = Live.nProxyPort = 80;
  Live.nRefresh = Live.nInterval = 0;
  nRobinFen = 0;

  LocatePath(szLineStr, "UCCILEAG.INI");
  fpIniFile = fopen(szLineStr, "rt");
  if (fpIniFile == NULL) {
    printf("错误：无法打开配置文件\"%s\"！\n", szLineStr);
    return 0;
  }
  for (i = 0; i < MAX_ROBIN; i ++) {
    League.szRobinFens[i][0] = '\0';
  }
  while (fgets(szLineStr, MAX_CHAR, fpIniFile) != NULL) {
    StrCutCrLf(szLineStr);
    lp = szLineStr;
    if (false) {
    } else if (StrEqvSkip(lp, "Event=")) {
      strcpy(League.szEvent, lp);
    } else if (StrEqvSkip(lp, "Site=")) {
      strcpy(League.szSite, lp);
    } else if (StrEqvSkip(lp, "Roundrobins=")) {
      League.nRobinNum = Str2Digit(lp, 1, MAX_ROBIN);
    } else if (StrEqvSkip(lp, "Processors=")) {
      League.nRemainProcs = Str2Digit(lp, 1, MAX_PROCESSORS);
    } else if (StrEqvSkip(lp, "InitialTime=")) {
      League.nInitTime = Str2Digit(lp, 1, 500);
    } else if (StrEqvSkip(lp, "IncrementalTime=")) {
      League.nIncrTime = Str2Digit(lp, 0, 500);
    } else if (StrEqvSkip(lp, "StoppingTime=")) {
      League.nStopTime = Str2Digit(lp, 0, 500);
    } else if (StrEqvSkip(lp, "StandardCpuTime=")) {
      League.nStandardCpuTime = Str2Digit(lp, 100, 10000);
    } else if (StrEqvSkip(lp, "Promotion=")) {
      if (StrEqv(lp, "True")) {
        League.bPromotion = true;
      } else if (StrEqv(lp, "On")) {
        League.bPromotion = true;
      }
    // 3.8新功能：设定初始局面
    } else if (StrEqvSkip(lp, "Position=")) {
      if (nRobinFen < MAX_ROBIN) {
        strcpy(League.szRobinFens[nRobinFen], lp);
        nRobinFen ++;
      }
    } else if (StrEqvSkip(lp, "Team=")) {
      if (League.nTeamNum < MAX_TEAM) {
        lpTeam = TeamList + League.nTeamNum;
        lpTeam->dwAbbr = *(uint32_t *) lp;
        StrSplitSkip(lp, ',');
        StrSplitSkip(lp, ',', lpTeam->szEngineName);
        League.nNameLen = MAX(League.nNameLen, (int) strlen(lpTeam->szEngineName));
        lpTeam->nEloValue = Str2Digit(lp, 0, 9999);
        StrSplitSkip(lp, ',');        
        lpTeam->nKValue = Str2Digit(lp, 0, 99);
        StrSplitSkip(lp, ',');
        StrSplitSkip(lp, ',', lpTeam->szEngineFile);
        nEngineFileLen = MAX(nEngineFileLen, (int) strlen(lpTeam->szEngineFile));
        StrSplitSkip(lp, ',', lpTeam->szOptionFile);
        StrSplitSkip(lp, ',', lpTeam->szUrl);
        StrSplitSkip(lp, ',', lpTeam->szGoParam);
        League.nTeamNum ++;
      }
    // 以下参数只跟转播有关
    } else if (StrEqvSkip(lp, "Host=")) {
      strcpy(Live.szHost, lp);     // 直播主机
    } else if (StrEqvSkip(lp, "Path=")) {
      strcpy(Live.szPath, lp);     // 上传页面路径
    } else if (StrEqvSkip(lp, "Password=")) {
      strcpy(Live.szPassword, lp); // 上传页面口令
    } else if (StrEqvSkip(lp, "Extension=")) {
      strcpy(Live.szExt, lp);      // 上传文件后缀
    } else if (StrEqvSkip(lp, "Counter=")) {
      strcpy(Live.szCounter, lp);  // 计数器路径
    } else if (StrEqvSkip(lp, "Header=")) {
      strcpy(Live.szHeader, lp);   // 页眉路径
    } else if (StrEqvSkip(lp, "Footer=")) {
      strcpy(Live.szFooter, lp);   // 页脚路径
    } else if (StrEqvSkip(lp, "Port=")) {
      Live.nPort = Str2Digit(lp, 1, 65535);      // HTTP端口
    } else if (StrEqvSkip(lp, "Refresh=")) {
      Live.nRefresh = Str2Digit(lp, 0, 60);      // 页面自动刷新时间(秒)
    } else if (StrEqvSkip(lp, "Interval=")) {
      Live.nInterval = Str2Digit(lp, 0, 60000);  // 上传文件间隔时间(毫秒)
    } else if (StrEqvSkip(lp, "ProxyHost=")) {
      strcpy(Live.szProxyHost, lp);              // 代理主机
    } else if (StrEqvSkip(lp, "ProxyPort=")) {
      Live.nProxyPort = Str2Digit(lp, 1, 65535); // 代理端口
    } else if (StrEqvSkip(lp, "ProxyUser=")) {
      strcpy(Live.szProxyUser, lp);              // 代理用户名
      Live.szProxyUser[1024] = '\0';
    } else if (StrEqvSkip(lp, "ProxyPassword=")) {
      strcpy(Live.szProxyPassword, lp);          // 代理口令
      Live.szProxyPassword[1024] = '\0';
    }
  }
  fclose(fpIniFile);
  if (League.nTeamNum < 2) {
    printf("错误：至少需要两个参赛队！\n");
    return 0;
  }
  printf("#======================#\n");
  printf("$ UCCI引擎联赛输出报告 $\n");
  printf("#======================#\n\n");
  printf("赛事：　　%s\n", League.szEvent);
  printf("地点：　　%s\n", League.szSite);
  printf("参赛队数：%d\n", League.nTeamNum);
  printf("处理器数：%d\n", League.nRemainProcs);
  printf("循环次数：%d\n", League.nRobinNum);
  printf("初始时间：%-4d 分钟\n", League.nInitTime);
  printf("每步加时：%-4d 秒\n", League.nIncrTime);
  printf("停止时间：%-4d 毫秒\n", League.nStopTime);
  printf("换算比例：%-4d 毫秒\n", League.nStandardCpuTime);
  if (League.bPromotion) {
    printf("规则：　　允许仕(士)相(象)升变成兵(卒)\n");
  }
  printf("模拟器：　UCCI引擎联赛模拟器 3.8\n\n");
  printf("参赛引擎：\n\n");
  printf("   缩写 引擎名称");
  PrintDup(' ', League.nNameLen - 8);
  printf(" ELO  K  引擎程序");
  PrintDup(' ', nEngineFileLen - 8);
  printf(" 配置文件\n");
  PrintDup('=', League.nNameLen + nEngineFileLen - 16);
  printf("================" "=================" "============\n");
  for (i = 0; i < League.nTeamNum; i ++) {
    lpTeam = TeamList + i;
    printf("%2d %.3s  %s", i + 1, (const char *) &lpTeam->dwAbbr, lpTeam->szEngineName);
    PrintDup(' ', League.nNameLen - strlen(lpTeam->szEngineName));
    printf(" %4d %2d %s", lpTeam->nEloValue, lpTeam->nKValue, lpTeam->szEngineFile);
    PrintDup(' ', nEngineFileLen - strlen(lpTeam->szEngineFile));
    printf(" %s\n", lpTeam->szOptionFile);
  }
  PrintDup('=', League.nNameLen + nEngineFileLen - 16);
  printf("================" "=================" "============\n\n");
  //     "   缩写 引擎名称" " ELO  K  引擎文件" " 配置文件"

  // 接下来生成循环赛对阵表，可参阅：http://www.xqbase.com/protocol/roundrobin.htm
  League.nGameNum = (League.nTeamNum + 1) / 2;
  League.nRoundNum = League.nGameNum * 2 - 1;
  for (i = 0; i < League.nGameNum; i ++) {
    RobinTable[0][i][0] = i;
    RobinTable[0][i][1] = League.nGameNum * 2 - 1 - i;
  }
  for (i = 1; i < League.nRoundNum; i ++) {
    RobinTable[i][0][1] = League.nGameNum * 2 - 1;
    for (j = 0; j < League.nGameNum - 1; j ++) {
      RobinTable[i][j][0] = RobinTable[i - 1][League.nGameNum - 1 - j][1];
      RobinTable[i][j + 1][1] = RobinTable[i - 1][League.nGameNum - 1 - j][0];
    }
    RobinTable[i][League.nGameNum - 1][0] = RobinTable[i - 1][0][0];
  }
  if (League.nTeamNum % 2 == 0) {
    for (i = 0; i < League.nRoundNum; i ++) {
      if (i % 2 != 0) {
        SWAP(RobinTable[i][0][0], RobinTable[i][0][1]);
      }
    }
  } else {
    League.nGameNum --;
    for (i = 0; i < League.nRoundNum; i ++) {
      for (j = 0; j < League.nGameNum; j ++) {
        RobinTable[i][j][0] = RobinTable[i][j + 1][0];
        RobinTable[i][j][1] = RobinTable[i][j + 1][1];
      }
    }
  }
  for (i = 0; i < League.nRoundNum; i ++) {
    for (j = 0; j < League.nGameNum; j ++) {
      RobinTable[League.nRoundNum + i][j][0] = RobinTable[i][j][1];
      RobinTable[League.nRoundNum + i][j][1] = RobinTable[i][j][0];
    }
  }
  League.nRoundNum *= 2;
  printf("赛程表：\n\n");
  printf("轮次 对局\n");
  printf("=====");
  for (i = 0; i < League.nGameNum; i ++) {
    printf("========");
  }
  printf("\n");
  for (i = 0; i < League.nRobinNum; i ++) {
    for (j = 0; j < League.nRoundNum; j ++) {
      printf("%3d ", i * League.nRoundNum + j + 1);
      for (k = 0; k < League.nGameNum; k ++) {
        printf(" %.3s-%.3s", (const char *) &TeamList[(int) RobinTable[j][k][0]].dwAbbr,
            (const char *) &TeamList[(int) RobinTable[j][k][1]].dwAbbr);
        Live.cResult[i][j][k] = -1;
      }
      printf("\n");
    }
  }
  printf("=====");
  for (i = 0; i < League.nGameNum; i ++) {
    printf("========");
  }
  printf("\n\n");

  // 初始化ECCO解析函数
  LocatePath(szLineStr, cszLibEccoFile);
  League.EccoApi.Startup(szLineStr);

  // 测试直播页面
  WSBStartup();
  nSocket = (Live.szProxyHost[0] == '\0' ? INVALID_SOCKET : WSBConnect(Live.szProxyHost, Live.nProxyPort));
  if (nSocket == INVALID_SOCKET) {
    Live.nProxyPort = 0;
    nSocket = (Live.szHost[0] == '\0' ? INVALID_SOCKET : WSBConnect(Live.szHost, Live.nPort));
    if (nSocket == INVALID_SOCKET) {
      Live.nPort = 0;
    } else {
      WSBDisconnect(nSocket);
    }
  } else {
    WSBDisconnect(nSocket);
  }
  Live.llTime = GetTime();

  // 捕获Ctrl-C和Ctrl-Break
  League.bRunning = true;
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler); 
#ifdef SIGBREAK
  signal(SIGBREAK, signal_handler);
#endif

  // 现在开始控制棋局队列，这是本模拟器的核心部分
  printf("=== 联赛进程开始 ===\n\n");
  fflush(stdout);
  PreGenInit();
  ChineseInit();
  PreEval.bPromotion = League.bPromotion;
  nRobinPush = nRoundPush = nGamePush = 0; // 压入队列的循环、轮次和棋局序号
  nRobinPop = nRoundPop = nGamePop = 0;    // 弹出队列的循环、轮次和棋局序号
  nQueueBegin = nQueueEnd = 0;             // 队列出口和队列入口
  while (League.bRunning && nRobinPop < League.nRobinNum) {
    // 把一个棋局压入队列的条件是：(1) 所有比赛完成，(2) 有剩余的处理器，(3) 队列未被填满
    if (nRobinPush < League.nRobinNum && League.nRemainProcs > 0 && (nQueueEnd + 1) % QUEUE_LEN != nQueueBegin) {
      GameList[nQueueEnd].BeginGame(nRobinPush, nRoundPush, nGamePush);
      nQueueEnd = (nQueueEnd + 1) % QUEUE_LEN;
      // 已把一个棋局压入队列，修改循环、轮次和棋局序号
      nGamePush ++;
      if (nGamePush == League.nGameNum) {
        nGamePush = 0;
        nRoundPush ++;
        if (nRoundPush == League.nRoundNum) {
          nRoundPush = 0;
          nRobinPush ++;
        }
      }
    }

    // 调度队列中的每个棋局，相当于用轮转方式管理多个进程
    nQueueIndex = nQueueBegin;
    while (nQueueIndex != nQueueEnd) {
      GameList[nQueueIndex].ResumeGame();
      nQueueIndex = (nQueueIndex + 1) % QUEUE_LEN;
    }

    // 如果队列不是空的，那么尝试将棋局弹出队列
    if (nQueueBegin != nQueueEnd) {
      if (GameList[nQueueBegin].EndGame(nRobinPop, nRoundPop, nGamePop)) {
        nQueueBegin = (nQueueBegin + 1) % QUEUE_LEN;
        // 已把一个棋局弹出队列，修改循环、轮次和棋局序号
        nGamePop ++;
        if (nGamePop == League.nGameNum) {
          // 如果一轮结束，那么输出排名表
          printf("\n");
          printf("第 %d 轮后排名：\n\n", nRobinPop * League.nRoundNum + nRoundPop + 1);
          PrintRankList();
          fflush(stdout);
          nGamePop = 0;
          nRoundPop ++;
          if (nRoundPop == League.nRoundNum) {
            nRoundPop = 0;
            nRobinPop ++;
          }
        }
      }
    }
    Idle();
  }

  // 如果队列不是空的，那么尝试将棋局中断
  nQueueIndex = nQueueBegin;
  while (nQueueIndex != nQueueEnd) {
    GameList[nQueueIndex].TerminateGame();
    nQueueIndex = (nQueueIndex + 1) % QUEUE_LEN;
  }

  printf("=== 联赛进程结束 ===\n\n");
  printf("最终排名：\n\n");
  PrintRankList();

  WSBCleanup();
  League.EccoApi.Shutdown();
  return 0;
}
