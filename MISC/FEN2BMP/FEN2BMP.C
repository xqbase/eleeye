/*
FEN->BMP Convertor - a Bitmap Generation Program for Chess and Chinese Chess
Designed by Morning Yellow, Version: 1.1, Last Modified: Feb. 2006
Copyright (C) 2004-2006 www.elephantbase.net

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

static int FenPiece(int nArg) {
  switch (nArg) {
  case 'K':
    return 1;
  case 'Q':
  case 'A':
    return 2;
  case 'B':
  case 'E':
    return 3;
  case 'N':
  case 'H':
    return 4;
  case 'R':
    return 5;
  case 'C':
    return 6;
  case 'P':
    return 7;
  default:
    return 0;
  }
}

static const char *cszWWPiece[8] = {NULL, "WWK.BMP", "WWQ.BMP", "WWB.BMP", "WWN.BMP", "WWR.BMP", "WWP.BMP", "WWP.BMP"};
static const char *cszWBPiece[8] = {NULL, "WBK.BMP", "WBQ.BMP", "WBB.BMP", "WBN.BMP", "WBR.BMP", "WBP.BMP", "WBP.BMP"};
static const char *cszBWPiece[8] = {NULL, "BWK.BMP", "BWQ.BMP", "BWB.BMP", "BWN.BMP", "BWR.BMP", "BWP.BMP", "BWP.BMP"};
static const char *cszBBPiece[8] = {NULL, "BBK.BMP", "BBQ.BMP", "BBB.BMP", "BBN.BMP", "BBR.BMP", "BBP.BMP", "BBP.BMP"};
static const char *cszBWPieceMono[8] = {NULL, "BWKM.BMP", "BWQM.BMP", "BWBM.BMP", "BWNM.BMP", "BWRM.BMP", "BWPM.BMP", "BWPM.BMP"};
static const char *cszBBPieceMono[8] = {NULL, "BBKM.BMP", "BBQM.BMP", "BBBM.BMP", "BBNM.BMP", "BBRM.BMP", "BBPM.BMP", "BBPM.BMP"};
static const char *cszCRPiece[8] = {NULL, "CRK.BMP", "CRA.BMP", "CRB.BMP", "CRN.BMP", "CRR.BMP", "CRC.BMP", "CRP.BMP"};
static const char *cszCRPieceMono[8] = {NULL, "CRKM.BMP", "CRAM.BMP", "CRBM.BMP", "CRNM.BMP", "CRRM.BMP", "CRCM.BMP", "CRPM.BMP"};
static const char *cszCBPiece[8] = {NULL, "CBK.BMP", "CBA.BMP", "CBB.BMP", "CBN.BMP", "CBR.BMP", "CBC.BMP", "CBP.BMP"};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  int i, j, k;
  BOOL bMono;
  char *lpChar, *lpBmpFileName;
  int nBoard[10][9];
  char szBmpFileName[1024];
  char szBuffer[1024];
  DWORD dwBytesAccessed;
  HANDLE hFenFile, hOutFile, hBoardFile, hPieceFile;
  // Init Proc:
  if (lpCmdLine == NULL || lpCmdLine[0] == '\0') {
    MessageBox(NULL, "=== FEN->BMP Convertor ===\nUsage: FEN2BMP FEN-File", NULL, MB_ICONEXCLAMATION);
    return 0;
  }
  strcpy(szBuffer, lpCmdLine + (lpCmdLine[0] == '\"' ? 1 : 0));
  lpChar = strchr(szBuffer, '\"');
  if (lpChar != NULL) {
    *lpChar = '\0';
  }
  hFenFile = CreateFile(szBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFenFile == INVALID_HANDLE_VALUE) {
    MessageBox(NULL, szBuffer, NULL, MB_ICONEXCLAMATION);
    strcpy(szBuffer, "Unable to Open File: ");
    strcat(szBuffer, lpCmdLine);
    MessageBox(NULL, szBuffer, NULL, MB_ICONEXCLAMATION);
    return 0;
  }
  lpChar = GetCommandLine();
  i = lpCmdLine - lpChar - strlen("FEN2BMP.EXE\" ") - 1;
  strncpy(szBmpFileName, lpChar + 1, i);
  lpBmpFileName = szBmpFileName + i;
  // Read Proc:
  ReadFile(hFenFile, szBuffer, 1023, &dwBytesAccessed, NULL);
  szBuffer[dwBytesAccessed] = '\0';
  for (i = 0; i < 10; i ++) {
    for (j = 0; j < 9; j ++) {
      nBoard[i][j] = 0;
    }
  }
  lpChar = szBuffer;
  i = 0;
  j = 0;
  while (*lpChar > ' ') {
    if (*lpChar == '/') {
      i ++;
      j = 0;
      if (i >= 10) {
        break;
      }
    } else if (*lpChar >= '1' && *lpChar <= '9') {
      for (k = '0'; k < *lpChar; k ++) {
        if (j >= 9) {
          break;
        }
        j ++;
      }
    } else if (*lpChar >= 'A' && *lpChar <= 'Z') {
      if (j < 9) {
        nBoard[i][j] = FenPiece(*lpChar);
      }
      j ++;
    } else if (*lpChar >= 'a' && *lpChar <= 'z') {
      if (j < 9) {
        nBoard[i][j] = -FenPiece(*lpChar - 'a' + 'A');
      }
      j ++;
    }
    lpChar ++;
  }
  CloseHandle(hFenFile);
  // Write Proc:
  bMono = FALSE;
  if (MessageBox(NULL, "Draw Board for Monochromatic Publications?", "Choose Board Mode", MB_YESNO | MB_ICONQUESTION) == IDYES) {
    bMono = TRUE;
  }
  if (i == 7) {    
    strcpy(lpBmpFileName, "FEN2BMP.BMP");
    hOutFile = CreateFile(szBmpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    strcpy(lpBmpFileName, bMono ? "BOARDM.BMP" : "BOARD.BMP");
    hBoardFile = CreateFile(szBmpFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile(hBoardFile, szBuffer, 118, &dwBytesAccessed, NULL);
    WriteFile(hOutFile, szBuffer, 118, &dwBytesAccessed, NULL); // Write Header;
    for (i = 0; i < 32; i ++) {
      ReadFile(hBoardFile, szBuffer, 1024, &dwBytesAccessed, NULL);
      WriteFile(hOutFile, szBuffer, 1024, &dwBytesAccessed, NULL); // Write Board;
    }
    CloseHandle(hBoardFile);
    for (i = 0; i < 8; i ++) {
      for (j = 0; j < 8; j ++) {
        k = nBoard[i][j];
        if (k != 0) {
          if ((i + j) % 2 == 0) { // Black Square
            strcpy(lpBmpFileName, k > 0 ? cszWWPiece[k] : cszWBPiece[-k]);
          } else { // White Square
            if (bMono) {
              strcpy(lpBmpFileName, k > 0 ? cszBWPieceMono[k] : cszBBPieceMono[-k]);
            } else {
              strcpy(lpBmpFileName, k > 0 ? cszBWPiece[k] : cszBBPiece[-k]);
            }
          }
          hPieceFile = CreateFile(szBmpFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
          SetFilePointer(hPieceFile, 118, NULL, FILE_BEGIN);
          for (k = 0; k < 32; k ++) {
            ReadFile(hPieceFile, szBuffer, 16, &dwBytesAccessed, NULL);
            SetFilePointer(hOutFile, (7 - i) * 4096 + k * 128 + j * 16 + 118, NULL, FILE_BEGIN);
            WriteFile(hOutFile, szBuffer, 16, &dwBytesAccessed, NULL);
          }
          CloseHandle(hPieceFile);
        }
      }
    }
    CloseHandle(hOutFile);
  } else if (i == 9) {
    strcpy(lpBmpFileName, "FEN2BMP.BMP");
    hOutFile = CreateFile(szBmpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    strcpy(lpBmpFileName, "CBOARD.BMP");
    hBoardFile = CreateFile(szBmpFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile(hBoardFile, szBuffer, 118, &dwBytesAccessed, NULL);
    WriteFile(hOutFile, szBuffer, 118, &dwBytesAccessed, NULL); // Write Header;
    for (i = 0; i < 36; i ++) {
      ReadFile(hBoardFile, szBuffer, 702, &dwBytesAccessed, NULL);
      WriteFile(hOutFile, szBuffer, 702, &dwBytesAccessed, NULL); // Write Board;
    }
    CloseHandle(hBoardFile);
    for (i = 0; i < 10; i ++) {
      for (j = 0; j < 9; j ++) {
        k = nBoard[i][j];
        if (k != 0) {
          strcpy(lpBmpFileName, k > 0 ? (bMono ? cszCRPieceMono[k] : cszCRPiece[k]) : cszCBPiece[-k]);
          hPieceFile = CreateFile(szBmpFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
          SetFilePointer(hPieceFile, 118, NULL, FILE_BEGIN);
          for (k = 0; k < 18; k ++) {
            ReadFile(hPieceFile, szBuffer, 12, &dwBytesAccessed, NULL);
            SetFilePointer(hOutFile, (9 - i) * 2592 + k * 108 + j * 12 + 118, NULL, FILE_BEGIN);
            WriteFile(hOutFile, szBuffer, 9, &dwBytesAccessed, NULL);
          }
          CloseHandle(hPieceFile);
        }
      }
    }
    CloseHandle(hOutFile);
  }
  strcpy(lpBmpFileName, "FEN2BMP.BMP");
  ShellExecute(NULL, NULL, szBmpFileName, NULL, NULL, SW_SHOW);
  return 0;
}
