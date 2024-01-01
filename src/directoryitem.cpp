#include "directoryitem.h"

OperationMenu::OperationMenu(std::shared_ptr<DirecotryItem> owner):
    QMenu()
    ,dir_item_(owner)
{
    // 同步文件夹
    QAction* sync_action = new QAction("同步该资料库");
    // 取消同步
    QAction* cancel_action = new QAction("解除同步");
    // 在云端删除文件夹
    QAction* delete_action = new QAction("删除该资料库");

    this->addAction(sync_action);
    this->addAction(cancel_action);
    this->addAction(delete_action);

    connect(sync_action, &QAction::triggered, this, [&](){
        qDebug() << "sync_action clicked";
    });
}

DirecotryItem::DirecotryItem(uint32_t unique_id, QWidget* parent, std::string name)
    :QLabel(parent)
{
    name_ = name;
    unique_id_ = unique_id;
    status_ = STATUS::UNSYNCRONIZED;
    initShape();
}

void DirecotryItem::initShape(){
    this->setFixedSize(width_, height_);
    this->setText(QString::fromStdString(name_));
    this->setMargin(5);
}

void DirecotryItem::setName(std::string name) {
    name_ = name;
}

uint32_t DirecotryItem::width_ = 200;
uint32_t DirecotryItem::height_ = 50;
