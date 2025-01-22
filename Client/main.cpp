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

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "WinSock initialization failed with error #" << iResult << endl;
		return;
	}

	addrinfo* result = NULL;
	addrinfo* ptr = NULL;
	addrinfo hInst;

	ZeroMemory(&hInst, sizeof(hInst));
	hInst.ai_family = AF_UNSPEC;
	hInst.ai_socktype = SOCK_STREAM;
	hInst.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo("192.168.0.111", DEFAULT_PORT, &hInst, &result);
	if (iResult != 0) {
		cout << "GetAddrInfo failed with error #" << iResult << endl;
		WSACleanup;
		return;
	}

	SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Socket error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//Присоединение к серверу
	iResult = connect(ConnectSocket, result->ai_addr, result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Unable to connect to Server #" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	//Отправка и получение данных
	const char sendbuffer[] = "Client Test meesage";
	char recvbuffer[BUFFER_SIZE]{};

	//Отправка данных
	iResult = send(ConnectSocket, sendbuffer, strlen(sendbuffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Send failed wth error #" << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	cout << "Bytes sent: " << iResult << endl;


	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Shutdown failed with error #" << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	//Получение данных
	int received = 0;
	do {
		received = recv(ConnectSocket, recvbuffer, BUFFER_SIZE, 0);
		if (received > 0)
		{
			cout << "Bytes received: \t" << received << endl;
			cout << "Recived message:\t" << recvbuffer << endl;
		}
		else if 
			(received == 0)	cout << "Connection closing..." << endl;
		else
			cout << "Received failed with error #" << WSAGetLastError() << endl;
	} while (received > 0);

	//Отключение от сервера
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Shutdown failed with error #" << WSAGetLastError() << endl;
	}
	closesocket(ConnectSocket);
	WSACleanup();
	system("PAUSE");
}