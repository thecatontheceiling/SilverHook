#ifndef SILVERHOOK_H
#define SILVERHOOK_H

#if defined(_WIN32)
#include <windows.h>
#else
#error "SilverHook only supports Windows"
#endif

/* 
 * When BUILDING the library:
 *   - Define SILVERHOOK_BUILD_DLL when building as a DLL
 *   - Define SILVERHOOK_BUILD_STATIC when building as static library
 * 
 * When USING the library:
 *   - Define SILVERHOOK_STATIC before including this header to link statically
 *   - Don't define anything to link dynamically (default)
 */
#if defined(_WIN32)
  #if defined(SILVERHOOK_BUILD_DLL)
    #define SILVERHOOK_API __declspec(dllexport)
  #elif defined(SILVERHOOK_BUILD_STATIC)
    #define SILVERHOOK_API
  #elif defined(SILVERHOOK_STATIC)
    #define SILVERHOOK_API
  #else
    #define SILVERHOOK_API __declspec(dllimport)
  #endif
#else
  #define SILVERHOOK_API
#endif

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    LPCSTR szFuncName;
    PROC* ppAddress;
} IMPORT_FUNC_INFO, *PIMPORT_FUNC_INFO;
typedef BOOL (*PMODULE_IMPORT_FUNC_CALLBACK)(PIMPORT_FUNC_INFO ifi, PVOID pContext);


/**
 * @param hTargetModule     The handle to the module whose IAT you want to patch (e.g., GetModuleHandle(NULL))
 * @param pszImportModule   The name of the module the function is imported from (e.g., "user32.dll")
 * @param pszFuncName       The name of the function to hook (e.g., "MessageBoxA")
 * @param pDetour           The pointer to your hook function
 * @param ppOriginal        A pointer to a variable that will receive the original function's address
 *                          This allows you to call the original function from your hook. Can be NULL.
 * @return TRUE on success, FALSE on failure. Use GetLastError() for more info.
 */
SILVERHOOK_API BOOL SilverHook_PatchIAT(
    HMODULE hTargetModule,
    LPCSTR pszImportModule,
    LPCSTR pszFuncName,
    void* pDetour,
    void** ppOriginal
);


/**
 * @brief Variant of PatchIAT which enumerates imported functions from a module and invokes a callback on each. 
 * 
 * @param hTargetModule     The handle to the module whose IAT you want to patch (e.g., GetModuleHandle(NULL))
 * @param pszImportModule   The null-terminated string name of the module the function is imported from (e.g., "user32.dll")
 * @param pCallback         A pointer to a callback function that will be invoked for each imported function found. The callback should return TRUE to continue enumeration or FALSE to stop.
 * @param pContext          A user-defined pointer that is passed to the callback function. Allows for state persistence.
 *                          This allows you to call the original function from your hook. Can be NULL.
 * @return TRUE if the specified import module was found and enumeratio was succesful. FALSE on failure. Use GetLastError() for more info.
 */
SILVERHOOK_API BOOL SilverHook_PatchIATAdvanced(
    HMODULE hTargetModule,
    LPCSTR pszImportModule,
    PMODULE_IMPORT_FUNC_CALLBACK pCallback,
    PVOID pContext
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SILVERHOOK_H