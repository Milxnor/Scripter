#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <Windows.h>

#include <json.hpp>
#include <zip_file.hpp>
#include <xorstr.hpp>

using namespace nlohmann;
namespace fs = std::filesystem;

enum Languages
{
	CSharp,
	CPP,
	JS,
	UNKNOWN
};

Languages ConvertLanguage(std::string Lang)
{
	std::transform(Lang.begin(), Lang.end(), Lang.begin(), ::tolower);

	if (Lang == _("c#") || Lang == _("csharp") || Lang == _("c sharp") || Lang == _("cs"))
		return Languages::CSharp;

	else if (Lang == _("cpp") || Lang == _("c++") || Lang == _("c plus plus"))
		return Languages::CPP;

	else if (Lang == _("js") || Lang == _("javascript") || Lang == _("java script") || Lang == _("nodejs"))
		return Languages::JS;

	return Languages::UNKNOWN;
}

int main(int argc, char* argv[]) // TODO: add args like ScriptBuilder.exe path="path" language="C#" description="this is made in F#" outputdir="output"
{
	if (argc < 1)
	{
		
	}

	std::string dllpathstr;
	std::cout << _("Enter dll path: ");
	std::getline(std::cin, dllpathstr);

	auto DllPath = fs::path(dllpathstr);

	if (!fs::exists(DllPath))
	{
		std::cout << _("Could not find path!\n");
		Sleep(2500);
		std::exit(0);
	}

	auto name = DllPath.filename().generic_string();
	const auto fileName = name; // TODO: Fix alot of this and dont copy so many variables;

	if (fs::exists(_("script.json")))
		fs::remove(_("script.json"));

	if (fs::exists(name) && name != DllPath)
		fs::remove(name);

	if (!name.contains(".dll") && !name.contains(".js"))
	{
		std::cout << _("The file is not a dll or js file!\n");
		Sleep(2500);
		std::exit(0);
	}

	std::string ScriptName;
	std::cout << _("Enter script name: ");
	std::getline(std::cin, ScriptName);

	std::string language;
	std::cout << _("Enter language: ");
	std::getline(std::cin, language);

	std::string description;
	std::cout << _("Enter description: ");
	std::getline(std::cin, description);

	ordered_json j;
	j[_("script_name")] = fileName;
	j[_("language")] = language;
	j[_("description")] = description; 

	name = ScriptName;
	
	std::ofstream stream(_("script.json"));

	stream << j.dump(4);

	stream.close();

	miniz_cpp::zip_file file;

	// the lib does this weird thing so we have to copy it to current directory.
	
	fs::copy(dllpathstr, name);

	auto lang = ConvertLanguage(j[_("language")]);

	switch (lang)
	{
	case CSharp:
		file.write(fileName);
		break;
	case CPP:
		file.write(fileName);
		break;
	case JS:
		file.write(fileName);
		break;
	}
	
	file.write(_("script.json"));

	auto postoremove = name.find_last_of('.');

	if (postoremove != std::string::npos)
		name.erase(name.find_last_of('.'), 50);

	const auto script = name + ".script";

	if (fs::exists(script))
		fs::remove(script);

	file.save(script);

	std::cout << _("Saved to ") + fs::absolute(script).generic_string() << ".\n";

	if (fs::exists(_("script.json")))
		fs::remove(_("script.json"));

	if (fs::exists(fileName))
		fs::remove(fileName);

	std::cin.get();
}