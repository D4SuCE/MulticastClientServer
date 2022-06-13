#include <winsock2.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable: 4996)

const int MaxConnections = 100;
int sConnections[MaxConnections];
int counter;

void ClientHandler(int index)
{
	const int SIZE = 1024;
	char buffer[SIZE];
	std::string message;
	int recvLength;

	while (true)
	{
		message = "";
		recvLength = recv(sConnections[index], buffer, SIZE, 0);

		for (int i = 0; i < recvLength; i++)
		{
			message += buffer[i];
		}

		for (int i = 0; i < counter; i++)
		{
			if (i == index)
			{
				continue;
			}
			send(sConnections[i], message.c_str(), message.size(), 0);
		}
	}
}

int main()
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 1), &wsa) != 0)
	{
		std::cout << "WSAStartup error" << std::endl;
		return 1;
	}

	SOCKADDR_IN addr;
	int addrSize = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9999);
	addr.sin_family = AF_INET;

	int sListen;
	if ((sListen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "Couldn't create socket" << std::endl;
		return 2;
	}

	if (bind(sListen, (SOCKADDR*)&addr, addrSize) != 0)
	{
		std::cout << "Couldn't bind socket" << std::endl;
		return 3;
	}

	if (listen(sListen, SOMAXCONN) != 0)
	{
		std::cout << "Couldn't listen socket" << std::endl;
		return 4;
	}

	int sConnection;

	for (int i = 0; i < MaxConnections; i++)
	{
		if ((sConnection = accept(sListen, (SOCKADDR*)&addr, &addrSize)) < 0)
		{
			std::cout << "Couldn't accept socket" << std::endl;
		}
		else
		{
			std::cout << "Client connected" << std::endl;
		}

		sConnections[i] = sConnection;
		counter++;

		std::thread th(ClientHandler, i);
		th.detach();
	}

	WSACleanup();
	closesocket(sListen);
	closesocket(sConnection);
	for (auto& socket : sConnections)
	{
		closesocket(socket);
	}

	return 0;
}