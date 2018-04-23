#include "stdafx.h"
#include "NetworkManager.h"



NetworkManager::NetworkManager()
{
	peerInterface = RakNet::RakPeerInterface::GetInstance();
}

void NetworkManager::StartServer(const unsigned short port)
{



	std::string returnString;

	std::cout << "Opening Socket On Port " << port << std::endl;

	RakNet::SocketDescriptor sd(port, 0);

	peerInterface->Startup(2, &sd, 1);
	peerInterface->SetMaximumIncomingConnections(2);

	std::cout << "Starting Server\n";
}

void NetworkManager::StartClientConnectionToServer(const char* ip, const unsigned short port)
{
	RakNet::SocketDescriptor sd;


	peerInterface->Startup(2, &sd, 1);

	std::cout << "Connecting to server at: " << ip << std::endl;

	RakNet::ConnectionAttemptResult res = peerInterface->Connect(ip, port, nullptr, 0);

	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Unable to start connection, Error number: " << res << std::endl;
		system("pause");
	}

	int start = clock();
	double diff;

	while (peerInterface->GetConnectionState(RakNet::AddressOrGUID(RakNet::SystemAddress(ip, port))) != RakNet::ConnectionState::IS_CONNECTED)
	{
		diff = (clock() - start) / (double)(CLOCKS_PER_SEC);

		if (diff > 3.0)
		{
			start = clock();

			std::cout << "Not Connected Yet: " << peerInterface->GetConnectionState(RakNet::AddressOrGUID(RakNet::SystemAddress(ip, port))) << std::endl;


		}
	}

	if (peerInterface->GetConnectionState(RakNet::AddressOrGUID(RakNet::SystemAddress(ip, port))) == RakNet::ConnectionState::IS_CONNECTED)
	{
		std::cout << "Connected :)\n";
		peerInterface->Ping(RakNet::SystemAddress(ip, port));
	}


	//while (true)
	//{
	//	RakNet::BitStream bs;
	//	bs.Write((RakNet::MessageID)NetworkMsg::ID_GET_FILE_MSG); 
	//	bs.Write("Ping!"); 
	////	
	//	rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	//	
	//}
}

std::vector<HashFile> NetworkManager::RunServer()
{
	std::vector<HashFile> returnArr;

	std::thread serverUpdate([&](std::vector<HashFile> arr)
	{
		UpdateServer(returnArr);
	}, returnArr);

	serverUpdate.join();

	return returnArr;

}

void NetworkManager::RunClient()
{
	std::thread clientUpdate([&]()
	{
		UpdateClient();
	});

	clientUpdate.join();
}



void NetworkManager::UpdateServer(std::vector<HashFile>& returnArr)
{
	int start = clock();
	double diff;

	RakNet::Packet* packet;
	std::vector<HashFile> newFileArr;

	while (true)
	{

		std::this_thread::sleep_for(std::chrono::seconds(1));

		std::cout << "Checking For Packets\n";

	

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
		//std::cout << "Number of Connections: " << peerInterface->GetLastPing() << std::endl;
	}

}

void NetworkManager::UpdateClient()
{
	RakNet::Packet* packet;

	while (true)
	{
		for (packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet), packet = peerInterface->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				std::cout << "Another client has disconnected.\n";
				break;
			case ID_REMOTE_CONNECTION_LOST:
				std::cout << "Another client has lost the connection.\n";
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				std::cout << "Another client has connected.\n";
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				std::cout << "Our connection request has been accepted.\n";
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				std::cout << "The server is full.\n";
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "We have been disconnected.\n";
				break;
			case ID_CONNECTION_LOST:
				std::cout << "Connection lost.\n";
				break;

			default:
				std::cout << "Received a message with a unknown id: " << packet->data[0];
				break;
			}
		}
	}
}






NetworkManager::~NetworkManager()
{
}






