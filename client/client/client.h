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

class Client
{
	public:
		Client(const SOCKET& client_sock);
		Client(const SOCKET& client_sock, const std::string& key);
		bool register_to_server(const std::string& username);
	private:
		const SOCKET& sock;
		RSAPrivateWrapper rsa_private;
		AESWrapper aes_wrapper;
		char client_id[CLIENT_ID_LEN];
};


#endif // !CLIENT_OPERATION_H

