#include "stdafx.h"
#include "FileManager.h"
#include "HashFile.h"
#include <experimental/filesystem> 
#include <assert.h>
#include <iostream>
#include <fstream>  
#include <thread>

namespace fs = std::experimental::filesystem::v1;

std::thread FileManager::m_readFromFileThread;


std::vector<HashFile> FileManager::CreateAllHashFiles(std::string a_dir)
{
	std::vector<HashFile> hashFiles;
	int amoutOfFiles = 0;
	int currentAmount = 0;




	for (auto& fileDir : fs::recursive_directory_iterator(a_dir))
	{
		amoutOfFiles++;
	}

	std::string fileData;
	//iterators over ever folder and every subfolder
	for (auto& fileDir : fs::recursive_directory_iterator(a_dir))
	{
		std::ifstream inFile(fileDir);

		//std::string fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
		inFile >> fileData;
		inFile.close();

		std::string name = fileDir.path().string();

		if (name.substr(a_dir.size(), a_dir.size() - name.size()) != "\FileStructure.txt")
		{
			hashFiles.push_back(HashFile(name.substr(a_dir.size(), a_dir.size() - name.size()), fileData));
		}

		currentAmount++;

		fileData.clear();
	}


	//assert(hashFiles.size() > 0);
	return hashFiles;

}

void FileManager::CreateNewFileStucFileInDir(std::vector<HashFile> hashFileArr, std::string a_dir)
{
	std::ofstream outfile(a_dir);

	for (auto& hashFile : hashFileArr)
	{
		outfile << hashFile.gethashValue() << std::endl;
	}

	outfile.close();

}


void FileManager::ReadFromfileStucFileByDir(std::string a_dir)
{
	std::vector<HashFile> returnArr;
	std::ifstream inFile;
	std::string line;

	inFile.open(a_dir);
	if (inFile.is_open())
	{
		while (std::getline(inFile, line))
		{
			returnArr.push_back(HashFile(line));
		}
	}
}

bool FileManager::ReadFromfileStucFileByDir(std::string a_dir, CallbackFnc callbackFnc)
{
	if (m_readFromFileThread.joinable())
	{
		return false;
	}

	m_readFromFileThread = std::thread([=]()
	{

		std::vector<HashFile> returnArr;
		std::ifstream inFile;
		std::string line;

		inFile.open(a_dir);
		if (inFile.is_open())
		{
			while (std::getline(inFile, line))
			{
				returnArr.push_back(HashFile(line));
			}
		}

		callbackFnc(std::move(returnArr));
	});
		return true;

}

std::vector<HashFile> FileManager::FindMissingFiles(std::vector<HashFile> clientArr, std::vector<HashFile> serverArr)
{
	std::vector<HashFile> returnArr;
	bool found = false;

	for (HashFile& sfile : serverArr)
	{
		found = false;
		for (HashFile& cfile : clientArr)
		{
			if (sfile.gethashValue() == cfile.gethashValue())
			{
				found = true;
			}
		}
		if (found == false)
		{
			returnArr.push_back(sfile);
		}
	}

	return returnArr;
}

std::vector<HashFile> FileManager::FindFilesToDelete(std::vector<HashFile> clientArr, std::vector<HashFile> serverArr)
{
	std::vector<HashFile> returnArr = FindMissingFiles(serverArr, clientArr);
	return returnArr;
}


