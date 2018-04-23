#include "stdafx.h"
#include "NetworkManager.h"
#include "FileList.h"
#include <iostream>
#include <fstream>


NetworkManager::NetworkManager()
{
	peerInterface = RakNet::RakPeerInterface::GetInstance();
	fileListTransfer = RakNet::FileListTransfer::GetInstance();
	fileList = RakNet::FileList::GetInstance();
}

class FileCB : RakNet::FileListTransferCBInterface
{
	bool OnFile(OnFileStruct* onFileStruct) override
	{
		//std::string("TempFileStucFile" + onFileStruct->senderSystemAddress.ToString() + ".txt")
		std::string systemAdress = onFileStruct->senderSystemAddress.ToString();
		std::ofstream outputFile("TempFileStucFile" + systemAdress + ".txt");

		outputFile << onFileStruct->fileData;

		return true;
	}

	void OnFileProgress(FileProgressStruct *fps) override
	{
		system("cls");
		std::cout << fps->partCount << "/" << fps->partTotal << std::endl;
	}

};

void NetworkManager::StartServer(const unsigned short port)
{



	std::string returnString;

	std::cout << "Opening Socket On Port " << port << std::endl;

	RakNet::SocketDescriptor sd(port, 0);

	peerInterface->Startup(2, &sd, 1);
	peerInterface->SetMaximumIncomingConnections(2);


	std::cout << "Starting Server\n";
}

void NetworkManager::StartClientConnectionToServer(const char* ip, const unsigned short port, std::string dir)
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



	FileListNodeContext flnc;
	fileList->AddFile(dir.c_str(),"test", flnc);

	std::cout << "the size of the file list is: " << fileList->fileList.Size() << std::endl;

	//while (true)
	//{
	/*RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)NetworkMsg::ID_GET_FILE_LENGTH_MSG);
	bs.Write("Ping!");*/
	////	
	//peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
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
	FileCB* fileCB = new FileCB();


	int lineCount = 0;
	RakNet::BitStream bs;
	RakNet::RakString str;

	while (true)
	{

		std::this_thread::sleep_for(std::chrono::seconds(1));

		std::cout << "Checking For Packets\n";



		for (packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet), packet = peerInterface->Receive())
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bs.Reset();

			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "A connection is incoming.\n";
				//	StartWaitingForFileToDownload(packet->systemAddress);
				bs.Write((RakNet::MessageID)NetworkMsg::ID_SENDFILE);
				bs.Write(fileListTransfer->SetupReceive((RakNet::FileListTransferCBInterface*)fileCB, true, RakNet::UNASSIGNED_SYSTEM_ADDRESS));
				peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
				std::cout << "Client please send your files\n";

				break;
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has disconnected.\n";
				break;
			case ID_CONNECTION_LOST:
				std::cout << "A client lost the connection.\n";
				break;
				//case ID_GET_FILE_LENGTH_MSG:

				//	/*bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				//	bsIn.Read(str);
				//	std::cout << str.C_String() << std::endl;
				//	lineCount = (int)str.C_String()[0];



				//	std::string hashFileMD5 = std::stoi(str.C_String());

				//	system("pause");
				//	std::cout << "Getting File From Client\n";

				//	if (str.C_String()[0] != '|')
				//	{
				//		newFileArr.push_back(std::string(str.C_String()));
				//	}
				//	else
				//	{
				//		returnArr = newFileArr;
				//		return;
				//	}*/
				//	break;
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
	RakNet::BitStream bs;
	std::string str;


	while (true)
	{
		for (packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet), packet = peerInterface->Receive())
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

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
			case ID_SENDFILE:
				std::cout << "Server is telling me to send a file\n";
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				bsIn.Read(str);

				unsigned short a = (unsigned short)std::strtoul(str.c_str(), NULL, 0);

				std::cout << str << std::endl;

				fileListTransfer->Send(fileList, peerInterface, RakNet::UNASSIGNED_SYSTEM_ADDRESS , std::stoi(str), HIGH_PRIORITY, RELIABLE_ORDERED);
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






