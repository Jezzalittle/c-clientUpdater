// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NetworkManager.h"
#include <vector>
#include <iostream>
#include <string>
#include "FileManager.h"

#include <direct.h>
#define GetCurrentDir _getcwd

int main()
{

	std::string path;


	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		return errno;
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */


	//path = cCurrentPath;

	path = "C:\\Users\\s170895\\Desktop\\Complex Game Systems Assessments\\Builds\\Server";

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

