#include <Windows.h>

#include <UE/structs.h>
#include <filesystem>

// dotnet includes

#include <mscoree.h>
#include <metahost.h>
#pragma comment(lib, "mscoree.lib")

namespace fs = std::filesystem;

bool Inject(const std::string& DLLName) // TODO: Remake // skidded go brr https://github.com/LouisTheXIV/DLL-Injection-Cpp/blob/main/Injector/Injector/Injector/Injector.cpp
{
	char dll_path[MAX_PATH];
	
	if (!GetFullPathNameA(DLLName.c_str(), MAX_PATH, dll_path, nullptr)) {
		MessageBoxA(0, "Failed to get full path of DLL!", "GetFullPathName", MB_ICONERROR);
		return false;
	}
	
	auto ProcID = GetCurrentProcessId();
	
	if (!ProcID) {
		MessageBoxA(0, "Failed to get process id!", "GetProcessID", MB_ICONERROR);
		return false;
	}

	HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, NULL, ProcID);
	
	if (!h_process) {
		MessageBoxA(0, "Failed to open a handle to process!", "OpenProcess", MB_ICONERROR);
		return false;
	}
	
	void* allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	if (!allocated_memory) {
		MessageBoxA(0, "Failed to allocate memory!", "AllocateMemory", MB_ICONERROR);
		return false;
	}

	if (!WriteProcessMemory(h_process, allocated_memory, dll_path, MAX_PATH, nullptr)) {
		MessageBoxA(0, "Failed to write to process memory!", "WriteProcessMemory", MB_ICONERROR);
		return false;
	}

	HANDLE h_thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocated_memory, NULL, nullptr);
	if (!h_thread) {
		MessageBoxA(0, "Failed to create remote thread!", "CreateRemoteThread", MB_ICONERROR);
		return false;
	}

	CloseHandle(h_process);
	VirtualFreeEx(h_process, allocated_memory, NULL, MEM_RELEASE);
	
	return true;
}

bool InjectCSharp(const std::wstring& DLLName) // https://github.com/Vacko/Managed-code-injection/blob/master/Bootstrap/DllMain.cpp
{
	ICLRRuntimeHost* lpRuntimeHost = NULL;
	ICLRRuntimeInfo* lpRuntimeInfo = NULL;
	ICLRMetaHost* lpMetaHost = NULL;
	
	HRESULT hr = CLRCreateInstance(
		CLSID_CLRMetaHost,
		IID_ICLRMetaHost,
		(LPVOID*)&lpMetaHost
	);

	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed.\n", "CLRCreateInstance", MB_ICONERROR);

		return 0;
	}

	hr = lpMetaHost->GetRuntime(
		L"v4.0.30319",
		IID_PPV_ARGS(&lpRuntimeInfo)
	);

	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed.", "GetRuntime", MB_ICONERROR);

		lpMetaHost->Release();

		return false;
	}

	BOOL fLoadable;
	hr = lpRuntimeInfo->IsLoadable(&fLoadable);

	if (FAILED(hr) || !fLoadable)
	{
		MessageBoxA(0, "Failed.", "IsLoadable", MB_ICONERROR);

		lpRuntimeInfo->Release();
		lpMetaHost->Release();

		return false;
	}

	hr = lpRuntimeInfo->GetInterface(
		CLSID_CLRRuntimeHost,
		IID_PPV_ARGS(&lpRuntimeHost)
	);

	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed.", "GetInterface", MB_ICONERROR);

		lpRuntimeInfo->Release();
		lpMetaHost->Release();

		return false;
	}

	hr = lpRuntimeHost->Start();

	if (FAILED(hr))
	{
		MessageBoxA(0, std::string("Failed " + std::to_string(hr) + '.').c_str(), "CLRStart", MB_ICONERROR);

		lpRuntimeHost->Release();
		lpRuntimeInfo->Release();
		lpMetaHost->Release();

		return false;
	}

	DWORD dwRetCode = 0;

	hr = lpRuntimeHost->ExecuteInDefaultAppDomain(
		DLLName.c_str(),
		L"Scripter.Main",
		L"Startup",
		L"",
		&dwRetCode
	);

	if (FAILED(hr))
	{
		MessageBoxA(0, std::string("Failed " + std::to_string(hr) + '.').c_str(), "ExecuteInDefaultAppDomain", MB_ICONERROR);

		lpRuntimeHost->Stop();
		lpRuntimeHost->Release();
		lpRuntimeInfo->Release();
		lpMetaHost->Release();

		return false;
	}
	
	return true;
}

namespace AppData
{
	static fs::path Path;

	bool Init()
	{
		char* buf{};
		size_t size = MAX_PATH;
		_dupenv_s(&buf, &size, "LOCALAPPDATA");
		Path = fs::path(buf) / "Scripts";

		bool res = true;

		if (!fs::exists(Path))
			res = fs::create_directory(Path);

		return res;
	}
}

enum Languages
{
	CSharp,
	JS,
	UNKNOWN
};

Languages ConvertLanguage(std::string Lang)
{
	std::transform(Lang.begin(), Lang.end(), Lang.begin(), ::tolower);

	if (Lang == "c#" || Lang == "csharp" || Lang == "c sharp")
		return Languages::CSharp;

	else if (Lang == "js" || Lang == "javascript" || Lang == "java script" || Lang == "nodejs")
		return Languages::JS;

	return Languages::UNKNOWN;
}