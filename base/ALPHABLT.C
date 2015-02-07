#include <windows.h>

__declspec(dllexport) VOID WINAPI AlphaBlt(
    HDC hdcDest, int xDest, int yDest, int nWidth, int nHeight,
    HDC hdcSrc, int xSrc, int ySrc, HDC hdcAlpha, int xAlpha, int yAlpha);

VOID WINAPI AlphaBlt(HDC hdcDest, int xDest, int yDest, int nWidth, int nHeight,
    HDC hdcSrc, int xSrc, int ySrc, HDC hdcAlpha, int xAlpha, int yAlpha) {
  int i, n;
  HDC hdc1Dest, hdc1Src, hdc1Alpha;
  HBITMAP hbmpDest, hbmpSrc, hbmpAlpha;
  COLORREF *lpDest, *lpSrc, *lpAlpha;
  BITMAPINFO bmi;

  memset(&bmi, 0, sizeof(BITMAPINFO));
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = nWidth;
  bmi.bmiHeader.biHeight = nHeight;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;

  hdc1Dest = CreateCompatibleDC(NULL);
  hdc1Src = CreateCompatibleDC(NULL);
  hdc1Alpha = CreateCompatibleDC(NULL);
  hbmpDest = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &lpDest, NULL, 0);
  hbmpSrc = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &lpSrc, NULL, 0);
  hbmpAlpha = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &lpAlpha, NULL, 0);
  SelectObject(hdc1Dest, hbmpDest);
  SelectObject(hdc1Src, hbmpSrc);
  SelectObject(hdc1Alpha, hbmpAlpha);

  BitBlt(hdc1Dest, 0, 0, nWidth, nHeight, hdcDest, xDest, yDest, SRCCOPY);
  BitBlt(hdc1Src, 0, 0, nWidth, nHeight, hdcSrc, xSrc, ySrc, SRCCOPY);
  BitBlt(hdc1Alpha, 0, 0, nWidth, nHeight, hdcAlpha, xAlpha, yAlpha, SRCCOPY);
  n = nHeight * nWidth;
  for (i = 0; i < n; i ++) {
    lpDest[i] = RGB(
        (GetRValue(lpDest[i]) * (256 - GetRValue(lpAlpha[i])) + GetRValue(lpSrc[i]) * GetRValue(lpAlpha[i])) >> 8,
        (GetGValue(lpDest[i]) * (256 - GetGValue(lpAlpha[i])) + GetGValue(lpSrc[i]) * GetGValue(lpAlpha[i])) >> 8,
        (GetBValue(lpDest[i]) * (256 - GetBValue(lpAlpha[i])) + GetBValue(lpSrc[i]) * GetBValue(lpAlpha[i])) >> 8);
  }
  BitBlt(hdcDest, xDest, yDest, nWidth, nHeight, hdc1Dest, 0, 0, SRCCOPY);

  DeleteObject(hbmpDest);
  DeleteObject(hbmpSrc);
  DeleteObject(hbmpAlpha);
  DeleteDC(hdc1Dest);
  DeleteDC(hdc1Src);
  DeleteDC(hdc1Alpha);
}