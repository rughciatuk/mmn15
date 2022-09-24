#include "client.h"


Client::Client(const SOCKET& client_sock) : sock(client_sock), rsa_private(), client_id{}
{
}

Client::Client(const SOCKET& client_sock, const std::string& key) : sock(client_sock), rsa_private(key), client_id{}
{
}

bool Client::register_to_server(const std::string& username)
{
	request_header header(
		nullptr, registration, sizeof(request_payload_registration)
	);

	request_payload_registration registration_payload = {0};


	memcpy(registration_payload.name, username.c_str(), username.size() + 1); // With null

	send(sock, reinterpret_cast<char*>(&header), sizeof(header), 0);
	send(sock, reinterpret_cast<char*>(&registration_payload), sizeof(registration_payload), 0);

	const response_header response(sock);
	switch (response.code)
	{
	case successful_registration:
		{
			// Handle the successful
			recv(this->sock, this->client_id, response.payload_size, 0);
			std::cout << "Successful registration" << std::endl;
			break;
		}
	case server_general_error:
		{
			// Handle server error, there is no need
			const auto server_error_text = new char[response.payload_size];
			recv(this->sock, server_error_text, response.payload_size, 0);
			std::cout << "An error return from the server: " << server_error_text << std::endl;
			std::cout << "Exiting..." << std::endl;
			delete[] server_error_text;
			exit(1);
		}
	default:
		{
			// The server return something unexpected.
			std::cout << "The server return unknown code: " << response.code << std::endl;
			std::cout << "Exiting..." << std::endl;
			exit(1);
		}
	}

	return true;
}
