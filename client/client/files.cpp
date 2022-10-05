#include "files.h"



bool get_data_from_transfer_file(sockaddr_in& server_sockaddr, std::string& username, std::string& file_to_upload)
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

	transfer_file.close();

	return true;
}

bool get_data_from_me_info(std::string& username, uint8_t client_id[], std::string& private_key)
{
	std::ifstream me_file(ME_FILE);
	if (!me_file)
	{
		std::cout << "No " << ME_FILE << " found, registering..." << std::endl;
		return false;
	}

	//Getting the username
	std::getline(me_file, username);

	// Getting and parsing the client_id
	std::string id_in_hex;
	std::getline(me_file, id_in_hex);

	for (UINT i = 0; i < id_in_hex.size(); i += 2)
	{
		std::string temp_byte_string = id_in_hex.substr(i, 2);
		client_id[i / 2] = std::strtol(temp_byte_string.c_str(), nullptr, 16);
	}

	// Getting the private key and decoding it.
	std::string base64_key;
	std::getline(me_file, base64_key);

	private_key = Base64Wrapper::decode(base64_key);

	me_file.close();

	return true;
}

bool write_data_from_me_info(const std::string& username, uint8_t client_id[CLIENT_ID_LEN], const std::string& private_key)
{
	std::ofstream me_file(ME_FILE);

	if (!me_file)
	{
		std::cout << "Error: Couldn't open the " << ME_FILE << " for writing";
		return false;
	}

	// Writing the username to the file.
	me_file << username << std::endl;

	// Writing the client id to the file.
	for (int i=0; i< CLIENT_ID_LEN; i++)
	{
		me_file << std::setfill('0') << std::setw(2) << std::right << std::hex << static_cast<int>(client_id[i]);
	}
	me_file << std::endl;

	// Writing the private key to the file.

	me_file << Base64Wrapper::encode(private_key);

	me_file.close();

	return true;
}

bool read_file(std::string& file_name, std::string& output)
{
	const std::ifstream file(file_name, std::ios::in | std::ios::binary);
	if (!file)
	{
		return false;
	}

	std::ostringstream temp_buffer;

	temp_buffer << file.rdbuf();
	output = temp_buffer.str();
	return true;
}

