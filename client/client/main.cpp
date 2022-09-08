#include "main.h"

int main()
{
	sockaddr_in server_sockaddr = { 0 };
	std::string username;
	std::string file_to_upload;


	get_data_from_transfer_file(server_sockaddr, username, file_to_upload);

	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != NO_ERROR) 
	{
		std::cout << "Could not start wsa: " << result << std::endl;
	}

	SOCKET connect_socket;
	connect_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connect_socket == INVALID_SOCKET)
	{
		std::cout << "Could not create socket: " << WSAGetLastError() << std::endl;
	}

	result = connect(connect_socket, (sockaddr*)&server_sockaddr, sizeof(server_sockaddr));
	if (result == SOCKET_ERROR)
	{
		std::cout << "Could not connect to the server" << WSAGetLastError() << std::endl;
	}

	return 0;
}