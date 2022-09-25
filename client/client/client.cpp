#include "client.h"


Client::Client(const SOCKET& client_sock, std::string& username) : sock_(client_sock), name_(std::move(username)),
                                                                   rsa_private_(), client_id_{}
{
}

Client::Client(const SOCKET& client_sock, std::string& username, const uint8_t client_id[CLIENT_ID_LEN],
               const std::string& key) : sock_(client_sock), name_(std::move(username)), client_id_{}, rsa_private_(key)
{
	memcpy((void*)this->client_id_, client_id, CLIENT_ID_LEN);
}

void Client::handle_server_general_error(const response_header& header) const
{
	const auto server_error_text = new char[header.payload_size];
	recv(this->sock_, server_error_text, header.payload_size, 0);
	std::cout << "An error return from the server: " << server_error_text << std::endl;
	delete[] server_error_text;
}

void Client::handle_server_general_error(const response_header& header, bool isExit) const
{
	this->handle_server_general_error(header);
	std::cout << "Exiting..." << std::endl;
	exit(1);
}


bool Client::op_register()
{
	request_header header(
		nullptr, request_header::registration, sizeof(request_payload_registration)
	);

	request_payload_registration registration_payload = {0};


	memcpy(registration_payload.name, this->name_.c_str(), this->name_.size() + 1); // With null

	send(this->sock_, reinterpret_cast<char*>(&header), sizeof(header), 0);
	send(this->sock_, reinterpret_cast<char*>(&registration_payload), sizeof(registration_payload), 0);

	const response_header response(this->sock_);
	switch (response.code)
	{
	case response_header::successful_registration:
		{
			// Handle the successful
			recv(this->sock_, (char*)this->client_id_, response.payload_size, 0);
			std::cout << "Successful registration" << std::endl;
			break;
		}
	case response_header::server_general_error:
		{
			// Handle server error, there is no need
			this->handle_server_general_error(response, true);
			break;
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

bool Client::op_send_public_key()
{
	request_header header(this->client_id_, request_header::public_key, sizeof(request_payload_pubic_key));
	request_payload_pubic_key payload = {0};

	memcpy(payload.name, this->name_.c_str(), this->name_.size() + 1); // For the null terminator
	memcpy(payload.public_key, this->rsa_private_.getPublicKey().c_str(), this->rsa_private_.getPublicKey().size());

	send(this->sock_, reinterpret_cast<char*>(&header), sizeof(header), 0);
	send(this->sock_, reinterpret_cast<char*>(&payload), sizeof(payload), 0);

	const response_header response(this->sock_);

	// We have gotten something weird from the server
	if (response.code != response_header::public_key_received)
	{
		std::cout << "The server return unexpected code: " << response.code << std::endl;
		std::cout << "Exiting..." << std::endl;
		exit(1);
	}

	// We get the response payload

	response_payload_public_key_received response_payload{};

	recv(this->sock_, reinterpret_cast<char*>(&response_payload), response.payload_size, 0);

	std::string aes_key = this->rsa_private_.decrypt(reinterpret_cast<char*>(&response_payload.enc_aes_key),
	                                                 AES_KEY_ENC_LEN);
	this->aes_wrapper_ = AESWrapper(reinterpret_cast<const unsigned char*>(aes_key.c_str()),
	                                AESWrapper::DEFAULT_KEYLENGTH);

	return false;
}

bool Client::save_me_info()
{
	return write_data_from_me_info(this->name_, this->client_id_, this->rsa_private_.getPrivateKey());
}
