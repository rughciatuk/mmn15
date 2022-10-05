#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cinttypes>
#include <cstring>
#include <WinSock2.h>

#define CLIENT_ID_LEN 16
#define MAX_NAME_LEN 255
#define DEFAULT_VERSION 3
#define PUBLIC_KEY_SIZE 160
#define AES_KEY_ENC_LEN 128
#define AES_KEY_LEN 16


#pragma pack(push, 1) // No padding as the struct isn't aligned
class request_header
{
public:
	enum request_code : uint16_t
	{
		registration = 1100,
		public_key = 1101,
		send_file = 1103,
		crc_correct = 1104,
		crc_failed = 1105,
		crc_final_failed = 1106
	};

	request_header(const uint8_t client_id[CLIENT_ID_LEN], uint8_t version, request_code code, uint32_t payload_size);
	request_header(const uint8_t client_id[CLIENT_ID_LEN], request_code code, uint32_t payload_size);
	request_header(request_code code, uint32_t payload_size);
	uint8_t client_id[CLIENT_ID_LEN]{};
	uint8_t version = 3;
	request_code code;
	uint32_t payload_size;
};

struct request_payload_registration
{
	char name[MAX_NAME_LEN];
};

struct request_payload_pubic_key
{
	char name[MAX_NAME_LEN];
	char public_key[PUBLIC_KEY_SIZE];
};

struct request_payload_send_file
{
	uint8_t client_id[CLIENT_ID_LEN]{};
	uint32_t content_size;
	char file_name[MAX_NAME_LEN];
};

class response_header
{
public:
	enum response_code : uint16_t
	{
		successful_registration = 2100,
		public_key_received = 2102,
		file_correct_crc = 2103,
		message_received = 2104,
		server_general_error = 3100
	};

	response_header(SOCKET sock);
	uint8_t version{};
	response_code code;
	uint32_t payload_size{};
};

struct response_payload_public_key_received
{
	uint8_t client_id[CLIENT_ID_LEN];
	uint8_t enc_aes_key[AES_KEY_ENC_LEN];
};

struct response_payload_file_received_crc
{
	uint8_t client_id[CLIENT_ID_LEN];
	uint32_t content_size;
	char file_name[MAX_NAME_LEN];
	uint32_t cksum;
};
#pragma pack(pop)

#endif // !PROTOCOL_H
