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

                    std::cout << "DLL: " << dll_path << " Language: " << lang << '\n';

                    switch (lang)
                    {
                    case CSharp:
                        ExecuteCSharp(dll_path.generic_wstring());
                        AmountOfInjects++;
                        break;
                    case CPP:
                        Inject(dll_path.generic_string());
                        AmountOfInjects++;
                        break;
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
	
    int Init() // This extracts the zips. If there is ever a launcher, this will not be needed.
    {
        for (const auto& entry : fs::directory_iterator(fs::current_path() / _("Scripts")))
        {
            auto path = entry.path();

            if (!path.filename().generic_string().contains(_(".script")))
            {
                std::cout << "Path " << path << " is not a valid path!\n";
				// return 0;
			}

            if (!fs::exists(path))
            {
                MessageBoxA(0, _("The file does not exist"), _("Error"), MB_ICONERROR);
                return 0;
            }

            auto tempPath = fs::temp_directory_path() / _("ScriptsExtracted"); // + std::to_string(rand() % 100)); // TODO: Remove

            if (fs::exists(tempPath))
                fs::remove_all(tempPath);

            if (!fs::create_directory(tempPath))
            {
                MessageBoxA(0, _("Could not create the temporary folder!"), _("Error"), MB_ICONERROR);
                return 0;
            }
			
            miniz_cpp::zip_file file(path.generic_string());
            
            file.extractall(tempPath.generic_string());

            // Read and parse script.json

            auto scriptPath = tempPath / _("script.json");

            if (!fs::exists(scriptPath))
            {
                MessageBoxA(0, _("Could not find script.json!"), _("Error"), MB_ICONERROR);
                return 0;
            }
            			
            if (!fs::exists(AppData::Path))
            {
                if (!fs::create_directory(AppData::Path));
                {
                    MessageBoxA(0, _("Unable to create AppData folder!"), _("Scripter"), MB_ICONERROR);
                    return 0;
                }
            }
            
            std::ifstream data(scriptPath);

            json j = json::parse(data);

            data.close();

            auto language = ConvertLanguage(j[_("language")]);
			
            auto newPathStr = (AppData::Path / AddExtension(j[_("script_name")], language)).generic_string();
			
            newPathStr.erase(newPathStr.find_last_of('.'), 50); // Remove the file extension.

            std::cout << newPathStr << '\n';

            auto newPath = fs::path(newPathStr);

            if (fs::exists(newPath))
                fs::remove_all(newPath);

            fs::copy(tempPath, newPathStr);

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

    if (AmountOfScripts == 0)
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