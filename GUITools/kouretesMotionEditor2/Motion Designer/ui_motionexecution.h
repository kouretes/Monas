/********************************************************************************
** Form generated from reading UI file 'motionexecution.ui'
**
** Created: Mon Mar 28 03:08:57 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOTIONEXECUTION_H
#define UI_MOTIONEXECUTION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MotionExecution
{
public:
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *progressLabel;
    QProgressBar *progressBar;
    QSpacerItem *verticalSpacer;
    QPushButton *stopExecutionButton;

    void setupUi(QDialog *MotionExecution)
    {
        if (MotionExecution->objectName().isEmpty())
            MotionExecution->setObjectName(QString::fromUtf8("MotionExecution"));
        MotionExecution->resize(362, 126);
        horizontalLayout_2 = new QHBoxLayout(MotionExecution);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        progressLabel = new QLabel(MotionExecution);
        progressLabel->setObjectName(QString::fromUtf8("progressLabel"));

        horizontalLayout->addWidget(progressLabel);

        progressBar = new QProgressBar(MotionExecution);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(0);

        horizontalLayout->addWidget(progressBar);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        stopExecutionButton = new QPushButton(MotionExecution);
        stopExecutionButton->setObjectName(QString::fromUtf8("stopExecutionButton"));

        verticalLayout->addWidget(stopExecutionButton);


        horizontalLayout_2->addLayout(verticalLayout);


        retranslateUi(MotionExecution);

        QMetaObject::connectSlotsByName(MotionExecution);
    } // setupUi

    void retranslateUi(QDialog *MotionExecution)
    {
        MotionExecution->setWindowTitle(QApplication::translate("MotionExecution", "Progress...", 0, QApplication::UnicodeUTF8));
        progressLabel->setText(QApplication::translate("MotionExecution", "0/0 Completed", 0, QApplication::UnicodeUTF8));
        stopExecutionButton->setText(QApplication::translate("MotionExecution", "Stop Execution", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MotionExecution: public Ui_MotionExecution {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOTIONEXECUTION_H
