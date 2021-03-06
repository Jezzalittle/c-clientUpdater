// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NetworkManager.h"
#include "FileManager.h"

#include <direct.h>
#define GetCurrentDir _getcwd


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

	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		return errno;
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

	std::string ip;

	//ip = "127.0.0.1";

	std::cout << "Please Enter IP To Server\n";
	
	std::cin >> ip;

	NetworkManager& nm = NetworkManager::GetInstance();

	std::string path;
	
	//path = "C:\\Users\\s170895\\Desktop\\Complex Game Systems Assessments\\Builds\\Other Computer";
	path = cCurrentPath;

	nm.Initialise(path);


	std::vector<HashFile> externalFileArr = FileManager::CreateAllHashFiles(path);
	FileManager::CreateNewFileStucFileInDir(externalFileArr, path);


	if (!nm.StartClientConnectionToServer(ip.c_str(), 60000))
	{
		std::cout << "Could Not Connect\n";
		system("pause");
		return 0;
	}



	nm.RunClient();

	std::string systemCmd = path + "\\Echo Command 1.4.exe";

	ShellExecute(NULL, "open", systemCmd.c_str(), NULL, NULL, SW_SHOWDEFAULT);

	return 0;


}