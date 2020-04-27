// DLL_Injector 2.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "DLL_Injector 2.h"
#include "resource.h"
#include<TlHelp32.h>
#include <Windows.h>
#include<stdio.h>
// 微信进程名
#define WECHAT_PROCESS_NAME "WeChat.exe";
INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
VOID InjectDll();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(ID_MAIN), NULL, &DialogProc);
	return 0;
}
INT_PTR CALLBACK DialogProc(HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG) {
		//MessageBox(NULL, "首次加载", "标题", 0);
	}
	// 关闭界面操作
	if (uMsg == WM_CLOSE) {
		EndDialog(hWnd, 0);
	}
	// 所有的界面上的按钮事件都会走这个宏
	if (uMsg == WM_COMMAND) {
		if (wParam == INJECT_DLL) {
			InjectDll();//注入 Dll
		}
		if (wParam == UN_DLL) {
		}
	}
	return FALSE;
}

// 获取进程快照
DWORD ProcessNameFindPID(LPCSTR ProcessName)
{
	// #include <TlHelp32.h>
	// 获取进程快照
	HANDLE ProcessAll = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);// 获取整个系统的进程快照
	// 在快照中对比过 processname 进程名称
	PROCESSENTRY32 processInfo = { 0 };
	processInfo.dwSize = sizeof(PROCESSENTRY32);
	do
	{
		// WeChat.exe
		// th32ProcessID 进程ID， szExeFile进程名
		if (strcmp(ProcessName, (char*)processInfo.szExeFile) == 0) {
			return processInfo.th32ProcessID;
		}
	} while (Process32Next(ProcessAll, &processInfo));
	return 0;
}

VOID InjectDll()
{
	CHAR pathStr[0x100] = { "C://Inject//GetMessage.dll" };
	// 获取微信PID
	DWORD PID = ProcessNameFindPID("WeChat.exe");
	if (PID == 0) {
		MessageBox(NULL, "没有找到微信进程或微信没有启动", "错误", 0);
		return;
	}
	// 通过PID获取句柄 C++中句柄类型为 HANDLE
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (NULL == hProcess) {
		MessageBox(NULL, "进程打开失败，可能权限不足或关闭了微信应用", "错误", 0);
		return;
	}
	//申请内存,内存大小为我们要注入 DLL 的大小
	LPVOID dllAdd = VirtualAllocEx(hProcess, NULL, sizeof(pathStr), MEM_COMMIT, PAGE_READWRITE);
	if (NULL == dllAdd) {
		MessageBox(NULL, "内存分配失败", "错误", 0);
		return;
	}
	// 将 DLL 写入到对应的内存地址中
	if (WriteProcessMemory(hProcess, dllAdd, pathStr, strlen(pathStr), NULL) == 0) {
		MessageBox(NULL, "路径写入失败", "错误", 0);
		return;
	}
	// 创建一块内存用来打印一段文本，方便看效果
	CHAR test[0x100] = { 0 };
	sprintf_s(test, "写入的地址为：%p", dllAdd);
	OutputDebugString(test);
	//将地址写入后，通过 Kernel32.dll 调用该地址的 DLL 就完成了
	HMODULE k32 = GetModuleHandle("Kernel32.dll");
	LPVOID loadAdd = GetProcAddress(k32, "LoadLibraryA");
	// 远程执行我们的dll，通过注入的dll地址以及CreateRemoteThread方法让微信进程调用起我们的进程
	HANDLE exec = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadAdd, dllAdd, 0, NULL);
	if (exec == NULL) {
		MessageBox(NULL, "远程注入失败", "错误", 0);
		return;
	}
}
