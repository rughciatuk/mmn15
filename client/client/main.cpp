#include "main.h"

int main()
{
	sockaddr_in server_sockaddr = {0};
	std::string username;
	std::string file_to_upload;

	// Reading the transfer file.
	get_data_from_transfer_file(server_sockaddr, username, file_to_upload);

	// Setting up the windows socket.
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
		std::cout << "Could not connect to the server: " << WSAGetLastError() << std::endl;
	}

	std::string private_key;
	uint8_t client_id[CLIENT_ID_LEN];

	// Create the client;
	Client main_client = Client::create(connect_socket, username);

	if(main_client.op_send_file(file_to_upload))
	{
		std::cout << "File uploaded to the server." << std::endl;
	}

	return 0;
}

