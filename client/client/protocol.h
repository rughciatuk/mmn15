#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cinttypes>
#include <cstring>
#include <WinSock2.h>

#define CLIENT_ID_LEN 16
#define MAX_NAME_LEN 255
#define DEFAULT_VERSION 3


enum request_code : uint16_t
{
	registration = 1100,
	public_key = 1101,
	send_file = 1103,
	crc_correct = 1104,
	crc_failed = 1105,
	crc_final_failed = 1106
};

enum response_code : uint16_t
{
	successful_registration = 2100,
	public_key_received = 2102,
	file_correct_crc = 2103,
	message_received = 2104,
	server_general_error = 3100
};

#pragma pack(push, 1) // No padding as the struct isn't aligned
class request_header
{
	public:
		request_header(uint8_t client_id[CLIENT_ID_LEN], uint8_t version, request_code code, uint32_t payload_size);
		request_header(uint8_t client_id[CLIENT_ID_LEN], request_code code, uint32_t payload_size);
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

class response_header
{
	public:
		response_header(SOCKET sock);
		uint8_t version{};
		response_code code;
		uint32_t payload_size{};
};
#pragma pack(pop)

#endif // !PROTOCOL_H
