#ifndef TEMPORARYFILE_H
#define TEMPORARYFILE_H
#include <string>

// Used to ensure that a file is deleted when an instance of this class goes out of scope
class TemporaryFile
{
public:
    TemporaryFile(const std::string &temporaryFilePath);
    ~TemporaryFile(); // Ensures the temporary file gets cleaned up
    const std::string &GetFilePath();

private:
    std::string temporaryFilePath;

    TemporaryFile() {} // Disallow ctor
};

#endif