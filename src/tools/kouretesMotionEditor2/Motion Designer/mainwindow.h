#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "sliderdialog.h"
#include "Robot.h"
#include "aboutdialog.h"
#include "motionexecution.h"
#include "client.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow: public QMainWindow
{
		Q_OBJECT
		Q_DISABLE_COPY(MainWindow)
	public:
		explicit MainWindow(QWidget *parent = 0);
		virtual ~MainWindow();

		sliderDialog *dlg;
		Robot *robot;
		Client *tcpClient;
		bool fileChanged;
		QStringList filename;
		MotionExecution *executionDlg;

		void setUpPoseEditorHeadtitles();
		void createActions();
		void deleteActions();
		void loadFile(QString);
		void clearPoseEditor();

	protected:

		virtual void changeEvent(QEvent *);
		virtual void closeEvent(QCloseEvent *);
		//void dropEvent(QDropEvent *);

	private:
		Ui::MainWindow *m_ui;
		bool connected;

		private slots:
		void on_stiffOn_clicked();
		void myShow();
		int maybeSave();

		//action slots

		void showAbout();
		void showDocumentation();
		void exportToKME();
		void viewGraph();
		void viewJoints();
		void setNaoAcademics();
		void setNaoRC();
		void newFile();
		void openFile();
		void loadNewRobot();

		void somethingChanged();

		//Pose Editor

		void moveUpAction();
		void moveDownAction();
		void swapAction();
		void removeAction();
		void storePoseAction();
		void insertPoseAction();

		//Play Motion
		void gotoPoseAction();
		void stepPoseAction();
		void playMotionAction();
		void stiffOnAction();
		void stiffOffAction();

		//Connect
		void connectAction();
		void sendPose();
		void decodeCommand(QString);

		QString compileMessage(int);
		QString compileMessage();

};

#endif // MAINWINDOW_H
