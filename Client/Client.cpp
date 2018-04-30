// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NetworkManager.h"
#include "FileManager.h"

int main()
{
	int start = clock();
	double diff;

	while (true)
	{
		diff = (clock() - start) / (double)(CLOCKS_PER_SEC);

		//wait a second before starting
		if (diff > 1.0f)
		{
			break;
		}
	}

	

	std::vector<HashFile> externalFileArr = FileManager::CreateAllHashFiles("C:\\Users\\s170895\\Desktop\\Client Updater (client)");
	FileManager::CreateNewFileStucFileInDir(externalFileArr, "C:\\Users\\s170895\\Desktop\\Client Updater (client)\\FileStructure.txt");

	NetworkManager::GetInstance().Initialise("C:\\Users\\s170895\\Desktop\\Client Updater (client)");


	NetworkManager::GetInstance().StartClientConnectionToServer("10.17.36.5", 60000);
	NetworkManager::GetInstance().RunClient();


	return 0;
}

