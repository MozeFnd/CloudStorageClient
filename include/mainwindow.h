#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QScrollArea>
#include <memory>
#include "directoryarea.h"
#include "corelogic.h"
#include "kvstore.h"
#include "communicator.h"
#include "pathtree.h"

#define DEBUG_MODE 1


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<DirectoryArea> directory_area_;
    std::shared_ptr<KVStore> kvstore_;
    std::shared_ptr<Communicator> comm_;
    std::shared_ptr<PathTree> path_tree_;
    std::shared_ptr<CoreLogic> core_logic_;
};
#endif // MAINWINDOW_H
