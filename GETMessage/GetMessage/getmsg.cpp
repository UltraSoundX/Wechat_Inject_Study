//WeChat 6A6F0000
//WXNAME 16B48AC
//WXPHONE  16B48E0  
//WXID 16B4A10
//WXPHOTO 16B4B74->zhizhen
#include "pch.h"
#include "resource1.h"
#include "getmsg.h"
#include <stdio.h>

//WechatWin.dll
DWORD getWechatWin() {
	HMODULE WechatAdd = LoadLibrary("WeChatWin.dll");
	return (DWORD)WechatAdd;
}
//Read Mem
VOID ReadMemory(HWND hWnd) {
	DWORD WeChatWin = getWechatWin();
	char Nickname_Get[0x100] = { 0 };
	sprintf_s(Nickname_Get, "%s", WeChatWin + 0x16B48AC);
	SetDlgItemText(hWnd,Nickname,Nickname_Get);

	char Phone_Get[0x100] = { 0 };
	sprintf_s(Phone_Get, "%s", WeChatWin + 0x16B48E0);
	SetDlgItemText(hWnd, Phone, Phone_Get);

	char WXID_Get[0x100] = { 0 };
	sprintf_s(WXID_Get, "%s", (WeChatWin + 0x16B4A10));
	SetDlgItemText(hWnd, ID, WXID_Get);

	char Photo_Get[0x100] = { 0 };
	DWORD pPic = WeChatWin + 0x16CE308;//Pointer 1
	sprintf_s(Photo_Get, "%s", *((DWORD *)pPic));
	SetDlgItemText(hWnd, Photo, Photo_Get);

	return;
}

VOID WriteMemory(HWND hWnd) {
	DWORD WeChatWin = getWechatWin();
	DWORD wxid = WeChatWin + 0x16B4A10; //wxid offset

	char wxidText[0x100] = { 0 };
	GetDlgItemText(hWnd, ID, wxidText, sizeof(wxidText));
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)wxid, wxidText, sizeof(wxidText), NULL);
}