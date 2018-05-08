
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
#include "FileManager.h"
#include "HashFile.h"
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;



class NetworkManager
{
public:

	//Get an instance
	static NetworkManager& GetInstance()
	{
		static NetworkManager instance;
		return instance;
	}


	NetworkManager(NetworkManager const&) = delete;
	void operator=(NetworkManager const&) = delete;


	//Call before anything else and pass the path we are in
	void Initialise(std::string& a_path);

	//Starts a Server Socket on port
	void StartServer(const unsigned short port);

	//Starts a Client Socket on port and tries to connect to the Server with IP
	bool StartClientConnectionToServer(const char* ip, const unsigned short port);

	//Send missing files from server to client
	void SendMissingFilesToClient(std::vector<HashFile>& missingFiles);

	//Run client waiting for servers files
	void RunClient();

	//A path to the solution directory
	void SetPath(std::string& a_path)
	{
		path = a_path;
	}
	
	//A path to the solution directory
	std::string& GetPath()
	{
		return path;
	}

	//Finished Downloading?
	void SetFinishedDownloading(bool value)
	{
		finishedDownloading = value;
	}



private:

	NetworkManager() { /*...*/ };

	std::string path;


	void UpdateServer(std::vector<HashFile>&);
	void UpdateClient();

	RakNet::RakPeerInterface * peerInterface;

	RakNet::FileListTransfer fileListTransfer;

	RakNet::FileList fileList;

	bool finishedDownloading;


	enum NetworkMsg
	{
		ID_SENDFILE = ID_USER_PACKET_ENUM + 1,
		ID_CLIENTCANFILESBESENT,
		ID_CLIENTFILESEND,
		ID_FILESFINISHED,

	};

};


//Callback class not for user
class FileCB : public RakNet::FileListTransferCBInterface
{
public:

	//Callback function not for user
	bool OnFile(OnFileStruct* onFileStruct)
	{

		printf("OnFile: %i. (100%%) %i/%i %s %ib / %ib\n",
			onFileStruct->setID,
			onFileStruct->fileIndex + 1,
			onFileStruct->numberOfFilesInThisSet,
			onFileStruct->fileName,
			onFileStruct->byteLengthOfThisFile,
			onFileStruct->byteLengthOfThisSet);

		std::string filePath;
		std::string fileName(onFileStruct->fileName);
		std::string systemAdress = onFileStruct->senderSystemAddress.ToString();
		std::size_t found = systemAdress.find('|');
		systemAdress.erase(found);

		std::string delim = "\\";

		auto next = 2u;

		if (fileName == "\\FileStructure.txt")
		{
			fileName = "\\FileStructure" + systemAdress + ".txt";
			filePath = NetworkManager::GetInstance().GetPath() + fileName;
		}
		else
		{
			filePath = NetworkManager::GetInstance().GetPath() + fileName;
			
			fs::path path(filePath);

			fs::create_directories(path.parent_path());

		}


		std::ofstream outFile(filePath, std::ios::out | std::ios::binary);



		outFile.write(onFileStruct->fileData, onFileStruct->byteLengthOfThisFile);

		outFile.close();

		return true;
	}

	//Callback function not for user
	virtual void OnFileProgress(FileProgressStruct *fps)
	{

		std::cout << fps->partCount << "/" << fps->partTotal << std::endl;
	}
	
	//Callback function not for user
	virtual bool OnDownloadComplete(DownloadCompleteStruct *dcs)
	{

		std::cout << "Download complete.\n";
		NetworkManager::GetInstance().SetFinishedDownloading(true);
		return false;
	}

};
