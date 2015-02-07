#include "../../base/base.h"

#ifndef BIGINT32_H
#define BIGINT32_H

uint32_t BigIntAdd(uint32_t *lpDst, const uint32_t *lpSrc1, const uint32_t *lpSrc2, uint32_t nLen);
uint32_t BigIntSub(uint32_t *lpDst, const uint32_t *lpSrc1, const uint32_t *lpSrc2, uint32_t nLen);
uint32_t BigIntInc(uint32_t *lp, uint32_t nLen);
uint32_t BigIntDec(uint32_t *lp, uint32_t nLen);
void BigIntMul(uint32_t *lpDst, const uint32_t *lpSrc1, const uint32_t *lpSrc2, uint32_t nLen1, uint32_t nLen2);

#endif
