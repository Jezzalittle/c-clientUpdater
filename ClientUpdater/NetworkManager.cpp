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

	std::cout << "Starting Server\n";
}

void NetworkManager::StartClientConnectionToServer(const char* ip, const unsigned short port)
{
	RakNet::SocketDescriptor sd;
	RakNet::RakPeerInterface* rakPeerInterface = RakNet::RakPeerInterface::GetInstance();

	rakPeerInterface->Startup(1, &sd, 1);

	std::cout << "Connecting to server at: " << ip << std::endl;

	RakNet::ConnectionAttemptResult res = rakPeerInterface->Connect(ip, port, nullptr, 0);

	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Unable to start connection, Error number: " << res << std::endl;
		system("pause");
	}
	system("pause");

	//while (true)
	//{
		RakNet::BitStream bs;
		bs.Write((RakNet::MessageID)NetworkMsg::ID_GET_FILE_MSG); 
		bs.Write("Ping!"); 
	//	
		rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

	//}
}

std::vector<HashFile> NetworkManager::RunServer()
{
	std::vector<HashFile> returnArr;

	//std::thread serverUpdate([&](std::vector<HashFile> arr)
	//{
	UpdateServer(returnArr);

	//}, returnArr);

	//serverUpdate.join();

	return returnArr;

}



void NetworkManager::UpdateServer(std::vector<HashFile>& returnArr)
{
	int start = clock();
	double diff;

	RakNet::Packet* packet = nullptr;
	RakNet::RakPeerInterface* peerInterface = RakNet::RakPeerInterface::GetInstance();
	std::vector<HashFile> newFileArr;

	while (true)
	{

		//diff = (clock() - start) / (double)(CLOCKS_PER_SEC);

		//if (diff > 3.0)
		//{
		//	start = clock();

		//	std::cout << "Checking For Packets\n";


			for (packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet), packet = peerInterface->Receive())
			{
				RakNet::RakString str;
				RakNet::BitStream bsIn(packet->data, packet->length, false);

				switch (packet->data[0])
				{
				case ID_NEW_INCOMING_CONNECTION:
					std::cout << "A connection is incoming.\n";
					//	StartWaitingForFileToDownload(packet->systemAddress);
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					std::cout << "A client has disconnected.\n";
					break;
				case ID_CONNECTION_LOST:
					std::cout << "A client lost the connection.\n";
					break;
				case ID_GET_FILE_MSG:
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(str);
					std::cout << str.C_String() << std::endl;

					system("pause");
					/*	std::cout << "Getting File From Client\n";

					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(str);
					if (str.C_String()[0] != '|')
					{
						newFileArr.push_back(std::string(str.C_String()));
					}
					else
					{
						returnArr = newFileArr;
						return;
					}*/
					break;
				default:
					std::cout << "Received a message with a unknown id: " << packet->data[0];
					break;
				}
			}
		//}
	}
}






NetworkManager::~NetworkManager()
{
}






