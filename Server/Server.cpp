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

	std::vector<HashFile> externalFileArr = FileManager::CreateAllHashFiles("C:\\Users\\s170895\\Desktop\\Client Updater (Server)");
	FileManager::CreateNewFileStucFileInDir(externalFileArr, "C:\\Users\\s170895\\Desktop\\Client Updater (Server)\\FileStructure.txt");

	NetworkManager& manager = NetworkManager::GetInstance();
	manager.Initialise("C:\\Users\\s170895\\Desktop\\Client Updater (Server)");

	manager.SetPath("C:\\Users\\s170895\\Desktop\\Client Updater (Server)");

	manager.StartServer(60000);
	manager.SendMissingFilesToClient(externalFileArr);

	std::vector<HashFile> missingFiles;

	


	return 0;
}

