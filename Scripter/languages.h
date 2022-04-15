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
		std::cout << "FindObject called!\n";
		return bruh(name);
	}

	DLL_EXPORT const char* UObject_GetFullName(UObject* Object)
	{
		return Object->GetFullName().c_str();
	}
}