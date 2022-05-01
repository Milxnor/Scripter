#include <Windows.h>
#include <filesystem>
#include <fstream>

#include <languages.h>
#include <json.hpp>

#include "util.h"
#include <zip_file.hpp>

using namespace nlohmann;
namespace fs = std::filesystem;

static int AmountOfInjects = 0; // for some reason, return values are just like broken idk.

namespace Scripter
{
    void InjectScripts(fs::path path)
    {
        if (!fs::exists(path))
        {
            MessageBoxA(0, path.generic_string().c_str(), _("Path not found!"), MB_ICONERROR);
            return;
        }

        for (auto& p : fs::directory_iterator(path)) // We could make a script class and then a vector of scripts and set that up in 'Init' but I'm too lazy.
        {
            if (p.is_directory()) // The user is able to sort the mods by author, version, etc. by creating a folder in Scripts and naming it whatever and putting the scripts in there.
                InjectScripts(p.path());

            else
            {
                auto filename = p.path().filename().generic_string();

                if (filename == _("script.json"))
                {
                    std::ifstream data(p.path());
                    json j = json::parse(data);
                    data.close();

                    auto lang = ConvertLanguage(j[_("language")]);
                    auto dll_path = path / j[_("filename")];

                    switch (lang)
                    {
                    case CSharp:
                        AmountOfInjects += 1;
                        ExecuteCSharp(dll_path.generic_wstring());
                        break;
                    case CPP:
                        AmountOfInjects += 1;
                        Inject(dll_path.generic_string());
                        break;
                    case JS:
                        AmountOfInjects += 1;
                        ExecuteJS(dll_path.generic_string());
                        break;
                    default:
                        std::cout << _("Could not deduce language!\n");
                        break;
                    }

                }
            }
        }
    }
	
    void Init() // This extracts the zips. If there is ever a launcher, this will not be needed.
    {
        for (const auto& entry : fs::directory_iterator(fs::current_path() / _("Scripts")))
        {
            auto path = entry.path();

            if (!path.filename().generic_string().contains(_(".script")))
            {
                std::cout << _("Path ") << path << _(" is not a valid path!\n");
				// return 0;
			}

            if (!fs::exists(path))
            {
                MessageBoxA(0, _("The file does not exist"), _("Error"), MB_ICONERROR);
                return;
            }

            auto tempPath = fs::temp_directory_path() / _("ScriptsExtracted"); // + std::to_string(rand() % 100)); // TODO: Remove

            if (fs::exists(tempPath))
                fs::remove_all(tempPath);

            if (!fs::create_directory(tempPath))
            {
                MessageBoxA(0, _("Could not create the temporary folder!"), _("Error"), MB_ICONERROR);
                return;
            }
			
            miniz_cpp::zip_file file(path.generic_string());
            
            file.extractall(tempPath.generic_string());

            // Read and parse script.json

            auto scriptPath = tempPath / _("script.json");

            if (!fs::exists(scriptPath))
            {
                MessageBoxA(0, _("Could not find script.json!"), _("Error"), MB_ICONERROR);
                return;
            }
            			
            if (!fs::exists(AppData::Path))
            {
                if (!fs::create_directory(AppData::Path));
                {
                    MessageBoxA(0, _("Unable to create AppData folder!"), _("Scripter"), MB_ICONERROR);
                    return;
                }
            }
            
            std::ifstream data(scriptPath);

            json j = json::parse(data);

            data.close();

            auto newPathStr = (AppData::Path / j[_("script_name")]).generic_string(); // (AppData::Path / AddExtension(j[_("filename")], language)).generic_string();
			
            // newPathStr.erase(newPathStr.find_last_of('.'), 50); // Remove the file extension.

            auto newPath = fs::path(newPathStr);

            if (fs::exists(newPath))
                fs::remove_all(newPath);

            fs::copy(tempPath, newPathStr);

            if (fs::exists(tempPath))
                fs::remove_all(tempPath);
        }

        InjectScripts(AppData::Path);

        return;
    }
}

DWORD WINAPI Main(LPVOID)
{
    AllocConsole();

    FILE* file;
    freopen_s(&file, _("CONOUT$"), _("w"), stdout);

    if (!Setup())
    {
        MessageBoxA(0, _("Failed!"), _("Setup"), MB_OK);
        return 0;
    }
	
    if (!AppData::Init())
    {
        MessageBoxA(0, _("Failed!"), _("AppData"), MB_ICONERROR);
        return 0;
    }

    std::cout << _("Setup for ") << FN_Version << "!\n";

    Scripter::Init();

    if (!AmountOfInjects)
    {
		MessageBoxA(0, _("Could not inject any scripts!"), _("Scripter::Init"), MB_OK);
		return 0;
    }
	
    std::cout << _("\nInitialized ") << AmountOfInjects << _(" Scripts!\n");

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