#include <iostream>
#include <string>
#include <Windows.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

HMODULE getHandle() {
	HMODULE hModule = nullptr;
	GetModuleHandleExA(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCSTR>(&getHandle),
		&hModule
	);
	return hModule;
}

struct h {
	static std::string metadata;
};

std::string h::metadata;

__declspec(dllexport) void entry(lua_State* L) {

	HMODULE hModule = getHandle();

	HRSRC res = FindResource(hModule, TEXT("SERPENTLUA_METADATA"), RT_RCDATA);

	HGLOBAL hRes = LoadResource(hModule, res);
	void* data = LockResource(hRes);
	DWORD size = SizeofResource(hModule, res);

	std::string metadata(reinterpret_cast<char*>(data), size);
	h::metadata = metadata;

	lua_pushcfunction(L, [](lua_State* L) -> int {
		std::cout << "METADATA FROM THE DLL BOI!!!!" << "\n" << h::metadata;
		
		return 0;
	});
	lua_setglobal(L, "the_Function");
}

#ifdef __cplusplus
}
#endif