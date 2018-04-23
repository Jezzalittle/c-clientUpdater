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
	NetworkManager* nm = new NetworkManager();

	std::vector<HashFile> externalFileArr = FileManager::CreateAllHashFiles("C:\\Users\\s170895\\Desktop\\Client Updater (Server)");
	FileManager::CreateNewFileStucFileInDir(externalFileArr, "C:\\Users\\s170895\\Desktop\\Client Updater (Server)\\FileStructure.txt");

	nm->StartServer(8080);
	std::vector<HashFile> localFileArr = nm->RunServer();
	return 0;
}

