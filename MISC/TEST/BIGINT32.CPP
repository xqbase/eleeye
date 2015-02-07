#include <string.h>
#include "../../base/base.h"

#ifdef _MSC_VER

#pragma warning(disable: 4035)

uint32_t BigIntAdd(uint32_t *lpDst, const uint32_t *lpSrc1, const uint32_t *lpSrc2, uint32_t nLen) {
  __asm {
    mov ecx, nLen;
    or ecx, ecx;
    jz f0;
    mov esi, lpSrc1;
    mov ebx, lpSrc2;
    mov edi, lpDst;
  r0:
    mov eax, [esi];
    lea esi, [esi + 4];
    adc eax, [ebx];
    lea ebx, [ebx + 4];
    mov [edi], eax;
    lea edi, [edi + 4];
    loop r0;
  f0:
    mov eax, 0;
    adc eax, 0;
  }
}

uint32_t BigIntSub(uint32_t *lpDst, const uint32_t *lpSrc1, const uint32_t *lpSrc2, uint32_t nLen) {
  __asm {
    mov ecx, nLen;
    or ecx, ecx;
    jz f0;
    mov edi, lpDst;
    mov esi, lpSrc1;
    mov ebx, lpSrc2;
  r0:
    mov eax, [esi];
    lea esi, [esi + 4];
    sbb eax, [ebx];
    lea ebx, [ebx + 4];
    mov [edi], eax;
    lea edi, [edi + 4];
    loop r0;
  f0:
    mov eax, 0;
    sbb eax, 0;
  }
}

uint32_t BigIntInc(uint32_t *lp, uint32_t nLen) {
  /* uint32_t cf = 1;
   * for (uint32_t i = 0; i < nLen; i ++) {
   *   cf:lp[i] += cf;
   *   if (cf != 0) {
   *     break;
   *   }
   * }
   * return cf;
   */
  __asm {
    mov ecx, nLen;
    or ecx, ecx;
    jz f0;
    mov ebx, lp;
    stc;
  r0:
    mov eax, [ebx];
    adc eax, 0;
    mov [ebx], eax;
    lea ebx, [ebx + 4];
    jnc f0;
    loop r0;
  f0:
    mov eax, 0;
    adc eax, 0;
  }
}

uint32_t BigIntDec(uint32_t *lp, uint32_t nLen) {
  /* uint32_t cf = -1;
   * for (uint32_t i = 0; i < nLen; i ++) {
   *   cf:lp[i] -= cf;
   *   if (cf != 0) {
   *     break;
   *   }
   * }
   * return cf;
   */
  __asm {
    mov ecx, nLen;
    or ecx, ecx;
    jz f0;
    mov ebx, lp;
    stc;
    r0:
    mov eax, [ebx];
    sbb eax, 0;
    mov [ebx], eax;
    lea ebx, [ebx + 4];
    jnc f0;
    loop r0;
  f0:
    mov eax, 0;
    sbb eax, 0;
  }
}

#pragma warning(default: 4035)

void BigIntMul(uint32_t *lpDst, const uint32_t *lpSrc1, const uint32_t *lpSrc2, uint32_t nLen1, uint32_t nLen2) {
  uint32_t i, iSrc1;
  memset(lpDst, 0, (nLen1 + nLen2) * sizeof(uint32_t));
  for (i = 0; i < nLen1; i ++) {
    iSrc1 = lpSrc1[i];
    /* for (uint32_t j = 0; j < nLen2; j ++) {
     *   uint32_t ebx = 0;
     *   edx:eax = lpSrc[i] * lpSrc[j] + lpDst[i + j] + ebx;
     *   lpDst[i + j] = eax;
     *   ebx = edx;
     * }
     * lpDst[i + j] = ebx;
     */
    __asm {
      mov ecx, nLen2;
      or ecx, ecx;
      jz f0;
      mov esi, lpSrc2;
      mov edi, i;
      lea edi, [edi * 4];
      add edi, lpDst;
      xor ebx, ebx;
    r0:
      mov eax, [esi];
      lea esi, [esi + 4];
      mul iSrc1;
      add eax, [edi];
      adc edx, 0;
      add eax, ebx;
      adc edx, 0;
      mov [edi], eax;
      lea edi, [edi + 4];
      mov ebx, edx;
      loop r0;
    f0:
      mov [edi], ebx;
    }
  }
}

#endif
