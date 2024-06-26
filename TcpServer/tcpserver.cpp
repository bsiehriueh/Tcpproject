#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QHostAddress>
#include <QFile>
#include "mytcpserver.h"

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray data =  file.readAll();
        QString strData = data.toStdString().c_str();
        file.close();
        strData.replace("\r\n"," ");
        QStringList strList = strData.split(" ");
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() <<"ip:" << m_strIP << "pore:" << m_usPort;
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}

