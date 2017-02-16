#include "xpy.h"
#include <string>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace std;


char* native_get_current_path()
{
	fs::path sCurrPath = boost::filesystem::current_path();
	size_t iBufferSize = sCurrPath.string().length() + 2;
	char *pBuffer = new char[iBufferSize];

	if (nullptr != pBuffer)
	{
		 strcpy(pBuffer, sCurrPath.string().c_str());
	}
	return pBuffer;
}

void  native_release_memory(void *pBuffer)
{
	if (nullptr != pBuffer)
	{
		delete pBuffer;
		pBuffer = nullptr;
	}
}

