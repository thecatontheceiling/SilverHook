# SilverHook

SilverHook is a minimal C-based library for Microsoft Windows designed to perform Import Address Table (IAT) hooking. It provides a straightforward API to intercept function calls made from a target module to an external DLL.

## Features

-   **IAT Hooking:** Modify a module's Import Address Table to redirect external function calls to custom detour functions.
-   **Simple API:** A single function call (`SilverHook_PatchIAT`) is all that's needed to hook a specific function by name. For more advanced use cases, a callback-based function (`SilverHook_PatchIATAdvanced`) allows enumeration and conditional patching of all functions imported from a specific module.
-   **Function Chaining:** The original function's address can be retrieved, allowing the detour to call the original implementation.

## Build

Use the CLANG64 environment in MSYS2 and run ``make all``. Run ``make static-example`` or ``make dll-example`` to build the example executable.

## Usage

Include the `silverhook.h` header and link against the compiled library. Further instructions are in the header itself.

## Example

See [example.c](https://github.com/thecatontheceiling/SilverHook/blob/main/example/example.c) for a very basic usage example. The example doesn't show PatchIATAdvanced, however, which can be very useful in some cases.

## Inspirations

This library was created out of necessity. I needed to modify the memory addresses defined in the IAT of an application, but many functions in its IAT seemed to point to the same address. MinHook didn't make it easy for me to handle this cleanly, so I wrote Goldhook (an earlier variation of this library) to do it myself.

After I got Goldhook working, a few months later I decided to turn it into an actual library, resulting in SilverHook. Hotpatching support was planned but ultimately scrapped due to the amount of work necessary to support all edge cases, which would effectively mirror the functionality of existing libraries such as MinHook.

### Credits

-  **Anonymous Friend:** Helped me get started making this library.

-  [**Matthijs Lavrijsen:**](https://github.com/Mattiwatti) Helped with the Makefile and explained some complicated things to me.

