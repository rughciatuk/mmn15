#ifndef CLIENT_OPERATION_H
#define CLIENT_OPERATION_H


#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <cinttypes>
#include "crypto_wrapper/RSAWrapper.h"
#include "crypto_wrapper/AESWrapper.h"
#include "crypto_wrapper/Base64Wrapper.h"
#include "protocol.h"
#include "files.h"

class Client
{
public:
	Client(const SOCKET& client_sock, std::string& username);
	Client(const SOCKET& client_sock, std::string& username, const uint8_t client_id[CLIENT_ID_LEN],
	       const std::string& key);
	bool op_register();
	bool op_send_public_key();
	bool save_me_info();
	void handle_server_general_error(const response_header& header) const;
	void handle_server_general_error(const response_header& header, bool isExit) const;
private:
	const SOCKET& sock_;
	const std::string name_;
	uint8_t client_id_[CLIENT_ID_LEN];
	RSAPrivateWrapper rsa_private_;
	AESWrapper aes_wrapper_;
};


#endif // !CLIENT_OPERATION_H
