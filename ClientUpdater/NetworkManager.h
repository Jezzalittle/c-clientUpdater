
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
#include <iostream>
#include <fstream>




class TestFileListProgress : public RakNet::FileListProgress
{
	virtual void OnFilePush(const char *fileName, unsigned int fileLengthBytes, unsigned int offset, unsigned int bytesBeingSent, bool done, RakNet::SystemAddress targetSystem, unsigned short setID)
	{
		printf("Sending %s bytes=%i offset=%i\n", fileName, bytesBeingSent, offset);
	}

	virtual void OnFilePushesComplete(RakNet::SystemAddress systemAddress, unsigned short setID)
	{
		char str[32];
		systemAddress.ToString(true, (char*)str);
		RAKNET_DEBUG_PRINTF("File pushes complete to %s\n", str);
	}
	virtual void OnSendAborted(RakNet::SystemAddress systemAddress)
	{
		char str[32];
		systemAddress.ToString(true, (char*)str);
		RAKNET_DEBUG_PRINTF("Send aborted to %s\n", str);
	}
};

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
	std::vector<HashFile> RunServer();
	void RunClient();


	void SetPath(std::string a_path)
	{
		path = a_path;
	}
	std::string GetPath()
	{
		return path;
	}




private:

	NetworkManager() { /*...*/ };

	std::string path;


	void UpdateServer(std::vector<HashFile>&);
	void UpdateClient();

	RakNet::RakPeerInterface * peerInterface;

	RakNet::FileListTransfer fileListTransfer;

	RakNet::FileList fileList;


	enum NetworkMsg
	{

		ID_SENDFILE = ID_USER_PACKET_ENUM + 1,

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


		std::string filePath = NetworkManager::GetInstance().GetPath() + "\\FileStructure_copy.txt";

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

		return false;
	}

};
