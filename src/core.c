#include "core.h"

PIMAGE_NT_HEADERS SilverHook_GetNtHeaders(HMODULE hModule) {
    if (hModule == NULL) {
        return NULL;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        SetLastError(ERROR_BAD_FORMAT);
        return NULL;
    }

    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        SetLastError(ERROR_BAD_FORMAT);
        return NULL;
    }

    return ntHeaders;
}