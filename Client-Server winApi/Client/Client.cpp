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
HWND hButtonStop, hButtonSend;

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}


DWORD WINAPI GetMessage(LPVOID lp) {
	
	char buffer[MAXSTRLEN];
	TCHAR buffer2[MAXSTRLEN];
	while (true)
	{
		int i = recv(_socket, buffer, MAXSTRLEN, 0);
		swprintf(buffer2, MAXSTRLEN, L"%hs", buffer);
		SetWindowText(hEditReceivedMessage,buffer2);
	}
	

	return 0;
}

DWORD WINAPI SendMessage(LPVOID lp) {
	TCHAR buffer[MAXSTRLEN];
	char converted[MAXSTRLEN];

	GetWindowText(hEditSentMessage, buffer, MAXSTRLEN);
	wcstombs(converted, buffer, wcslen(buffer) + 1);
	send(_socket, converted, wcslen(buffer), 0);

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
		hButtonStop = GetDlgItem(hWnd, IDC_STOP);
		hButtonSend = GetDlgItem(hWnd, IDC_SEND);
		EnableWindow(hButtonSend, FALSE);
		EnableWindow(hButtonStop, FALSE);
		SetWindowText(hEditIp, TEXT("127.0.0.1"));



		return TRUE;

	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CONNECT: {
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonStop, TRUE);

			TCHAR buff[256];
			char buff2[256];
			GetWindowText(hEditIp, buff, 256);
			wcstombs(buff2, buff, MAXSTRLEN);

			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) // ðåãèñòðàöèÿ
			{
				WSACleanup();
				exit(10);
				EndDialog(hWnd, 0);
			}

			_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (_socket == INVALID_SOCKET)
			{
				WSACleanup();
				exit(11);
				EndDialog(hWnd, 0);
			}


			addr.sin_family = AF_INET;
			inet_pton(AF_INET, buff2, &addr.sin_addr);
			addr.sin_port = htons(20000);

			wcstombs(buff2, buff, 256);
			connect(_socket, (SOCKADDR*)&addr, sizeof(addr));
			bind(acceptSocket, (SOCKADDR*)&addr, sizeof(addr));
			listen(acceptSocket, 1);
			CreateThread(NULL, 0, GetMessage, hWnd, 0, NULL);
			break;
		}
		case IDC_SEND: {
			CreateThread(NULL, 0, SendMessage, hWnd, 0, NULL);
			break;
		}

		case IDC_STOP:
			WSACleanup();
			exit(10);
			EndDialog(hWnd, 0);

			break;
		}
	}
	return FALSE;
}