#include "mainwindow.h"

#include <QApplication>
#include <QTcpSocket>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // QTcpSocket socket;
    // socket.connectToHost("47.106.240.195", 6900); // Adjust the IP and port as needed

    // if (!socket.waitForConnected()) {
    //     qDebug() << "Error: " << socket.errorString();
    //     return -1;
    // }

    // Open the file to be sent
    // QFile file("file_to_send.txt");
    // if (!file.open(QIODevice::ReadOnly)) {
    //     qDebug() << "Error opening file: " << file.errorString();
    //     return -1;
    // }

    // // Send file data over the TCP connection
    // while (!file.atEnd()) {
    //     QByteArray buffer = file.read(1024); // Read file in chunks
    //     socket.write(buffer);
    // }

    // file.close();

    // QByteArray buffer("hello, this is client");
    // socket.write(buffer);

    // socket.disconnectFromHost();
    // socket.waitForDisconnected();

    // qDebug() << "Msg sent successfully";

    return a.exec();
}
