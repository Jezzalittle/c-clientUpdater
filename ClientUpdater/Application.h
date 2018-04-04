#pragma once

#include <iostream>
#include <vector>
#include "NetworkManager.h"

class Application
{
private:

	std::string ip;
	NetworkManager* nm;
	std::vector<HashFile> localFileArr;
	std::vector<HashFile> externalFileArr;
	std::vector<HashFile> missingFiles;

public:
	Application();

	void Start();


	~Application();
};

