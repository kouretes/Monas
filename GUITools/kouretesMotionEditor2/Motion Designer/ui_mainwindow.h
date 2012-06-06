/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Mar 28 03:08:57 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionExport;
    QAction *actionQuit;
    QAction *actionDocumentation;
    QAction *actionAbout;
    QAction *actionJoints;
    QAction *actionGraph;
    QAction *actionNao_RC_Edition;
    QAction *actionNao_Academics;
    QAction *actionOpen_Robot;
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QMenu *menuNetwork;
    QMenu *menuView;
    QStatusBar *statusbar;
    QDockWidget *dockWidget_9;
    QWidget *dockWidgetContents_9;
    QHBoxLayout *horizontalLayout;
    QTableWidget *poseEditor;
    QDockWidget *settingsDockWidget;
    QWidget *dockWidgetContents;
    QHBoxLayout *horizontalLayout_7;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout;
    QLabel *iPLabel;
    QLineEdit *iPLineEdit;
    QLabel *portLabel;
    QLineEdit *portLineEdit;
    QPushButton *connectButton;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;
    QPushButton *moveUpPoseButton;
    QPushButton *swapPoseButton;
    QPushButton *moveDownPoseButton;
    QPushButton *removePoseButton;
    QFrame *line;
    QPushButton *storePoseButton;
    QPushButton *insertPoseButton;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label;
    QDoubleSpinBox *transitionTime;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QDoubleSpinBox *scaleFactor;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout;
    QPushButton *gotoPoseButton;
    QPushButton *stepPoseButton;
    QPushButton *playMotionButton;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *stiffOn;
    QPushButton *stiffOff;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(831, 499);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(600, 400));
        MainWindow->setLayoutDirection(Qt::LeftToRight);
        MainWindow->setAutoFillBackground(false);
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        actionNew->setShortcutContext(Qt::WindowShortcut);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionExport = new QAction(MainWindow);
        actionExport->setObjectName(QString::fromUtf8("actionExport"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionDocumentation = new QAction(MainWindow);
        actionDocumentation->setObjectName(QString::fromUtf8("actionDocumentation"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionJoints = new QAction(MainWindow);
        actionJoints->setObjectName(QString::fromUtf8("actionJoints"));
        actionJoints->setCheckable(true);
        actionJoints->setChecked(true);
        actionGraph = new QAction(MainWindow);
        actionGraph->setObjectName(QString::fromUtf8("actionGraph"));
        actionGraph->setCheckable(true);
        actionGraph->setChecked(true);
        actionNao_RC_Edition = new QAction(MainWindow);
        actionNao_RC_Edition->setObjectName(QString::fromUtf8("actionNao_RC_Edition"));
        actionNao_RC_Edition->setCheckable(true);
        actionNao_Academics = new QAction(MainWindow);
        actionNao_Academics->setObjectName(QString::fromUtf8("actionNao_Academics"));
        actionNao_Academics->setCheckable(true);
        actionNao_Academics->setChecked(true);
        actionOpen_Robot = new QAction(MainWindow);
        actionOpen_Robot->setObjectName(QString::fromUtf8("actionOpen_Robot"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 831, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuNetwork = new QMenu(menubar);
        menuNetwork->setObjectName(QString::fromUtf8("menuNetwork"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        dockWidget_9 = new QDockWidget(MainWindow);
        dockWidget_9->setObjectName(QString::fromUtf8("dockWidget_9"));
        dockWidget_9->setStyleSheet(QString::fromUtf8("QDockWidget {\n"
"                         background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 2.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #9EDEE6);\n"
"                         border: 2px solid gray;\n"
"                         border-radius: 5px;\n"
"                         margin-top: 1ex; /* leave space at the top for the title */}"));
        dockWidget_9->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        dockWidgetContents_9 = new QWidget();
        dockWidgetContents_9->setObjectName(QString::fromUtf8("dockWidgetContents_9"));
        horizontalLayout = new QHBoxLayout(dockWidgetContents_9);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        poseEditor = new QTableWidget(dockWidgetContents_9);
        poseEditor->setObjectName(QString::fromUtf8("poseEditor"));
        poseEditor->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"                         background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 4.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #9EDEE6);\n"
"                         border: 2px solid gray;\n"
"                         border-radius: 5px;\n"
"                         margin-top: 1ex; /* leave space at the top for the title */}\n"
"\n"
" QTableView {\n"
"     selection-background-color: qlineargradient(x1: 0, y1: 0, x2: 0.9, y2: 0.9,\n"
"                                 stop: 0 #53A0F6, stop: 1 black);\n"
" }"));
        poseEditor->setFrameShadow(QFrame::Sunken);
        poseEditor->setLineWidth(1);
        poseEditor->setDragDropOverwriteMode(false);
        poseEditor->setDragDropMode(QAbstractItemView::DropOnly);
        poseEditor->setAlternatingRowColors(true);
        poseEditor->setSelectionMode(QAbstractItemView::ExtendedSelection);
        poseEditor->setTextElideMode(Qt::ElideMiddle);
        poseEditor->setShowGrid(true);
        poseEditor->horizontalHeader()->setCascadingSectionResizes(true);
        poseEditor->verticalHeader()->setCascadingSectionResizes(true);

        horizontalLayout->addWidget(poseEditor);

        dockWidget_9->setWidget(dockWidgetContents_9);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(8), dockWidget_9);
        settingsDockWidget = new QDockWidget(MainWindow);
        settingsDockWidget->setObjectName(QString::fromUtf8("settingsDockWidget"));
        settingsDockWidget->setLayoutDirection(Qt::LeftToRight);
        settingsDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        settingsDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::TopDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        horizontalLayout_7 = new QHBoxLayout(dockWidgetContents);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        sizePolicy1.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy1);
        groupBox_3->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"                         background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 2.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #FF0000);\n"
"                         border: 4px solid gray;\n"
"                         border-radius: 5px;\n"
"                         margin-top: 1ex;}"));
        horizontalLayout_5 = new QHBoxLayout(groupBox_3);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->setHorizontalSpacing(6);
        formLayout->setContentsMargins(-1, 6, -1, -1);
        iPLabel = new QLabel(groupBox_3);
        iPLabel->setObjectName(QString::fromUtf8("iPLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, iPLabel);

        iPLineEdit = new QLineEdit(groupBox_3);
        iPLineEdit->setObjectName(QString::fromUtf8("iPLineEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, iPLineEdit);

        portLabel = new QLabel(groupBox_3);
        portLabel->setObjectName(QString::fromUtf8("portLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, portLabel);

        portLineEdit = new QLineEdit(groupBox_3);
        portLineEdit->setObjectName(QString::fromUtf8("portLineEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, portLineEdit);

        connectButton = new QPushButton(groupBox_3);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));
        sizePolicy1.setHeightForWidth(connectButton->sizePolicy().hasHeightForWidth());
        connectButton->setSizePolicy(sizePolicy1);
        connectButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"                         background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 3,\n"
"                         stop: 0 #E0E0E0, stop: 1 #FF0000);\n"
"                         margin-top: 1ex; /* leave space at the top for the title */}\n"
"\n"
"\n"
" QPushButton::hover {\n"
"     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                       stop: 0 #dadbde, stop: 1 #f6f7fa);\n"
" }\n"
"\n"
""));

        formLayout->setWidget(2, QFormLayout::SpanningRole, connectButton);


        verticalLayout_2->addLayout(formLayout);


        horizontalLayout_4->addLayout(verticalLayout_2);


        horizontalLayout_5->addLayout(horizontalLayout_4);


        horizontalLayout_2->addWidget(groupBox_3);

        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        sizePolicy1.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy1);
        groupBox->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"                         background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 2.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #9EDEE6);\n"
"                         border: 4px solid gray;\n"
"                         border-radius: 5px;\n"
"                         margin-top: 1ex; /* leave space at the top for the title */}"));
        verticalLayout_4 = new QVBoxLayout(groupBox);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(-1, 6, -1, -1);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        moveUpPoseButton = new QPushButton(groupBox);
        moveUpPoseButton->setObjectName(QString::fromUtf8("moveUpPoseButton"));
        moveUpPoseButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"color: white;\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #88d, stop: 0.1 #99e, stop: 0.49 #77c, stop: 0.5 #66b, stop: 1 #77c);\n"
"border-width: 1px;\n"
"border-color: #339;\n"
"border-style: solid;\n"
"border-radius: 7;\n"
"padding: 3px;\n"
"padding-left: 5px;\n"
"padding-right: 5px;\n"
"\n"
"}"));

        gridLayout->addWidget(moveUpPoseButton, 0, 0, 1, 1);

        swapPoseButton = new QPushButton(groupBox);
        swapPoseButton->setObjectName(QString::fromUtf8("swapPoseButton"));
        swapPoseButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"color: white;\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #88d, stop: 0.1 #99e, stop: 0.49 #77c, stop: 0.5 #66b, stop: 1 #77c);\n"
"border-width: 1px;\n"
"border-color: #339;\n"
"border-style: solid;\n"
"border-radius: 7;\n"
"padding: 3px;\n"
"padding-left: 5px;\n"
"padding-right: 5px;\n"
"}"));

        gridLayout->addWidget(swapPoseButton, 0, 1, 1, 1);

        moveDownPoseButton = new QPushButton(groupBox);
        moveDownPoseButton->setObjectName(QString::fromUtf8("moveDownPoseButton"));
        moveDownPoseButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"color: white;\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #88d, stop: 0.1 #99e, stop: 0.49 #77c, stop: 0.5 #66b, stop: 1 #77c);\n"
"border-width: 1px;\n"
"border-color: #339;\n"
"border-style: solid;\n"
"border-radius: 7;\n"
"padding: 3px;\n"
"padding-left: 5px;\n"
"padding-right: 5px;\n"
"\n"
"}"));

        gridLayout->addWidget(moveDownPoseButton, 1, 0, 1, 1);

        removePoseButton = new QPushButton(groupBox);
        removePoseButton->setObjectName(QString::fromUtf8("removePoseButton"));
        removePoseButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"color: white;\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #88d, stop: 0.1 #99e, stop: 0.49 #77c, stop: 0.5 #66b, stop: 1 #77c);\n"
"border-width: 1px;\n"
"border-color: #339;\n"
"border-style: solid;\n"
"border-radius: 7;\n"
"padding: 3px;\n"
"padding-left: 5px;\n"
"padding-right: 5px;\n"
"}"));

        gridLayout->addWidget(removePoseButton, 1, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout);

        line = new QFrame(groupBox);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_3->addWidget(line);

        storePoseButton = new QPushButton(groupBox);
        storePoseButton->setObjectName(QString::fromUtf8("storePoseButton"));
        storePoseButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"color: white;\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #88d, stop: 0.1 #99e, stop: 0.49 #77c, stop: 0.5 #66b, stop: 1 #77c);\n"
"border-width: 1px;\n"
"border-color: #339;\n"
"border-style: solid;\n"
"border-radius: 7;\n"
"padding: 3px;\n"
"padding-left: 5px;\n"
"padding-right: 5px;\n"
"}"));

        verticalLayout_3->addWidget(storePoseButton);

        insertPoseButton = new QPushButton(groupBox);
        insertPoseButton->setObjectName(QString::fromUtf8("insertPoseButton"));
        insertPoseButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"color: white;\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #88d, stop: 0.1 #99e, stop: 0.49 #77c, stop: 0.5 #66b, stop: 1 #77c);\n"
"border-width: 1px;\n"
"border-color: #339;\n"
"border-style: solid;\n"
"border-radius: 7;\n"
"padding: 3px;\n"
"padding-left: 5px;\n"
"padding-right: 5px;\n"
"}"));

        verticalLayout_3->addWidget(insertPoseButton);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_6->addWidget(label);

        transitionTime = new QDoubleSpinBox(groupBox);
        transitionTime->setObjectName(QString::fromUtf8("transitionTime"));
        transitionTime->setStyleSheet(QString::fromUtf8("QDoubleSpinBox {\n"
"color: blue;\n"
"}"));
        transitionTime->setMinimum(0.1);
        transitionTime->setSingleStep(0.1);
        transitionTime->setValue(1);

        horizontalLayout_6->addWidget(transitionTime);


        verticalLayout_3->addLayout(horizontalLayout_6);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_3->addWidget(label_2);

        scaleFactor = new QDoubleSpinBox(groupBox);
        scaleFactor->setObjectName(QString::fromUtf8("scaleFactor"));
        scaleFactor->setMinimum(0.01);
        scaleFactor->setMaximum(10);
        scaleFactor->setSingleStep(0.01);
        scaleFactor->setValue(1);

        horizontalLayout_3->addWidget(scaleFactor);


        verticalLayout_3->addLayout(horizontalLayout_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        verticalLayout_4->addLayout(verticalLayout_3);


        horizontalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(dockWidgetContents);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        groupBox_2->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"                         background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 2.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #81F27F);\n"
"                         border: 4px solid gray;\n"
"                         border-radius: 5px;\n"
"                         margin-top: 1ex; /* leave space at the top for the title */}"));
        verticalLayout_5 = new QVBoxLayout(groupBox_2);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gotoPoseButton = new QPushButton(groupBox_2);
        gotoPoseButton->setObjectName(QString::fromUtf8("gotoPoseButton"));
        gotoPoseButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 6.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #f90707);\n"
"     border-width: 2px;\n"
"     border-radius: 12px;\n"
"padding: 3px;\n"
"}\n"
"\n"
"QPushButton::hover{\n"
"\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #f90707);\n"
"border-width: 1px;\n"
"border-style: no;\n"
"padding: 3px;\n"
"\n"
"}\n"
"\n"
"\n"
"QPushButton:pressed {\n"
"    /* We'll just invert the gradient by changing the colors around. */\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.2,\n"
"                                      stop: 0 #E0E0E0, stop: 1 #f90707);\n"
"}\n"
""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/goto.png"), QSize(), QIcon::Normal, QIcon::Off);
        gotoPoseButton->setIcon(icon);
        gotoPoseButton->setIconSize(QSize(32, 32));

        verticalLayout->addWidget(gotoPoseButton);

        stepPoseButton = new QPushButton(groupBox_2);
        stepPoseButton->setObjectName(QString::fromUtf8("stepPoseButton"));
        stepPoseButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 6.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #81F27F);\n"
"     border-width: 2px;\n"
"     border-radius: 12px;\n"
"padding: 3px;\n"
"}\n"
"\n"
"QPushButton::hover{\n"
"\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #81F27F);\n"
"border-width: 1px;\n"
"border-style: no;\n"
"padding: 3px;\n"
"\n"
"}\n"
"\n"
"/* This is style when button is pressed */\n"
"QPushButton:pressed {\n"
"    /* We'll just invert the gradient by changing the colors around. */\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.2,\n"
"                                     stop: 0 #E0E0E0, stop: 1 #81F27F);\n"
"}"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/step.png"), QSize(), QIcon::Normal, QIcon::Off);
        stepPoseButton->setIcon(icon1);
        stepPoseButton->setIconSize(QSize(32, 32));

        verticalLayout->addWidget(stepPoseButton);

        playMotionButton = new QPushButton(groupBox_2);
        playMotionButton->setObjectName(QString::fromUtf8("playMotionButton"));
        playMotionButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 6.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #4383ef);\n"
"     border-width: 2px;\n"
"     border-radius: 12px;\n"
"padding: 3px;\n"
"}\n"
"\n"
"QPushButton::hover{\n"
"\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #4383ef);\n"
"border-width: 1px;\n"
"border-style: no;\n"
"padding: 3px;\n"
"\n"
"}\n"
"\n"
"/* This is style when button is pressed */\n"
"QPushButton:pressed {\n"
"    /* We'll just invert the gradient by changing the colors around. */\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.2,\n"
"                                      stop: 0 #E0E0E0, stop: 1 #4383ef);\n"
"}"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);
        playMotionButton->setIcon(icon2);
        playMotionButton->setIconSize(QSize(32, 32));

        verticalLayout->addWidget(playMotionButton);

        verticalSpacer_2 = new QSpacerItem(20, 68, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        stiffOn = new QPushButton(groupBox_2);
        stiffOn->setObjectName(QString::fromUtf8("stiffOn"));
        stiffOn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 3.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #81F27F);\n"
"     border-width: 2px;\n"
"     border-radius: 12px;\n"
"padding: 3px;\n"
"}\n"
"\n"
"QPushButton::hover{\n"
"\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #81F27F);\n"
"border-width: 1px;\n"
"border-style: no;\n"
"padding: 3px;\n"
"\n"
"}\n"
"\n"
"/* This is style when button is pressed */\n"
"QPushButton:pressed {\n"
"    /* We'll just invert the gradient by changing the colors around. */\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.2,\n"
"                         stop: 0 #E0E0E0, stop: 1 #81F27F);\n"
"}"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/on.png"), QSize(), QIcon::Normal, QIcon::Off);
        stiffOn->setIcon(icon3);
        stiffOn->setIconSize(QSize(32, 32));

        horizontalLayout_8->addWidget(stiffOn);

        stiffOff = new QPushButton(groupBox_2);
        stiffOff->setObjectName(QString::fromUtf8("stiffOff"));
        stiffOff->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 6.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #f90707);\n"
"     border-width: 2px;\n"
"     border-radius: 12px;\n"
"padding: 3px;\n"
"}\n"
"\n"
"QPushButton::hover{\n"
"\n"
"background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1.5,\n"
"                         stop: 0 #E0E0E0, stop: 1 #f90707);\n"
"border-width: 1px;\n"
"border-style: no;\n"
"padding: 3px;\n"
"\n"
"}\n"
"\n"
"\n"
"QPushButton:pressed {\n"
"    /* We'll just invert the gradient by changing the colors around. */\n"
"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.2,\n"
"                         stop: 0 #E0E0E0, stop: 1 #f90707);\n"
"}\n"
""));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/off.png"), QSize(), QIcon::Normal, QIcon::Off);
        stiffOff->setIcon(icon4);
        stiffOff->setIconSize(QSize(32, 32));

        horizontalLayout_8->addWidget(stiffOff);


        verticalLayout->addLayout(horizontalLayout_8);


        verticalLayout_5->addLayout(verticalLayout);


        horizontalLayout_2->addWidget(groupBox_2);


        verticalLayout_6->addLayout(horizontalLayout_2);


        horizontalLayout_7->addLayout(verticalLayout_6);

        settingsDockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(4), settingsDockWidget);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuNetwork->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addSeparator();
        menuFile->addAction(actionExport);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);
        menuEdit->addAction(actionNao_RC_Edition);
        menuEdit->addAction(actionNao_Academics);
        menuEdit->addSeparator();
        menuEdit->addAction(actionOpen_Robot);
        menuHelp->addAction(actionDocumentation);
        menuHelp->addSeparator();
        menuHelp->addAction(actionAbout);
        menuView->addAction(actionJoints);
        menuView->addAction(actionGraph);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Motion Editor", 0, QApplication::UnicodeUTF8));
        actionNew->setText(QApplication::translate("MainWindow", "New", 0, QApplication::UnicodeUTF8));
        actionNew->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("MainWindow", "Open...", 0, QApplication::UnicodeUTF8));
        actionOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionExport->setText(QApplication::translate("MainWindow", "Export", 0, QApplication::UnicodeUTF8));
        actionExport->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionDocumentation->setText(QApplication::translate("MainWindow", "Documentation", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        actionAbout->setShortcut(QApplication::translate("MainWindow", "Ctrl+H", 0, QApplication::UnicodeUTF8));
        actionJoints->setText(QApplication::translate("MainWindow", "Joints", 0, QApplication::UnicodeUTF8));
        actionJoints->setShortcut(QApplication::translate("MainWindow", "Ctrl+J", 0, QApplication::UnicodeUTF8));
        actionGraph->setText(QApplication::translate("MainWindow", "Graph", 0, QApplication::UnicodeUTF8));
        actionGraph->setShortcut(QApplication::translate("MainWindow", "Ctrl+G", 0, QApplication::UnicodeUTF8));
        actionNao_RC_Edition->setText(QApplication::translate("MainWindow", "Nao RC Edition", 0, QApplication::UnicodeUTF8));
        actionNao_Academics->setText(QApplication::translate("MainWindow", "Nao Academics", 0, QApplication::UnicodeUTF8));
        actionOpen_Robot->setText(QApplication::translate("MainWindow", "Load Robot...", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Robots", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
        menuNetwork->setTitle(QApplication::translate("MainWindow", "Network", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));
        dockWidget_9->setWindowTitle(QApplication::translate("MainWindow", "Robot Poses", 0, QApplication::UnicodeUTF8));
        settingsDockWidget->setWindowTitle(QApplication::translate("MainWindow", "Settings", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Connection", 0, QApplication::UnicodeUTF8));
        iPLabel->setText(QApplication::translate("MainWindow", "IP", 0, QApplication::UnicodeUTF8));
        iPLineEdit->setText(QApplication::translate("MainWindow", "127.0.0.1", 0, QApplication::UnicodeUTF8));
        portLabel->setText(QApplication::translate("MainWindow", "Port", 0, QApplication::UnicodeUTF8));
        portLineEdit->setText(QApplication::translate("MainWindow", "50000", 0, QApplication::UnicodeUTF8));
        connectButton->setText(QApplication::translate("MainWindow", "Connect", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Pose Editor", 0, QApplication::UnicodeUTF8));
        moveUpPoseButton->setText(QApplication::translate("MainWindow", "Move Up", 0, QApplication::UnicodeUTF8));
        swapPoseButton->setText(QApplication::translate("MainWindow", "Swap", 0, QApplication::UnicodeUTF8));
        moveDownPoseButton->setText(QApplication::translate("MainWindow", "Move Down", 0, QApplication::UnicodeUTF8));
        removePoseButton->setText(QApplication::translate("MainWindow", "Remove", 0, QApplication::UnicodeUTF8));
        storePoseButton->setText(QApplication::translate("MainWindow", "Store Pose", 0, QApplication::UnicodeUTF8));
        insertPoseButton->setText(QApplication::translate("MainWindow", "Insert Pose", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Time", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Scale", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Pose Executor", 0, QApplication::UnicodeUTF8));
        gotoPoseButton->setText(QString());
        stepPoseButton->setText(QString());
        playMotionButton->setText(QString());
        stiffOn->setText(QString());
        stiffOff->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
