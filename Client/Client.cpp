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

	NetworkManager& nm = NetworkManager::GetInstance();

	std::string path;
	path = "C:\\Users\\s170895\\Desktop\\Client Updater (client)";
	

	std::vector<HashFile> externalFileArr = FileManager::CreateAllHashFiles(path);
	FileManager::CreateNewFileStucFileInDir(externalFileArr, path);

	nm.Initialise(path);


	nm.StartClientConnectionToServer("127.0.0.1", 60000);
	nm.RunClient();

	system("pause");

	return 0;
}

