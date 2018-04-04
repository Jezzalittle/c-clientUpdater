#pragma once
#include <RakPeerInterface.h>
#include <FileListTransfer.h>
#include <FileListTransferCBInterface.h>
#include <MessageIdentifiers.h>
#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>
#include <vector>
#include <chrono>
#include "HashFile.h"
#include "FileManager.h"


class NetworkManager
{
public:
	NetworkManager();

	void StartServer(const unsigned short port);
	void StartClientConnectionToServer(std::string ip, const unsigned short port);
	std::vector<HashFile> RunServer();

	~NetworkManager();
private:

	void UpdateServer(std::vector<HashFile>&);

	enum NetworkMsg
	{
		ID_GET_FILE_MSG = ID_USER_PACKET_ENUM + 1
	};

};

