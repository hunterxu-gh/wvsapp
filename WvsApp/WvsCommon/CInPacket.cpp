#include <stdio.h>
#include "CInPacket.h"
#include "Helper.h"



CInPacket *CInPacket::CopyFrom(CInPacket *packet) {
	
	this->m_uRawSeq = packet->m_uRawSeq;
	this->m_bLoopback = packet->m_bLoopback;
	this->m_nState = packet->m_nState;
	this->m_uDataLen = packet->m_uDataLen;
	this->m_uOffset = packet->m_uOffset;
	if (packet->m_uLength > this->m_uLength) {
		this->m_aRecvBuff.a = (unsigned char*) realloc(this->m_aRecvBuff.a, packet->m_uLength);
	}
	this->m_uLength = packet->m_uLength;
	memcpy(this->m_aRecvBuff.a, packet->m_aRecvBuff.a, packet->m_uLength);
	return this;
}

CInPacket::CInPacket(int bLoopback) {
	this->m_bLoopback = 0;
	this->m_aRecvBuff.a = (unsigned char*)malloc(256);
	this->m_nState = 0;
	this->m_uLength = 0;
	this->m_uOffset = 0;
}

int CInPacket::getBytesLeft() {
	return this->m_uLength - this->m_uOffset;
}

BYTE CInPacket::Decode1() {
	if ((this->m_uLength - this->m_uOffset) < 1) {
		_CxxThrowException(38,  -1);
	}
	char ret = this->m_aRecvBuff.a[m_uOffset];
	this->m_uOffset = this->m_uOffset + 1;
	
	return ret;
}
short CInPacket::Decode2() {
	if ((this->m_uLength - this->m_uOffset) < 2) {
		_CxxThrowException(38, -1);
	}
	return this->Decode1() + (this->Decode1() << 8);
}
int CInPacket::Decode4() {
	if (this->m_uLength - this->m_uOffset < 4) {
		_CxxThrowException(38, -1);
	}
	return this->Decode1() + (this->Decode1() << 8) + (this->Decode1() << 16) + (this->Decode1() << 24);
}

ZXString<char> * CInPacket::DecodeStr() {
	if (this->getBytesLeft() < 2) {
		_CxxThrowException(38, -1);
	}
	ZXString<char> *str = new ZXString<char>();
	__int16 len = this->Decode2();
	char *copy = new char[len + 1];
    if (len < 0 || (len > this->getBytesLeft())) {
		_CxxThrowException(38, -1);
	}
	copy[len] = '\0';
	memcpy(copy, (char*)&this->m_aRecvBuff.a[this->m_uOffset], len);
	this->m_uOffset += len;
	str->Assign(copy, -1);
	delete [] copy;
	return str;
}

void CInPacket::dump() {
	printf("[Recv] Len: %d \n", this->m_uLength);
	if (this->m_uLength < 1) {
		return;
	}
	for (int i = 0; i < this->m_uLength; i++) {
		printf("%x ", (unsigned int) this->m_aRecvBuff.a[i]);
	}
	printf("End Packet. \n");
}

short CInPacket::GetType() {
	return *(__int16 *)&this->m_aRecvBuff.a[this->m_uOffset];
}

void CInPacket::reset() {
	this->m_uOffset = 4;
}

void CInPacket::DecodeBuffer(void *p, int uSize) {
	if ((this->m_uLength - this->m_uOffset) < uSize) {
		_CxxThrowException(38, -1);
	}
	memcpy(p, &this->m_aRecvBuff.a[this->m_uOffset], uSize);
	this->m_uOffset += uSize;
}

CInPacket::~CInPacket() {
	free(this->m_aRecvBuff.a);
}