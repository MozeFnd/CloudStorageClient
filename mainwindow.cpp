#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>
#include <QPushButton>
#include <QFileDialog>


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
        comm_->buildConnection(serverIP, port.toLong());
        comm_->login(uname, pwd);

        // qDebug() << uname << " " << upwd;
    });

    connect(ui->SelectDirButton, &QPushButton::clicked,[&](){
        QString path = QFileDialog::getExistingDirectory(this, "选择要同步的目录");
        qDebug() << path << " ";
        core_logic_->trackDirecotory(path.toStdString());
    });

    connect(ui->displayTree, &QPushButton::clicked, [&](){
        path_tree_->display(ui->treeView);
    });

    // QWidget *centralWidget = new QWidget();
    // QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);


    // centralLayout->addWidget(ui->scrollArea);

    // this->setCentralWidget(centralWidget);
    // this->resize(400,300);

    // KVStore kvstore;
    // // kvstore.insert("123","456");
    // QString v = kvstore.read("123");
    // qDebug() << v;


}

MainWindow::~MainWindow()
{
    delete ui;
}
