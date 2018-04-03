#include "stdafx.h"
#include "Application.h"
#include "FileManager.h"


Application::Application()
{
	
}

void Application::Start()
{
	//client 
	localFileArr = FileManager::CreateAllHashFiles("C:\\Users\\s170895\\Desktop\\Client Updater (client)");
	FileManager::CreateNewFileStucFileInDir(localFileArr, "C:\\Users\\s170895\\Desktop\\Client Updater (client)");

	for (HashFile& file : localFileArr)
	{
		std::cout << file.getFileLocation().c_str() << file.gethashValue().c_str() << std::endl;
	}

	std::cout  << std::endl;

	//server
	externalFileArr = FileManager::CreateAllHashFiles("C:\\Users\\s170895\\Desktop\\Client Updater (Server)");
	FileManager::CreateNewFileStucFileInDir(externalFileArr, "C:\\Users\\s170895\\Desktop\\Client Updater (Server)");


	for (HashFile& file : externalFileArr)
	{
		std::cout << file.getFileLocation().c_str() << file.gethashValue().c_str() << std::endl;
	}

	std::cout << std::endl;


	missingFiles = FileManager::FindMissingFiles(localFileArr, externalFileArr);

	for (HashFile& file : missingFiles)
	{
		std::cout << file.getFileLocation().c_str() << file.gethashValue().c_str() << std::endl;
	}
	std::cout << std::endl;


	missingFiles = FileManager::FindFilesToDelete(localFileArr, externalFileArr);

	for (HashFile& file : missingFiles)
	{
		std::cout << file.getFileLocation().c_str() << file.gethashValue().c_str() << std::endl;
	}
	std::cout << std::endl;


	system("PAUSE");
}


Application::~Application()
{
}
