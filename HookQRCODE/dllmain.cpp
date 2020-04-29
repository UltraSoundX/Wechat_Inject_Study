// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include <atlimage.h>
#include "resource.h"

BYTE BackUpCode[5] = { 0 };
VOID StartHook(LPVOID fAdd, HWND hDlg);
DWORD GetWeChatWin();
VOID showQRCODE();
VOID Unhook();


INT_PTR CALLBACK Dlgproc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DialogBox(hModule, MAKEINTRESOURCE(Main), NULL, &Dlgproc);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

INT_PTR CALLBACK Dlgproc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam) {
    switch (message)
    {
    case WM_INITDIALOG:
        break;
    case WM_CLOSE:
        EndDialog(hDlg, 0);
    case WM_COMMAND:
        if (wParam == Hook) {
            //Start Hook
            StartHook(showQRCODE,hDlg);
            SetDlgItemText(hDlg, Status, "Hook Start");
        }
        if (wParam == UnHook)
        {
            //Start UnHook
            Unhook();
            SetDlgItemText(hDlg, Status, "Hook End");
        }
        break;
    }
    return FALSE;
}

//声明裸函数 最小化运行
//pushad pushsd popad popsd 直接操作出入栈
//寄存器 FPU声明
HWND picpos;
DWORD oldEAX = 0;
DWORD oldECX = 0;
DWORD oldEDX = 0;
DWORD oldEBX = 0;
DWORD oldESP = 0;
DWORD oldEBP = 0;
DWORD oldESI = 0;
DWORD oldEDI = 0;
DWORD returnadd = 0;

VOID saveQRCODE(DWORD ecx) {
    //ECX -> 真二维码数据
    //ECX + 0x4 -> 二维码长度
    char PicData[0xFFF] = { 0 };
    DWORD picLen = ecx + 0x04;//图片长度地址
    size_t cpyLen = (size_t)*((LPVOID *)picLen);
    memcpy(PicData, *(LPVOID *)ecx, cpyLen);
    FILE *pFile;
    fopen_s(&pFile, "qrcode.png", "wb");
    //数据写入
    fwrite(PicData, sizeof(char), sizeof(PicData), pFile);
    fclose(pFile);

    CImage img;
    CRect rect;
    HWND picshowpos = GetDlgItem(picpos, Photo);
    GetClientRect(picshowpos, &rect);
    img.Load("qrcode.png");
    img.Draw(GetDC(picshowpos), rect);
}

VOID __declspec(naked) showQRCODE() {
    //备份FPU __asm操作汇编 mov 和 = 一样 裸函数无声明
    __asm {
        mov oldEAX, eax
        mov oldECX, ecx
        mov oldEDX, edx
        mov oldEBX, ebx
        mov oldESP, esp
        mov oldEBP, ebp
        mov oldESI, esi
        mov oldEDI, edi
    }
    saveQRCODE(oldECX);//保存二维码 转交ECX
    returnadd = GetWeChatWin() + 0x22E34C;
    __asm {
        mov eax, oldEAX
        mov ecx, oldECX 
        mov edx, oldEDX
        mov ebx, oldEBX
        mov esp, oldESP
        mov ebp, oldEBP
        mov esi, oldESI
        mov edi, oldEDI
        jmp returnadd
    }//恢复FPU

}

DWORD GetWeChatWin() {
    return (DWORD)LoadLibrary("WeChatWin.dll");
}

VOID StartHook(LPVOID fAdd, HWND hDlg) {
    picpos = hDlg;
    DWORD WeChatWin = GetWeChatWin();
    DWORD HookAdd = WeChatWin + 0x22E347;//要Hook的地址

    BYTE JMPCode[5] = { 0 };
    JMPCode[0] = 0xE9;
    *(DWORD*)&JMPCode[1] = (DWORD)fAdd - HookAdd - 5; //公式 减去JMP的5个字节

    //BackUp Hook
    HANDLE WeChatHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
    if (ReadProcessMemory(WeChatHandle, (LPCVOID)HookAdd, BackUpCode, 5, NULL) == 0) {
        MessageBox(NULL, "备份失败", "Error", 0);
        return;
    }

    //Write Hook
    if (WriteProcessMemory(WeChatHandle, (LPVOID)HookAdd, JMPCode, 5, NULL) == 0) {
        MessageBox(NULL, "写入失败", "Error", 0);
    }

    return;
}

VOID Unhook() {
    HANDLE WeChatHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
    DWORD WeChatWin = GetWeChatWin();
    DWORD HookAdd = WeChatWin + 0x22E347;//要Hook的地址
    if (WriteProcessMemory(WeChatHandle, (LPVOID)HookAdd, BackUpCode, 5, NULL) == 0) {
        MessageBox(NULL, "Error", "Error", 0);
    }
}