#pragma once

#include <UE/structs.h>
#include <duktape/duktape.h>

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

	DLL_EXPORT auto ProcessEvent(UObject* Object, UObject* Function, void* Params)
	{
		// if (Object && Function)
		return Object->ProcessEvent(Function, Params);
	}
	
	DLL_EXPORT auto ProcessEventStr(UObject* Object, const char* FuncName, void* Params)
	{
		return Object->ProcessEvent(FuncName, Params);
	}
	
	DLL_EXPORT void cout(const char* str)
	{
		std::cout << str << '\n';
	}
}

static bool bHasDuktapeInitialized = false;

static duk_ret_t cout(duk_context* ctx) {
	std::cout << duk_to_string(ctx, 0) << '\n';
	return 0;  /* no return value (= undefined) */
}

static duk_ret_t FindObject(duk_context* ctx) {
	auto objName = duk_to_string(ctx, 0);
	auto obj = FindObject(duk_to_string(ctx, 0));
	
	if (!obj)
	{
		std::cout << "Object not found: " << objName << '\n';
		return 0;
	}
	
	duk_push_pointer(ctx, obj);
	
	return 1;  
}

duk_context* ctx;

void initDuktape()
{
	if (bHasDuktapeInitialized)
		return;

	ctx = duk_create_heap_default();
	
	duk_push_c_function(ctx, cout, 1 /*nargs*/);
	duk_put_global_string(ctx, "cout");

	duk_push_c_function(ctx, cout, 1 /*nargs*/);
	duk_put_global_string(ctx, "cout");

	bHasDuktapeInitialized = true;
}