#pragma once
#include <RakPeerInterface.h>
#include <FileListTransfer.h>
#include <FileListTransferCBInterface.h>
#include <MessageIdentifiers.h>
#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>

class NetworkManager
{
public:
	NetworkManager();

	void StartServer(const unsigned short port);
	void StartNetworkSocketAsClient(std::string ip, const unsigned short port);
	void StartWaitingForFileToDownload(RakNet::SystemAddress id);
	~NetworkManager();
private:


};

