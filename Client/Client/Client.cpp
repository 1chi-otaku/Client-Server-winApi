// Client.cpp : Defines the entry point for the application.
//

#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "Client.h"
#include <iostream>
#include <string>
#include "WinSock2.h" 
#include <ws2tcpip.h> 

#pragma comment(lib, "Ws2_32.lib") 


const int MAXSTRLEN = 255;
using namespace std;

WSADATA wsaData;
SOCKET _socket; 
SOCKET acceptSocket;
sockaddr_in addr; 

HWND hEditReceivedMessage, hEditSentMessage, hEditIp;

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}


DWORD WINAPI GetMessage(LPVOID lp) {
	TCHAR str[MAXSTRLEN];
	char buf[MAXSTRLEN];
	int i = recv(_socket, buf, MAXSTRLEN, 0);
	buf[i] = '\0';

	wsprintf(str, TEXT(""), buf);
	SetWindowText(hEditReceivedMessage, str);

	return 0;
}


BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_APP:

		break;

	case WM_CLOSE:
		closesocket(_socket);
		WSACleanup();
		EndDialog(hWnd, 0);
		return TRUE;

	case WM_INITDIALOG: {
		hEditReceivedMessage = GetDlgItem(hWnd, IDC_RECEIVE);
		hEditSentMessage = GetDlgItem(hWnd, IDC_SENT);
		hEditIp = GetDlgItem(hWnd, IDC_IP);
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) // ðåãèñòðàöèÿ
		{
			WSACleanup();
			exit(10);
		}
		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (_socket == INVALID_SOCKET)
		{
			WSACleanup();
			exit(11);
		}
		addr.sin_family = AF_INET;
		inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
		addr.sin_port = htons(20000);
		SetWindowText(hEditIp, TEXT("127.0.0.1"));


		return TRUE;

	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CONNECT: {
			connect(_socket, (SOCKADDR*)&addr, sizeof(addr));
			CreateThread(NULL, 0, GetMessage, hWnd, 0, NULL);
			break;
		}
		case IDC_SEND: {
			TCHAR buffer[MAXSTRLEN];
			char converted[MAXSTRLEN];
			
			GetWindowText(hEditSentMessage, buffer, MAXSTRLEN);
			wcstombs(converted, buffer, wcslen(buffer) + 1);
			send(_socket, converted, wcslen(buffer), 0);

			break;
		}
		}
	}
	return FALSE;
}