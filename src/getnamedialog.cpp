#include "getnamedialog.h"
#include "ui_getnamedialog.h"

GetNameDialog::GetNameDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GetNameDialog)
{
    ui->setupUi(this);
    ui->confirmButton->setEnabled(false);
    this->setWindowFlag(Qt::WindowCloseButtonHint, false);
    connect(ui->nameContext, &QTextEdit::textChanged, [&](){
        if(ui->nameContext->toPlainText().isEmpty()) {
            ui->confirmButton->setEnabled(false);
        } else {
            ui->confirmButton->setEnabled(true);
        }
    });

    connect(ui->confirmButton, &QPushButton::clicked, [&](){
        name_ = ui->nameContext->toPlainText();
        this->close();
    });
    this->exec();
}

QString GetNameDialog::getName() {
    return name_;
}

GetNameDialog::~GetNameDialog()
{
    delete ui;
}
