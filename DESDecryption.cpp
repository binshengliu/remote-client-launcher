#include "stdafx.h"
#include "DESDecryption.h"
#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>

// Link with the Advapi32.lib file.
#pragma comment (lib, "advapi32")
bool des_decrypt(const BYTE *key, BYTE *text, DWORD *length)
{ 
	bool fReturn = false;

	// Get the handle to the default provider. 
	HCRYPTPROV hCryptProv = NULL; 
	if(!CryptAcquireContext(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0)) {
		fprintf(stderr, "Error during CryptAcquireContext!\n");
		goto Exit_MyDecryptFile;
	}
	//-----------------------------------------------------------
	// Create a hash object. 
	HCRYPTHASH hHash = NULL; 
	if(!CryptCreateHash( hCryptProv, CALG_MD5, 0, 0, &hHash)) {
		fprintf(stderr, "Error during CryptCreateHash!\n");
		goto Exit_MyDecryptFile;
	}

	//-----------------------------------------------------------
	// Hash in the password data. 
	if(!CryptHashData(hHash, key, strlen((const char *)key), 0)) 
	{
		fprintf(stderr, "Error during CryptHashData!\n"); 
		goto Exit_MyDecryptFile;
	}

	//-----------------------------------------------------------
	// Derive a session key from the hash object. 
	HCRYPTKEY hKey = NULL; 
	if(!CryptDeriveKey(hCryptProv, CALG_DES, hHash, 0, &hKey))
	{ 
		fprintf(stderr, "Error during CryptDeriveKey!\n"); 
		goto Exit_MyDecryptFile;
	}

	BYTE IV[] = {0, 0, 0, 0, 0, 0, 0, 0};
	if (!CryptSetKeyParam(hKey, KP_IV, IV, 0)) {
		fprintf(stderr, "Error during Set IV!\n");
		goto Exit_MyDecryptFile;
	}

	//---------------------------------------------------------------
	// Decrypt the block of data. 
	if(!CryptDecrypt(hKey, 0, TRUE, 0, text, length))
	{
		fprintf(stderr, "Error during CryptDecrypt!\n");
		goto Exit_MyDecryptFile;
	}

	fReturn = true;

Exit_MyDecryptFile:

	//-----------------------------------------------------------
	// Release the hash object. 
	if(hHash) 
	{
		if(!(CryptDestroyHash(hHash)))
		{
			fprintf(stderr, "Error during CryptDestroyHash.\n"); 
		}

		hHash = NULL;
	}

	//---------------------------------------------------------------
	// Release the session key. 
	if(hKey)
	{
		if(!(CryptDestroyKey(hKey)))
		{
			fprintf(stderr, "Error during CryptDestroyKey!\n");
		}
	} 

	//---------------------------------------------------------------
	// Release the provider handle. 
	if(hCryptProv)
	{
		if(!(CryptReleaseContext(hCryptProv, 0)))
		{
			fprintf(stderr, "Error during CryptReleaseContext!\n");
		}
	} 

	return fReturn;
}

void str2byte( DWORD _dwLen, LPSTR _pStr, LPBYTE _pByte)
{
	char pTmp[3] = {0};

	for(DWORD i = 0; i < _dwLen / 2; ++i)
	{
		pTmp[0] = _pStr[2*i];
		pTmp[1] = _pStr[2*i+1];
		*_pByte = (BYTE)strtol(pTmp, 0, 16);
		++_pByte;
	}
}