#include <stdio.h>
#include <windows.h>
#include <silverhook.h>

// Define a function pointer type for the original function.
typedef int (WINAPI *MessageBoxA_t)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

// A variable to store the address of the original function.
MessageBoxA_t g_pOriginalMessageBoxA = NULL;

// Define the detour function with a matching signature.
int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    printf("  >> Hook called!\n");
    printf("  >> Original Caption: %s\n", lpCaption);
    printf("  >> Original Text: %s\n", lpText);

    // In this example, we are modifying the parameters.
    // You can also call the original function or block the call entirely.
    LPCSTR newText = "This message has been intercepted by an IAT patch!";
    LPCSTR newCaption = "IAT Hooked!";

    if (g_pOriginalMessageBoxA) {
        return g_pOriginalMessageBoxA(hWnd, newText, newCaption, uType | MB_ICONINFORMATION);
    }
    return 0;
}

int main() {
    // Get a handle to the module where the patch should be applied.
    HMODULE hTargetModule = GetModuleHandle(NULL);

    // Apply the patch
    BOOL success = SilverHook_PatchIAT(
        hTargetModule,
        "user32.dll",
        "MessageBoxA",
        (void*)MyMessageBoxA,
        (void**)&g_pOriginalMessageBoxA 
    );

    if (!success) {
        printf("Failed to patch IAT. Error code: %lu\n", GetLastError());
        return 1;
    }

    printf("Successfully patched IAT.");

    MessageBoxA(NULL, "This is the original text.", "Original Caption", MB_OK);

    return 0;
}