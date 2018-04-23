// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NetworkManager.h"
#include "FileManager.h"

int main()
{

	NetworkManager* nm = new NetworkManager();

	std::vector<HashFile> externalFileArr = FileManager::CreateAllHashFiles("C:\\Users\\s170895\\Desktop\\Client Updater (client)");
	FileManager::CreateNewFileStucFileInDir(externalFileArr, "C:\\Users\\s170895\\Desktop\\Client Updater (client)\\FileStructure.txt");

	nm->StartClientConnectionToServer("10.17.36.5", 8080);
	nm->RunClient();
	

    return 0;
}

