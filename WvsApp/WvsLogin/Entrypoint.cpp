#include "Windows.h"
#include "stdio.h"
#include "../WvsCommon/ZSocketEx.h"
#include "WvsLogin.h"
#include "../WvsCommon/AesKey.h"
#include "../WvsCommon/Config.h"
#include "../WvsCommon/AvatarLook.h"
#include "../WvsCommon/GW_CharacterStat.h"


FARPROC CoInitializeHookAddy;
FARPROC CoUninitializeHookAddy;
FARPROC CoTaskMemFreeHookAddy;


void _declspec(naked) CoInitialize(struct ZIStream *){
	__asm{
		jmp [CoUninitializeHookAddy]
	}
}

void _declspec(naked) CoUninitialize(struct ZIStream *){
	__asm{
		jmp [CoInitializeHookAddy]
	}
}

void _declspec(naked) CoTaskMemFree(struct ZIStream *){
	__asm{
		jmp [CoTaskMemFreeHookAddy]
	}
}

void installHook(){
	HMODULE hHandle = LoadLibraryA("ole32.dll");
	CoInitializeHookAddy = GetProcAddress(hHandle, "CoInitialize");
	CoUninitializeHookAddy = GetProcAddress(hHandle, "CoUninitialize");
	CoTaskMemFreeHookAddy = GetProcAddress(hHandle, "CoTaskMemFree");


	WvsLogin::setupHook();
	AesKey::setUpAesKey((int*)0x00454044);
	//Disable PreProcessPacket
	Hook::changeByte((BYTE*)0x0040DAD0, 0xEB);
	
	ZSocketEx::SendPacketHook_Original_ptr = (void*) 0x0041EACE;
	Hook::hookCall((BYTE*)0x0041D45C, forceCast(&ZSocketEx::SendPacket));

	// Adjust the version of the server
	DWORD PacketHeaderCheck_ptr = 0x0040D488 + 1;
	Hook::protect(PacketHeaderCheck_ptr, 4);
	*reinterpret_cast<__int16*>(PacketHeaderCheck_ptr) = 0xFFFF - VERSION;

	//Disable CSecuritySession onUpdate
	Hook::ret(0x0042A416, 4);
	
	
}



BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved) { 
	switch ( dwReason ) { 
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hModule);	
			#if __ENABLE_CONSOLE__
				AllocConsole();
				freopen("CONIN$", "r", stdin);
				freopen("CONOUT$", "w", stdout);
				freopen("CONOUT$", "w", stderr);
			#endif
			CreateThread(0,0,(LPTHREAD_START_ROUTINE)&installHook,0,0,0);
			break; 
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