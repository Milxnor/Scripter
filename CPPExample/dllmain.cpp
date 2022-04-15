#include <Windows.h>
#include <iostream>

#include "scripter.h"

void Main()
{
    std::cout << "CPP: " << FindObject("FortEngine_")->GetFullName() << '\n';
}

DWORD WINAPI Startup(LPVOID)
{
    if (bHasInitialized)
        return 0;

    // put the functions you need to call when the dll is injected here, for example: Main().
    Main();

    bHasInitialized = true;
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dllReason, LPVOID lpReserved)
{
    switch (dllReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, Startup, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
