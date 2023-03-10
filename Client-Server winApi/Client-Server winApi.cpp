// Client-Server winApi.cpp : Defines the entry point for the application.
//
#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "Client-Server winApi.h"
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

HWND hEditReceivedMessage, hEditSentMessage;
HWND hButtonStop, hButtonSend;


BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}


DWORD WINAPI GetMessage(LPVOID lp) {

	char buf[MAXSTRLEN];
	TCHAR buffer2[MAXSTRLEN];
	while (true)
	{
		acceptSocket = accept(_socket, NULL, NULL);
		int i = recv(acceptSocket, buf, MAXSTRLEN, 0);
		buf[i] = '\0';
		swprintf(buffer2, MAXSTRLEN, L"%hs", buf);
		SetWindowText(hEditReceivedMessage, buffer2);
	}
	

	return 0;
}


DWORD WINAPI SendMessage(LPVOID lp) {

	TCHAR buffer[MAXSTRLEN];
	char converted[MAXSTRLEN];

	GetWindowText(hEditSentMessage, buffer, MAXSTRLEN);
	wcstombs(converted, buffer, MAXSTRLEN);
	send(acceptSocket, converted, MAXSTRLEN, 0);

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
		hButtonSend = GetDlgItem(hWnd, IDC_SEND);
		hButtonStop = GetDlgItem(hWnd, IDC_STOP);
		EnableWindow(hEditReceivedMessage, TRUE);
		EnableWindow(hEditSentMessage, FALSE);
		EnableWindow(hButtonSend, FALSE);
		EnableWindow(hButtonStop, FALSE);
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) 
		{
			system("pause");
			WSACleanup();
			exit(10);
		}
		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		addr.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
		addr.sin_port = htons(20000);

		return TRUE;

	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_START: {
			EnableWindow(hEditSentMessage, TRUE);
			EnableWindow(hButtonSend, TRUE);
			EnableWindow(hButtonStop, TRUE);
			bind(_socket, (SOCKADDR*)&addr, sizeof(addr));
			listen(_socket, 1);
			CreateThread(NULL, 0, GetMessage, hWnd, 0, NULL);
			break;
		}
		case IDC_SEND:
			CreateThread(NULL, 0, SendMessage, hWnd, 0, NULL);

			break;
		case IDC_STOP:
			WSACleanup();
			exit(10);
			EndDialog(hWnd, 0);

			break;
		
		}

	}
	return FALSE;
}