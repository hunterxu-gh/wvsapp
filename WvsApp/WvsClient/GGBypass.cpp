#include <Windows.h> 
#include "../WvsCommon/Hook.h"
#include "../WvsCommon/Config.h"
#include "SocketUtils.h"
#include "Addy.h"


namespace GGBypass {


	extern void __forceinline hookIpAddress();

	constexpr char * buildIp() {
		char str[] = "localhost";
		int i = 0;
		for (char c : str) {
			str[i++] ^= 1;
		}
		return str;
	}
	
	void __forceinline hookIpAddress() {
		char *protectedIp = buildIp();
		for (int i = 0; i < strlen(protectedIp); i++) {
			protectedIp[i] ^= 1;
		}
		BYTE *ipAddress3 = SocketUtils::getIpAddressAsByteArray(protectedIp);
		memcpy((void*)ipAddressPtr, ipAddress3, 4 * sizeof(BYTE));
	}




	void __forceinline bypassGameguard() {
#if !IS_GMS
	Hook::nop(FixExtraDecode4, 5);
#endif
		Hook::nop(HookRet1, 5);
		Hook::ret(HookRet2);
		Hook::ret(HookRet3);
	}

	/* For some reason the client thinks that the account is new.
	 Search for 20160 in Ida. 
	*/
	void fixIsNewAccount() {
		Hook::changeByte((BYTE*)IsNewAccount_ptr, 0x31);
        Hook::changeByte((BYTE*)IsNewAccount_ptr + 1, 0xC0);
	}

	

	void hookFunctions() {
		hookIpAddress();
		bypassGameguard();
		fixIsNewAccount();
	}



}