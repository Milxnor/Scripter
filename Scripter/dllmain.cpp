#include <Windows.h>
#include <filesystem>

#include <languages.h>
#include <json.hpp>

#include "util.h"

namespace fs = std::filesystem;

namespace Scripter
{
    int Init()
    {
        int NumOfInjects = 0;

        // Loop through every dll in "Script" folder and inject.
        for (const auto& entry : fs::directory_iterator(fs::current_path() / "Scripts"))
        {
            auto Path = entry.path().string();
            auto wPath = entry.path().wstring();
			
            if (Path.contains(".dll"))
            {
                if (Path.contains("Managed") && InjectManaged(wPath))
                {
                    std::cout << "Loaded " << Path << '\n';
                    NumOfInjects++;
                }

                else if (Path.contains("Native") && Inject(Path))
                {
                    std::cout << "Injected " << Path << '\n';
                    NumOfInjects++;
                }

                else
                    std::cout << "Failed to inject " << Path << ".\n";

            }
			
            else
                std::cout << Path << " is not a dll!\n";
        }

        return NumOfInjects;
    }
}

DWORD WINAPI Main(LPVOID)
{
    // AllocConsole();

    FILE* file;
    freopen_s(&file, "Scripter.txt", "w", stdout);

    if (!AppData::Init())
    {
        MessageBoxA(0, "Failed!", "AppData", MB_ICONERROR);
        return 0;
    }

    if (!Setup())
    {
        MessageBoxA(0, "Failed!", "Setup", MB_OK);
        return 0;
    }

    std::cout << "Setup for " << FN_Version << "!\n";

    auto AmountOfScripts = Scripter::Init();

    if (!AmountOfScripts)
    {
		MessageBoxA(0, "Failed!", "Scripter::Init", MB_OK);
		return 0;
    }

    std::cout << "\nInitialized " << AmountOfScripts << " Scripts!\n";
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
