#pragma once


int __stdcall decodeData(char * sOutBuffer, const char *pInputStr, char *sKey);

int __stdcall encodeData(char * sOutBuffer, const char *pInputStr, char *sKey);

char* __stdcall _GetModulePath(HMODULE h = NULL);

bool __stdcall getLocalInfo(char* sMac, char * sIp = NULL);

time_t __stdcall StringToDatetime(char *str);