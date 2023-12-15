#include "jsontool.h"

// Function to create JSON representation of directory structure
void createDirectoryJSON(const std::string& dirPath, Document& doc, Value& parent) {
    Value files(kArrayType);
    Value directories(kObjectType);

    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            Value file;
            file.SetString(entry.path().filename().string().c_str(), static_cast<SizeType>(entry.path().filename().string().size()), doc.GetAllocator());
            files.PushBack(file, doc.GetAllocator());
        } else if (entry.is_directory()) {
            Value directory(kObjectType);
            createDirectoryJSON(entry.path().string(), doc, directory);
            directories.AddMember(Value().SetString(entry.path().filename().string().c_str(), static_cast<SizeType>(entry.path().filename().string().size()), doc.GetAllocator()), directory, doc.GetAllocator());
        }
    }

    parent.AddMember("files", files, doc.GetAllocator());
    parent.AddMember("directories", directories, doc.GetAllocator());
}

JsonTool::JsonTool() {}

std::string JsonTool::generateJsonForPath(std::string path) {
    Document doc;
    doc.SetObject();

    createDirectoryJSON(path, doc, doc);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}
