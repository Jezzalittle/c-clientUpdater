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


	NetworkManager::GetInstance().Initialise("C:\\Users\\s170895\\Desktop\\Client Updater (Server)");

	NetworkManager::GetInstance().SetPath("C:\\Users\\s170895\\Desktop\\Client Updater (Server)");

	NetworkManager::GetInstance().StartServer(60000);
	std::vector<HashFile> localFileArr = NetworkManager::GetInstance().RunServer();
	return 0;
}

