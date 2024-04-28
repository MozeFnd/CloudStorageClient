#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>
#include <QPushButton>
#include <QFileDialog>
#include "jsontool.h"
#include "node.pb.h"

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

    /*
    为方便debug，在初始时给数据库增加一些内容
    */
#ifdef DEBUG_MODE
    kvstore_->store("used_id", "1");
    kvstore_->set_id_path("1", "C:/Users/XUN/Desktop/data/A");
    // kvstore_->set_id_path("2", "C:/Users/XUN/Desktop/qt_projects/data/中文");
#endif

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
        if (path.isEmpty()) {
            qDebug() << path << " is empty.";
            return;
        }
        qDebug() << path;
        auto root = Node::fromPath(path.toStdWString(), str2wstr(""), true);

        uint32_t id = comm_->acquireID();
        root->set_id(id);

        if (root == nullptr) {
            qDebug() << "Fail to build Node object.";
            return;
        }
        std::string print;
        Node::formatted(root, print, 0);
        qDebug() << QString::fromStdString(print);

        auto serialized_root = root->serialize();

        qDebug() << QString::fromStdString(serialized_root);
        comm_->remoteAddNewDirectory(id, serialized_root);

        core_logic_->beginToTrackDirecotory(path.toStdString(), id);
        return;
    });

    connect(ui->displayTree, &QPushButton::clicked, [&](){
        path_tree_->display(ui->treeView);
    });

    const std::wstring filepath = L"C:/Users/XUN/Desktop/data/中文\\文本.txt";
    // std::string filepath = "C:/Users/XUN/Desktop/data/A\\a.txt";
    std::filesystem::path w_path(filepath);
    std::ifstream file(w_path, std::ios::binary);
    const size_t BUFFER_SIZE = 1000;
    char buffer[BUFFER_SIZE];
    if (!file.is_open()) {
        exit(-1);
    } else {
        file.read(buffer, BUFFER_SIZE);
    }
    std::string text = "{\"dirname\":\"A\",\"type\":\"direcory\",\"contents\":[{\"dirname\":\"B\",\"type\":"
                       "\"direcory\",\"contents\":[{\"dirname\":\"D\",\"type\":\"direcory\"},{\"dirname\":\"E\","
                       "\"type\":\"direcory\"}]},{\"dirname\":\"C\",\"type\":\"direcory\"},{\"size\":\"31\","
                       "\"filename\":\"textfile.txt\",\"type\":\"regular_file\"}]}";
    // Json::formattedOutput(text);
    // auto js = Json::fromJsonString(text);
    // std::string back = js->toString();
    // qDebug() << QString::fromStdString(back);
    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}
