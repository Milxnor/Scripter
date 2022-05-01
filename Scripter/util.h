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
		MessageBoxA(0, _("Failed to get full path of DLL!"), _("GetFullPathName"), MB_ICONERROR);
		return false;
	}
	
	auto ProcID = GetCurrentProcessId();
	
	if (!ProcID) {
		MessageBoxA(0, _("Failed to get process id!"), _("GetProcessID"), MB_ICONERROR);
		return false;
	}

	HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, NULL, ProcID);
	
	if (!h_process) {
		MessageBoxA(0, _("Failed to open a handle to process!"), _("OpenProcess"), MB_ICONERROR);
		return false;
	}
	
	void* allocated_memory = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	if (!allocated_memory) {
		MessageBoxA(0, _("Failed to allocate memory!"), _("AllocateMemory"), MB_ICONERROR);
		return false;
	}

	if (!WriteProcessMemory(h_process, allocated_memory, dll_path, MAX_PATH, nullptr)) {
		MessageBoxA(0, _("Failed to write to process memory!"), _("WriteProcessMemory"), MB_ICONERROR);
		return false;
	}

	HANDLE h_thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocated_memory, NULL, nullptr);
	if (!h_thread) {
		MessageBoxA(0, _("Failed to create remote thread!"), _("CreateRemoteThread"), MB_ICONERROR);
		return false;
	}

	CloseHandle(h_process);
	VirtualFreeEx(h_process, allocated_memory, NULL, MEM_RELEASE);
	
	return true;
}

bool ExecuteCSharp(const std::wstring& DLLName) // https://github.com/Vacko/Managed-code-injection/blob/master/Bootstrap/DllMain.cpp // TODO: Return error string instead.
{
	MessageBoxW(0, DLLName.c_str(), DLLName.c_str(), MB_ICONINFORMATION);
	
	ICLRRuntimeHost* lpRuntimeHost = nullptr;
	ICLRRuntimeInfo* lpRuntimeInfo = nullptr;
	ICLRMetaHost* lpMetaHost = nullptr;
	
	HRESULT hr = CLRCreateInstance(
		CLSID_CLRMetaHost,
		IID_ICLRMetaHost,
		(LPVOID*)&lpMetaHost
	);

	if (FAILED(hr))
	{
		MessageBoxA(0, _("Failed.\n"), _("CLRCreateInstance"), MB_ICONERROR);

		return 0;
	}

	hr = lpMetaHost->GetRuntime(
		L"v4.0.30319",
		IID_PPV_ARGS(&lpRuntimeInfo)
	);

	if (FAILED(hr))
	{
		MessageBoxA(0, _("Failed."), _("GetRuntime"), MB_ICONERROR);

		lpMetaHost->Release();

		return false;
	}

	BOOL fLoadable;
	hr = lpRuntimeInfo->IsLoadable(&fLoadable);

	if (FAILED(hr) || !fLoadable)
	{
		MessageBoxA(0, _("Failed."), _("IsLoadable"), MB_ICONERROR);

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
		MessageBoxA(0, _("Failed."), _("GetInterface"), MB_ICONERROR);

		lpRuntimeInfo->Release();
		lpMetaHost->Release();

		return false;
	}

	hr = lpRuntimeHost->Start();

	if (FAILED(hr))
	{
		MessageBoxA(0, std::string(_("Failed ") + std::to_string(hr) + _(".")).c_str(), _("CLRStart"), MB_ICONERROR);

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
		MessageBoxA(0, std::format(_("Failed to load {} with error code {}."), std::string(DLLName.begin(), DLLName.end()), std::to_string(hr) + '.').c_str(), _("ExecuteInDefaultAppDomain"), MB_ICONERROR);

		lpRuntimeHost->Stop();
		lpRuntimeHost->Release();
		lpRuntimeInfo->Release();
		lpMetaHost->Release();

		return false;
	}
	
	return true;
}

bool ExecuteJS(const std::string& Path)
{
	initDuktape();

	std::ifstream input_file(Path);
	
	if (!input_file.is_open()) return false;

	auto code = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>()).c_str();

	duk_eval_string_noresult(ctx, code);

	return true;
}

namespace AppData
{
	static fs::path Path;

	bool Init()
	{
		char* buf{};
		size_t size = MAX_PATH;
		_dupenv_s(&buf, &size, _("LOCALAPPDATA"));
		Path = fs::path(buf) / _("Scripts");
		
		bool res = true;

		if (!fs::exists(Path))
			res = fs::create_directory(Path);

		return res;
	}
}

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

std::string AddExtension(const std::string& Path, Languages lang)
{	
	if (Path.contains("."))
		return Path;
	
	switch (lang)
	{
	case Languages::CSharp:
		return Path + ".dll";
	case Languages::CPP:
		return Path + ".dll";
	case Languages::JS:
		return Path + ".js";
	default:
		return Path;
	}
}