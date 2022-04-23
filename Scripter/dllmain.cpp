#include <Windows.h>
#include <filesystem>
#include <fstream>

#include <languages.h>
#include <json.hpp>

#include "util.h"
#include <zip_file.hpp>

using namespace nlohmann;
namespace fs = std::filesystem;

namespace Scripter
{
    int InjectScripts(fs::path path)
    {
        int AmountOfInjects = 0;
		
        if (!fs::exists(path))
            return AmountOfInjects;

        for (auto& p : fs::directory_iterator(path))
        {
            if (p.is_directory())
                InjectScripts(p.path());

            else
            {
                auto filename = p.path().filename().generic_string();

                if (filename == _("script.json"))
                {
                    std::ifstream data(p.path());

                    json j = json::parse(data);

                    data.close();

                    std::string language = j[_("language")];
                    std::string scriptName = j[_("script_name")];

                    auto lang = ConvertLanguage(language);

                    auto dll_path = path / scriptName;

                    switch (lang)
                    {
                    case CSharp:
                        ExecuteCSharp(dll_path.generic_wstring());
                        AmountOfInjects++;
                        break;
                    case CPP:
                        Inject(dll_path.generic_string());
                        AmountOfInjects++;
                    case JS:
                        ExecuteJS(dll_path.generic_string());
                        AmountOfInjects++;
                        break;
                    default:
                        std::cout << _("Could not deduce language!\n");
                        break;
                    }

                }
            }
        }

        return AmountOfInjects;
    }
	
    int Init()
    {
        for (const auto& entry : fs::directory_iterator(fs::current_path() / _("Scripts")))
        {
            auto path = entry.path();

            if (!path.filename().generic_string().contains(_(".script")))
            {
				MessageBoxA(0, _("Scripts folder contains files that are not scripts!\n"), _("Error"), MB_OK);
				return 0;
			}

            if (!fs::exists(path))
            {
                MessageBoxA(0, _("The file does not exist"), _("Error"), MB_ICONERROR);
                return 0;
            }

            auto tempPath = fs::temp_directory_path() / _("ScriptsExtracted"); // + std::to_string(rand() % 100));

            if (fs::exists(tempPath))
                fs::remove_all(tempPath);

            if (!fs::create_directory(tempPath))
            {
                MessageBoxA(0, _("Could not create the temporary folder!"), _("Error"), MB_ICONERROR);
                return 0;
            }
			
            miniz_cpp::zip_file file(path.generic_string());
            // file.printdir();
            file.extractall(tempPath.generic_string());

            // Read and parse script.json

            auto scriptPath = tempPath / _("script.json");

            if (!fs::exists(scriptPath))
            {
                MessageBoxA(0, _("Could not find script.json!"), _("Error"), MB_ICONERROR);
                return 0;
            }

            if (!fs::exists(AppData::Path))
                fs::create_directory(AppData::Path);

            std::ifstream data(scriptPath);

            json j = json::parse(data);

            data.close();

            // fs::rename(tempPath, Settings::Path / j[_("script_name")]);
			
            auto newPath = AppData::Path / j[_("script_name")];

            if (fs::exists(newPath))
                fs::remove_all(newPath);

            fs::copy(tempPath, AppData::Path / j[_("script_name")]);

            if (fs::exists(tempPath))
                fs::remove_all(tempPath);
        }

        return InjectScripts(AppData::Path);
    }
}

DWORD WINAPI Main(LPVOID)
{
    AllocConsole();

    FILE* file;
    freopen_s(&file, _("CONOUT$"), _("w"), stdout);

    if (!AppData::Init())
    {
        MessageBoxA(0, _("Failed!"), _("AppData"), MB_ICONERROR);
        return 0;
    }

    if (!Setup())
    {
        MessageBoxA(0, _("Failed!"), _("Setup"), MB_OK);
        return 0;
    }

    std::cout << _("Setup for ") << FN_Version << "!\n";

    auto AmountOfScripts = Scripter::Init();

    if (!AmountOfScripts)
    {
		MessageBoxA(0, _("Could not inject any scripts!"), _("Scripter::Init"), MB_OK);
		return 0;
    }

    std::cout << _("\nInitialized ") << AmountOfScripts << _(" Scripts!\n");
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