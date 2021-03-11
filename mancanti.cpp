
#include "malloc.h"

void * __cdecl operator new[](unsigned int t) {
  return malloc(t);
}

void __cdecl operator delete[](void *t) {
  free(t);
}

extern "C" {
  int __stdcall _security_check_cookie(int a) {
    return 1;
  }

  int __security_cookie;
  void __declspec(naked) _ftol2() {
    __asm {
      ret
    }
  }
}