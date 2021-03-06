// Public domain
#include <stdio.h>
#include <windows.h>
#ifdef __TINYC__
LPWSTR*     WINAPI CommandLineToArgvW(LPCWSTR,int*);
#else
#include <shellapi.h>
#endif
int main (int argc,char *argv[]) {
    int pe32;
    if( argc < 2 ) {
        printf ("No file specified\n");
        return -1;
    }
    wchar_t** wargv = CommandLineToArgvW (GetCommandLineW(), &argc);
    HANDLE hFile = CreateFileW (wargv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if ((ERROR_SHARING_VIOLATION != GetLastError()) && (ERROR_FILE_NOT_FOUND != GetLastError())) {
        HANDLE hFileMapping = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        LPVOID lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
        PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
        PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + (DWORD)pDosHeader->e_lfanew);
        if ((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_32BIT_MACHINE)) {
            if ((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE))
                pe32 = 1;
            else if ((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_DLL))
                pe32 = 1;
            else
                pe32 = 0;
        }
        else
            pe32 = 0;
        UnmapViewOfFile (lpFileBase);
        CloseHandle (hFileMapping);
        CloseHandle (hFile);
        if (pe32) {
            printf ("1\n");
            return 0;
        }
        else {
            printf ("0\n");
            return 1;
        }
    }
    else {
            printf ("Input error:\n%s\n", argv[1]);
            return -1;
    }
}