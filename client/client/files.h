#ifndef FILES_H
#define FILES_H

#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <fstream>
#include <string>
#include <iomanip>

#include "crypto_wrapper/Base64Wrapper.h"


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define CLIENT_ID_LEN 16

#define TRANSFER_FILE "transfer.info"
#define TRANSFER_PORT_DELIMITER ':'

#define ME_FILE "me.info"

bool get_data_from_transfer_file(sockaddr_in& server_sockaddr, std::string& username, std::string& file_to_upload);
bool get_data_from_me_info(std::string& username, uint8_t client_id[CLIENT_ID_LEN], std::string& private_key);
bool write_data_from_me_info(const std::string& username, uint8_t client_id[CLIENT_ID_LEN], const std::string& private_key);
bool read_file(std::string& file_name, std::string& output);

#endif // !FILES_H
