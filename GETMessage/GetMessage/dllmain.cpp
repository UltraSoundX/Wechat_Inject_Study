// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "resource1.h"
#include <Windows.h>
#include <stdio.h>
#include "getmsg.h"

INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DialogBox(hModule, MAKEINTRESOURCE(Main),NULL,&DialogProc);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

INT_PTR CALLBACK DialogProc(HWND hWndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        ReadMemory(hWndDlg);
        MessageBox(hWndDlg, "First Load", "Go", 0);
        break;
    case WM_COMMAND:
        if (wParam == GETMSG) {
            ReadMemory(hWndDlg);
        }
        if (wParam == WRITEMSG) {
            WriteMemory(hWndDlg);
        }
    }
    return FALSE;
}
