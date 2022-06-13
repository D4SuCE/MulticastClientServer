#include <winsock2.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable: 4996)

int sConnect;

void ClientHandler()
{
	const int SIZE = 1024;
	char buffer[SIZE];
	int recvLength;

	while (true)
	{
		if ((recvLength = recv(sConnect, buffer, SIZE, 0)) < 0)
		{
			std::cout << "Couldn't receive message" << std::endl;
		}

		for (int i = 0; i < recvLength; i++)
		{
			std::cout << buffer[i];
		}
		std::cout << std::endl;
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

	std::string ipAddress;
	std::cout << "Enter an IP-address: ";
	std::cin >> ipAddress;

	int port;
	std::cout << "Enter a port: ";
	std::cin >> port;

	SOCKADDR_IN addr;
	int addrSize = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	if ((sConnect = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << "Couldn't create socket" << std::endl;
		return 2;
	}

	if (connect(sConnect, (SOCKADDR*)&addr, addrSize) != 0)
	{
		std::cout << "Couldn't connect to server" << std::endl;
		return 3;
	}

	std::string message;
	
	std::string name;
	std::cout << "Enter a name: ";
	std::cin >> name;

	std::cout << "Enter a message: " << std::endl;

	std::thread th(ClientHandler);
	th.detach();

	while (true)
	{		
		std::getline(std::cin, message);

		std::string newMessage = name + ": " + message;

		if (newMessage != name + ": ")
		{
			if (send(sConnect, newMessage.c_str(), newMessage.size(), 0) < 0)
			{
				std::cout << "Couldn't send message" << std::endl;
				return 4;
			}
		}	
	}

	WSACleanup();
	closesocket(sConnect);

	return 0;
}