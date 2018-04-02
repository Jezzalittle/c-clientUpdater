#pragma once

#include <iostream>
#include <vector>
#include "HashFile.h"

class Application
{
private:

	std::string ip;
	std::vector<HashFile> localFileArr;
	std::vector<HashFile> externalFileArr;
	std::vector<HashFile> missingFiles;

public:
	Application();

	void Start();


	~Application();
};

