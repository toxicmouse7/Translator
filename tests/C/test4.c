#include <Windows.h>

typedef int (*MessageBoxAProto)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
typedef char* (*ptrFun)(char*);

char* getCaption(char* caption)
{
    return caption;
}

int main()
{
    HMODULE user32 = LoadLibraryA("user32.dll");
    FARPROC messageBoxAddress = GetProcAddress(user32, "MessageBoxA");

    MessageBoxAProto MessageBoxA = (MessageBoxAProto)messageBoxAddress;

    ptrFun getCaptionPtr = (ptrFun)&getCaption;

    char* caption = getCaptionPtr("Test ptr caption");

    MessageBoxA(0, "Test", caption, 0);

    return 0;
}