#include "base.h"

#ifndef X86ASM_H
#define X86ASM_H

inline uint32_t LOW_LONG(uint64_t Operand) {
  return (uint32_t) Operand;
}

inline uint32_t HIGH_LONG(uint64_t Operand) {
  return (uint32_t) (Operand >> 32);
}

inline uint64_t MAKE_LONG_LONG(uint32_t LowLong, uint32_t HighLong) {
  return (uint64_t) LowLong | ((uint64_t) HighLong << 32);
}

#if defined __arm__ || defined __mips__

static int cnBitScanTable[64] = {
  32,  0,  1, 12,  2,  6, -1, 13,  3, -1,  7, -1, -1, -1, -1, 14,
  10,  4, -1, -1,  8, -1, -1, 25, -1, -1, -1, -1, -1, 21, 27, 15,
  31, 11,  5, -1, -1, -1, -1, -1,  9, -1, -1, 24, -1, -1, 20, 26,
  30, -1, -1, -1, -1, 23, -1, 19, 29, -1, 22, 18, 28, 17, 16, -1
};

inline int BitScan(uint32_t Operand) {
  uint32_t dw = (Operand << 4) + Operand;
  dw += dw << 6;
  dw = (dw << 16) - dw;
  return cnBitScanTable[dw >> 26];  
}

inline int Bsf(uint32_t Operand) {
  return BitScan(Operand & -Operand);
}

inline int Bsr(uint32_t Operand) {
  uint32_t dw = Operand | (Operand >> 1);
  dw |= dw >> 2;
  dw |= dw >> 4;
  dw |= dw >> 8;
  dw |= dw >> 16;
  return BitScan(dw - (dw >> 1));
}

#else

#ifdef _MSC_VER

#pragma warning(disable: 4035)

__forceinline int Bsf(uint32_t Operand) {
  __asm {
    bsf eax, Operand;
  }
}

__forceinline int Bsr(uint32_t Operand) {
  __asm {
    bsr eax, Operand;
  }
}

__forceinline uint64_t TimeStampCounter(void) {
  __asm {
    rdtsc;
  }
}

__forceinline uint64_t LongMul(uint32_t Multiplier, uint32_t Multiplicand) {
  __asm {
    mov eax, Multiplier;
    mul Multiplicand;
  }
}

__forceinline uint64_t LongSqr(uint32_t Multiplier) {
  __asm {
    mov eax, Multiplier;
    mul Multiplier;
  }
}

__forceinline uint32_t LongDiv(uint64_t Dividend, uint32_t Divisor) {
  __asm {
    mov eax, dword ptr Dividend[0];
    mov edx, dword ptr Dividend[4];
    div Divisor;
  }
}

__forceinline uint32_t LongMod(uint64_t Dividend, uint32_t Divisor) {
  __asm {
    mov eax, dword ptr Dividend[0];
    mov edx, dword ptr Dividend[4];
    div Divisor;
    mov eax, edx;
  }
}

__forceinline uint32_t LongMulDiv(uint32_t Multiplier, uint32_t Multiplicand, uint32_t Divisor) {
  __asm {
    mov eax, Multiplier;
    mul Multiplicand;
    div Divisor;
  }
}

__forceinline uint32_t LongMulMod(uint32_t Multiplier, uint32_t Multiplicand, uint32_t Divisor) {
  __asm {
    mov eax, Multiplier;
    mul Multiplicand;
    div Divisor;
    mov eax, edx;
  }
}

__forceinline uint32_t Shld(uint32_t HighLong, uint32_t LowLong, uint32_t Count) {
  __asm {
    mov eax, HighLong;
    mov edx, LowLong;
    mov ecx, Count;
    shld eax, edx, cl;
  }
}

__forceinline uint32_t Shrd(uint32_t LowLong, uint32_t HighLong, uint32_t Count) {
  __asm {
    mov eax, LowLong;
    mov edx, HighLong;
    mov ecx, Count;
    shrd eax, edx, cl;
  }
}

#pragma warning(default: 4035)

#else

static __inline__ int Bsf(uint32_t Operand) {
  int eax;
  asm __volatile__ (
    "bsfl %0, %0" "\n\t"
    : "=a" (eax)
    : "0" (Operand)
  );
  return eax;
}

static __inline__ int Bsr(uint32_t Operand) {
  int eax;
  asm __volatile__ (
    "bsrl %0, %0" "\n\t"
    : "=a" (eax)
    : "0" (Operand)
  );
  return eax;
}

static __inline__ uint64_t TimeStampCounter(void) {
  uint32_t eax, edx;
  asm __volatile__ (
    "rdtsc" "\n\t"
    : "=a" (eax), "=d" (edx)
    :
  );
  return MAKE_LONG_LONG(eax, edx);
}

static __inline__ uint64_t LongMul(uint32_t Multiplier, uint32_t Multiplicand) {
  uint32_t eax, edx;
  asm __volatile__ (
    "mull %1" "\n\t"
    : "=a" (eax), "=d" (edx)
    : "0" (Multiplier), "1" (Multiplicand)
  );
  return MAKE_LONG_LONG(eax, edx);
}

static __inline__ uint64_t LongSqr(uint32_t Multiplier) {
  uint32_t eax, edx;
  asm __volatile__ (
    "mull %1" "\n\t"
    : "=a" (eax), "=d" (edx)
    : "0" (Multiplier), "1" (Multiplier)
  );
  return MAKE_LONG_LONG(eax, edx);
}

static __inline__ uint32_t LongDiv(uint64_t Dividend, uint32_t Divisor) {
  uint32_t eax, edx, dummy;
  asm __volatile__ (
    "divl %2" "\n\t"
    : "=a" (eax), "=d" (edx), "=g" (dummy)
    : "0" (LOW_LONG(Dividend)), "1" (HIGH_LONG(Dividend)), "2" (Divisor)
  );
  return eax;
}

static __inline__ uint32_t LongMod(uint64_t Dividend, uint32_t Divisor) {
  uint32_t eax, edx, dummy;
  asm __volatile__ (
    "divl %2"     "\n\t"
    : "=a" (eax), "=d" (edx), "=g" (dummy)
    : "0" (LOW_LONG(Dividend)), "1" (HIGH_LONG(Dividend)), "2" (Divisor)
  );
  return edx;
}

static __inline__ uint32_t LongMulDiv(uint32_t Multiplier, uint32_t Multiplicand, uint32_t Divisor) {
  uint32_t eax, edx, dummy;
  asm __volatile__ (
    "mull %1" "\n\t"
    "divl %2" "\n\t"
    : "=a" (eax), "=d" (edx), "=g" (dummy)
    : "0" (Multiplier), "1" (Multiplicand), "2" (Divisor)
  );
  return eax;
}

static __inline__ uint32_t LongMulMod(uint32_t Multiplier, uint32_t Multiplicand, uint32_t Divisor) {
  uint32_t eax, edx, dummy;
  asm __volatile__ (
    "mull %1"     "\n\t"
    "divl %2"     "\n\t"
    : "=a" (eax), "=d" (edx), "=g" (dummy)
    : "0" (Multiplier), "1" (Multiplicand), "2" (Divisor)
  );
  return edx;
}

static __inline uint32_t Shld(uint32_t High, uint32_t Low, uint32_t Count) {
  uint32_t eax, edx, ecx;
  asm __volatile__ (
    "shldl %%cl, %1, %0" "\n\t"
    : "=a" (eax), "=d" (edx), "=c" (ecx)
    : "0" (High), "1" (Low), "2" (Count)
  );
  return eax;
}

static __inline uint32_t Shrd(uint32_t Low, uint32_t High, uint32_t Count) {
  uint32_t eax, edx, ecx;
  asm __volatile__ (
    "shrdl %%cl, %1, %0" "\n\t"
    : "=a" (eax), "=d" (edx), "=c" (ecx)
    : "0" (Low), "1" (High), "2" (Count)
  );
  return eax;
}

#endif

inline uint64_t LongShl(uint64_t Operand, uint32_t Count) {
  if (Count < 32) {
    return MAKE_LONG_LONG(LOW_LONG(Operand) << Count, Shld(HIGH_LONG(Operand), LOW_LONG(Operand), Count));
  } else if (Count < 64) {
    return MAKE_LONG_LONG(0, LOW_LONG(Operand) << (Count - 32));
  } else {
    return MAKE_LONG_LONG(0, 0);
  }
}

inline uint64_t LongShr(uint64_t Operand, uint32_t Count) {
  if (Count < 32) {
    return MAKE_LONG_LONG(Shrd(LOW_LONG(Operand), HIGH_LONG(Operand), Count), HIGH_LONG(Operand) >> Count);
  } else if (Count < 64) {
    return MAKE_LONG_LONG(HIGH_LONG(Operand) >> (Count - 32), 0);
  } else {
    return MAKE_LONG_LONG(0, 0);
  }
}

#endif

#endif
