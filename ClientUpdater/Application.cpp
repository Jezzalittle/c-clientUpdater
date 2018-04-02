#include "stdafx.h"
#include "Application.h"
#include "FileManager.h"


Application::Application()
{
	
}

void Application::Start()
{
	//client 
	localFileArr = FileManager::CreateAllHashFiles("C:\\Users\\Owner\\Desktop\\RakNet-master");
	FileManager::CreateNewFileStucFileInDir(localFileArr, "C:\\Users\\Owner\\Desktop\\RakNet-master");

	//server
	externalFileArr = FileManager::CreateAllHashFiles("C:\\Users\\Owner\\Desktop\\ClientTest_Data");
	FileManager::CreateNewFileStucFileInDir(externalFileArr, "C:\\Users\\Owner\\Desktop\\ClientTest_Data");

	missingFiles = FileManager::FindMissingFiles(localFileArr, externalFileArr);

	for (HashFile& file : missingFiles)
	{
		std::cout << file.gethashValue().c_str() << std::endl;
	}

	system("PAUSE");
}


Application::~Application()
{
}
