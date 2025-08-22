#include "silverhook.h"
#include "core.h"

typedef struct {
    LPCSTR szFuncName;
    void* pDetour;
    void** ppOriginal;
    BOOL bFound;
} IAT_PATCH_CONTEXT, *PIAT_PATCH_CONTEXT;

BOOL PatchIATAddress(PROC* ppAddress, PROC pNewAddress, PROC* ppOriginal) {
    DWORD dwOldProtect;
    if (!VirtualProtect(ppAddress, sizeof(PROC), PAGE_READWRITE, &dwOldProtect)) {
        return FALSE;
    }

    if (ppOriginal) {
        *ppOriginal = *ppAddress;
    }

    InterlockedExchangePointer((PVOID)ppAddress, (PVOID)pNewAddress);

    VirtualProtect(ppAddress, sizeof(PROC), dwOldProtect, &dwOldProtect);
    return TRUE;
}

BOOL FindAndPatchImportCallback(PIMPORT_FUNC_INFO pifi, PVOID pContext) {
    PIAT_PATCH_CONTEXT pCtx = (PIAT_PATCH_CONTEXT)pContext;

    if (lstrcmpiA(pifi->szFuncName, pCtx->szFuncName) == 0) {
        if (PatchIATAddress(pifi->ppAddress, pCtx->pDetour, (PROC*)pCtx->ppOriginal)) {
            pCtx->bFound = TRUE;
        }
        return FALSE;
    }
    return TRUE;
}

SILVERHOOK_API BOOL SilverHook_PatchIATAdvanced(HMODULE hTargetModule, LPCSTR pszImportModule, PMODULE_IMPORT_FUNC_CALLBACK pCallback, PVOID pContext) {
    if (!hTargetModule || !pszImportModule || !pCallback) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    PIMAGE_NT_HEADERS ntHeaders = SilverHook_GetNtHeaders(hTargetModule);
    if (ntHeaders == NULL) return FALSE;

    IMAGE_DATA_DIRECTORY importDir = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (importDir.VirtualAddress == 0) {
        SetLastError(ERROR_MOD_NOT_FOUND);
        return FALSE;
    }

    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hTargetModule + importDir.VirtualAddress);

    while (importDesc->Name != 0) {
        LPCSTR moduleName = (LPCSTR)((BYTE*)hTargetModule + importDesc->Name);
        if (lstrcmpiA(moduleName, pszImportModule) == 0) {
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((BYTE*)hTargetModule + importDesc->OriginalFirstThunk);
            PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)((BYTE*)hTargetModule + importDesc->FirstThunk);

            while (pThunk->u1.AddressOfData != 0) {
                if (!(pThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)) {
                    PIMAGE_IMPORT_BY_NAME pibn = (PIMAGE_IMPORT_BY_NAME)((BYTE*)hTargetModule + pThunk->u1.AddressOfData);
                    IMPORT_FUNC_INFO ifi = { pibn->Name, (PROC*)&pIAT->u1.Function };
                    if (!pCallback(&ifi, pContext)) {
                        return TRUE;
                    }
                }
                pThunk++;
                pIAT++;
            }
            return TRUE;
        }
        importDesc++;
    }

    SetLastError(ERROR_MOD_NOT_FOUND);
    return FALSE;
}

SILVERHOOK_API BOOL SilverHook_PatchIAT(HMODULE hTargetModule, LPCSTR pszImportModule, LPCSTR pszFuncName, void* pDetour, void** ppOriginal) {
    if (!hTargetModule || !pszImportModule || !pszFuncName || !pDetour) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    IAT_PATCH_CONTEXT ctx = { pszFuncName, pDetour, ppOriginal, FALSE };
    SilverHook_PatchIATAdvanced(hTargetModule, pszImportModule, FindAndPatchImportCallback, &ctx);

    if (!ctx.bFound && GetLastError() == 0) {
        SetLastError(ERROR_PROC_NOT_FOUND);
    }

    return ctx.bFound;
}

