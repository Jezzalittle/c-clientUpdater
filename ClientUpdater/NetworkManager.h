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


class FileCB : public RakNet::FileListTransferCBInterface
{
public:
	bool OnFile(OnFileStruct* onFileStruct) 
	{

		std::cout << "did it work?\n";

		//std::string("TempFileStucFile" + onFileStruct->senderSystemAddress.ToString() + ".txt")
		std::string systemAdress = onFileStruct->senderSystemAddress.ToString();
		std::ofstream outputFile("TempFileStucFile" + systemAdress + ".txt");

		outputFile << onFileStruct->fileData;

		outputFile.close();

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

	RakNet::FileListTransfer fileListTransfer;

	RakNet::FileList fileList;

	enum NetworkMsg
	{

		ID_SENDFILE = ID_USER_PACKET_ENUM + 1,
		
	};

};

