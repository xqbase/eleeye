/* 
Add ECCO - Adds ECCO Opening & Variation to PGN Files
Designed by Morning Yellow, Version: 3.14, Last Modified: Jun. 2008
Copyright (C) 2004-2008 www.elephantbase.net

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
#include <windows.h>
#include <shlobj.h>
#include "../base/base2.h"
#include "../eleeye/pregen.h"
#include "../cchess/cchess.h"
#include "../cchess/ecco.h"
#include "../cchess/pgnfile.h"

static void AddEcco(const char *szPgnFile, const EccoApiStruct &EccoApi) {
  int i, nStatus;
  uint32_t dwEccoIndex, dwFileMove[20];
  PgnFileStruct pgn;
  PositionStruct pos;

  if (pgn.Read(szPgnFile, NO_ADVERT)) {
    pos.FromFen(cszStartFen);
    for (i = 1; i <= MIN(pgn.nMaxMove, 20); i ++) {
      dwFileMove[i - 1] = Move2File(pgn.wmvMoveTable[i], pos);
      TryMove(pos, nStatus, pgn.wmvMoveTable[i]);
    }
    if (pgn.nMaxMove < 20) {
      dwFileMove[pgn.nMaxMove] = 0;
    }
    dwEccoIndex = EccoApi.EccoIndex((const char *) dwFileMove);
    strcpy(pgn.szEcco, (const char *) &dwEccoIndex);
    strcpy(pgn.szOpen, EccoApi.EccoOpening(dwEccoIndex));
    strcpy(pgn.szVar, EccoApi.EccoVariation(dwEccoIndex));
    pgn.Write(szPgnFile);
  }
}

static void SearchFolder(const char *szFolderPath, const EccoApiStruct &EccoApi);

static void SearchFile(const char *szFilePath, const WIN32_FIND_DATA &wfd, const EccoApiStruct &EccoApi) {
  int nPathLen;
  if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
    nPathLen = strlen(szFilePath) - 4;
    if (nPathLen > 0 && strnicmp(szFilePath + nPathLen, ".PGN", 4) == 0) {
      AddEcco(szFilePath, EccoApi);
    }
  } else {
    if (strcmp(wfd.cFileName, ".") != 0 && strcmp(wfd.cFileName, "..") != 0) {
      SearchFolder(szFilePath, EccoApi);
    }
  }
}

static void SearchFolder(const char *szFolderPath, const EccoApiStruct &EccoApi) {
  char szFilePath[1024];
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
    SearchFile(szFilePath, wfd, EccoApi);
    while (FindNextFile(hFind, &wfd)) {
      strcpy(lpFilePath, wfd.cFileName);
      SearchFile(szFilePath, wfd, EccoApi);
    }
  }
  FindClose(hFind);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  char szPath[MAX_PATH], szLibEccoFile[MAX_PATH];
  EccoApiStruct EccoApi;
  BROWSEINFO bi;
  LPITEMIDLIST pidl;

  LocatePath(szLibEccoFile, cszLibEccoFile);
  if (!EccoApi.Startup(szLibEccoFile)) {
    MessageBox(NULL, "没有找到ECCO.DLL！", "ECCO开局信息整理工具", MB_ICONEXCLAMATION);
    return 0;
  }
  bi.hwndOwner = NULL;
  bi.pidlRoot = NULL;
  bi.pszDisplayName = NULL;
  bi.lpszTitle = "请选择可移植棋谱(*.PGN)所在的文件夹";
  bi.ulFlags = BIF_RETURNONLYFSDIRS;
  bi.lpfn = NULL;
  bi.lParam = NULL;
  bi.iImage = 0;
  pidl = SHBrowseForFolder(&bi);
  if (SHGetPathFromIDList(pidl, szPath)) {
    PreGenInit();
    ChineseInit();
    SearchFolder(szPath, EccoApi);
    MessageBox(NULL, "全部棋谱已加入ECCO开局信息。", "ECCO开局信息整理工具", MB_ICONINFORMATION);
  }
  EccoApi.Shutdown();
  return 0;
}