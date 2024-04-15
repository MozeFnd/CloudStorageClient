/********************************************************************************
** Form generated from reading UI file 'getnamedialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GETNAMEDIALOG_H
#define UI_GETNAMEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_GetNameDialog
{
public:
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpacerItem *horizontalSpacer_2;
    QTextEdit *nameContext;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *confirmButton;
    QSpacerItem *horizontalSpacer_4;

    void setupUi(QDialog *GetNameDialog)
    {
        if (GetNameDialog->objectName().isEmpty())
            GetNameDialog->setObjectName("GetNameDialog");
        GetNameDialog->resize(372, 137);
        groupBox = new QGroupBox(GetNameDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(20, 10, 331, 111));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName("gridLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label = new QLabel(groupBox);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        nameContext = new QTextEdit(groupBox);
        nameContext->setObjectName("nameContext");

        verticalLayout->addWidget(nameContext);


        gridLayout->addLayout(verticalLayout, 1, 1, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        confirmButton = new QPushButton(groupBox);
        confirmButton->setObjectName("confirmButton");

        horizontalLayout_3->addWidget(confirmButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        gridLayout->addLayout(horizontalLayout_3, 2, 1, 1, 1);


        retranslateUi(GetNameDialog);

        QMetaObject::connectSlotsByName(GetNameDialog);
    } // setupUi

    void retranslateUi(QDialog *GetNameDialog)
    {
        GetNameDialog->setWindowTitle(QCoreApplication::translate("GetNameDialog", "\351\246\226\346\254\241\347\231\273\345\275\225", nullptr));
        groupBox->setTitle(QString());
        label->setText(QCoreApplication::translate("GetNameDialog", "\344\270\272\344\275\240\347\232\204\350\256\276\345\244\207\345\221\275\345\220\215", nullptr));
        confirmButton->setText(QCoreApplication::translate("GetNameDialog", "\347\241\256\345\256\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GetNameDialog: public Ui_GetNameDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GETNAMEDIALOG_H
