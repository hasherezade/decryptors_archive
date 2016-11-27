#include <stdio.h>
#include <windows.h>

#define DLL_PATH "polarsslwrapper"
#define DECRYPT_FUNC "DecryptFileWrapper"
#define ID_FUNC "GetHardwareIdWrapper"

typedef bool (__cdecl *t_DecryptFileWrapper)(const char *filePath, BYTE *privateKey, size_t privateKeyLen);

t_DecryptFileWrapper loadDecryptFunc()
{
	HMODULE hLib = LoadLibraryA(DLL_PATH);
	if (hLib == NULL) {
		printf("Could not load the DLL!\n");
		return NULL;
	} else {
		printf("DLL loaded\n");
	}

	t_DecryptFileWrapper func = (t_DecryptFileWrapper) GetProcAddress(hLib, DECRYPT_FUNC);
	if (func == NULL) {
		printf("Could not get the %s\n", DECRYPT_FUNC);
		return NULL;
	}
	printf("Got the %s\n", DECRYPT_FUNC);
	return func;
}
