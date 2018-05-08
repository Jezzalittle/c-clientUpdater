#include "stdafx.h"
#include "NetworkManager.h"
#include "FileList.h"
#include "FileOperations.h"



void NetworkManager::Initialise(std::string& a_path)
{
	peerInterface = RakNet::RakPeerInterface::GetInstance();
	path = a_path;
}


void NetworkManager::StartServer(const unsigned short port)
{
	std::string returnString;

	std::cout << "Opening Socket On Port " << port << std::endl;

	RakNet::SocketDescriptor sd(port, 0);

	peerInterface->Startup(2, &sd, 1);
	peerInterface->SetMaximumIncomingConnections(2);


	std::cout << "Starting Server\n";

	peerInterface->AttachPlugin(&fileListTransfer);

}

bool NetworkManager::StartClientConnectionToServer(const char* ip, const unsigned short port)
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
	int connectionTrys = 0;

	while (peerInterface->GetConnectionState(RakNet::AddressOrGUID(RakNet::SystemAddress(ip, port))) != RakNet::ConnectionState::IS_CONNECTED)
	{

		diff = (clock() - start) / (double)(CLOCKS_PER_SEC);

		if (diff > 3.0)
		{
			start = clock();

			std::cout << "Not Connected Yet: " << peerInterface->GetConnectionState(RakNet::AddressOrGUID(RakNet::SystemAddress(ip, port))) << std::endl;
			connectionTrys++;
			
			if (connectionTrys >= 5)
			{
				return false;
			}

		}
	}

	if (peerInterface->GetConnectionState(RakNet::AddressOrGUID(RakNet::SystemAddress(ip, port))) == RakNet::ConnectionState::IS_CONNECTED)
	{
		std::cout << "Connected :)\n";
		peerInterface->Ping(RakNet::SystemAddress(ip, port));
	}

	peerInterface->AttachPlugin(&fileListTransfer);


	std::string pathToFile = path + "\\FileStructure.txt";


	std::ifstream fileExist;
	
	
	fileExist.open(pathToFile);

	if (!fileExist.is_open())
	{
		std::cout << "file " << path << " not found\n";
		assert(false);
	}

	fileExist.close();

	fileList.AddFile(pathToFile.c_str(), "\\FileStructure.txt", FileListNodeContext(0, 0, 0, 0));
	return true;
}


void NetworkManager::SendMissingFilesToClient(std::vector<HashFile>& missingFiles)
{

	std::thread serverUpdate([&](std::vector<HashFile> missingFiles)
	{
		UpdateServer(missingFiles);
	}, missingFiles);
	
	serverUpdate.join();

	//UpdateServer(missingFiles);

}

void NetworkManager::RunClient()
{
	std::thread clientUpdate([&]()
	{
		UpdateClient();
	});
	
	clientUpdate.join();

	//UpdateClient();
}




void NetworkManager::UpdateServer(std::vector<HashFile>& returnArr)
{
	int start = clock();
	double diff;

	RakNet::Packet* packet = nullptr;


	RakNet::BitStream bs;

	int lineCount = 0;


	while (true)
	{

		std::this_thread::sleep_for(std::chrono::seconds(1));

		packet = nullptr;


		for (packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet), packet = peerInterface->Receive())
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			std::string str;
			bs.Reset();

			unsigned short returnID;


			switch (packet->data[0])
			{
			case ID_NEW_INCOMING_CONNECTION:
			{

				unsigned short sendID;
				std::cout << "A connection is incoming.\n";
				//	StartWaitingForFileToDownload(packet->systemAddress);
				bs.Write((RakNet::MessageID)NetworkMsg::ID_SENDFILE);
				sendID = fileListTransfer.SetupReceive(new FileCB, true, packet->systemAddress);

				bs.Write(sendID);
				peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				std::cout << "Client please send your files\n";

				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has disconnected.\n\n\n\n";
				break;
			case ID_CONNECTION_LOST:
				std::cout << "A client lost the connection.\n";
				break;

			case ID_SENDFILE:
			{
				std::cout << "the client has started sending the file\n";

				bool succeeded = false;
				RakNet::SystemAddress addressToSend = packet->systemAddress;
				std::string SystemAddressStr = addressToSend.ToString();
				SystemAddressStr.erase(SystemAddressStr.find('|'));

				do
				{
					succeeded = FileManager::ReadFromfileStucFileByDir(path + "\\FileStructure" + SystemAddressStr + ".txt", [=](FileManager::HashList&& hashList)
					{


						std::vector<HashFile> filesForClient = (FileManager::FindMissingFiles(hashList, returnArr));
						std::vector<HashFile> filesForClientToDelete(FileManager::FindFilesToDelete(hashList, returnArr));
						// add the files that are missing

						for (auto& file : filesForClient)
						{
							if (file.getFileLocation() != "\\FileStructure" + SystemAddressStr + ".txt" && file.getFileLocation() != "\\Server.exe")
							{
								std::string filePath = path + file.getFileLocation();
								fileList.AddFile(filePath.c_str(), file.getFileLocation().c_str(), FileListNodeContext(0, 0, 0, 0));
								std::cout << filePath << std::endl;
							}
						}

						RakNet::BitStream tempBS;

						tempBS.Write((RakNet::MessageID)NetworkMsg::ID_CLIENTCANFILESBESENT);

						for (auto& file : filesForClientToDelete)
						{
							RakNet::RakString writeString = file.gethashValue().c_str();
							writeString += '%';
							tempBS.Write(writeString);
						}


						peerInterface->Send(&tempBS, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addressToSend, false);

						FileManager::DeleteFilesFromDir(path + "\\FileStructure.txt");

					});
				} while (succeeded == false);


				break;
			}
			case ID_CLIENTFILESEND:
			{
				unsigned short msg;
				std::cout << "client is ready to be given the files\n";

				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				bsIn.Read(msg);

				fileListTransfer.Send(&fileList, peerInterface, packet->systemAddress, msg, HIGH_PRIORITY, 0);

				std::cout << "Files have been sent\n";

				bsIn.Reset();

				RakNet::SystemAddress systemAddress = packet->systemAddress;
				std::string SystemAddressStr = systemAddress.ToString();
				SystemAddressStr.erase(SystemAddressStr.find('|'));
				FileManager::DeleteFilesFromDir(path + "\\FileStructure" + SystemAddressStr + ".txt");

				std::thread waitingForDownload;

				waitingForDownload = std::thread([&, systemAddress]()
				{

					while (true)
					{
						if (finishedDownloading == true)
						{
							bs.Write((RakNet::MessageID)NetworkMsg::ID_FILESFINISHED);
							peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
							return;

						}
					}

				});

				waitingForDownload.join();

				break;
			}
			default:
				std::cout << "Received a message with a unknown id: " << packet->data[0];
				break;
			}
		}
	}
}

void NetworkManager::UpdateClient()
{
	RakNet::Packet* packet;

	bool waitForReconnect = false;

	std::cout << "the size of the file list is: " << fileList.fileList.Size() << std::endl;

	std::string sendID;

	RakNet::BitStream bs;
	unsigned short returnID;

	bool finsished = false;

	while (finsished == false)
	{

		for (packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet), packet = peerInterface->Receive())
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bs.Reset();
			std::string str = "";

			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:

				break;
			case ID_REMOTE_CONNECTION_LOST:

				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:

				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				std::cout << "Our connection request has been accepted.\n";
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				std::cout << "The server is full.\n";
				break;
			case ID_DISCONNECTION_NOTIFICATION:
			{
				std::cout << "We have been disconnected.\n";
				finsished = true;
				bs.Reset();
				bsIn.Reset();
				break;
			}
			case ID_CONNECTION_LOST:
				std::cout << "Connection lost.\n";
				break;
			case ID_SENDFILE:
			{
				std::cout << "Server is telling me to send a file\n";
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				unsigned short fileReturnID;

				bsIn.Read(fileReturnID);

				//returnID = (unsigned short)std::stoul(str, nullptr, 0);


				//std::this_thread::sleep_for(std::chrono::seconds(5));


				fileListTransfer.Send(&fileList, peerInterface, packet->systemAddress, fileReturnID, HIGH_PRIORITY, 0);


				std::cout << "files have been sent\n";
				bs.Write((RakNet::MessageID)NetworkMsg::ID_SENDFILE);

				peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				break;
			}
			case ID_CLIENTCANFILESBESENT:
			{
				std::cout << "server said its ready to send files\n";
				std::vector<HashFile> filesToDelete;
				std::vector<HashFile> clientFiles;

				RakNet::RakString filesToDeleteStr;

				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));


				bsIn.Read(filesToDeleteStr);

				int previousPos = 0;


				RakNet::RakString fileLoc;
				RakNet::RakString hashvalue;

				while (filesToDeleteStr.Find("%", previousPos) != std::string::npos)
				{
					hashvalue = filesToDeleteStr.SubStr(previousPos, filesToDeleteStr.Find("%", previousPos));
					//hashvalue.Erase(0, 1);
					HashFile hashFile(hashvalue.C_StringUnsafe());
					filesToDelete.push_back(hashFile);
					previousPos = filesToDeleteStr.Find("%", previousPos) + 1;
				}

				clientFiles = FileManager::CreateAllHashFiles(path);

				for (auto& clientFile : clientFiles)
				{
					if (clientFile.getFileLocation() != "\\Client.exe")
					{
						for (auto& fileToDelete : filesToDelete)
						{
							if (clientFile.gethashValue() == fileToDelete.gethashValue())
							{
								FileManager::DeleteFilesFromDir(path + clientFile.getFileLocation());
							}
						}
					}
				}

				sendID = std::to_string(fileListTransfer.SetupReceive(new FileCB, true, packet->systemAddress));


				bs.Write((RakNet::MessageID)NetworkMsg::ID_CLIENTFILESEND);
				bs.Write(sendID);

				peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				break;
			}
			case ID_FILESFINISHED:
			{
				//clean up
				FileManager::DeleteFilesFromDir(path + "\\FileStructure.txt");

				peerInterface->CloseConnection(packet->systemAddress, true);
				break;
			}
			default:
				std::cout << "Received a message with a unknown id: " << packet->data[0];
				break;
			}
		}

	}

	fileList.Clear();
	peerInterface->DetachPlugin(&fileListTransfer);
	fileListTransfer.ClearCallbacks();
	RakNet::RakPeerInterface::DestroyInstance(peerInterface);
//	fileListTransfer.RemoveCallback()
}











