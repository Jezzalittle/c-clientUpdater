#pragma once
#include <RakPeerInterface.h>
#include <FileListTransfer.h>
#include <FileListTransferCBInterface.h>
#include <MessageIdentifiers.h>
#include <string>
#include <thread>
#include <stdio.h>
#include <vector>
#include <chrono>
#include "HashFile.h"



class NetworkManager
{
public:
	NetworkManager();

	void StartServer(const unsigned short port);
	void StartClientConnectionToServer(const char* ip, const unsigned short port, std::string dir);
	std::vector<HashFile> RunServer();
	void RunClient();

	~NetworkManager();

private:

	void UpdateServer(std::vector<HashFile>&);
	void UpdateClient();

	class RakNet::RakPeerInterface * peerInterface;
	class RakNet::FileListTransfer* fileListTransfer;
	class RakNet::FileList* fileList;

	enum NetworkMsg
	{

		ID_SENDFILE = ID_USER_PACKET_ENUM + 1,
		
	};

};

