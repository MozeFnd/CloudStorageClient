#ifndef JSONTOOL_H
#define JSONTOOL_H

#include <filesystem>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

class JsonTool
{
public:
    JsonTool();
    std::string generateJsonForPath(std::string path);
};

#endif // JSONTOOL_H
