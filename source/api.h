#pragma once

#ifdef _CHECKLIC_MODE
#define CHECKLIC_MODE
int __stdcall decodeData(char * sOutBuffer, const char *pInputStr, char *sKey);
int __stdcall checkLicFile(char * sFile, const char *pKey1, const char *pKey2);
#endif

#ifdef _MAKELIC_MODE
#define MAKELIC_MODE
int __stdcall encodeData(char * sOutBuffer, const char *pInputStr, char *sKey);
int __stdcall makeLicFile(char * sFile, const char *pKey1, const char *pKey2, const char * macCode, const char* sLicTime, char licType);
#endif
/***********************************/
char* __stdcall _GetModulePath(HMODULE h = NULL);

bool __stdcall getLocalInfo(char* sMac, char * sIp = NULL);

time_t __stdcall StringToDatetime(char *str);

int __stdcall MacEncode(char * sOut, const char * sMac, const char *pKey1, const char *pKey2);

int __stdcall MacDecode(char * sMacOut, const char * sEncodeMac, const char *pKey1, const char *pKey2);




