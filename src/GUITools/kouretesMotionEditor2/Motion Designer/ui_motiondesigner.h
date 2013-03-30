/********************************************************************************
** Form generated from reading ui file 'motiondesigner.ui'
**
** Created: Thu Nov 26 11:58:08 2009
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MOTIONDESIGNER_H
#define UI_MOTIONDESIGNER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MotionDesignerClass
{
public:
    QWidget *centralWidget;
    QTableWidget *tableWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MotionDesignerClass)
    {
        if (MotionDesignerClass->objectName().isEmpty())
            MotionDesignerClass->setObjectName(QString::fromUtf8("MotionDesignerClass"));
        MotionDesignerClass->resize(653, 504);
        centralWidget = new QWidget(MotionDesignerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        tableWidget = new QTableWidget(centralWidget);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        if (tableWidget->rowCount() < 2)
            tableWidget->setRowCount(2);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(1, __qtablewidgetitem3);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(30, 130, 256, 192));
        MotionDesignerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MotionDesignerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 653, 24));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MotionDesignerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MotionDesignerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MotionDesignerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MotionDesignerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MotionDesignerClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuHelp->menuAction());

        retranslateUi(MotionDesignerClass);

        QMetaObject::connectSlotsByName(MotionDesignerClass);
    } // setupUi

    void retranslateUi(QMainWindow *MotionDesignerClass)
    {
        MotionDesignerClass->setWindowTitle(QApplication::translate("MotionDesignerClass", "MotionDesigner", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MotionDesignerClass", "hello", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MotionDesignerClass", "test", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->verticalHeaderItem(0);
        ___qtablewidgetitem2->setText(QApplication::translate("MotionDesignerClass", "test2", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->verticalHeaderItem(1);
        ___qtablewidgetitem3->setText(QApplication::translate("MotionDesignerClass", "hi there", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MotionDesignerClass", "File", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("MotionDesignerClass", "Edit", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MotionDesignerClass", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MotionDesignerClass: public Ui_MotionDesignerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOTIONDESIGNER_H
