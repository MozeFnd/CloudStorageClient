#include "directoryarea.h"

DirectoryArea::DirectoryArea() {
    scroll_content_ = new QWidget();
    layout_ = new QVBoxLayout(scroll_content_);
    scroll_content_->setStyleSheet("QFrame { border: 1px solid black; padding: 10px; }");
}

DirectoryArea::~DirectoryArea(){
    delete scroll_content_;
    delete layout_;
}

void DirectoryArea::setScrollArea(QScrollArea* scroll_area) {
    scroll_area_ = scroll_area;
    scroll_area_->setWidgetResizable(true);
    scroll_area_->setWidget(scroll_content_);
    DirecotryItem::width_ = scroll_area_->width() - 40;
    DirecotryItem::height_ = 50;
}

void DirectoryArea::addDirectoryItem(std::string name, uint32_t unique_id){
    std::shared_ptr<DirecotryItem> item = std::make_shared<DirecotryItem>(unique_id, nullptr, name);
    auto menu = std::make_shared<OperationMenu>(item);
    item->setMenu(menu);
    dirs_[unique_id] = item;
    layout_->addWidget(item.get());
    height_ = (dirs_.size() <= 8 ? dirs_.size() : 8) * 65 + 20;
    scroll_area_->resize(scroll_area_->width(), height_);
}

void DirectoryArea::removeDirectoryItem(uint32_t unique_id) {
    layout_->removeWidget(dirs_[unique_id].get());
    dirs_.erase(unique_id);
    height_ = (dirs_.size() <= 8 ? dirs_.size() : 8) * 65 + 20;
    scroll_area_->resize(scroll_area_->width(), height_);
}
