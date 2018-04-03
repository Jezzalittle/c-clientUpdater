#include "stdafx.h"
#include "NetworkManager.h"



NetworkManager::NetworkManager()
{
}

void NetworkManager::StartServer(const unsigned short port)
{
	RakNet::RakPeerInterface* peerInterface = nullptr;

	std::string returnString;

	std::cout << "Opening Socket On Port " << port << std::endl;

	peerInterface = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd(port, 0);

	peerInterface->Startup(32, &sd, 1);
	peerInterface->SetMaximumIncomingConnections(32);

	RakNet::FileListTransfer* ftp = RakNet::FileListTransfer::GetInstance();

	RakNet::Packet* packet = nullptr;

	while (true)
	{
		std::cout << "Waiting For A Connection\n";

		for (packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet), packet = peerInterface->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "A connection is incoming.\n";
				StartWaitingForFileToDownload(packet->systemAddress);
				break;
			default:
				assert(true);
				break;
			}
		}
	}

	system("cls");

}

void NetworkManager::StartNetworkSocketAsClient(std::string ip, const unsigned short port)
{
	RakNet::SocketDescriptor sd;
	RakNet::RakPeerInterface* rakPeerInterface = RakNet::RakPeerInterface::GetInstance();

	rakPeerInterface->Startup(1, &sd, 1);

	std::cout << "Connecting to server at: " << ip.c_str() << std::endl;

	RakNet::ConnectionAttemptResult res = rakPeerInterface->Connect(ip.c_str(), port, nullptr, 0);
	
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED) 
	{ 
		std::cout << "Unable to start connection, Error number: " << res << std::endl;
		assert(true);
	}
}

void NetworkManager::StartWaitingForFileToDownload(RakNet::SystemAddress id)
{
	RakNet::FileListTransfer* ftp = RakNet::FileListTransfer::GetInstance();
	RakNet::FileListTransferCBInterface* file;
	ftp->SetupReceive(file, true, id);
}




NetworkManager::~NetworkManager()
{
}
