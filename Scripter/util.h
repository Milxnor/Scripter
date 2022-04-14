#include <Windows.h>

#include <UE/structs.h>

static uint64_t FindPattern(const char* signature, bool bRelative = false, uint32_t offset = 0, bool bIsVar = false)
{
	auto base_address = (uint64_t)GetModuleHandleW(NULL);
	static auto patternToByte = [](const char* pattern)
	{
		auto bytes = std::vector<int>{};
		const auto start = const_cast<char*>(pattern);
		const auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?') ++current;
				bytes.push_back(-1);
			}
			else { bytes.push_back(strtoul(current, &current, 16)); }
		}
		return bytes;
	};

	const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = patternToByte(signature);
	const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);

	const auto s = patternBytes.size();
	const auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i)
	{
		bool found = true;
		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			auto address = (uint64_t)&scanBytes[i];
			if (bIsVar)
				address = (address + offset + *(int*)(address + 3));
			if (bRelative && !bIsVar)
				address = ((address + offset + 4) + *(int*)(address + offset));
			return address;
		}
	}
	return NULL;
}

bool Setup() // TODO: Add Realloc
{
	GetEngineVersion = decltype(GetEngineVersion)(FindPattern("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B C8 41 B8 04 ? ? ? 48 8B D3"));

	if (!GetEngineVersion)
	{
		GetEngineVersion = decltype(GetEngineVersion)(FindPattern("")); // TODO: Put new pattern

		if (!GetEngineVersion)
		{
			MessageBoxA(NULL, "Failed to find UKismetSystemLibrary::GetEngineVersion", "Fortnite", MB_OK);
			return false;
		}
	}

	auto FullVersion = GetEngineVersion().ToString();

	std::string FNVer = FullVersion;
	std::string EngineVer = FullVersion;

	FNVer.erase(0, FNVer.find_last_of("-", FNVer.length() - 1) + 1);
	EngineVer.erase(EngineVer.find_first_of("-", FNVer.length() - 1), 40);

	if (EngineVer.find_first_of(".") != EngineVer.find_last_of("."))
		EngineVer.erase(EngineVer.find_last_of("."), 2);

	FN_Version = std::stod(FNVer);
	Engine_Version = std::stod(EngineVer) * 100;

	ObjObjects = decltype(ObjObjects)(FindPattern("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB 03 48 8B ? 81 48 08 ? ? ? 40 49", false, 7, true));

	if (!ObjObjects)
	{
		MessageBoxA(NULL, "Failed to find FUObjectArray::ObjObjects", "Fortnite", MB_OK);
		return false;
	}

	uint64_t ToStringAddr = 0;
	uint64_t ProcessEventAddr = 0;
	uint64_t FreeMemoryAddr = 0;

	if (Engine_Version >= 421 && Engine_Version <= 424)
	{
		ToStringAddr = FindPattern("48 89 5C 24 ? 57 48 83 EC 30 83 79 04 00 48 8B DA 48 8B F9");
		ProcessEventAddr = FindPattern("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41");
	}

	if (Engine_Version >= 425)
	{
		ToStringAddr = FindPattern("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 30 8B 01 48 8B F1 44 8B 49 04 8B F8 C1 EF 10 48 8B DA 0F B7 C8 89 4D 24 89 7D 20 45 85 C9");
		ProcessEventAddr = FindPattern("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6");
	}

	if (Engine_Version >= 421 && Engine_Version <= 426)
	{
		FreeMemoryAddr = FindPattern("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9");
	}

	if (!ToStringAddr)
	{
		MessageBoxA(NULL, "Failed to find FName::ToString", "Fortnite", MB_OK);
		return false;
	}

	ToStringO = decltype(ToStringO)(ToStringAddr);

	if (!ProcessEventAddr)
	{
		MessageBoxA(NULL, "Failed to find UObject::ProcessEvent", "Fortnite", MB_OK);
		return false;
	}

	ProcessEventO = decltype(ProcessEventO)(ProcessEventAddr);

	if (!FreeMemoryAddr)
	{
		MessageBoxA(NULL, "Failed to find FMemory::Free", "Fortnite", MB_OK);
		return false;
	}

	FMemory::Free = decltype(FMemory::Free)(FreeMemoryAddr);

}