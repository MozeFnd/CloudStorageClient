#ifndef GETNAMEDIALOG_H
#define GETNAMEDIALOG_H

#include <QDialog>

namespace Ui {
class GetNameDialog;
}

class GetNameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GetNameDialog(QWidget *parent = nullptr);
    ~GetNameDialog();
    QString getName();
private:
    Ui::GetNameDialog *ui;
    QString name_;
};

#endif // GETNAMEDIALOG_H
