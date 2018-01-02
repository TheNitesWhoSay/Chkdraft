#ifndef SETTINGS_H
#define SETTINGS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"

bool GetPreSavePath(std::string &outPreSavePath); // Gets path holding assets to be written to the map file on save

bool GetLoggerPath(std::string &outLoggerPath); // Gets the path at which logs are stored

#endif