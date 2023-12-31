#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>
#include <QPushButton>
#include <QFileDialog>
#include "jsontool.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , kvstore_(std::make_shared<KVStore>())
    , comm_(std::make_shared<Communicator>(kvstore_))
    , path_tree_(std::make_shared<PathTree>())
    , core_logic_(std::make_shared<CoreLogic>(kvstore_, comm_, path_tree_))
{
    ui->setupUi(this);

    directory_area_ = std::make_shared<DirectoryArea>();
    directory_area_->setScrollArea(ui->scrollArea);

    core_logic_->init(directory_area_);

    connect(ui->ConnectButton, &QPushButton::clicked, [&](){
        QString uname = ui->textEditUserName->toPlainText();
        QString pwd = ui->textEditPassword->toPlainText();
        QString serverIP = ui->textEditIP->toPlainText();
        QString port = ui->textEditPort->toPlainText();
        qDebug() << serverIP << " " << port;

        comm_->login(uname, pwd);
        // auto arr = comm_->acquireIDinBatch();
        return;
        // qDebug() << uname << " " << upwd;
    });

    connect(ui->SelectDirButton, &QPushButton::clicked,[&](){
        QString path = QFileDialog::getExistingDirectory(this, "选择要同步的目录");
        qDebug() << path << "\n";
        // core_logic_->trackDirecotory(path.toStdString());

        auto js = Json::fromPath(path.toStdString());
        qDebug() << QString::fromStdString(js->toString()) << "\n";
        return;
    });

    connect(ui->displayTree, &QPushButton::clicked, [&](){
        path_tree_->display(ui->treeView);
    });

    std::string text = "{\"dirname\":\"A\",\"type\":\"direcory\",\"contents\":[{\"dirname\":\"B\",\"type\":"
                       "\"direcory\",\"contents\":[{\"dirname\":\"D\",\"type\":\"direcory\"},{\"dirname\":\"E\","
                       "\"type\":\"direcory\"}]},{\"dirname\":\"C\",\"type\":\"direcory\"},{\"size\":\"31\","
                       "\"filename\":\"textfile.txt\",\"type\":\"regular_file\"}]}";
    Json::formattedOutput(text);
    // auto js = Json::fromJsonString(text);
    // std::string back = js->toString();
    // qDebug() << QString::fromStdString(back);
    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}
