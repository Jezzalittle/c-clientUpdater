// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NetworkManager.h"
#include <vector>
#include <iostream>
#include <string>
#include "FileManager.h"


int main()
{

	std::string path;

	path = "C:\\Users\\s170895\\Desktop\\Client Updater (Server)";

	std::vector<HashFile> externalFileArr = FileManager::CreateAllHashFiles(path);
	FileManager::CreateNewFileStucFileInDir(externalFileArr, path);

	NetworkManager& manager = NetworkManager::GetInstance();
	manager.Initialise(path);

	manager.SetPath(path);

	manager.StartServer(60000);
	manager.SendMissingFilesToClient(externalFileArr);

	std::vector<HashFile> missingFiles;

	


	return 0;
}

