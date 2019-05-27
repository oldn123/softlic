#include "stdafx.h"
#include "CBase64Code.h"
#include "ida_def.h"
#include "stdio.h"

#include <IPHlpApi.h>  
#include <stdio.h>
#include <assert.h>

#include "api.h"


char g_sAppPath[250] = {0};

char* __stdcall _GetModulePath(HMODULE h)
{
	if (strlen(g_sAppPath) > 1)
	{
		return g_sAppPath;
	}

	::GetModuleFileNameA(h, g_sAppPath, 250);

	for (int i = strlen(g_sAppPath) - 1; i > 0; i--)
	{
		if (g_sAppPath[i] == '\\')
		{
			g_sAppPath[i + 1] = 0;
			break;
		}
	}
	return g_sAppPath;
} 


bool getAdapterState(DWORD index)
{
	MIB_IFROW Info;    // 存放获取到的Adapter参数
	memset(&Info, 0, sizeof(MIB_IFROW));
	Info.dwIndex = index; // dwIndex是需要获取的Adapter的索引，可以通过GetAdaptersInfo和其他相关函数获取
	if (GetIfEntry(&Info) != NOERROR)
	{
		return false;
	}
	if (Info.dwOperStatus == IF_OPER_STATUS_NON_OPERATIONAL
		|| Info.dwOperStatus == IF_OPER_STATUS_UNREACHABLE
		|| Info.dwOperStatus == IF_OPER_STATUS_DISCONNECTED

		|| Info.dwOperStatus == IF_OPER_STATUS_CONNECTING)
		return false;
	else if (Info.dwOperStatus == IF_OPER_STATUS_OPERATIONAL

		|| Info.dwOperStatus == IF_OPER_STATUS_CONNECTED)
		return true;

	return false;
}

bool __stdcall getLocalInfo(char * mac, char * ipMe)
{
	IP_ADAPTER_INFO IOInfo[20];
	PIP_ADAPTER_INFO pIOInfo = NULL;
	DWORD Result = 0;
	unsigned long nLen = sizeof(IOInfo);

	Result = GetAdaptersInfo(IOInfo, &nLen);
	pIOInfo = &IOInfo[0];
	bool bGetOK = false;
	while (pIOInfo)
	{
		IP_ADDR_STRING *pIpAddrString = &(pIOInfo->IpAddressList);
		if (bGetOK || !getAdapterState(pIOInfo->Index))
		{
			//网络未连接
			pIOInfo = pIOInfo->Next;
			continue;
		}

		{	
			if (/*strstr(pIOInfo->Description, "PCI")>0 &&*/ pIOInfo->Type == MIB_IF_TYPE_ETHERNET) //有线网可用时直接返回
			{
				bGetOK = true;
			}	
			else if (pIOInfo->Type == 71)	//无线网络
			{
				bGetOK = true;
			}

			if (bGetOK)
			{
				//仅考虑有线网络与无线网络
				if (ipMe)
				{	
					strcpy(ipMe, pIpAddrString->IpAddress.String);
				}
		
				sprintf(mac, "%2x%2x%2x%2x%2x%2x", pIOInfo->Address[0], pIOInfo->Address[1], pIOInfo->Address[2],
					pIOInfo->Address[3], pIOInfo->Address[4], pIOInfo->Address[5]);
			}
		}

		pIOInfo = pIOInfo->Next;
	}

	return bGetOK;
}

time_t __stdcall StringToDatetime(char *str)  
{  
	tm tm_;  
	int year, month, day, hour, minute,second;  
	sscanf(str,"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);  
	tm_.tm_year  = year-1900;  
	tm_.tm_mon   = month-1;  
	tm_.tm_mday  = day;  
	tm_.tm_hour  = hour;  
	tm_.tm_min   = minute;  
	tm_.tm_sec   = second;  
	tm_.tm_isdst = 0;  
   
	time_t t_ = mktime(&tm_); //已经减了8个时区  
	return t_; //秒时间  
}  

class CDataReaderImpl
{
public:
	CDataReaderImpl(){
		m_hModuleRes = 0;
		m_pBuffer = NULL;
		m_dwBaseAddr = 0x100f0000;
		char sFile[250];
		strcpy(sFile, _GetModulePath());
		strcat(sFile, "EarthbinData.bin");

		FILE * fp = fopen(sFile, "rb");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			int nLen = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			m_pBuffer = new BYTE[nLen];
			int nFrom = 0;
			do 
			{
				int nr = fread(&m_pBuffer[nFrom], 1, nLen, fp);
				nLen -= nr;
				nFrom += nr;
			} while (nLen > 0);
			fclose(fp);
		}
	}

	~CDataReaderImpl(){
		if (m_pBuffer)
		{
			delete [] m_pBuffer;
			m_pBuffer = NULL;
		}

	}

	virtual BYTE* ReadData(DWORD ptr, int nSize) {
		if (m_pBuffer)
		{
			return  (BYTE *)&m_pBuffer[ptr - m_dwBaseAddr];
		}
		return NULL;
	}

	bool IsOk(){return m_pBuffer != 0;}

protected:
	BYTE * m_pBuffer;
	DWORD m_dwBaseAddr;
	HMODULE	m_hModuleRes;
}; 


CDataReaderImpl g_fileReader;


BYTE * GetAddrBYTE(DWORD dwAddr)
{
	return g_fileReader.ReadData(dwAddr, 0);
}

DWORD * GetAddrDWORD(DWORD dwAddr)
{
	return (DWORD*)GetAddrBYTE(dwAddr);
}

int __stdcall sub_1001EBF0(int a1, int a2)
{
	BYTE *v2; // eax
	DWORD *v3; // ecx
	signed int v4; // edi

	v2 = (BYTE *)(a2 + 1);
	v3 = GetAddrDWORD(0x100F629C);
	v4 = 8;
	do
	{
		*(v2 - 1) = *(BYTE *)(a1 + *(v3 - 1));
		*v2 = *(BYTE *)(a1 + *v3);
		v2[1] = *(BYTE *)(a1 + v3[1]);
		v2[2] = *(BYTE *)(a1 + v3[2]);
		v2[3] = *(BYTE *)(a1 + v3[3]);
		v2[4] = *(BYTE *)(a1 + v3[4]);
		v2 += 6;
		v3 += 6;
		--v4;
	}
	while ( v4 );
	return 0;
}

int __stdcall sub_1001EC60(void *a1, size_t a2)
{
	size_t v2; // esi
	char v4[56]; // [esp+18h] [ebp-3Ch]

	v2 = a2;
	memcpy(v4, a1, a2);
	memcpy(&v4[v2], (char *)a1 + 28, v2);
	memcpy(a1, (char *)a1 + v2, 28 - v2);
	memcpy((char *)a1 - a2 + 28, v4, a2);
	memcpy((char *)a1 + 28, (char *)a1 + a2 + 28, 28 - a2);
	memcpy((char *)a1 - a2 + 56, &v4[a2], a2);
	return 0;
}

int __stdcall sub_1001EBA0(int a1, int a2)
{
	BYTE *v2; // eax
	DWORD *v3; // ecx
	signed int v4; // edi

	v2 = (BYTE *)(a2 + 1);
	v3 = GetAddrDWORD(0x100F61BC);
	v4 = 14;
	do
	{
		*(v2 - 1) = *(BYTE *)(a1 + *(v3 - 1));
		*v2 = *(BYTE *)(a1 + *v3);
		v2[1] = *(BYTE *)(a1 + v3[1]);
		v2[2] = *(BYTE *)(a1 + v3[2]);
		v2 += 4;
		v3 += 4;
		--v4;
	}
	while ( v4 );
	return 0;
}

// 数据转换，需要详细分析
int __stdcall convBitData_1001F5A0(int a1, int a2)
{
	int v2; // ebx
	size_t *v3; // esi
	char v5[0x1000]; // [esp+Ch] [ebp-3Ch]

	v2 = a2;
	sub_1001EBA0(a1, (int)&v5[0]);
	v3 = (size_t *)GetAddrDWORD(0x100F6358);
	do
	{
		sub_1001EC60(&v5[0], *v3);
		sub_1001EBF0((int)&v5[0], v2);
		++v3;
		v2 += 48;
	}
	while ( (signed int)v3 < (signed int)GetAddrDWORD(0x100F6398));
	return 0;
}

int __stdcall base64Decode_1000F080(char *a1, char *encodeString, int encodeDataLen)
{
	return Base64_Decode(a1, encodeString, encodeDataLen);
}

// 字符串转数据，如"8a9d4c...."-->0x8a,0x9d,0x4c....
signed int __stdcall stringDataToByteData_1000EF90(int a1, BYTE *a2, int a3)
{
	BYTE *v3; // edi
	char *v4; // esi
	char v5; // al
	char v6; // cl
	char v7; // al
	char v8; // cl

	if ( a3 <= 0 )
		return 0;
	v3 = a2;
	v4 = (char *)(a1 + 1);
	while ( 1 )
	{
		v5 = *(v4 - 1);
		v6 = *v4;
		if ( (unsigned __int8)(*(v4 - 1) - 48) > 9u )
		{
			if ( (unsigned __int8)(v5 - 65) > 5u )
			{
				if ( (unsigned __int8)(v5 - 97) > 5u )
					return -1;
				v7 = v5 - 87;
			}
			else
			{
				v7 = v5 - 55;
			}
		}
		else
		{
			v7 = *(v4 - 1) - 48;
		}
		if ( (unsigned __int8)(v6 - 48) > 9u )
			break;
		v8 = v6 - 48;
LABEL_15:
		v4 += 2;
		*v3++ = v8 | 16 * v7;
		if ( (signed int)&v4[-1 - a1] >= a3 )
			return 0;
	}
	if ( (unsigned __int8)(v6 - 65) <= 5u )
	{
		v8 = v6 - 55;
		goto LABEL_15;
	}
	if ( (unsigned __int8)(v6 - 97) <= 5u )
	{
		v8 = v6 - 87;
		goto LABEL_15;
	}
	return -1;
}

// 将字符串转成bit数据存储，如：'a'的ascII是0x61,0x61的二进制是0110 0001,则转换结果为：01 00 00 00  00 01 01 00，即转为2进制形式，反向存储，每一位用一个字节存储
int __stdcall stringToBitData_1001EAE0(int a1, int a2)
{
	int v2; // esi
	signed int v3; // edi
	char v4; // dl
	signed int v5; // eax
	char v6; // bl

	v2 = a2;
	v3 = 0;
	do
	{
		v4 = *(BYTE *)(v3 + a1);
		v5 = 0;
		do
		{
			v6 = v4 >> v5++;
			*(BYTE *)(v2 + v5 - 1) = v6 & 1;
		}
		while ( v5 < 8 );
		++v3;
		v2 += 8;
	}
	while ( v3 < 8 );
	return 0;
}

void CharToBit(bool *Output, const char *Input) {
	for (int i = 0; i<64; i++) {
		Output[i] = (Input[i / 8] << (i % 8)) & 0x80;
	}
}

int __stdcall stringToBitData(char * a1, BYTE * a2)
{
	CharToBit((bool*)a2, a1);
	return 0;
}


int __stdcall sub_1001ED00(BYTE *a1)
{
	signed int v1; // eax
	int v2; // ecx
	char v3; // dl
	int v4; // ecx
	char v5; // dl
	int v6; // ecx
	char v8[64]; // [esp+8h] [ebp-44h]

	v1 = 0;
	do
	{
		v2 = GetAddrDWORD(0x100F567C)[v1];
		v8[v1] = a1[GetAddrDWORD(0x100F5678)[v1]];
		v3 = a1[v2];
		v4 = GetAddrDWORD(0x100F5680)[v1];
		v8[v1 + 1] = v3;
		v5 = a1[v4];
		v6 = GetAddrDWORD(0x100F5684)[v1];
		v8[v1 + 2] = v5;
		v8[v1 + 3] = a1[v6];
		v1 += 4;
	}
	while ( v1 < 64 );
	memcpy(a1, &v8, 0x40u);
	return 0;
}

int __stdcall sub_1001EE20(BYTE *a1)
{
	signed int v1; // eax
	int v2; // ecx
	char v3; // dl
	int v4; // ecx
	char v5; // dl
	int v6; // ecx
	char v7; // dl
	int v8; // ecx
	char v9; // dl
	int v10; // ecx
	char v12[48]; // [esp+8h] [ebp-34h]
	v1 = 0;
	do
	{
		
		v2 = GetAddrDWORD(0x100F587C)[v1];
		v12[v1] = a1[GetAddrDWORD(0x100F5878)[v1]];
		v3 = a1[v2];
		v4 = GetAddrDWORD(0x100F5880)[v1];
		v12[v1 + 1] = v3;
		v5 = a1[v4];
		v6 = GetAddrDWORD(0x100F5884)[v1];
		v12[v1 + 2] = v5;
		v7 = a1[v6];
		v8 = GetAddrDWORD(0x100F5888)[v1];
		v12[v1 + 3] = v7;
		v9 = a1[v8];
		v10 = GetAddrDWORD(0x100F588C)[v1];
		v12[v1 + 4] = v9;
		v12[v1 + 5] = a1[v10];
		v1 += 6;
	}
	while ( v1 < 48 );
	memcpy(a1, &v12, 0x30u);
	return 0;
}

int __stdcall sub_1001EF60(char *a1)
{
	char *v1; // edx
	char *v2; // edi
	char *v3; // ecx
	char *v4; // eax
	signed int v5; // esi
	int v6; // edx
	int v7; // edx
	int v8; // ST14_4
	int v9; // ebp
	char *v10; // ebx
	char *v12; // [esp+18h] [ebp-44h]
	char *v13; // [esp+1Ch] [ebp-40h]
	char *v14; // [esp+20h] [ebp-3Ch]
	char *v15; // [esp+34h] [ebp-28h]

	v1 = a1 + 3;
	v13 = a1 + 15;
	v2 = a1 + 5;
	v14 = a1 + 10;
	v3 = a1 + 13;
	v12 = a1 + 20;
	v4 = a1 + 11;
	v5 = 0;
	v15 = a1 + 3;
	do
	{
		v6 = GetAddrDWORD(0x100F59B8)[v1[1]
		+ 2 * (*v1 + 2 * (*(v12 - 18) + 2 * (*(v3 - 12) + 2 * (*(v4 - 6) + 2 * (2 * v5 + *(v1 - 3))))))];
		a1[4 * v5] = (v6 >> 3) & 1;
		a1[4 * v5 + 1] = (v6 >> 2) & 1;
		a1[4 * v5 + 2] = (v6 >> 1) & 1;
		a1[4 * v5 + 3] = v6 & 1;
		v7 = GetAddrDWORD(0x100F5AB8)[*(v4 - 1)
			+ 2 * (*(v4 - 2) + 2 * (*(v4 - 3) + 2 * (*(v4 - 4) + 2 * (*v4 + 2 * (2 * v5 + *(v4 - 5))))))];
		*(v2 - 1) = (GetAddrDWORD(0x100F5AB8)[*(v4 - 1)
			+ 2
			* (*(v4 - 2) + 2 * (*(v4 - 3) + 2 * (*(v4 - 4) + 2 * (*v4 + 2 * (2 * v5 + *(v4 - 5))))))] >> 3) & 1;
		*v2 = (v7 >> 2) & 1;
		v2[1] = (v7 >> 1) & 1;
		v2[2] = v7 & 1;
		v8 = GetAddrDWORD(0x100F5BB8)[v3[3] + 2 * (v3[2] + 2 * (v3[1] + 2 * (*v3 + 2 * (v4[6] + 2 * (2 * v5 + *(v3 - 1))))))];
		*(v14 - 2) = (GetAddrDWORD(0x100F5BB8)[v3[3] + 2 * (v3[2] + 2 * (v3[1] + 2 * (*v3 + 2 * (v4[6] + 2 * (2 * v5 + *(v3 - 1))))))] >> 3) & 1;
		v2[4] = (v8 >> 2) & 1;
		*v14 = (v8 >> 1) & 1;
		v14[1] = v8 & 1;
		v9 = GetAddrDWORD(0x100F5CB8)[v12[2] + 2 * (v12[1] + 2 * (*v12 + 2 * (v3[6] + 2 * (v4[12] + 2 * (2 * v5 + *(v12 - 2))))))];
		v12 += 24;
		*(v13 - 3) = (v9 >> 3) & 1;
		v2[8] = (v9 >> 2) & 1;
		v14[4] = (v9 >> 1) & 1;
		v10 = v13;
		v13 += 16;
		*v10 = v9 & 1;
		v5 += 4;
		v1 = v15 + 24;
		v4 += 24;
		v3 += 24;
		v2 += 16;
		v15 += 24;
		v14 += 16;
	}
	while ( v5 < 8 );
	return 0;
}

int __stdcall DesEncode_1001EED0(BYTE *a1)
{
	signed int v1; // eax
	int v2; // ecx
	char v3; // dl
	int v4; // ecx
	char v5; // dl
	int v6; // ecx
	char v8[32]; 
	v1 = 0;
	do
	{
		v2 = GetAddrDWORD(0x100F593C)[v1];
		v8[v1 + 0] = a1[GetAddrDWORD(0x100F5938)[v1]];
		v3 = a1[v2];
		v4 = GetAddrDWORD(0x100F5940)[v1];
		v8[v1 + 1] = v3;
		v5 = a1[v4];
		v6 = GetAddrDWORD(0x100F5944)[v1];
		v8[v1 + 2] = v5;
		v8[v1 + 3] = a1[v6];
		v1 += 4;
	}
	while ( v1 < 32 );
	memcpy(a1, &v8, 0x20u);
	return 0;
}

int __stdcall sub_1001ED90(BYTE *a1)
{
	signed int v1; // eax
	int v2; // ecx
	char v3; // dl
	int v4; // ecx
	char v5; // dl
	int v6; // ecx
	char v8[64]; // [esp+8h] [ebp-44h]

	v1 = 0;
	do
	{
		v2 = GetAddrDWORD(0x100F577C)[v1];
		v8[v1 + 0] = a1[GetAddrDWORD(0x100F5778)[v1]];
		v3 = a1[v2];
		v4 = GetAddrDWORD(0x100F5780)[v1];
		v8[v1 + 1] = v3;
		v5 = a1[v4];
		v6 = GetAddrDWORD(0x100F5784)[v1];
		v8[v1 + 2] = v5;
		v8[v1 + 3] = a1[v6];
		v1 += 4;
	}
	while ( v1 < 64 );
	memcpy(a1, &v8, 0x40u);
	return 0;
}

int __stdcall sub_1001EB20(int a1, DWORD *a2)
{
	signed int v2; // eax
	BYTE *v3; // edi
	char v4; // cl
	char v5; // dl
	char v6; // cl
	char v7; // dl
	char v8; // cl
	char v9; // dl

	v2 = 0;
	*a2 = 0;
	a2[1] = 0;
	v3 = (BYTE *)(a1 + 2);
	do
	{
		*((BYTE *)a2 + v2) |= *(v3 - 2);
		v4 = *((BYTE *)a2 + v2) | 2 * *(v3 - 1);
		*((BYTE *)a2 + v2) = v4;
		v5 = v4 | 4 * *v3;
		*((BYTE *)a2 + v2) = v5;
		v6 = v5 | 8 * v3[1];
		*((BYTE *)a2 + v2) = v6;
		v7 = v6 | 16 * v3[2];
		*((BYTE *)a2 + v2) = v7;
		v8 = v7 | 32 * v3[3];
		*((BYTE *)a2 + v2) = v8;
		v9 = v8 | (v3[4] << 6);
		*((BYTE *)a2 + v2) = v9;
		*((BYTE *)a2 + v2++) = v9 | (v3[5] << 7);
		v3 += 8;
	}
	while ( v2 < 8 );
	return 0;
}

int __stdcall sub_1001F400(int a1, int a2, int a3)
{
	int v3; // ebx
	signed int v4; // eax
	char v5; // cl
	char v6; // cl
	char v7; // cl
	signed int v8; // eax
	char v9; // dl
	char v10; // dl
	signed int v11; // eax
	signed int v13; // [esp+10h] [ebp-9Ch]
	char v14[48]; 
	char v20[64]; 
	char v25[32]; // [esp+88h] [ebp-24h]

	stringToBitData_1001EAE0(a1, (int)&v20[0]);
	sub_1001ED00((BYTE*)v20);
	v13 = 15;
	v3 = a2 + 721;
	do
	{
		memcpy(v14, &v20[32], 0x20);
		sub_1001EE20((BYTE*)v14);
		v4 = 0;
		do
		{
			v5 = *(BYTE *)(v3 + v4);
			v14[v4] ^= *(BYTE *)(v3 + v4 - 1);
			v14[1 + v4] ^= v5;
			v6 = *(BYTE *)(v3 + v4 + 2);
			v14[2 + v4] ^= *(BYTE *)(v3 + v4 + 1);
			v14[3 + v4] ^= v6;
			v7 = *(BYTE *)(v3 + v4 + 4);
			v14[4 + v4] ^= *(BYTE *)(v3 + v4 + 3);
			v14[5 + v4] ^= v7;
			v4 += 6;
		}
		while ( v4 < 48 );
		sub_1001EF60(v14);
		DesEncode_1001EED0((BYTE*)v14);
		v8 = 0;
		do
		{
			v9 = v14[1 + v8];
			v20[v8] ^= v14[v8];
			v20[1 + v8] ^= v9;
			v10 = v14[3 + v8];
			v20[2 + v8] ^= v14[2 + v8];
			v20[3 + v8] ^= v10;
			v8 += 4;
		}
		while ( v8 < 32 );
		v11 = v13;
		if ( v13 )
		{
			memcpy(v25, v20, 0x20u);
			memcpy(v20, &v20[32], 0x20u);
			memcpy(&v20[32], v25, 0x20u);
		}
		v3 -= 48;
		--v13;
	}
	while ( v11 - 1 >= 0 );
	sub_1001ED90((BYTE*)&v20);
	sub_1001EB20((int)&v20, (DWORD *)a3);
	return 0;
}

// 关键函数*****
int __stdcall sub_1001F960(int a3, const char *sEncodeData, char *sKeyString)
{
	char *v3; // esi
	int v4; // ebp
	int v5; // edi
	int v6; // ebp
	int v7; // esi
	char *v8; // edi
	int v9; // edx
	char *v10; // edi
	char *v11; // esi
	char v12; // dl
	char v13; // al
	char v14; // cl
	char v15; // dl
	char v16; // al
	char v17; // cl
	char v18; // dl
	char v19; // al
	char v20; // dl
	char v21; // cl
	int v22; // eax
	char v23; // cl
	char v24; // dl
	char v25; // cl
	int v26; // eax
	bool v27; // zf
	int v28; // ebx
	int v29; // esi
	char v31; // [esp+17h] [ebp-379h]
	void *v32; // [esp+18h] [ebp-378h]
	void *pByteDatas; // [esp+1Ch] [ebp-374h]
	int v34; // [esp+20h] [ebp-370h]
	int v35; // [esp+24h] [ebp-36Ch]
	int v36; // [esp+28h] [ebp-368h]
	int v37; // [esp+2Ch] [ebp-364h]
	char buffer[0x360];
	memset(buffer, 0, 0x360);

	char * v38 = &buffer[0]; // [esp+30h] [ebp-360h]
	int * v40 = (int*)&buffer[8]; // [esp+38h] [ebp-358h]
	char * v42 = &buffer[0x10]; // [esp+40h] [ebp-350h]
	char * v44 = &buffer[0x50]; // [esp+80h] [ebp-310h]
	int v50; // [esp+38Ch] [ebp-4h]

	v35 = a3;
	v50 = 0;
	v34 = 0;
	v50 = 1;
	v3 = (char *) new char[0x800];
	memset(v3, 0, 0x800);

	v3[base64Decode_1000F080(v3, (char *)sEncodeData, strlen(sEncodeData))] = 0;
	v4 = strlen(v3);
	v5 = v4 / 2;
	pByteDatas =  new char[v4 / 2];
	memset(pByteDatas, 0, v4 / 2);
	stringDataToByteData_1000EF90((int)v3, (BYTE*)pByteDatas, v4);

	delete [] v3;
	v6 = v5 / 8;
	v7 = 8 * (v5 / 8);
	v37 = 8 * (v5 / 8);
	v8 = (char *)new char[v7 + 1];
	memset(v8, 0, v7 + 1);

	v9 = *((DWORD *)sKeyString + 1);             // sKeyString 为8个字节，取后4个字节
	v40[0] = *(DWORD *)sKeyString;                  // sKeyString 为8个字节，取前4个字节
	v32 = v8;
	v40[1] = v9;
	stringToBitData_1001EAE0((int)&v40[0], (int)&v42[0]);// 将字符串转成bit数据存储，如：'a'的ascII是0x61,0x61的二进制是0110 0001,则转换结果为：01 00 00 00  00 01 01 00，即转为2进制形式，反向存储，每一位用一个字节存储
	
	
	convBitData_1001F5A0((int)&v42[0], (int)&v44[0]);
	if ( v6 <= 0 )
	{
		v25 = v38[7];
	}
	else
	{
		v10 = v8 + 3;
		v11 = (char *)pByteDatas + 2;
		v36 = (BYTE *)v32 - (BYTE *)pByteDatas;
		do
		{
			v12 = *(v11 - 1);
			v13 = *v11;
			LOBYTE(v40[0]) = *(v11 - 2);
			v14 = v11[1];
			BYTE1(v40[0]) = v12;
			v15 = v11[2];
			BYTE2(v40[0]) = v13;
			v16 = v11[3];
			BYTE3(v40[0]) = v14;
			v17 = v11[4];
			LOBYTE(v40[1]) = v15;
			v18 = v11[5];
			BYTE1(v40[1]) = v16;
			BYTE2(v40[1]) = v17;
			BYTE3(v40[1]) = v18;
			sub_1001F400((int)&v40[0], (int)&v44[0], (int)&v38[0]);
			v19 = v38[0];
			v20 = v38[2];
			*(v10 - 2) = v38[1];
			v21 = v38[3];
			*(v10 - 3) = v19;
			v22 = v36;
			*v10 = v21;
			v23 = v38[6];
			v11[v22] = v20;
			v24 = v38[4];
			LOBYTE(v22) = v38[5];
			v10[3] = v23;
			v25 = v38[7];
			v10[1] = v24;
			v10[2] = v22;
			v10[4] = v25;
			v11 += 8;
			v10 += 8;
			--v6;
		}
		while ( v6 );
		v8 = (char *)v32;
		v7 = v37;
	}
	if ( v25 >= 8 )
	{
		v26 = v34;
	}
	else
	{
		v26 = 8 - v25;
		v27 = v26 == 7;
		if ( v26 >= 7 )
			goto LABEL_13;
		do
		{
			if ( v38[v26] )
				break;
			++v26;
		}
		while ( v26 < 7 );
	}
	v27 = v26 == 7;
LABEL_13:
	if ( v27 )
	{
		v28 = v25;
		if ( 8 - v25 > 0 )
			memcpy(&v8[v7 - 8], v38, 8 - v25);
		v7 -= v28;
	}
	v8[v7] = 0;
	v29 = v35;
	strcpy((char*)v35, v8);

	v34 = 1;
	delete [] pByteDatas;
	delete [] v8;
	LOBYTE(v50) = 0;
	return v29;
}


/*************************************************************************************/



int __stdcall sub_1001EAE0(int a1, int a2)
{
	return stringToBitData_1001EAE0(a1, a2);
}

int __stdcall sub_1001F5A0(int a1, int a2)
{
	return convBitData_1001F5A0(a1, a2);
}

int __stdcall sub_1001F260(int a1, int a2, _DWORD *a3)
{
	int v3; // ebx
	signed int v4; // eax
	char v5; // cl
	char v6; // cl
	char v7; // cl
	signed int v8; // eax
	char v9; // dl
	char v10; // dl
	int v11; // eax
	int v13; // [esp+10h] [ebp-9Ch]
	char sbuf[1024];
	memset(sbuf, 0, 1024);
	
	char *v14 = &sbuf[0]; // [esp+18h] [ebp-94h]
	char *v20 = &sbuf[0x30]; // [esp+48h] [ebp-64h]
	char *v25 = &sbuf[0x70]; // [esp+88h] [ebp-24h]


	stringToBitData_1001EAE0(a1, (int)v20);
	sub_1001ED00((BYTE*)&v20[0]);
	v13 = 0;
	v3 = a2 + 1;
	do
	{
		qmemcpy(&v14[0], &v20[0x20], 0x20u);
		sub_1001EE20((BYTE*)v14);
		v4 = 0;
		do
		{
			v5 = *(_BYTE *)(v3 + v4);
			v14[ 0 + v4] ^= *(_BYTE *)(v3 + v4 - 1);
			v14[ 1 + v4] ^= v5;
			v6 = *(_BYTE *)(v3 + v4 + 2);
			v14[ 2 + v4] ^= *(_BYTE *)(v3 + v4 + 1);
			v14[ 3 + v4] ^= v6;
			v7 = *(_BYTE *)(v3 + v4 + 4);
			v14[ 4 + v4] ^= *(_BYTE *)(v3 + v4 + 3);
			v14[ 5 + v4] ^= v7;
			v4 += 6;
		}
		while ( v4 < 48 );
		sub_1001EF60(&v14[0]);
		DesEncode_1001EED0((BYTE*)&v14[0]);
		v8 = 0;
		do
		{
			v9 = v14[ 1 + v8];
			v20[0 + v8] ^= v14[v8];
			v20[1 + v8] ^= v9;
			v10 = v14[ 3 + v8];
			v20[2 + v8] ^= v14[ 2 + v8];
			v20[3 + v8] ^= v10;
			v8 += 4;
		}
		while ( v8 < 32 );
		v11 = v13;
		if ( v13 != 15 )
		{
			qmemcpy(v25, v20, 0x20u);
			qmemcpy(v20, &v20[0x20], 0x20u);
			qmemcpy(&v20[0x20], v25, 0x20u);
		}
		v3 += 48;
		++v13;
	}
	while ( v11 + 1 < 16 );
	sub_1001ED90((BYTE*)&v20[0]);
	sub_1001EB20((int)&v20[0], (DWORD*)a3);
	return 0;
}

int __stdcall sub_1000F030(char *a1, int a2, int a3)
{
	int v3; // esi
	char *v4; // edi
	int result; // eax

	v3 = 0;
	if ( a3 > 0 )
	{
		v4 = a1;
		do
		{
			result = sprintf(v4, "%02x", *(unsigned __int8 *)(v3++ + a2));
			v4 += 2;
		}
		while ( v3 < a3 );
	}
	return result;
}


int __stdcall toBase64String_1000F200(_BYTE *a1, int a2, unsigned int a3)
{
	return Base64_Encode((char*)a1, (char*)a2, a3);
}

int __stdcall sub_1001F610(int a2, const char *pInputStr, char *sKey)
{
	int v4; // edx
	signed int v5; // ecx
	int v6; // esi
	int v7; // edi
	char *v8; // eax
	_BYTE *v9; // ebx
	char *v10; // esi
	_BYTE *v11; // edi
	char v12; // dl
	char v13; // al
	char v14; // cl
	char v15; // dl
	char v16; // al
	char v17; // cl
	char v18; // dl
	char v19; // cl
	char v20; // dl
	char v21; // dl
	char v22; // al
	char v23; // cl
	char v24; // dl
	size_t v25; // eax
	char v26; // al
	char v27; // cl
	char v28; // dl
	char v29; // al
	char v30; // cl
	char v31; // dl
	char v32; // al
	int v33; // esi
	int v34; // edi
	char *v35; // esi
	_BYTE *v36; // eax
	_BYTE *v37; // edi
	int v38; // eax
	int v39; // ecx
//	char v41; // [esp+17h] [ebp-38Dh]
	int v42; // [esp+18h] [ebp-38Ch]
	int v43; // [esp+1Ch] [ebp-388h]
	int v44; // [esp+20h] [ebp-384h]
	unsigned int v45; // [esp+24h] [ebp-380h]
	const char *v46; // [esp+28h] [ebp-37Ch]
//	void *v47; // [esp+2Ch] [ebp-378h]
	size_t v48; // [esp+30h] [ebp-374h]
	int v49; // [esp+34h] [ebp-370h]
	int v50; // [esp+38h] [ebp-36Ch]
	char v51[8]; // [esp+3Ch] [ebp-368h]
	int v59[2]; // [esp+44h] [ebp-360h]
	//int v60; // [esp+48h] [ebp-35Ch]
	char v61[0x40]; // [esp+4Ch] [ebp-358h]
	char v62[0x314]; // [esp+8Ch] [ebp-318h]
	int v63; // [esp+3A0h] [ebp-4h]

	memset(v61, 0, 0x40);
	memset(v62, 0, 0x314);
	//v47 = this;
	v49 = a2;
	v50 = 0;
	v4 = ((DWORD*)sKey)[1];
	v59[0] = *(DWORD*)sKey;
	v63 = 0;
	v46 = pInputStr;
	v59[1] = v4;
	sub_1001EAE0((int)&v59, (int)&v61);
	sub_1001F5A0((int)&v61, (int)&v62);
	v5 = strlen(pInputStr);
	v6 = v5 / 8;
	v43 = v5 / 8;
	v48 = v5 % 8;
	if ( v5 % 8 <= 0 )
	{
		v44 = 8 * v6;
		v42 = 8 * v6;
		v7 = 8 * v6;
	}
	else
	{
		v7 = 8 * v6;
		v44 = 8 * v6;
		v42 = 8 * v6 + 8;
	}
	v8 = (char *)new char[v42 + 1];
	memset(v8, 0, v42+1);
	v9 = (_BYTE*)v8;
	if ( v6 > 0 )
	{
		v10 = (char *)(v46 + 2);
		v11 = (_BYTE*)&v8[3];
		v45 = v8 - v46;
		do
		{
			v12 = *(v10 - 1);
			v13 = *v10;
			LOBYTE(v59[0]) = *(v10 - 2);
			v14 = v10[1];
			BYTE1(v59[0]) = v12;
			v15 = v10[2];
			BYTE2(v59[0]) = v13;
			v16 = v10[3];
			BYTE3(v59[0]) = v14;
			v17 = v10[4];
			LOBYTE(v59[1]) = v15;
			v18 = v10[5];
			BYTE1(v59[1]) = v16;
			BYTE2(v59[1]) = v17;
			BYTE3(v59[1]) = v18;
			sub_1001F260((int)&v59[0], (int)&v62[0], (uint32*)&v51[0]);
			v19 = v51[1];
			v20 = v51[2];
			*(v11 - 3) = v51[0];
			v10[v45] = v20;
			v21 = v51[4];
			v22 = v51[5];
			*(v11 - 2) = v19;
			*v11 = v51[3];
			v23 = v51[6];
			v11[1] = v21;
			v24 = v51[7];
			v11[2] = v22;
			v11[3] = v23;
			v11[4] = v24;
			v10 += 8;
			v11 += 8;
			--v43;
		}
		while ( v43 );
		v7 = v44;
	}
	v25 = v48;
	if ( (signed int)v48 > 0 )
	{
		v59[0] = 0;
		v59[1] = 0;
		if ( (signed int)v48 > 0 )
		{
			memcpy(&v59, &v46[v7], v48);
			v25 = v48;
		}
		*((_BYTE *)&v59[0] + v25) = 0;
		BYTE3(v59[1]) = 8 - v25;
		sub_1001F260((int)&v59, (int)&v62, (uint32*)&v51[0]);
		v26 = v51[1];
		v27 = v51[2];
		v9[v7] = v51[0];
		v28 = v51[3];
		v9[v7 + 1] = v26;
		v29 = v51[4];
		v9[v7 + 2] = v27;
		v30 = v51[5];
		v9[v7 + 3] = v28;
		v31 = v51[6];
		v9[v7 + 4] = v29;
		v32 = v51[7];
		v9[v7 + 5] = v30;
		v9[v7 + 6] = v31;
		v9[v7 + 7] = v32;
	}
	v33 = v42;
	v9[v42] = 0;
	//sub_10018FE0(&v41);
	v34 = 2 * v33;
	v63 = 1;
	v45 = 2 * v33;
	v35 = (char *)new char[2 * v33 + 1];
	sub_1000F030(v35, (int)v9, v42);
	v35[v34] = 0;
	v36 = (_BYTE*)new char[0x800u];
	v37 = v36;
	v38 = toBase64String_1000F200(v36, (int)v35, v45);	//--to  base64string
	v39 = v49;
	v37[v38] = 0;
	strcpy((char*)a2, (char*)v37);
	v50 = 1;
	delete [] v37;
	delete [] v35;
	delete [] v9;
	LOBYTE(v63) = 0;
//	nullsub_1(&v41);
	return v49;
}



int __stdcall decodeData(char * sOutBuffer, const char *pInputStr, char *sKey)
{
	if (g_fileReader.IsOk())
	{	
		return sub_1001F960((int)sOutBuffer, pInputStr, sKey);
	}
	return -1;
}

int __stdcall encodeData(char * sOutBuffer, const char *pInputStr, char *sKey)
{
	if (g_fileReader.IsOk())
	{
		return sub_1001F610((int)sOutBuffer, pInputStr, sKey);
	}
	return -1;
}