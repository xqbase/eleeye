/* 
Test-Position Maker - for UCCI-Engines
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
#ifdef _WIN32
  #include <windows.h>
#else
  #include <dirent.h>
#endif
#include "../base/base2.h"
#include "../base/parse.h"
#include "../eleeye/position.h"
#include "../cchess/cchess.h"
#include "../cchess/pgnfile.h"

const int MAX_CHAR = 1024;
const int MAX_PLAYER = 16;

static struct {
  bool bWinMove, bDrawMove, bLossMove, bUnknownMove;
  int nPlayerNum, nSkipHead, nSkipTail;
  char szPlayerList[MAX_PLAYER][MAX_CHAR];
  FILE *fpOutput;
} MakeTest;

static bool InPlayerList(const char *szPlayer) {
  int i;
  if (MakeTest.nPlayerNum == 0) {
    return true;
  }
  for (i = 0; i < MakeTest.nPlayerNum; i ++) {
    if (StrEqv(szPlayer, MakeTest.szPlayerList[i])) {
      return true;
    }
  }
  return false;
}

static void BuildTestFromFile(const char *szFilePath) {
  int i, mv;
  bool bOutput[2];
  uint32_t dwMoveStr;
  char szFen[128];
  char szFileName[MAX_CHAR];
  char *szResult, *lpSeparator;
  PgnFileStruct pgn;

  if (!pgn.Read(szFilePath)) {
    return;
  }
  if (pgn.nMaxMove <= MakeTest.nSkipHead + MakeTest.nSkipTail) {
    return;
  }
  switch (pgn.nResult) {
  case 1:
    bOutput[0] = MakeTest.bWinMove;
    bOutput[1] = MakeTest.bLossMove;
    szResult = "1-0";
    break;
  case 2:
    bOutput[0] = MakeTest.bDrawMove;
    bOutput[1] = MakeTest.bDrawMove;
    szResult = "1/2-1/2";
    break;
  case 3:
    bOutput[0] = MakeTest.bLossMove;
    bOutput[1] = MakeTest.bWinMove;
    szResult = "0-1";
    break;
  default:
    bOutput[0] = MakeTest.bUnknownMove;
    bOutput[1] = MakeTest.bUnknownMove;
    szResult = "?-?";
    break;
  }
  bOutput[0] = bOutput[0] && InPlayerList(pgn.szRed);
  bOutput[1] = bOutput[1] && InPlayerList(pgn.szBlack);
  if (!bOutput[0] && !bOutput[1]) {
    return;
  }
  lpSeparator = strrchr(szFilePath, PATH_SEPARATOR);
  if (lpSeparator == NULL) {
    strcpy(szFileName, szFilePath);
  } else {
    strcpy(szFileName, lpSeparator + 1);
  }
  fprintf(MakeTest.fpOutput, "; %s: %s%s %s %s%s\n", szFileName,
      bOutput[0] ? "-> " : "", pgn.szRed, szResult, pgn.szBlack, bOutput[1] ? " <-" : "");
  for (i = 0; i < MakeTest.nSkipHead; i ++) {
    mv = pgn.wmvMoveTable[i + 1];
    if (pgn.posStart.ucpcSquares[DST(mv)] == 0) {
      pgn.posStart.MakeMove(mv);
    } else {
      pgn.posStart.MakeMove(mv);
      pgn.posStart.SetIrrev();
    }
  }
  for (i = MakeTest.nSkipHead; i < pgn.nMaxMove - MakeTest.nSkipTail; i ++) {
    mv = pgn.wmvMoveTable[i + 1];
    if (bOutput[pgn.posStart.sdPlayer]) {
      dwMoveStr = MOVE_COORD(mv);
      pgn.posStart.ToFen(szFen);
      fprintf(MakeTest.fpOutput, "%.4s %s\n", (const char *) &dwMoveStr, szFen);
    }
    if (pgn.posStart.ucpcSquares[DST(mv)] == 0) {
      pgn.posStart.MakeMove(mv);
    } else {
      pgn.posStart.MakeMove(mv);
      pgn.posStart.SetIrrev();
    }
  }
}

#ifdef _WIN32

static void SearchFolder(const char *szFolderPath);

static void SearchFile(const char *szFilePath, const WIN32_FIND_DATA &wfd) {
  if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
    if (strlen(szFilePath) > 4) {
      if (strnicmp(szFilePath + strlen(szFilePath) - 4, ".PGN", 4) == 0) {
        BuildTestFromFile(szFilePath);
      }
    }
  } else {
    if (strcmp(wfd.cFileName, ".") != 0 && strcmp(wfd.cFileName, "..") != 0) {
      SearchFolder(szFilePath);
    }
  }
}

static void SearchFolder(const char *szFolderPath) {
  char szFilePath[MAX_CHAR];
  WIN32_FIND_DATA wfd;
  HANDLE hFind;
  char *lpFilePath;

  strcpy(szFilePath, szFolderPath);
  lpFilePath = szFilePath + strlen(szFolderPath);
  if (*(lpFilePath - 1) == '\\') {
    lpFilePath --;
  }
  strcpy(lpFilePath, "\\*");
  lpFilePath ++;
  hFind = FindFirstFile(szFilePath, &wfd);
  if (hFind != INVALID_HANDLE_VALUE) {
    strcpy(lpFilePath, wfd.cFileName);
    SearchFile(szFilePath, wfd);
    while (FindNextFile(hFind, &wfd)) {
      strcpy(lpFilePath, wfd.cFileName);
      SearchFile(szFilePath, wfd);
    }
  }
}

#else

static void SearchFolder(const char *szFolderPath);

static void SearchFile(const char *szFilePath, const dirent *lpdir) {
  if (false) {
  } else if (lpdir->d_type == DT_REG) {
    if (strlen(szFilePath) > 4) {
      if (strncasecmp(szFilePath + strlen(szFilePath) - 4, ".PGN", 4) == 0) {
        BuildTestFromFile(szFilePath);
      }
    }
  } else if (lpdir->d_type == DT_DIR) {
    if (strcmp(lpdir->d_name, ".") != 0 && strcmp(lpdir->d_name, "..") != 0) {
      SearchFolder(szFilePath);
    }
  }
}

static void SearchFolder(const char *szFolderPath) {
  char szFilePath[MAX_CHAR];
  DIR *dp;
  dirent *lpdir;
  char *lpFilePath;

  strcpy(szFilePath, szFolderPath);
  lpFilePath = szFilePath + strlen(szFolderPath);
  if (*(lpFilePath - 1) != '/') {
    strcpy(lpFilePath, "/");
    lpFilePath ++;
  }  
  dp = opendir(szFilePath);
  if (dp != NULL) {
    while ((lpdir = readdir(dp)) != NULL) {
      strcpy(lpFilePath, lpdir->d_name);
      SearchFile(szFilePath, lpdir);
    }
    closedir(dp);
  }
}

#endif

int main(void) {
  char szIniFile[MAX_CHAR], szLineStr[MAX_CHAR];
  char szOutput[MAX_CHAR], szFolder[MAX_CHAR];
  char *lp;
  FILE *fpIniFile;

  LocatePath(szIniFile, "MAKETEST.INI");
  fpIniFile = fopen(szIniFile, "rt");
  if (fpIniFile == NULL) {
    printf("%s: File Opening Error!\n", szIniFile);
    return 0;
  }
  MakeTest.bUnknownMove = MakeTest.bWinMove = MakeTest.bDrawMove = MakeTest.bLossMove = false;
  MakeTest.nPlayerNum = MakeTest.nSkipHead = MakeTest.nSkipTail = 0;
  MakeTest.fpOutput = stdout;
  szOutput[0] = '\0';
  strcpy(szFolder, ".");
  while (fgets(szLineStr, MAX_CHAR, fpIniFile) != NULL) {
    StrCutCrLf(szLineStr);
    lp = szLineStr;
    if (false) {
    } else if (StrEqvSkip(lp, "WinMove=On")) {
      MakeTest.bWinMove = true;
    } else if (StrEqvSkip(lp, "WinMove=True")) {
      MakeTest.bWinMove = true;
    } else if (StrEqvSkip(lp, "DrawMove=On")) {
      MakeTest.bDrawMove = true;
    } else if (StrEqvSkip(lp, "DrawMove=True")) {
      MakeTest.bDrawMove = true;
    } else if (StrEqvSkip(lp, "LossMove=On")) {
      MakeTest.bLossMove = true;
    } else if (StrEqvSkip(lp, "LossMove=True")) {
      MakeTest.bLossMove = true;
    } else if (StrEqvSkip(lp, "UnknownMove=On")) {
      MakeTest.bUnknownMove = true;
    } else if (StrEqvSkip(lp, "UnknownMove=True")) {
      MakeTest.bUnknownMove = true;

    } else if (StrEqvSkip(lp, "Player=")) {
      if (MakeTest.nPlayerNum < MAX_PLAYER) {
        strcpy(MakeTest.szPlayerList[MakeTest.nPlayerNum], lp);
        MakeTest.nPlayerNum ++;
      }
    } else if (StrEqvSkip(lp, "SkipHead=")) {
      MakeTest.nSkipHead = Str2Digit(lp, 0, 100);
    } else if (StrEqvSkip(lp, "SkipTail=")) {
      MakeTest.nSkipTail = Str2Digit(lp, 0, 100);
    } else if (StrEqvSkip(lp, "Output=")) {
      LocatePath(szOutput, lp);
    } else if (StrEqvSkip(lp, "Folder=")) {
      LocatePath(szFolder, lp);
    }
  }
  fclose(fpIniFile);

  PreGenInit();
  ChineseInit();
  if (szOutput[0] != '\0') {
    MakeTest.fpOutput = fopen(szOutput, "wt");
    if (MakeTest.fpOutput == NULL) {
      MakeTest.fpOutput = stdout;
    }
  }
  SearchFolder(szFolder);
  if (MakeTest.fpOutput != stdout) {
    fclose(MakeTest.fpOutput);
  }
  return 0;
}
