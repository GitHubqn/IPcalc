#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<CommCtrl.h>
#include<cstdio>
#include<iostream>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
	HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
	HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
	DWORD dwIPaddress = 0;
	DWORD dwIPmask = 0;
	CHAR sz_prefix[3]{};
	CONST INT SIZE = 256;

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		//InitCommonControls();
		HWND hUpDown = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hUpDown, UDM_SETRANGE, 0, MAKELPARAM(30, 1));
		//https://learn.microsoft.com/en-us/windows/win32/controls/udm-setrange
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
		std::cout << "WM_INITDIALOG:\n";
		std::cout << dwIPmask << std::endl;
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				BYTE first = FIRST_IPADDRESS(dwIPaddress);
				if (first < 128)dwIPmask = MAKEIPADDRESS(255, 0, 0, 0);
				else if (first < 192)dwIPmask = MAKEIPADDRESS(255, 255, 0, 0);
				else if (first < 224)dwIPmask = MAKEIPADDRESS(255, 255, 255, 0);
				SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
			}
		}
		break;
		case IDC_IPMASK:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
				int i = 1;
				for (; dwIPmask <<= 1; i++);

				sprintf(sz_prefix, "%i", i);
				SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)sz_prefix);
			}
		}
		break;
		case IDC_EDIT_PREFIX:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hEditPrefix, WM_GETTEXT, 3, (LPARAM)sz_prefix);
				DWORD dwIPprefix = atoi(sz_prefix);
				UINT dwIPmask = UINT_MAX;
				//					1111 1111	1111 1111	1111 1111	1111 1111
				//TODO: 

				//dwIPmask >>= (32 - dwIPprefix);
				dwIPmask <<= (32 - dwIPprefix);
				std::cout << "IDC_EDIT_PREFIX:\n";
				std::cout << std::hex << dwIPmask << "\n";
				if (dwIPprefix != 0)SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
			}
		}
		break;
		case IDOK:
		{

			CHAR sz_info[SIZE] = "Info:\nЗдесь будет информация о сети";	//sz_ - String Zero (NULL-Terminated Line)
			CHAR sz_number_of_hosts[SIZE]{};

			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			DWORD dwNetworkAddress = dwIPaddress & dwIPmask;
			DWORD dwBroadcastAddress = dwNetworkAddress | ~dwIPmask;
			std::cout << dwNetworkAddress << std::endl;
			strcat(sz_info, FormatIPaddress("Адресс сети:\t\t", dwNetworkAddress));
			strcat(sz_info, FormatIPaddress("Широковещательный адрес:", dwBroadcastAddress));
			strcat(sz_info, FormatMessageWithNumber("Количестов IP-адресов:\t",dwBroadcastAddress-dwNetworkAddress + 1));
			strcat(sz_info, FormatMessageWithNumber("Количестов узло:\t\t", dwBroadcastAddress - dwNetworkAddress - 1));



			HWND hInfo = GetDlgItem(hwnd, IDC_STATIC_INFO);
			SendMessage(hInfo, WM_SETTEXT, 0, (LPARAM)sz_info);
		}
		break;
		case IDCANCEL:EndDialog(hwnd, 0); break;
		}
	}
	break;
	case WM_CLOSE:
		FreeConsole();
		EndDialog(hwnd, 0);
	}
	return FALSE;
}

LPSTR FormatIPaddress(CONST CHAR* sz_message, DWORD IPaddress) 
{
	CHAR sz_buffer[256]{};
	sprintf(
		sz_buffer,
		"%s\t%i.%i.%i.%i\n",
		sz_message,
		FIRST_IPADDRESS(IPaddress),
		SECOND_IPADDRESS(IPaddress),
		THIRD_IPADDRESS(IPaddress),
		FOURTH_IPADDRESS(IPaddress)
	);
	return sz_buffer;
}
LPSTR FormatMessageWithNumber(CONST CHAR* sz_message, DWORD number) {
	CHAR sz_buffer[256]{};
	sprintf(sz_buffer, "%s%i\n", sz_message, number);
	return sz_buffer;
}