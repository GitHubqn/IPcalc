#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>


using std::cin;
using std::cout;
using std::endl;


#define DEFAULT_PORT		"27015"
#define BUFFER_SIZE			1500


#pragma comment(lib, "Ws2_32.lib")

void main()
{
	setlocale(LC_ALL, " ");

	//1. инициализация winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult!=0)
	{
		cout << "WinSock initialization failed with error #" << iResult << endl;
		return;
	}
	//2. создаем сокет
	addrinfo* result = NULL;
	addrinfo* ptr = NULL;
	addrinfo hInst;

	//2.1 Получаем адресс текущего узла
	ZeroMemory(&hInst, sizeof(hInst));
	hInst.ai_family = AF_INET;
	hInst.ai_socktype = SOCK_STREAM;
	hInst.ai_protocol = IPPROTO_TCP;
	hInst.ai_flags = AI_PASSIVE;

	iResult = GetAddrInfo(NULL, DEFAULT_PORT, &hInst, &result);
	if (iResult != 0) {
		cout << "GetAddrInfo failed with error #" << iResult << endl;
		WSACleanup;
		return;
	}

	//2.2 создаем сокет
	SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Socket error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//3. Привязываем сокет к порту
	iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Bind failed with error #" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	freeaddrinfo(result);

	//4. Прослушиваем порт
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) 
	{
		cout << "Listen failed with error #" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	cout << "Server started on " << DEFAULT_PORT << endl;

	//5. Сокет клиента
	SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) 
	{
		cout<<"Accept failed with error #" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	//6. Получение и отправка данных
	char recvbuffer[BUFFER_SIZE]{};
	int received = 0;
	do {
		received = recv(ClientSocket, recvbuffer, BUFFER_SIZE, 0);
		if (received > 0)
		{
			cout << "Bytes received: \t" << received << endl;
			cout << "Recived message:\t" << recvbuffer << endl;
			int iSendResult = send(ClientSocket, "Привет Client", received, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				cout << "Send failed with error #" << WSAGetLastError() << endl;
				closesocket(ClientSocket);
				WSACleanup();
				return;
			}
			cout << "Bytes sent: " << iSendResult << endl;
		}
		else if (received == 0)	cout << "Connection closing..." << endl;
		else
		{
			cout <<"Received failed with error #" << WSAGetLastError() << endl;
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}
	} while (received > 0);

	//7.Отключить соединения
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Shutdown failed with error #" << WSAGetLastError() << endl;
		
	}
	closesocket(ClientSocket);
	WSACleanup();
	system("PAUSE");
}