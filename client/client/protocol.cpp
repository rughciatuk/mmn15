#include "protocol.h"

request_header::request_header(const uint8_t client_id[CLIENT_ID_LEN], uint8_t version, request_code code,
                               uint32_t payload_size)
{
	if (client_id == nullptr)
	{
		memset(this->client_id, 0, CLIENT_ID_LEN);
	}
	else // We need to copy the data to the client_id
	{
		memcpy(this->client_id, client_id, CLIENT_ID_LEN);
	}

	this->version = 3;
	this->code = code;
	this->payload_size = payload_size;
}

request_header::request_header(const uint8_t client_id[CLIENT_ID_LEN], request_code code, uint32_t payload_size) :
	request_header(
		client_id, DEFAULT_VERSION, code, payload_size)
{
}

request_header::request_header(request_code code, uint32_t payload_size): request_header(
	nullptr, DEFAULT_VERSION, code, payload_size)
{
}

// Reading the response_header from the sock
response_header::response_header(SOCKET sock)
{
	char data[sizeof(response_header)];
	recv(sock, data, sizeof(response_header), 0);

	memcpy(&this->version, &data[0], sizeof(this->version));
	memcpy(&this->code, &data[1], sizeof(this->code));
	memcpy(&this->payload_size, &data[3], sizeof(this->payload_size));
}
