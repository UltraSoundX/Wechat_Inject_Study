// DLL_Injector.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "DLL_Injector.h"
#include <Windows.h>
#include "resource.h"
#include <TlHelp32.h>
#include <stdio.h>
#define WECHAT_PROCESS_NAME "WeChat.exe"



INT_PTR Dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID InjectDll();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    DialogBox(hInstance, MAKEINTRESOURCE(Injector), NULL, &Dlgproc);
    return 0;
}


INT_PTR Dlgproc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG) {
        //MessageBox(NULL, "First", "Caption", 0);
    }

    if (uMsg == WM_CLOSE) {
        EndDialog(hwndDlg,0);
    }

    if (uMsg == WM_COMMAND) {
        if (wParam == Inject) {
            InjectDll();
        }
        if (wParam == Uninstall) {

        }
    }

    
    return FALSE;
}

//第一步 拿到微信句柄 获得微信位置
DWORD ProcessNameFindPID(LPCSTR Processname) {
    //获得所有系统进程
    HANDLE ProcessAll = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);
    //在快照中找到微信
    PROCESSENTRY32 processInfo = { 0 };
    processInfo.dwSize = sizeof(PROCESSENTRY32);
    do
    {
        if (strcmp(Processname, processInfo.szExeFile) == 0) {
            return processInfo.th32ProcessID;
        }
    } while (Process32Next(ProcessAll, &processInfo));

    //用找到的进程pid打开句柄
    return 0;
}
//第二步 申请dll内存存放dll路径
VOID InjectDll() {
    CHAR path[0x100] = ("C://Inject//inject.dll");
    //获取句柄
    DWORD PID = ProcessNameFindPID(WECHAT_PROCESS_NAME);
    if (PID == 0) {
        MessageBox(NULL, "没有找到微信进程", "Error", 0);
        return;
    }
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
    if (hProcess == NULL) {
        MessageBox(NULL, "Wechat Open Error", "Error", 0);
        return;
    }
    //申请内存地址
    LPVOID address = VirtualAllocEx(hProcess, NULL, strlen(path), MEM_COMMIT, PAGE_READWRITE);
    if (address == NULL) {
        MessageBox(NULL, "AllocEx Open Error", "Error", 0);
        return;
    }

    //DLL写入内存
    if (WriteProcessMemory(hProcess, address, path, strlen(path), NULL) == 0) {
        MessageBox(NULL, "Write Error", "Error", 0);
        return;
    }

    CHAR test[0x100] = { 0 };
    sprintf_s(test, "Address is %p", address);
    OutputDebugString(test);

    HMODULE k32 = GetModuleHandle("Kernel32.dll");
    LPVOID loadadd = GetProcAddress(k32, "LoadLibraryA");
    //LoadLibrary
    //第三步 注入dll 通过LoadLibrary加载Dll
    HANDLE execute = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadadd, address, 0, NULL);
    if (execute == NULL) {
        MessageBox(NULL, "Inject Error", "Error", 0);
        return;
    }
}
//Uninstall
//FreeLibrary(k32);