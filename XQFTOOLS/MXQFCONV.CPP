/* 
MXQ/XQF/CHE/CHN/CCM->PGN Convertor - a Chinese Chess Score Convertion Program
Designed by Morning Yellow, Version: 3.15, Last Modified: Oct. 2009
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

int Xqf2Pgn(const char *szXqfFile, const char *szPgnFile, const EccoApiStruct &EccoApi);
int Mxq2Pgn(const char *szMxqFile, const char *szPgnFile, const EccoApiStruct &EccoApi);
int Che2Pgn(const char *szCheFile, const char *szPgnFile, const EccoApiStruct &EccoApi);
int Chn2Pgn(const char *szChnFile, const char *szPgnFile, const EccoApiStruct &EccoApi);
int Ccm2Pgn(const char *szCcmFile, const char *szPgnFile, const EccoApiStruct &EccoApi);
int Pgn2Xqf(const char *szPgnFile, const char *szXqfFile);

#ifdef MXQFCONV_DLL

static EccoApiStruct EccoApi;

extern "C" __declspec(dllexport) VOID WINAPI ConvInit(BOOL bTrad) {
  char szLibEccoFile[MAX_PATH];
  PreGenInit();
  ChineseInit(bTrad != FALSE);
  LocatePath(szLibEccoFile, cszLibEccoFile);
  EccoApi.Startup(szLibEccoFile, bTrad != FALSE);
}

extern "C" __declspec(dllexport) VOID WINAPI ConvXqf2Pgn(LPCSTR szXqfFile, LPCSTR szPgnFile) {
  Xqf2Pgn(szXqfFile, szPgnFile, EccoApi);
}

extern "C" __declspec(dllexport) VOID WINAPI ConvMxq2Pgn(LPCSTR szMxqFile, LPCSTR szPgnFile) {
  Mxq2Pgn(szMxqFile, szPgnFile, EccoApi);
}

extern "C" __declspec(dllexport) VOID WINAPI ConvChe2Pgn(LPCSTR szCheFile, LPCSTR szPgnFile) {
  Che2Pgn(szCheFile, szPgnFile, EccoApi);
}

extern "C" __declspec(dllexport) VOID WINAPI ConvChn2Pgn(LPCSTR szChnFile, LPCSTR szPgnFile) {
  Chn2Pgn(szChnFile, szPgnFile, EccoApi);
}

extern "C" __declspec(dllexport) VOID WINAPI ConvCcm2Pgn(LPCSTR szCcmFile, LPCSTR szPgnFile) {
  Ccm2Pgn(szCcmFile, szPgnFile, EccoApi);
}

extern "C" __declspec(dllexport) VOID WINAPI ConvPgn2Xqf(LPCSTR szPgnFile, LPCSTR szXqfFile) {
  Pgn2Xqf(szPgnFile, szXqfFile);
}

#else

static void SearchFolder(const char *szFolderPath, const EccoApiStruct &EccoApi);

static void SearchFile(const char *szFilePath, const WIN32_FIND_DATA &wfd, const EccoApiStruct &EccoApi) {
  int nPathLen;
  char szPgnFile[1024];
  if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
    nPathLen = strlen(szFilePath) - 4;
    if (nPathLen > 0) {
      if (false) {
      } else if (strnicmp(szFilePath + nPathLen, ".XQF", 4) == 0) {
        strncpy(szPgnFile, szFilePath, nPathLen);
        strcpy(szPgnFile + nPathLen, ".PGN");
        Xqf2Pgn(szFilePath, szPgnFile, EccoApi);
      } else if (strnicmp(szFilePath + nPathLen, ".MXQ", 4) == 0) {
        strncpy(szPgnFile, szFilePath, nPathLen);
        strcpy(szPgnFile + nPathLen, ".PGN");
        Mxq2Pgn(szFilePath, szPgnFile, EccoApi);
      } else if (strnicmp(szFilePath + nPathLen, ".CHE", 4) == 0) {
        strncpy(szPgnFile, szFilePath, nPathLen);
        strcpy(szPgnFile + nPathLen, ".PGN");
        Che2Pgn(szFilePath, szPgnFile, EccoApi);
      } else if (strnicmp(szFilePath + nPathLen, ".CHN", 4) == 0) {
        strncpy(szPgnFile, szFilePath, nPathLen);
        strcpy(szPgnFile + nPathLen, ".PGN");
        Chn2Pgn(szFilePath, szPgnFile, EccoApi);
      } else if (strnicmp(szFilePath + nPathLen, ".CCM", 4) == 0) {
        strncpy(szPgnFile, szFilePath, nPathLen);
        strcpy(szPgnFile + nPathLen, ".PGN");
        Ccm2Pgn(szFilePath, szPgnFile, EccoApi);
      }
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

  bi.hwndOwner = NULL;
  bi.pidlRoot = NULL;
  bi.pszDisplayName = NULL;
  bi.lpszTitle = "请选择象棋演播室棋谱(*.XQF)、弈天棋谱(*.MXQ)、QQ象棋棋谱(*.CHE)、联众象棋棋谱(*.CHN)和中游象棋棋谱(*.CCM)所在的文件夹";
  bi.ulFlags = BIF_RETURNONLYFSDIRS;
  bi.lpfn = NULL;
  bi.lParam = NULL;
  bi.iImage = 0;
  pidl = SHBrowseForFolder(&bi);
  if (SHGetPathFromIDList(pidl, szPath)) {
    PreGenInit();
    ChineseInit();
    LocatePath(szLibEccoFile, cszLibEccoFile);
    EccoApi.Startup(szLibEccoFile);
    SearchFolder(szPath, EccoApi);
    EccoApi.Shutdown();
    MessageBox(NULL, "象棋演播室棋谱(*.XQF)、弈天棋谱(*.MXQ)、QQ象棋棋谱(*.CHE)、联众象棋棋谱(*.CHN)和中游象棋棋谱(*.CCM)已全部转换为可移植棋谱(*.PGN)。",
        "象棋棋谱批量转换工具", MB_ICONINFORMATION);
  }
  return 0;
}

#endif