#pragma once

#include <UE/structs.h>

//  https://youtu.be/o-ass4mkdiA

#define DLL_EXPORT _declspec(dllexport)

extern "C" {
	DLL_EXPORT UObject* FindObject(const std::string& name)
	{
		return FindObject(name);
	}
}