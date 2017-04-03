#include "TemporaryFile.h"
#include "FileIO.h"

TemporaryFile::TemporaryFile(const std::string &temporaryFilePath) : temporaryFilePath(temporaryFilePath)
{

}

const std::string &TemporaryFile::GetFilePath()
{
    return temporaryFilePath;
}

TemporaryFile::~TemporaryFile()
{
    if ( temporaryFilePath.length() > 0 )
        RemoveFile(temporaryFilePath.c_str());
}
