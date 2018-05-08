#pragma once
#include <iostream>

class HashFile
{
private:

	std::string fileLocation;
	std::string hashValue;

public:
	
	HashFile() = delete;

	HashFile(std::string& a_fileLocation, const char* a_fileData);
	HashFile(std::string a_hash);

	std::string getFileLocation()
	{
		return fileLocation;
	}

	void SetFileLocation(std::string value)
	{
		fileLocation = value;
	}

	std::string gethashValue()
	{
		return hashValue;
	}

	~HashFile();
};

