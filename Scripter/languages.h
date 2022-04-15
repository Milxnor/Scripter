#pragma once

#include <UE/structs.h>

//  https://youtu.be/o-ass4mkdiA

#define DLL_EXPORT _declspec(dllexport)

auto bruh(const char* name)
{
	return FindObject(name);
}

extern "C" {
	DLL_EXPORT UObject* FindObject(const char* name)
	{
		return bruh(name);
	}

	DLL_EXPORT const char* GetFullName(UObject* Object)
	{
		return Object->GetFullName().c_str();
	}

	DLL_EXPORT void* Member(UObject* Object, const char* MemberName)
	{
		return Object->Member<void>(MemberName);
	}

	DLL_EXPORT void ProcessEvent(UObject* Object, UObject* Function, void* Params)
	{
		// if (Object && Function)
		Object->ProcessEvent(Function, Params);
	}
}