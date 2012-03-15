#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent), m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);

	m_ui->portLineEdit->setValidator(new QIntValidator(1, 65535, this));

	fileChanged = false;
	filename.append("Untitled.kme");

	robot = new Robot();

	dlg = new sliderDialog(robot, QString("naoRC.xml"), this);
	dlg->move(x() + 800, y() + 50);



	setUpPoseEditorHeadtitles();

	createActions();

	tcpClient = new Client();
	connected = false;
}

MainWindow::~MainWindow()
{
	delete m_ui;
}

void MainWindow::setUpPoseEditorHeadtitles()
{

	QStringList jointNames;

	m_ui->poseEditor->setColumnCount(robot->numOfJoints + 1);

	for (int i = 0; i < robot->numOfJoints; i++)
	{

		jointNames += QString(robot->joint[i]->jointName.c_str());

	}
	jointNames += QString("Transition");

	m_ui->poseEditor->setHorizontalHeaderLabels(jointNames);

}

void MainWindow::changeEvent(QEvent *e)
{
	switch (e->type())
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void MainWindow::myShow()
{

	show();
	dlg->show();

}

void MainWindow::createActions()
{

	connect(m_ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
	connect(m_ui->actionDocumentation, SIGNAL(triggered()), this, SLOT(showDocumentation()));
	connect(m_ui->actionExport, SIGNAL(triggered()), this, SLOT(exportToKME()));
	connect(m_ui->actionGraph, SIGNAL(triggered()), this, SLOT(viewGraph()));
	connect(m_ui->actionJoints, SIGNAL(triggered()), this, SLOT(viewJoints()));
	connect(m_ui->actionNao_Academics, SIGNAL(triggered()), this, SLOT(setNaoAcademics()));
	connect(m_ui->actionNao_RC_Edition, SIGNAL(triggered()), this, SLOT(setNaoRC()));
	connect(m_ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
	connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(m_ui->actionOpen_Robot, SIGNAL(triggered()), this, SLOT(loadNewRobot()));
	connect(m_ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

	connect(dlg, SIGNAL(slidersClosed(bool)),m_ui->actionJoints, SLOT(setChecked(bool)));
	connect(m_ui->poseEditor, SIGNAL(cellChanged(int, int)), this, SLOT(somethingChanged()));

	//Pose Editor
	connect(m_ui->moveUpPoseButton, SIGNAL(clicked()), this, SLOT(moveUpAction()));
	connect(m_ui->moveDownPoseButton, SIGNAL(clicked()), this, SLOT(moveDownAction()));
	connect(m_ui->swapPoseButton, SIGNAL(clicked()), this, SLOT(swapAction()));
	connect(m_ui->removePoseButton, SIGNAL(clicked()), this, SLOT(removeAction()));
	connect(m_ui->storePoseButton, SIGNAL(clicked()), this, SLOT(storePoseAction()));
	connect(m_ui->insertPoseButton, SIGNAL(clicked()), this, SLOT(insertPoseAction()));

	//Play Motion
	connect(m_ui->gotoPoseButton, SIGNAL(clicked()), this, SLOT(gotoPoseAction()));
	connect(m_ui->stepPoseButton, SIGNAL(clicked()), this, SLOT(stepPoseAction()));
	connect(m_ui->playMotionButton, SIGNAL(clicked()), this, SLOT(playMotionAction()));
	connect(m_ui->stiffOn, SIGNAL(clicked()), this, SLOT(stiffOnAction()));
	connect(m_ui->stiffOff, SIGNAL(clicked()), this, SLOT(stiffOffAction()));

	//Networking
	connect(m_ui->connectButton, SIGNAL(clicked()), this, SLOT(connectAction()));

	//Sliders
	connect(dlg->jointsUI, SIGNAL(poseChanged()), this, SLOT(sendPose()));

}

int MainWindow::maybeSave()
{

	if (fileChanged)
	{

		QMessageBox msgBox;
		msgBox.setText("The document has been modified.");
		msgBox.setInformativeText("Do you want to save your changes?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();

		switch (ret)
		{
			case QMessageBox::Save:
				return 1;
				break;
			case QMessageBox::Discard:
				return 0;
				break;
			case QMessageBox::Cancel:
				return -1;
				break;
			default:
				return -1;
				break;
		}
	}

	return 0;
}

void MainWindow::showAbout()
{

	aboutDialog *aboutDlg = new aboutDialog;
	aboutDlg->show();

}

void MainWindow::showDocumentation()
{

	//DO SOMETHING...OPEN THE MANUAL...

}

void MainWindow::exportToKME()
{

	QString s = QFileDialog::getSaveFileName(this, "Choose a filename to save under", "./kme", "KME Files (*.kme);;All FIles (*.*)");

	QFile outputFile(s);

	if (!outputFile.open(QIODevice::WriteOnly))
	{
		QMessageBox msgBox;
		msgBox.setText("Error!");
		msgBox.setInformativeText("The file could not be opened.");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		return;
	}

	for (int i = 0; i < m_ui->poseEditor->rowCount(); i++)
	{
		outputFile.write(qPrintable(compileMessage(i)));
	}

	outputFile.close();
	fileChanged = false;

}

void MainWindow::viewGraph()
{
}

void MainWindow::viewJoints()
{

	if (m_ui->actionJoints->isChecked())
	{
		dlg->show();
	} else
	{
		dlg->hide();
	}
}

void MainWindow::setNaoAcademics()
{
}
void MainWindow::setNaoRC()
{
}

void MainWindow::newFile()
{

	int reply = maybeSave();
	if (reply)
	{ // Save the file
		exportToKME();
	} else if (reply == -1) // The user pressed Cancel
		return;

	clearPoseEditor(); //Discard Data and Start from scratch
	fileChanged = false;
	filename.clear();
	filename.append("Untitled.kme");

}

void MainWindow::openFile()
{

	QString str;
	int reply = maybeSave();
	if (reply == 0)
	{
		filename = QFileDialog::getOpenFileNames(this, "Looking for .kme files...", "./kme", "KME Files( *.kme );;All Files( *.* )");
	if	(!filename.isEmpty())
		foreach(str, filename)
			loadFile(str);
	}
}

void MainWindow::loadNewRobot()
{
	/*
	 int reply = maybeSave();
	 if(reply == 1)
	 exportToKME();
	 if(reply == -1)
	 return;

	 m_ui->poseEditor->clearContents();
	 delete dlg;
	 delete robot;

	 fileChanged = false;
	 filename.append("Untitled.kme");

	 robot = new Robot();

	 dlg = new sliderDialog(robot,this);
	 dlg->move(x()+800,y()+50);

	 setUpPoseEditorHeadtitles();

	 deleteActions();
	 createActions();
	 */
}

void MainWindow::loadFile(QString filename)
{

	QFile inputFile(filename);
	QString line;
	QStringList pose;
	int curColumn, curRow;

	inputFile.open(QIODevice::ReadWrite);

	QTextStream in(&inputFile);

	curRow = m_ui->poseEditor->rowCount(); //It might be zero or appending to a previous file
	while (!in.atEnd())
	{
		line = in.readLine();
		pose = line.split("%");
		pose = pose.filter(QRegExp("[-]?[0-9]+[.][0-9]+"));

		m_ui->poseEditor->insertRow(m_ui->poseEditor->rowCount());

		curColumn = 0;
		foreach(QString str,pose)
		{
			QTableWidgetItem * item = new QTableWidgetItem(str);
			item->setTextAlignment(Qt::AlignCenter);
			m_ui->poseEditor->setItem(curRow,curColumn, item);
			curColumn++;
		}
		curRow++;
	}
	inputFile.close();
}

void MainWindow::clearPoseEditor()
{

	while (m_ui->poseEditor->rowCount() > 0)
	{
		m_ui->poseEditor->removeRow(0);
	}

}

void MainWindow::somethingChanged()
{

	fileChanged = true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{

	int reply = maybeSave();

	if (reply == 1)
	{
		exportToKME();
		event->accept();
	} else if (reply == 0)
	{
		event->accept();
	} else
	{
		event->ignore();
	}
}

// Pose Editor

void MainWindow::moveUpAction()
{

	int curRow;
	if (m_ui->poseEditor->currentItem())
		curRow = m_ui->poseEditor->currentItem()->row();
	else
		curRow = -1;
	if (curRow < 1)
		return;

	m_ui->poseEditor->insertRow(curRow - 1);

	for (int i = 0; i < m_ui->poseEditor->columnCount(); i++)
	{
		m_ui->poseEditor->setItem(curRow - 1, i, m_ui->poseEditor->takeItem(curRow + 1, i));
	}

	m_ui->poseEditor->selectRow(curRow - 1);
	m_ui->poseEditor->removeRow(curRow + 1);
}

void MainWindow::moveDownAction()
{

	int curRow;
	if (m_ui->poseEditor->currentItem())
		curRow = m_ui->poseEditor->currentItem()->row();
	else
		return;

	if (curRow == m_ui->poseEditor->rowCount() - 1)
		return;

	m_ui->poseEditor->insertRow(curRow + 2);

	for (int i = 0; i < m_ui->poseEditor->columnCount(); i++)
	{
		m_ui->poseEditor->setItem(curRow + 2, i, m_ui->poseEditor->takeItem(curRow, i));
	}

	m_ui->poseEditor->selectRow(curRow + 2);
	m_ui->poseEditor->removeRow(curRow);
}

void MainWindow::swapAction()
{

	std::vector<int> rows;

	for (int i = 0; i < m_ui->poseEditor->rowCount(); i++)
	{
		for (int j = 0; j < m_ui->poseEditor->columnCount(); j++)
		{

			if (m_ui->poseEditor->item(i, j)->isSelected())
			{
				rows.push_back(i);
				continue;
			}
		}
		if (rows.size() == 2)
			break;
	}

	if (rows.size() < 2)
		return;

	m_ui->poseEditor->insertRow(m_ui->poseEditor->rowCount());

	for (int i = 0; i < m_ui->poseEditor->columnCount(); i++)
	{

		m_ui->poseEditor->setItem(m_ui->poseEditor->rowCount() - 1, i, m_ui->poseEditor->takeItem(rows[0], i));
		m_ui->poseEditor->setItem(rows[0], i, m_ui->poseEditor->takeItem(rows[1], i));
		m_ui->poseEditor->setItem(rows[1], i, m_ui->poseEditor->takeItem(m_ui->poseEditor->rowCount() - 1, i));

	}
	m_ui->poseEditor->removeRow(m_ui->poseEditor->rowCount() - 1);

}

void MainWindow::removeAction()
{

	m_ui->poseEditor->removeRow(m_ui->poseEditor->currentRow());

}

void MainWindow::storePoseAction()
{

	int lastRow = m_ui->poseEditor->rowCount();

	m_ui->poseEditor->insertRow(lastRow);

	for (int i = 0; i < robot->numOfJoints; i++)
	{

		QTableWidgetItem * item = new QTableWidgetItem(QString::number(dlg->jointsUI->sliders[i]->dblSpinBox->value(), 'f', 3));
		item->setTextAlignment(Qt::AlignCenter);

		m_ui->poseEditor->setItem(lastRow, i, item);
	}

	QTableWidgetItem * item = new QTableWidgetItem(QString::number(m_ui->transitionTime->value(), 'f', 2));
	item->setTextAlignment(Qt::AlignCenter);

	m_ui->poseEditor->setItem(lastRow, m_ui->poseEditor->columnCount() - 1, item);

}

void MainWindow::insertPoseAction()
{

	int curRow;
	if (m_ui->poseEditor->currentItem())
		curRow = m_ui->poseEditor->currentItem()->row();
	else
		curRow = -1;

	if (curRow < 0)
		emit storePoseAction();

	m_ui->poseEditor->insertRow(curRow);

	for (int i = 0; i < m_ui->poseEditor->columnCount() - 1; i++)
	{

		QTableWidgetItem * item = new QTableWidgetItem(QString::number(dlg->jointsUI->sliders[i]->dblSpinBox->value(), 'f', 3));
		item->setTextAlignment(Qt::AlignCenter);

		m_ui->poseEditor->setItem(curRow, i, item);

	}

	QTableWidgetItem * item = new QTableWidgetItem(QString::number(m_ui->transitionTime->value(), 'f', 2));
	item->setTextAlignment(Qt::AlignCenter);

	m_ui->poseEditor->setItem(curRow, m_ui->poseEditor->columnCount() - 1, item);

}

void MainWindow::deleteActions()
{

	//Pose Editor
	//connect(m_ui->moveUpPoseButton, SIGNAL(clicked()), this, SLOT(moveUpAction()));
	//connect(m_ui->moveDownPoseButton, SIGNAL(clicked()), this, SLOT(moveDownAction()));
	//connect(m_ui->swapPoseButton, SIGNAL(clicked()), this, SLOT(swapAction()));
	//connect(m_ui->removePoseButton, SIGNAL(clicked()), this, SLOT(removeAction()));
	//disconnect(m_ui->storePoseButton);
	//connect(m_ui->insertPoseButton, SIGNAL(clicked()), this, SLOT(insertPoseAction()));

}

void MainWindow::gotoPoseAction()
{

	int curRow;
	if (m_ui->poseEditor->currentItem())
		curRow = m_ui->poseEditor->currentItem()->row();
	else
		return;

	tcpClient->writeData(compileMessage(curRow));

}

void MainWindow::stepPoseAction()
{

	int curRow;

	if (m_ui->poseEditor->rowCount() < 1)
		return;

	if (m_ui->poseEditor->currentItem())
		curRow = m_ui->poseEditor->currentItem()->row();
	else
		curRow = 0;

	tcpClient->writeData(compileMessage(curRow));

	m_ui->poseEditor->selectRow(curRow + 1);

}

void MainWindow::playMotionAction()
{

	int rows;
	QStringList *motion = new QStringList;

	if ((rows = m_ui->poseEditor->rowCount()) < 1)
		return;

	for (int i = 0; i < rows; i++)
	{
		//motion->append(compileMessage(i));
		tcpClient->writeData(compileMessage(i));
	}

	//executionDlg = new MotionExecution(this, motion);
}

void MainWindow::stiffOnAction()
{

	tcpClient->writeData(QString("stiffness_on_all\n\0"));

}

void MainWindow::stiffOffAction()
{

	tcpClient->writeData(QString("stiffness_off_all\n\0"));

}

void MainWindow::connectAction()
{

	tcpClient = new Client(m_ui->iPLineEdit->text(), m_ui->portLineEdit->text().toInt());
	tcpClient->connectTo();
	//std::cout << QString << std::endl;
	connect(tcpClient, SIGNAL(newData(QString)), this, SLOT(decodeCommand(QString)));
	sleep(10);	
	//dlg->connect_sliders();
	connected = true;
}

QString MainWindow::compileMessage(int curRow)
{

	//This type of message is sent only for compatibility reasons with v1.0
	//We should change that soon...
	std::cout<<"compileMessage in row"<<std::endl;
	QString message("play");

	double timeScale = 1.0;
	std::cout <<  m_ui->poseEditor->columnCount() <<std::endl;
	for (int i = 0; i < m_ui->poseEditor->columnCount(); i++)
	{

		if (i == m_ui->poseEditor->columnCount() - 1)
			timeScale = m_ui->scaleFactor->value();

		message += "%" + QString::number(timeScale * m_ui->poseEditor->item(curRow, i)->text().toDouble(), 'f', 3);

	}

	message += "\n\0";
	std::cout<<qPrintable(message)<<std::endl;

	return message;
}

QString MainWindow::compileMessage()
{
	std::cout<<"compileMessage()"<<std::endl;

	QString message("play");

	for (int i = 0; i < robot->numOfJoints; i++)
	{

		//std::cout<<robot->orderedJointNames[i]<<"        "<<robot->indexOrder[robot->orderedJointNames[i]]<<std::endl;
		message += "%" + QString::number(dlg->jointsUI->sliders[robot->indexOrder[robot->orderedJointNames[i]]]->dblSpinBox->value(), 'f', 3);
		//message += "%" + QString::number(dlg->jointsUI->sliders[i]->dblSpinBox->value(), 'f', 3);
	}
	message += "%" + QString::number(m_ui->transitionTime->value(), 'f', 2) + "\n\0";
	std::cout<<qPrintable(message)<<std::endl;

	return message;
}

void MainWindow::sendPose()
{
	if(connected){
		
	
		std::cout << "Send Pose " << std::endl;
		tcpClient->writeData(compileMessage());
	}else{
			std::cout << "No Send Pose still not connected " << std::endl;

	}

}

void MainWindow::decodeCommand(QString command)
{

	std::cout << "Decode commands " << std::endl;
	QStringList list;

	list = command.split(QString('%'));

	if (list.at(0) == "store")
	{
		emit storePoseAction();
		return;
	}

	int curJoint;
	for (int cur = 0; cur < robot->numOfJoints; cur++)
	{

		std::cout<<list.at(cur+1).toDouble()<<" ";
		dlg->jointsUI->sliders[cur]->dblSpinBox->setValue((list.at(cur + 1).toDouble()));
	}
	//std::cout<<std::endl<<std::endl;

}

void MainWindow::on_stiffOn_clicked()
{
	tcpClient->writeData(QString("stiffness_on_all\n\0"));
}
