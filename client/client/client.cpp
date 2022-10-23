#include "client.h"


Client Client::create(const SOCKET& client_sock, std::string& fallback_username)
{
	std::string username;
	uint8_t client_id[CLIENT_ID_LEN];
	std::string private_key;

	// If we were able to read the needed data from the file we can create the client with this data.
	if (get_data_from_me_info(username, client_id, private_key))
	{
		Client registered_client(client_sock, username, client_id, private_key);
		registered_client.op_send_public_key();
		return registered_client;
	}

	Client new_user(client_sock, fallback_username);
	if (new_user.op_register())
	{
		// We can save the client data
		new_user.save_me_info();
		// Sending out public key
		new_user.op_send_public_key();
	}

	return new_user;
}

Client::Client(const SOCKET& client_sock, std::string& username) : sock_(client_sock), name_(std::move(username)),
                                                                   rsa_private_(), client_id_{}
{
}

Client::Client(const SOCKET& client_sock, std::string& username, const uint8_t client_id[CLIENT_ID_LEN],
               const std::string& key) : sock_(client_sock), name_(std::move(username)), client_id_{}, rsa_private_(key)
{
	memcpy((void*)this->client_id_, client_id, CLIENT_ID_LEN);
}

Client::~Client() = default;

Client::Client(const Client& client): sock_(client.sock_), name_(client.name_),
                                      rsa_private_(client.rsa_private_.getPrivateKey()),
                                      aes_wrapper_(client.aes_wrapper_.getKey(), AESWrapper::DEFAULT_KEYLENGTH)
{
	memcpy((void*)this->client_id_, client.client_id_, CLIENT_ID_LEN);
}

bool Client::op_register()
{
	// Creating the registration header.
	request_header header(
		nullptr, request_header::registration, sizeof(request_payload_registration)
	);

	request_payload_registration registration_payload = {0};

	memcpy(registration_payload.name, this->name_.c_str(), this->name_.size() + 1); // With null

	// Sending the request to the server
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
	case response_header::failed_registration:
		{
			// Failed to register
			std::cout << "Failed to register" << std::endl;
			exit(1);
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

bool Client::op_send_file(std::string& file_name, int num_of_retry)
{
	// We need to create the encrypted file first (we need to know the size);
	std::string file_data;
	if (!read_file(file_name, file_data))
	{
		std::cout << "The file " << file_name << " does not exists" << std::endl;
		return false;
	}

	// We need to encrypt the data.
	const std::string enc_data = this->aes_wrapper_.encrypt(file_data.c_str(), file_data.size());

	// Creating the request to send.
	request_payload_send_file send_file_payload;
	memcpy(send_file_payload.client_id, this->client_id_, CLIENT_ID_LEN);
	send_file_payload.content_size = enc_data.size();
	memcpy(send_file_payload.file_name, file_name.c_str(), file_name.size() + 1);

	request_header header(this->client_id_, request_header::send_file,
	                      sizeof(send_file_payload));


	// Send the header->payload->enc_file.
	send(this->sock_, reinterpret_cast<char*>(&header), sizeof(header), 0);
	send(this->sock_, reinterpret_cast<char*>(&send_file_payload), sizeof(send_file_payload), 0);
	send(this->sock_, enc_data.c_str(), enc_data.size(), 0);


	CRC crc;
	const uint32_t cksum_value = crc.calc_crc(file_data);

	// Getting the response.
	const response_header response(this->sock_);

	// We have gotten something weird from the server
	if (response.code != response_header::file_correct_crc)
	{
		std::cout << "The server return unexpected code: " << response.code << std::endl;
		std::cout << "Exiting..." << std::endl;
		exit(1);
	}

	response_payload_file_received_crc response_payload{};

	recv(this->sock_, reinterpret_cast<char*>(&response_payload), response.payload_size, 0);

	request_payload_crc_answer crc_payload;
	memcpy(crc_payload.client_id, send_file_payload.client_id, CLIENT_ID_LEN);
	memcpy(send_file_payload.file_name, file_name.c_str(), file_name.size() + 1);

	// The file was uploaded successfully
	if (response_payload.cksum == cksum_value)
	{
		request_header answer_header(this->client_id_, request_header::crc_correct, 0);
		send(this->sock_, reinterpret_cast<char*>(&answer_header), sizeof(header), 0);
		send(this->sock_, reinterpret_cast<char*>(&crc_payload), sizeof(crc_payload), 0);
		// We need to wait for the server ok.
		const response_header answer_response(this->sock_);

		return true;
	}
	else if (num_of_retry == 0) // Done trying.
	{
		std::cout << "Failed to upload and done retrying, Exiting..." << std::endl;
		request_header answer_header(this->client_id_, request_header::crc_final_failed, 0);
		send(this->sock_, reinterpret_cast<char*>(&answer_header), sizeof(header), 0);
		send(this->sock_, reinterpret_cast<char*>(&crc_payload), sizeof(crc_payload), 0);
		// We need to wait for the server ok.
		const response_header answer_response(this->sock_);

		exit(1);
	}
	else
	{
		std::cout << "CRC not valid, trying again." << std::endl;
		// TODO: send crc not valid
		request_header answer_header(this->client_id_, request_header::crc_failed, 0);
		send(this->sock_, reinterpret_cast<char*>(&answer_header), sizeof(header), 0);
		send(this->sock_, reinterpret_cast<char*>(&crc_payload), sizeof(crc_payload), 0);

		// We need to wait for the server ok.
		const response_header answer_response(this->sock_);

		return this->op_send_file(file_name, num_of_retry - 1);
	}
}

bool Client::save_me_info()
{
	return write_data_from_me_info(this->name_, this->client_id_, this->rsa_private_.getPrivateKey());
}
