#include "CSignature.h"
#include <time.h>

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

DWORD GetPattern(const char* IDASig, DWORD Start, size_t Length);
DWORD GetPattern(const char* Bytes, const char* Mask, DWORD Start, size_t End);

DWORD CSignature::dwFindPattern(DWORD dwAddress, DWORD dwLength, const char* szPattern)
{
	//return GetPattern(szPattern, dwAddress, dwLength);
	const char* pat = szPattern;
	DWORD firstMatch = NULL;
	for (DWORD pCur = dwAddress; pCur < dwLength; pCur++)
	{
		if (!*pat) return firstMatch;
		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
			if (!firstMatch) firstMatch = pCur;
			if (!pat[2]) return firstMatch;
			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
			else pat += 2;
		}
		else {
			pat = szPattern;
			firstMatch = 0;
		}
	}
	return NULL;
}

DWORD GetPattern(const char* IDASig, DWORD Start, size_t End)
{
	std::string pattern = "", mask = "";
	for (size_t i = 0; i < strlen(IDASig); i++)
	{
		if (IDASig[i] == ' ')
			continue;
		if (IDASig[i] == '?')
		{
			mask += '?', pattern += '?';
			continue;
		}
		byte newByte = 0;
		for (size_t c = i; c < i + 2; c++)
			newByte <<= 4, newByte += (IDASig[c] >= 'A' && IDASig[c] <= 'F') ? IDASig[c] - 'A' + 0xA : IDASig[c] - '0';
		pattern += newByte, mask += 'x', i++;
	}

	return GetPattern(pattern.c_str(), mask.c_str(), Start, End);
}

DWORD GetPattern(const char* Bytes, const char* Mask, DWORD Start, size_t End)
{
	size_t match = 0;
	for (DWORD i = Start; i < End; i++)
	{
		if (match >= strlen(Mask))
			return i - match;
		match = (Mask[match] == '?' || *(byte*)(i + match) == Bytes[match]) ? match + 1 : 0;
	}
	return NULL;
}

//===================================================================================
HMODULE CSignature::GetModuleHandleSafe( const char* pszModuleName )
{
	HMODULE hmModuleHandle = NULL;

	do
	{
		hmModuleHandle = GetModuleHandle( pszModuleName );
		Sleep( 1 );
	}
	while(hmModuleHandle == NULL);

	return hmModuleHandle;
}
//===================================================================================
DWORD CSignature::GetClientSignature(char* chPattern)
{
	static HMODULE hmModule = GetModuleHandleSafe("client.dll");
	static PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hmModule;
	static PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hmModule) + pDOSHeader->e_lfanew);
	return dwFindPattern(((DWORD)hmModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hmModule) + pNTHeaders->OptionalHeader.SizeOfCode, chPattern);
}
//===================================================================================
DWORD CSignature::GetEngineSignature(char* chPattern)
{
	static HMODULE hmModule = GetModuleHandleSafe("engine.dll");
	static PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hmModule;
	static PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hmModule) + pDOSHeader->e_lfanew);
	return dwFindPattern(((DWORD)hmModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hmModule) + pNTHeaders->OptionalHeader.SizeOfCode, chPattern);
}

CSignature gSignatures;