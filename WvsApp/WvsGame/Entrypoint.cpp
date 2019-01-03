#include "windows.h"
#include "stdio.h"
#include "WvsGame.h"
#include "ExpTable.h"
#include "../WvsCommon/AesKey.h"
#include "../WvsCommon/Config.h"
#include "../WvsCommon/GW_CharacterStat.h"
#include "../WvsCommon/AvatarLook.h"
#include "../WvsCommon/ZSocketEx.h"

#define ENABLE_CONSOLE 1



FARPROC CoInitializeHookAddy;
FARPROC CoUninitializeHookAddy;


void _declspec(naked) CoInitialize(struct ZIStream *) {
	__asm {
		jmp[CoUninitializeHookAddy]
	}
}

void _declspec(naked) CoUninitialize(struct ZIStream *) {
	__asm {
		jmp[CoInitializeHookAddy]
	}
}


void installHook() {
	HMODULE hHandle = LoadLibraryA("ole32.dll");
	CoInitializeHookAddy = GetProcAddress(hHandle, "CoInitialize");
	CoUninitializeHookAddy = GetProcAddress(hHandle, "CoUninitialize");


	WvsGame::setupHooks();
	ExpTable::fixExpTable();
	AesKey::setUpAesKey((int*)0x0066BB34);
#if IS_GMS
    //Disable CClient__SocketOnReceive Validation
	Hook::changeByte((BYTE*)0x00423A00, 0xEB);
#endif

	ZSocketEx::SendPacketHook_Original_ptr = (void*)0x0049BE4F;
	Hook::hookCall((BYTE*)0x0049A0B9, forceCast(&ZSocketEx::SendPacket));

	//AvatarLook::hookDecode((VOID*)0x00433B6C);
	//AvatarLook::hookEncode((VOID*)0x00433A8A);
	//GW_CharacterStat::hookEncodeDecode((VOID*)0x00430E88, (VOID*)0x00430FCB);
    Hook::hookCall(WvsGame::CSecuritySession_PreProcessPacket_ptr, 
		forceCast(&CSecuritySession::PreProcessPacket));

	DWORD PacketHeaderCheck_ptr = 0x004238AE + 1;
	Hook::protect(PacketHeaderCheck_ptr, 4);
	*reinterpret_cast<__int16*>(PacketHeaderCheck_ptr) = 0xFFFF - VERSION;

}





BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
#if __ENABLE_CONSOLE__
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
#endif
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&installHook, 0, 0, 0);
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