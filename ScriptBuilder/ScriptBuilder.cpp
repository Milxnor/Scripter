#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <Windows.h>

#include <json.hpp>
#include <zip_file.hpp>

using namespace nlohmann;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) // TODO: add args like ScriptBuilder.exe path="path" language="C#" description="this is made in F#" outputdir="output"
{
	if (argc < 1)
	{
		
	}

	std::string dllpathstr;
	std::cout << "Enter dll path: ";
	std::getline(std::cin, dllpathstr);

	auto DllPath = fs::path(dllpathstr);

	if (!fs::exists(DllPath))
	{
		std::cout << "Could not find path!\n";
		Sleep(2500);
		std::exit(0);
	}

	auto name = DllPath.filename().generic_string();

	if (!name.contains(".dll"))
	{
		std::cout << "The file is not a dll!\n";
		Sleep(2500);
		std::exit(0);
	}

	std::string language;
	std::cout << "Enter language: ";
	std::getline(std::cin, language);

	std::string description;
	std::cout << "Enter description: ";
	std::getline(std::cin, description);

	ordered_json j;
	j["script_name"] = name;
	j["language"] = language;
	j["description"] = description; 
	
	std::ofstream stream("script.json");

	stream << j.dump(4);

	stream.close();

	miniz_cpp::zip_file file;

	// the lib does this weird thing so we have to copy it to current directory.
	
	fs::copy(dllpathstr, name);
	
	file.write(name);

	file.write("script.json");

	file.save(name.erase(name.length() - 4, 4) + ".script");

	std::cout << "Saved to " + fs::absolute(name + ".script").generic_string() << ".\n";

	fs::remove("script.json");
	fs::remove(name + ".dll");

	std::cin.get();
}