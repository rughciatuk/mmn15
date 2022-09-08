#ifndef FILES_H
#define FILES_H

#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <fstream>
#include <string>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define TRANSFER_FILE "transfer.info"
#define TRANSFER_PORT_DELEIMTER ':'

#endif // !FILES_H

bool get_data_from_transfer_file(sockaddr_in& server_sockaddr, std::string& username, std::string file_to_upload);
