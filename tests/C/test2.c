#include <Windows.h>

typedef int (*MessageBoxAProto)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

char* getCaption()
{
    return "Test Caption";
}

int main()
{
    HMODULE user32 = LoadLibraryA("user32.dll");
    FARPROC messageBoxAddress = GetProcAddress(user32, "MessageBoxA");

    MessageBoxAProto MessageBoxA = (MessageBoxAProto)messageBoxAddress;

    MessageBoxA(0, "Test", getCaption(), 0);

    return 0;
}