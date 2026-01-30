/********************************************************************************
** Form generated from reading UI file 'server10.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVER10_H
#define UI_SERVER10_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_server10Class
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *server10Class)
    {
        if (server10Class->objectName().isEmpty())
            server10Class->setObjectName(QString::fromUtf8("server10Class"));
        server10Class->resize(600, 400);
        menuBar = new QMenuBar(server10Class);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        server10Class->setMenuBar(menuBar);
        mainToolBar = new QToolBar(server10Class);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        server10Class->addToolBar(mainToolBar);
        centralWidget = new QWidget(server10Class);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        server10Class->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(server10Class);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        server10Class->setStatusBar(statusBar);

        retranslateUi(server10Class);

        QMetaObject::connectSlotsByName(server10Class);
    } // setupUi

    void retranslateUi(QMainWindow *server10Class)
    {
        server10Class->setWindowTitle(QCoreApplication::translate("server10Class", "server10", nullptr));
    } // retranslateUi

};

namespace Ui {
    class server10Class: public Ui_server10Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVER10_H
