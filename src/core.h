#ifndef SILVERHOOK_CORE_H
#define SILVERHOOK_CORE_H

#include <windows.h>

PIMAGE_NT_HEADERS SilverHook_GetNtHeaders(HMODULE hModule);

#endif // SILVERHOOK_CORE_H