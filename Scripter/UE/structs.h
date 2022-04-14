#pragma once

#include <Windows.h>
#include <string>
#include <locale>
#include <format>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "enums.h"

using namespace std::chrono;

#define INL __forceinline

static inline void (*ToStringO)(struct FName*, class FString&);
static inline void (*ProcessEventO)(void*, void*, void*);

enum
{
	DEFAULT_ALIGNMENT = 0,
	MIN_ALIGNMENT = 8,
};

namespace FMemory
{
	void (*Free)(void* Original);
	void (*Realloc)(void* Original, SIZE_T Count, uint32_t Alignment /* = DEFAULT_ALIGNMENT */);
}

struct FVector
{
	float X;
	float Y;
	float Z;
};

struct FQuat
{
	float X;
	float Y;
	float Z;
	float W;
};

struct FTransform // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Public/Math/TransformNonVectorized.h#L28
{
	FQuat Rotation;
	FVector Translation;
	char pad_1C[0x4]; // Padding never changes
	FVector Scale3D;
	char pad_2C[0x4];
};

struct Timer
{
	time_point<std::chrono::steady_clock> start, end;
	duration<float> dura;

	Timer()
	{
		start = high_resolution_clock::now();
	}

	~Timer()
	{
		end = high_resolution_clock::now();
		dura = end - start;

		float ms = dura.count() * 1000.0f;
		std::cout << "Took " << ms << "ms \n";
	}
};

template<class TEnum>
struct TEnumAsByte // https://github.com/EpicGames/UnrealEngine/blob/4.21/Engine/Source/Runtime/Core/Public/Containers/EnumAsByte.h#L18
{
	uint8_t Value;
};

template <class ElementType>
class TArray // https://github.com/EpicGames/UnrealEngine/blob/4.21/Engine/Source/Runtime/Core/Public/Containers/Array.h#L305
{
	friend class FString;
protected:
	ElementType* Data = nullptr;
	int32_t ArrayNum = 0;
	int32_t ArrayMax = 0;
public:
	void Free()
	{
		if (!FMemory::Free)
			MessageBoxA(0, "No FMemory::Free!", "Fortnite", MB_ICONERROR);
		// VirtualFree(Data, 0, MEM_RELEASE);
		else
			FMemory::Free(Data);

		Data = nullptr; // incase someone for some reason tries to access it.

		ArrayNum = 0;
		ArrayMax = 0;
	}

	INL ElementType operator[](int Index) const { return Data[Index]; }

	INL ElementType At(int Index) const { return Data[Index]; }

	INL int32_t Slack() const
	{
		return ArrayMax - ArrayNum;
	}

	INL void Reserve(int Number)
	{
		if (!FMmeory::Realloc)
		{
			MessageBoxA(0, "How are you expecting to reserve with no Realloc?", "Fortnite", MB_ICONERROR);
			return;
		}

		if (Number > ArrayMax)
			Data = Slack() >= NumElements ? Data : (ElementType*)FMemory::Realloc(Data, (MaxElements = ArrayNum + NumElements) * sizeof(ElementType), 0); // thanks fischsalat for this line of code.
	}

	INL void Add(const ElementType& New)
	{
		Reserve(1);
		Data[ArrayNum] = New;
		++ArrayNum;
	};

	INL void RemoveSingle(const ElementType& Index) // TODO
	{

	}
};

class FString // https://github.com/EpicGames/UnrealEngine/blob/4.21/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h#L59
{
public:
	TArray<TCHAR> Data;

	void Set(const wchar_t* NewStr) // by fischsalat
	{
		if (!NewStr || std::wcslen(NewStr) == 0) return;

		Data.ArrayMax = Data.ArrayNum = *NewStr ? (int)std::wcslen(NewStr) + 1 : 0;

		if (Data.ArrayNum)
			Data.Data = const_cast<wchar_t*>(NewStr);
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data.Data);
		std::string str(length, '\0');
		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data.Data, Data.Data + length, '?', &str[0]);

		return str;
	}

	INL void operator=(const std::string& str) { Set(std::wstring(str.begin(), str.end()).c_str()); }

	void FreeString()
	{
		Data.Free();
	}

	~FString()
	{
		// FreeString();
	}

	/*

	template <typename StrType>
	FORCEINLINE FString& operator+=(StrType&& Str)	{ return Append(Forward<StrType>(Str)); }

	FORCEINLINE FString& operator+=(ANSICHAR Char) { return AppendChar(Char); }
	FORCEINLINE FString& operator+=(WIDECHAR Char) { return AppendChar(Char); }
	FORCEINLINE FString& operator+=(UCS2CHAR Char) { return AppendChar(Char); }

	*/
};

struct FName // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Public/UObject/NameTypes.h#L403
{
	uint32_t ComparisonIndex;
	uint32_t Number;

	INL std::string ToString()
	{
		FString temp;

		ToStringO(this, temp);

		auto Str = temp.ToString();

		temp.FreeString();

		return Str;
	}
};

struct UObject // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectBase.h#L20
{
	void** VFTable;
	int32_t ObjectFlags;
	int32_t InternalIndex;
	UObject* ClassPrivate; // Keep it an object because the user will have to cast it to the correct type depending on the version.
	FName NamePrivate;
	UObject* OuterPrivate;

	INL std::string GetName() { return NamePrivate.ToString(); }

	INL std::string GetFullName()
	{
		std::string temp;

		for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate)
			temp = std::format("{}.{}", outer->GetName(), temp);

		return std::format("{} {}{}", ClassPrivate->GetName(), temp, this->GetName());
	}

	template <typename MemberType>
	INL MemberType* Member(std::string MemberName);

	INL void ProcessEvent(UObject* Function, void* Params)
	{
		return ProcessEventO(this, Function, Params);
	}
};

struct FUObjectItem // https://github.com/EpicGames/UnrealEngine/blob/4.27/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectArray.h#L26
{
	UObject* Object;
	int32_t Flags;
	int32_t ClusterRootIndex;
	int32_t SerialNumber;
	// int pad_01;
};

struct FFixedUObjectArray
{
	FUObjectItem* Objects;
	int32_t MaxElements;
	int32_t NumElements;

	INL const int32_t Num() const { return NumElements; }

	INL const int32_t Capacity() const { return MaxElements; }

	INL bool IsValidIndex(int32_t Index) const { return Index < Num() && Index >= 0; }

	INL UObject* GetObjectById(int32_t Index) const
	{
		return Objects[Index].Object;
	}
};

struct FChunkedFixedUObjectArray // https://github.com/EpicGames/UnrealEngine/blob/7acbae1c8d1736bb5a0da4f6ed21ccb237bc8851/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectArray.h#L321
{
	enum
	{
		NumElementsPerChunk = 64 * 1024,
	};

	FUObjectItem** Objects;
	FUObjectItem* PreAllocatedObjects;
	int32_t MaxElements;
	int32_t NumElements;
	int32_t MaxChunks;
	int32_t NumChunks;

	INL const int32_t Num() const { return NumElements; }

	INL const int32_t Capacity() const { return MaxElements; }

	INL UObject* GetObjectById(int32_t Index) const
	{
		if (Index > NumElements || Index < 0) return nullptr;

		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;

		if (ChunkIndex > NumChunks) return nullptr;
		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;

		auto obj = (Chunk + WithinChunkIndex)->Object;

		return obj;
	}
};

static FChunkedFixedUObjectArray* ObjObjects;
static FFixedUObjectArray* OldObjects;

template <typename ReturnType = UObject>
static ReturnType* FindObject(const std::string& str, bool bIsEqual = false, bool bIsName = false)
{
	if (bIsName) bIsEqual = true;

	for (int32_t i = 0; i < (ObjObjects ? ObjObjects->Num() : OldObjects->Num()); i++)
	{

		auto Object = ObjObjects ? ObjObjects->GetObjectById(i) : OldObjects->GetObjectById(i);

		auto ObjectName = bIsName ? Object->GetName() : Object->GetFullName(); // bool ? true : false

		if (bIsEqual)
		{
			if (Object && ObjectName == str)
				return (ReturnType*)Object;
		}
		else
		{
			if (Object && ObjectName.contains(str))
				return (ReturnType*)Object;
		}
	}

	return nullptr;
}

struct UField : UObject
{
	UField* Next;
};

struct UFieldNewProps : UObject
{
	UField* Next;
	void* pad_01;
	void* pad_02;
};

struct UProperty : UField
{
	uint32_t ArrayDim; // 0x30
	uint32_t ElementSize; // 0x34
	uint64_t PropertyFlags; // 0x38
	char pad_40[4]; // 0x40
	uint32_t Offset_Internal; // 0x44
	char pad_48[0x70 - 0x48];
};

class UPropertyOld : public UField
{
public:
	int32_t ArrayDim;
	int32_t ElementSize;
	uint64_t PropertyFlags;
	uint16_t RepIndex;
	TEnumAsByte<ELifetimeCondition> BlueprintReplicationCondition;
	int32_t Offset_Internal;
	FName RepNotifyFunc;
	UPropertyOld* PropertyLinkNext;
	UPropertyOld* NextRef;
	UPropertyOld* DestructorLinkNext;
	UPropertyOld* PostConstructLinkNext;
};

static double FN_Version;
static int Engine_Version;

struct FRotator
{
	float Pitch;
	float Yaw;
	float Roll;
};

struct FField
{
	void** VFT;
	void* ClassPrivate;
	void* Owner;
	void* pad;
	FField* Next;
	FName Name;
	EObjectFlags FlagsPrivate;

	std::string GetName()
	{
		return Name.ToString();
	}
};

class FProperty : public FField
{
public:
	int32_t	ArrayDim;
	int32_t	ElementSize;
	EPropertyFlags PropertyFlags;
	uint16_t RepIndex;
	TEnumAsByte<ELifetimeCondition> BlueprintReplicationCondition;
	int32_t	Offset_Internal;
	FName RepNotifyFunc;
	FProperty* PropertyLinkNext;
	FProperty* NextRef;
	FProperty* DestructorLinkNext;
	FProperty* PostConstructLinkNext;
};

class UStruct : public UFieldNewProps
{
public:
	UStruct* SuperStruct;
	UFieldNewProps* Children;
	FField* ChildProperties;
	int32_t PropertiesSize;
	int32_t MinAlignment;
	TArray<uint8_t> Script;
	FProperty* PropertyLink;
	FProperty* RefLink;
	FProperty* DestructorLink;
	FProperty* PostConstructLink;
	TArray<UObject*> ScriptAndPropertyObjectReferences;
};

struct UClass : public UStruct
{

};

struct UStructOld : public UField
{
	UStructOld* SuperStruct;
	UField* ChildProperties; // Children
	int32_t PropertiesSize;
	int32_t MinAlignment;
	UProperty* PropertyLink;
	UProperty* RefLink;
	UProperty* DestructorLink;
	UProperty* PostConstructLink;
	TArray<UObject*> ScriptObjectReferences;
};

struct UClassOld : UStructOld
{

};

struct UStructOldest : public UField
{
	UStructOldest* SuperStruct;
	UField* ChildProperties; // Children
	int32_t PropertiesSize;
	int32_t MinAlignment;
	TArray<uint8_t> Script;
	UPropertyOld* PropertyLink;
	UPropertyOld* RefLink;
	UPropertyOld* DestructorLink;
	UPropertyOld* PostConstructLink;
	TArray<UObject*> ScriptObjectReferences;
};

struct UClassOldest : public UStructOldest
{

};

template <typename ClassType, typename PropertyType>
int LoopMembersAndFindOffset(UObject* Object, const std::string& MemberName)
{
	// We loop through the whole class hierarchy to find the offset.

	for (auto CurrentClass = (ClassType*)Object->ClassPrivate; CurrentClass; CurrentClass = (ClassType*)CurrentClass->SuperStruct)
	{
		auto Property = CurrentClass->ChildProperties;

		while (Property)
		{
			std::cout << Property->GetName() << '\n';

			if (Property->GetName() == MemberName)
				return ((PropertyType*)Property)->Offset_Internal;

			Property = Property->Next;
		}
	}
}

static int GetOffset(UObject* Object, const std::string& MemberName)
{
	if (!MemberName.contains(" /") && Object)
	{
		if (Engine_Version <= 420)
			return LoopMembersAndFindOffset<UClassOldest, UPropertyOld>(Object, MemberName);

		else if (Engine_Version >= 421 && Engine_Version <= 424)
			return LoopMembersAndFindOffset<UClassOld, UProperty>(Object, MemberName);

		else if (Engine_Version >= 425)
			return LoopMembersAndFindOffset<UClass, FProperty>(Object, MemberName);
	}

	return 0;
}

template <typename MemberType>
INL MemberType* UObject::Member(std::string MemberName)
{
	MemberName.erase(0, MemberName.find_last_of(".", MemberName.length() - 1) + 1);

	return (MemberType*)(__int64(this) + GetOffset(this, MemberName));
}

FString(*GetEngineVersion)();