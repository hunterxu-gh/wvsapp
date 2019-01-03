#include "../WvsCommon/Helper.h"

namespace WvsGame {

   

	extern MAPLE_PTR parseScript__ptr;

	extern MAPLE_PTR CUser__SendSetFieldPacket_ptr;

	extern MAPLE_PTR CSecuritySession_PreProcessPacket_ptr;


	extern void SetEXPRate(int nIncEXPRate,
		                   int dwIncEXPStartCharacterID,
		                   int dwIncEXPEndCharacterID,
		                   int bOnConnect);

	extern void SetDropRate(float rate);

	void setupHooks();
}