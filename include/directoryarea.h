#ifndef DIRECTORYAREA_H
#define DIRECTORYAREA_H

#include <QVBoxLayout>
#include <QWidget>
#include <QScrollArea>
#include <string>
#include <unordered_map>
#include "directoryitem.h"

class DirectoryArea
{
public:
    DirectoryArea();
    ~DirectoryArea();
    void setScrollArea(QScrollArea* scroll_area);
    void addDirectoryItem(std::string name, uint32_t unique_id);
    void removeDirectoryItem(uint32_t unique_id);
private:
    std::unordered_map<uint32_t, std::shared_ptr<DirecotryItem>> dirs_;
    uint32_t height_ = 20;
    QWidget* scroll_content_;
    QVBoxLayout* layout_;
    QScrollArea* scroll_area_ = nullptr;
};

#endif // DIRECTORYAREA_H
