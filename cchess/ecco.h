#ifndef ECCO_H
#define ECCO_H

#ifdef _WIN32

#include <windows.h>
#include "../base/base.h"

const char *const cszLibEccoFile = "ECCO.DLL";

struct EccoApiStruct {
  HMODULE hModule;
  VOID (WINAPI *EccoInitOpenVar)(LONG);
  LONG (WINAPI *EccoIndex)(LPCSTR);
  LPCSTR (WINAPI *EccoOpening)(LONG);
  LPCSTR (WINAPI *EccoVariation)(LONG);
  bool Startup(const char *szLibEccoPath, bool bTrad = false) {
    hModule = LoadLibrary(szLibEccoPath);
    if (hModule != NULL) {
      EccoInitOpenVar = (VOID (WINAPI *)(LONG)) GetProcAddress(hModule, "_EccoInitOpenVar@4");
      EccoIndex = (LONG (WINAPI *)(LPCSTR)) GetProcAddress(hModule, "_EccoIndex@4");
      EccoOpening = (LPCSTR (WINAPI *)(LONG)) GetProcAddress(hModule, "_EccoOpening@4");
      EccoVariation = (LPCSTR (WINAPI *)(LONG)) GetProcAddress(hModule, "_EccoVariation@4");
      EccoInitOpenVar((LONG) bTrad);
      return true;
    } else {
      return false;
    }
  }
  bool Available(void) const {
    return hModule != NULL;
  }
  void Shutdown(void) {
    if (hModule != NULL) {
      FreeLibrary(hModule);
    }
  }
};

#else

#include <dlfcn.h>
#include "../base/base.h"

const char *const cszLibEccoFile = "libecco.so";

struct EccoApiStruct {
  void *hModule;
  void (*EccoInitOpenVar)(int);
  uint32_t (*EccoIndex)(const char *);
  const char *(*EccoOpening)(uint32_t);
  const char *(*EccoVariation)(uint32_t);
  bool Startup(const char *szLibEccoPath, bool bTrad = false) {
    hModule = dlopen(szLibEccoPath, RTLD_LAZY);
    if (hModule != NULL) {
      EccoInitOpenVar = (void (*)(int)) dlsym(hModule, "EccoInitOpenVar");
      EccoIndex = (uint32_t (*)(const char *)) dlsym(hModule, "EccoIndex");
      EccoOpening = (const char *(*)(uint32_t)) dlsym(hModule, "EccoOpening");
      EccoVariation = (const char *(*)(uint32_t)) dlsym(hModule, "EccoVariation");
      EccoInitOpenVar(false);
      return true;
    } else {
      return false;
    }
  }
  bool Available(void) const {
    return hModule != NULL;
  }
  void Shutdown(void) {
    if (hModule != NULL) {
      dlclose(hModule);
    }
  }
};

#endif

#endif
