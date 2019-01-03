#include "Helper.h"
#include <stdio.h>

int CSecuritySession::PreProcessPacket(int nType, void *iPacket, unsigned __int16 usSeq) {
	return 1;
}


void __declspec(noreturn)_CxxThrowException(int code, int exceptionCode) {
	DWORD dwExceptionCode = 0xE06D7363;
	DWORD nNumberOfArguments= 3;
	int Arguments[7] = {
		0x19930520,
		3,
		0,
		0,
		1,
		0xE06D7363,
		7
	};
	RaiseException(dwExceptionCode, 1, nNumberOfArguments, (ULONG_PTR*)&Arguments[0]);
}

ParamDecoder::ParamDecoder(char *arg) {
	this->arguments = new char[100];
	strcpy_s(arguments, 100, arg);
	this->offset = 0;
};

ParamDecoder::~ParamDecoder() {
	if (this->arguments) {
		delete[] this->arguments;
	}
};

int indexOf(char * str, char c) {
	int i = 0;
	auto backup = str;
	while (*(backup++) != c) {
		if (i == '\n') {
			return -1;
		}
		i++;
	}
	return i;
}


void ParamDecoder::SkipCommand() {
	this->offset += indexOf(this->arguments, ' ') + 1;
}

bool ParamDecoder::hasMoreCommand() {
	return indexOf(this->arguments, ' ') > 0;
}

int ParamDecoder::DecodeInt() {
	int pos = indexOf(&this->arguments[this->offset], ' ');
	if (pos == -1) {
		pos = strlen(&this->arguments[this->offset]);
	}
	char *buff = new char[pos + 1];
	memcpy(buff, &this->arguments[this->offset], pos);
	int number = atoi(buff);
	delete[] buff;
	return number;
}

