#pragma once

#include <windows.h>

struct SCNotification;

struct NppData
{
    HWND _nppHandle = nullptr;
    HWND _scintillaMainHandle = nullptr;
    HWND _scintillaSecondHandle = nullptr;
};

typedef void (__cdecl * PFUNCPLUGINCMD)();

struct ShortcutKey
{
    bool _isCtrl = false;
    bool _isAlt = false;
    bool _isShift = false;
    UCHAR _key = 0;
};

const int menuItemSize = 64;

struct FuncItem
{
    wchar_t _itemName[menuItemSize] = { L'\0' };
    PFUNCPLUGINCMD _pFunc = nullptr;
    int _cmdID = 0;
    bool _init2Check = false;
    ShortcutKey* _pShKey = nullptr;
};

extern "C" __declspec(dllexport) void setInfo(NppData);
extern "C" __declspec(dllexport) const wchar_t* getName();
extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int*);
extern "C" __declspec(dllexport) void beNotified(SCNotification*);
extern "C" __declspec(dllexport) LRESULT messageProc(UINT, WPARAM, LPARAM);
extern "C" __declspec(dllexport) BOOL isUnicode();
