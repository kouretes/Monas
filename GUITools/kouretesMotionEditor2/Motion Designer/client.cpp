/****************************************************************************
 **
 ** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 ** Contact: Qt Software Information (qt-info@nokia.com)
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Commercial License Agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Nokia.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain
 ** additional rights. These rights are described in the Nokia Qt LGPL
 ** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
 ** package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** If you are unsure which license is appropriate for your use, please
 ** contact the sales department at qt-sales@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

 #include <QtNetwork>

 #include "client.h"
#include "iostream"
Client::Client(QWidget *parent){}

 Client::Client(QString inHost, int inPort, QWidget *parent)
 {
     host = inHost;
     port = inPort;

     tcpSocket = new QTcpSocket(this);

     connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readData()));
     connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
 }

 void Client::connectTo()
 {
     tcpSocket->abort();
     tcpSocket->connectToHost(host,port);
 }

 void Client::readData()
 {
        QTextStream in(tcpSocket);

        QString input;
        in >> input;
        //std::cout << input << std::endl;
        emit newData(input);
 }

 void Client::writeData(QString data){

    if( tcpSocket->ListeningState )
        tcpSocket->write(qPrintable(data));

 }

 void Client::displayError(QAbstractSocket::SocketError socketError)
 {
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
         QMessageBox::information(this, tr("Connect..."),
                                  tr("The host has closed connection."));
         break;
     case QAbstractSocket::HostNotFoundError:
         QMessageBox::information(this, tr("Connect..."),
                                  tr("The host was not found. Please check the "
                                     "host name and port settings."));
         break;
     case QAbstractSocket::ConnectionRefusedError:
         QMessageBox::information(this, tr("Connect"),
                                  tr("The connection was refused by the peer. "
                                     "Make sure the Server is running, "
                                     "and check that the host name and port "
                                     "settings are correct."));
         break;
     default:
         QMessageBox::information(this, tr("Connect"),
                                  tr("The following error occurred: %1.")
                                  .arg(tcpSocket->errorString()));
     }
 }
