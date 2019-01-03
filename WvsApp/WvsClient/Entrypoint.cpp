#include <Windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include "CClientSocket.h"
#include "GGBypass.h"
#include "AesKeyInitializer.h"
#include "StringPool.h"
#include "MemoryCheck.h"
#include "../WvsCommon/Config.h"
#include "CX_ExceptionDebug.h"
#include "addy.h"
#include "WindowHax.h"


extern MemoryCheck * memCheckInstance;

DWORD ijlErrorStrHookAddy;
DWORD ijlFreeHookAddy;
DWORD ijlGetLibVersionHookAddy;
DWORD ijlInitHookAddy;
DWORD ijlReadHookAddy;
DWORD ijlWriteHookAddy;


bool isAdminClient() {
	BYTE *bAddy = (BYTE*)0x004FFFE9;
	return *(bAddy) == 0x50 && *(bAddy + 1) == 0xE8;
}


void _declspec(naked) ijlErrorStr() {
	__asm {
		jmp dword ptr ds: ijlErrorStrHookAddy
	}
}

void _declspec(naked) ijlFree() {
	__asm {
		jmp dword ptr ds: ijlFreeHookAddy
	}
}
void _declspec(naked) ijlGetLibVersion() {
	__asm {
		jmp dword ptr ds: ijlGetLibVersionHookAddy
	}
}
void _declspec(naked) ijlInit() {
	__asm {
		jmp dword ptr ds: ijlInitHookAddy
	}
}
void _declspec(naked) ijlRead() {
	__asm {
		jmp dword ptr ds: ijlReadHookAddy
	}
}
void _declspec(naked) ijlWrite() {
	__asm {
		jmp dword ptr ds: ijlWriteHookAddy
	}
}



void installHook() {

	HMODULE hHandle = LoadLibraryA("suipre.dll");
	ijlErrorStrHookAddy = (DWORD) GetProcAddress(hHandle, "ijlErrorStr");
	ijlFreeHookAddy = (DWORD) GetProcAddress(hHandle, "ijlFree");
	ijlGetLibVersionHookAddy = (DWORD)GetProcAddress(hHandle, "ijlGetLibVersion");
	ijlInitHookAddy = (DWORD)GetProcAddress(hHandle, "ijlInit");
	ijlReadHookAddy = (DWORD)GetProcAddress(hHandle, "ijlRead");
	ijlWriteHookAddy = (DWORD)GetProcAddress(hHandle, "ijlWrite");

}


void installBypass() {
#if !__PRODUCTION__ 
	if (isAdminClient()) {
		AesKeyInitializer::patchAesKeyAdmin();
		return;
	}
	Sleep(1000);
#endif
	
	if (!isAdminClient()) {
		GGBypass::hookFunctions();
		StringPool::hook_StringPoolGetString();
        Hook::hook(CClientSocket__ProcessPacket_ptr, forceCast(&CClientSocket::ProcessPacket));
		Hook::hook(CClientSocket__SendPacket_ptr, forceCast(&CClientSocket::SendPacket));
		
		AesKeyInitializer::patchAesKey();
#if IS_GMS
		//Hook::hook(CX_ThrowExpBlock_ptr, (DWORD)&_CxxThrowException_Hook);
#endif
        memCheckInstance->initBlocks();
	}
	
	
}




bool isInitialized = false;
// Explained in p. 2 below
void NTAPI tls_callback(PVOID DllHandle, DWORD dwReason, PVOID pContext)
{
	
	if (!isInitialized) {
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&installBypass, 0, 0, 0);
		installHook();
		isInitialized = true;
	}


}

#ifdef _WIN64
#pragma comment (linker, "/INCLUDE:_tls_used")  // See p. 1 below
#pragma comment (linker, "/INCLUDE:tls_callback_func")  // See p. 3 below
#else
#pragma comment (linker, "/INCLUDE:__tls_used")  // See p. 1 below
#pragma comment (linker, "/INCLUDE:_tls_callback_func")  // See p. 3 below
#endif

// Explained in p. 3 below
#ifdef _WIN64
#pragma const_seg(".CRT$XLF")
EXTERN_C const
#else
#pragma data_seg(".CRT$XLF")
EXTERN_C
#endif
PIMAGE_TLS_CALLBACK tls_callback_func = tls_callback;
#ifdef _WIN64
#pragma const_seg()
#else
#pragma data_seg()
#endif //_WIN64

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	ExitThread(0);
}




BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(hModule);
#if __ENABLE_CONSOLE__
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
#endif 
		
		break;
	}
	case DLL_PROCESS_DETACH:
		ExitProcess(0);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

