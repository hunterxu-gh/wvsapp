#include "Windows.h"
#include "stdio.h"
#include "../WvsCommon/Config.h"
#include "../WvsCommon/AesKey.h"
#include "../WvsCommon/Helper.h"
#include "../WvsCommon/Hook.h"
#include "../WvsCommon/ZSocketEx.h"
#include "../WvsCommon/Config.h"
#include "CClientSocket.h"

FARPROC CoInitializeHookAddy;
FARPROC CoUninitializeHookAddy;
MAPLE_PTR CSecuritySession_PreProcessPacket_ptr = CAST_MAPLE_PTR(0x00426477);


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


void installHook(){
	HMODULE hHandle = LoadLibraryA("ole32.dll");
	CoInitializeHookAddy = GetProcAddress(hHandle, "CoInitialize");
	CoUninitializeHookAddy = GetProcAddress(hHandle, "CoUninitialize");
	
	AesKey::setUpAesKey((int*)0x00546260);
	
	Hook::hookCall(CSecuritySession_PreProcessPacket_ptr, forceCast(&CSecuritySession::PreProcessPacket));

	ZSocketEx::SendPacketHook_Original_ptr = (void*)0x00482CA0;
	Hook::hookCall((BYTE*)0x0047F131, forceCast(&ZSocketEx::SendPacket));

	DWORD PacketHeaderCheck_ptr = 0x00425FF1 + 1;
	Hook::protect(PacketHeaderCheck_ptr, 4);
	*reinterpret_cast<__int16*>(PacketHeaderCheck_ptr) = 0xFFFF - VERSION;

	DWORD HeaderCheck_1 = 0x00426231 + 2;
	Hook::protect(HeaderCheck_1, 1);
	*reinterpret_cast<char*>(HeaderCheck_1) = VERSION;
	
}


 


BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved) { 
	switch ( dwReason ) { 
		case DLL_PROCESS_ATTACH:
#if __ENABLE_CONSOLE__
			AllocConsole();
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
#endif
			DisableThreadLibraryCalls(hModule);	
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