#include <Windows.h>
#include <iostream>

#include "scripter.h"

DWORD WINAPI Main(LPVOID)
{
    std::cout << FindObject("FortEngine_")->GetFullName() << '\n';
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dllReason, LPVOID lpReserved)
{
    switch (dllReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, Main, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
