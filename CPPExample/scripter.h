#pragma once

#include <Windows.h>
#include <cstdint>
#include <string>

struct FName
{
	uint32_t ComparisonIndex;
	uint32_t Number;
};

struct UObject;

extern "C" 
{
	namespace Imports
	{
		UObject* FindObject(const char* name);

		const char* GetFullName(UObject* Object);

		void* Member(UObject* Object, const char* MemberName);

		void ProcessEvent(UObject* Object, UObject* Function, void* Params);
	}
}

using namespace Imports;

struct UObject
{
	void** VFTable;
	int32_t ObjectFlags;
	int32_t InternalIndex;
	UObject* ClassPrivate;
	FName NamePrivate;
	UObject* OuterPrivate;

	template <typename T>
	T* Member(const std::string& MemberName)
	{
		return (T*)Imports::Member(this, MemberName);
	}

	auto GetFullName()
	{
		return Imports::GetFullName(this);
	}

	auto ProcessEvent(auto Fn, void* Params)
	{
		Imports::ProcessEvent(this, Fn, nullptr);
	}
};