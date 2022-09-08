#include "files.h"

bool get_data_from_transfer_file(sockaddr_in& server_sockaddr, std::string& username, std::string file_to_upload)
{
	std::ifstream transfer_file(TRANSFER_FILE);

	if (!transfer_file)
	{
		std::cout << "Error: file could not be opened" << std::endl;
		return false;
	}

	std::string server_address;
	std::string server_port;

	// Extracting the address and port.
	std::getline(transfer_file, server_address, ':');
	std::getline(transfer_file, server_port);

	server_sockaddr.sin_family = AF_INET;
	inet_pton(AF_INET, server_address.c_str(), &server_sockaddr.sin_addr.s_addr);
	server_sockaddr.sin_port = htons(atoi(server_port.c_str()));

	// Getting the username
	std::getline(transfer_file, username);

	// Getting the file to upload
	std::getline(transfer_file, file_to_upload);

	return true;
}
