// MultiWeChat.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "framework.h"
#include "MultiWeChat.h"
#include "resource.h"
#include <Windows.h>
#include <TlHelp32.h>


INT_PTR CALLBACK  Dialog(HWND, UINT, WPARAM, LPARAM);
VOID HookOpen();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    DialogBox(hInstance, MAKEINTRESOURCE(Main), NULL, &Dialog);
}



INT_PTR CALLBACK Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (message == Open)
        {
            HookOpen();
            MUTEX_ALL_ACCESS //All Access
            OpenMutexA(2031617, 1, name);
            CreateMutexW(NULL, 0, Wechat);
        }

    case WM_CLOSE:
        EndDialog(hDlg, 0);

    }


    return (INT_PTR)FALSE;
}

VOID HookOpen() {
    CreateToolhelp32Snapshot();
}
