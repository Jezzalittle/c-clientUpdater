
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



class NetworkManager
{
public:

	static NetworkManager& GetInstance()
	{
		static NetworkManager instance;
		return instance;
	}
	NetworkManager(NetworkManager const&) = delete;
	void operator=(NetworkManager const&) = delete;

	void Initialise(std::string a_path);

	void StartServer(const unsigned short port);
	void StartClientConnectionToServer(const char* ip, const unsigned short port);
	void SendMissingFilesToClient(std::vector<HashFile>& missingFiles);

	void RunClient();


	void SetPath(std::string a_path)
	{
		path = a_path;
	}
	std::string GetPath()
	{
		return path;
	}

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



class FileCB : public RakNet::FileListTransferCBInterface
{
public:
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

		if (fileName == "\\FileStructure.txt")
		{
			fileName = "\\FileStructure" + systemAdress + ".txt";
			filePath = NetworkManager::GetInstance().GetPath() + fileName;
		}
		else
		{
			filePath = NetworkManager::GetInstance().GetPath() + onFileStruct->fileName;
		}


		std::ofstream outFile(filePath, std::ios::out | std::ios::binary);



		outFile.write(onFileStruct->fileData, onFileStruct->byteLengthOfThisFile);

		outFile.close();

		return true;
	}

	virtual void OnFileProgress(FileProgressStruct *fps)
	{

		std::cout << fps->partCount << "/" << fps->partTotal << std::endl;
	}

	virtual bool OnDownloadComplete(DownloadCompleteStruct *dcs)
	{

		std::cout << "Download complete.\n";
		NetworkManager::GetInstance().SetFinishedDownloading(true);
		return false;
	}

};
