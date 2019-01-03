#include "CClientSocket.h"
#include "CUser.h"
#include "Singletons.h"

#define CRC_CHECK 500
/*
Calls the original SendPacket function. Since we are intercepting the request using
the first 10 bytes, then we call the next :)
*/
void CClientSocket::SendPacket(COutPacket *packet, int bForce) {
	reinterpret_cast<void(__thiscall*)(CClientSocket*, COutPacket*, int)>
		(0x00423784 + 10)(this, packet, bForce);
}

/*
Added an exception handler for when processing packets

*/
void CClientSocket::ProcessPacket(CInPacket *packet) {
	__try {
		reinterpret_cast<void(__thiscall*)(CClientSocket*, CInPacket*)>(0x00423B59)(this, packet);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		printf("Exception processing packet: %x\n", packet->GetType());
	}
}

void CClientSocket::ProcessUserPacket(int nType, CInPacket *packet) {
	__try {
		if (nType == CRC_CHECK) {
			CInPacket *clone = new CInPacket(0);
			clone->CopyFrom(packet);
			clone->reset();
			this->CustomProcessPacket(clone);
			return;
		}
		reinterpret_cast<void(__thiscall*)(CClientSocket*, int, CInPacket*)>(0x00423F65)
			(this, nType, packet);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("Exception processing user packet: %x\n", nType);
	}
}



void CClientSocket::CustomProcessPacket(CInPacket *packet) {
	short type = packet->Decode2();
	switch (type) {
	case CRC_CHECK: {
		int blockStart = packet->Decode4();
		int expected = packet->Decode4();
		int crc = packet->Decode4();
		printf("Memory Hack: Name: %s Address: %x expected: %x actual: %x\n",
			this->m_pUser->m_sCharacterName._m_pStr,
			blockStart,
			expected,
			crc
		);
		COutPacket *pkt = new COutPacket(31, 0);
		pkt->Encode4(-1);
		pkt->Encode1(4);
		pkt->EncodeStr(this->m_pUser->m_sCharacterName._m_pStr);
		pkt->Encode1(1);
		pkt->Encode4(7);
		pkt->Encode1(0);
		pkt->Encode4(this->m_pUser->m_pField->m_dwField);
		pkt->EncodeStr("Coders bot");
		pkt->EncodeStr("Memory Hacking");
		pkt->EncodeStr("Bolsonaro System");
		TSingleton<CCenter> *cCenterInstance = new TSingleton<CCenter>((unsigned int*)0x00695BA8);
		CCenter *center = cCenterInstance->GetInstance();
		center->SendPacket(pkt);
	    this->m_pUser->CloseSocket();
	  }
	}
}
