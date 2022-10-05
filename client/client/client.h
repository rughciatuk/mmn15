#ifndef CLIENT_OPERATION_H
#define CLIENT_OPERATION_H


#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <cinttypes>
#include "crypto_wrapper/RSAWrapper.h"
#include "crypto_wrapper/AESWrapper.h"
#include "protocol.h"
#include "files.h"
#include "crc.h"

class Client
{
public:
	static Client create(const SOCKET& client_sock, std::string& fallback_username);
	Client(const SOCKET& client_sock, std::string& username);
	Client(const SOCKET& client_sock, std::string& username, const uint8_t client_id[CLIENT_ID_LEN],
	       const std::string& key);
	~Client();
	Client(const Client& client);
	bool op_register();
	bool op_send_public_key();
	bool op_send_file(std::string& file_name, int num_of_retry = 3);
	bool save_me_info();
	void handle_server_general_error(const response_header& header) const;
	void handle_server_general_error(const response_header& header, bool isExit) const;
private:
	const SOCKET& sock_;
	const std::string name_;
	uint8_t client_id_[CLIENT_ID_LEN]{};
	RSAPrivateWrapper rsa_private_;
	AESWrapper aes_wrapper_;
};


#endif // !CLIENT_OPERATION_H
