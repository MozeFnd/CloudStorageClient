#ifndef DIRECTORYITEM_H
#define DIRECTORYITEM_H

#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QCursor>
#include <QMenu>
#include <memory>

class DirecotryItem;

class OperationMenu : public QMenu
{
public:
    OperationMenu() = delete;
    OperationMenu(std::shared_ptr<DirecotryItem> owner);
private:
    std::weak_ptr<DirecotryItem> dir_item_;
};

class DirecotryItem : public QLabel
{
    Q_OBJECT
public:
    enum STATUS {
        UNSYNCRONIZED = 0,
        SYNCRONIZING = 1,
        SYNCRONIZED = 2,
    };
    DirecotryItem() = delete;
    explicit DirecotryItem(uint32_t unique_id, QWidget* parent = nullptr, std::string name = "empty");
    void initShape();
    void setName(std::string name);
    void setMenu(std::shared_ptr<OperationMenu> menu) {
        menu_ = menu;
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QWidget::customContextMenuRequested, this, [&](){
            qDebug() << "DirecotryItem clicked";
            menu_->exec(QCursor::pos());
        });
    }

public:
    static uint32_t width_;
    static uint32_t height_;
private:
    std::shared_ptr<OperationMenu> menu_;
    std::string name_;
    uint32_t unique_id_;
    uint8_t status_;
};

#endif // DIRECTORYITEM_H
