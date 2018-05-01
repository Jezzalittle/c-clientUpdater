#include "stdafx.h"
#include "HashFile.h"
#include "md5Hash.h"


HashFile::HashFile(std::string a_fileLocation, std::string a_fileData)
{
	fileLocation = a_fileLocation;
	hashValue = MD5(a_fileLocation).hexdigest();
	hashValue += MD5(a_fileData).hexdigest();
}

HashFile::HashFile(std::string a_hash)
{
	hashValue = a_hash;
}


HashFile::~HashFile()
{
}
