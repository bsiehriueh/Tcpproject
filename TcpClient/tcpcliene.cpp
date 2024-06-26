#include "tcpcliene.h"
#include "ui_tcpcliene.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "privatechat.h"

TcpCliene::TcpCliene(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpCliene)
{
    ui->setupUi(this);
    resize(500, 250);
    loadConfig();

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnect()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(recvMsg()));
    //连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);
}

TcpCliene::~TcpCliene()
{
    delete ui;
}

void TcpCliene::loadConfig()
{
    QFile file(":/client.config");
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

TcpCliene &TcpCliene::getInstance()
{
    static TcpCliene instance;
    return instance;
}

QTcpSocket &TcpCliene::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpCliene::loginName()
{
    return m_strLoginName;
}

QString TcpCliene::curPath()
{
    return m_strCurPath;
}

void TcpCliene::setCurPath(QString strCurPath)
{
    m_strCurPath = m_strCurPath;
}

void TcpCliene::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpCliene::recvMsg()
{
    if(!OpeWidget::getInstance().getBook()->getDownloadStatus())
    {
        qDebug() << m_tcpSocket.bytesAvailable();
        uint uiPDULen = 0;
        m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
        uint uiMsgLen = uiPDULen - sizeof(PDU);
        PDU *pdu = mkPDU(uiMsgLen);
        m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen - sizeof(uint));
    //    qDebug() << pdu->uiMsgType << (char*)(pdu->caMsg);
        switch(pdu->uiMsgType)
        {
        case ENUM_MSG_TYPE_RESPONSE:
        {
            if(0 == strcmp(pdu->caData,REGIST_OK))
            {
                QMessageBox::information(this,"注册",REGIST_OK);
            }
            else if(0 == strcmp(pdu->caData,REGIST_FAILED))
            {
                QMessageBox::warning(this,"注册",REGIST_FAILED);
            }
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_RESPONSE:
        {
            if(0 == strcmp(pdu->caData,LOGIN_OK))
            {
                m_strCurPath = QString("./%1").arg(m_strLoginName);
                QMessageBox::information(this,"登录",LOGIN_OK);
                OpeWidget::getInstance().show();
                this->hide();
            }
            else if(0 == strcmp(pdu->caData,LOGIN_FAILED))
            {
                QMessageBox::warning(this,"登录",LOGIN_FAILED);
            }
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_RESPONSE:
        {
            OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);

            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USR_RESPONSE:
        {
            if (0 == strcmp(SEARCH_USR_NO,pdu->caData))
            {
                QMessageBox::information(this,"搜索",QString("%1: not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));

            }
            else if(0 == strcmp(SEARCH_USR_ONLINE,pdu->caData))
            {
                QMessageBox::information(this,"搜索",QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
            }
            else if(0 == strcmp(SEARCH_USR_OFFLINE,pdu->caData))
            {
                QMessageBox::information(this,"搜索",QString("%1: offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData + 32, 32);
            int ret = QMessageBox::information(this, "添加好友", QString("%1 want to be your friend?").arg(caName),
                                     QMessageBox::Yes, QMessageBox::No);
            PDU *respdu = mkPDU(0);
            strncpy(respdu->caData, pdu->caData, 32); // 被加好友者用户名
            strncpy(respdu->caData + 32, pdu->caData + 32, 32); // 加好友者用户名
            if(QMessageBox::Yes == ret){
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
            }
            else
            {
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
            }
            m_tcpSocket.write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE:
        {
            QMessageBox::information(this,"添加好友",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE: // 对方同意加好友
        {
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData , 32);
            QMessageBox::information(this, "添加好友", QString("%1 已同意您的好友申请！").arg(pdu->caData));
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE: // 对方拒绝加好友
        {
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData , 32);
            QMessageBox::information(this, "添加好友", QString("%1 已拒绝您的好友申请！").arg(pdu->caData));
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPONSE:
        {
            OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        {
            char caName[32] = {'\0'};
            memcpy(caName,pdu->caData, 32);
            QMessageBox::information(this,"删除好友",QString("%1 删除你作为他的好友").arg(caName));

            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE:
        {
            QMessageBox::information(this,"删除好友","删除好友成功");
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        {
            if(PrivateChat::getInstance().isHidden())
            {
                PrivateChat::getInstance().show();
            }
            char caSendName[32] = {'\0'};
            memcpy(caSendName, pdu->caData, 32);
            QString strSendName = caSendName;
            PrivateChat::getInstance().setChatName(strSendName);
            PrivateChat::getInstance().updateMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
        {
            OpeWidget::getInstance().getFriend()->updateFriendMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_RESPONSE:
        {
            QMessageBox::information(this,"创建文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_RESPONSE:
        {
            OpeWidget::getInstance().getBook()->updateFileList(pdu);
            QString strEnterDir = OpeWidget::getInstance().getBook()->enterDir();
            if(!strEnterDir.isEmpty())
            {
                m_strCurPath = m_strCurPath+"/"+strEnterDir;
                qDebug() << "enter dir:" << m_strCurPath << "文件夹名："<< strEnterDir;
            }
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_RESPONSE:
        {
            QMessageBox::information(this,"删除文件夹",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_RESPONSE:
        {
            QMessageBox::information(this,"重命名文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_RESPONSE:
        {
            OpeWidget::getInstance().getBook()->clearEnterDir();
            QMessageBox::information(this,"进入文件夹",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPONSE:
        {
            QMessageBox::information(this,"上传文件",pdu->caData);
        }
        case ENUM_MSG_TYPE_DEL_FILE_RESPONSE:
        {
            QMessageBox::information(this,"删除文件",pdu->caData);
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPONSE:
        {
            qDebug() << pdu->caData;
            char caFileName[32] = {'\0'};
            sscanf(pdu->caData, "%s %lld", caFileName, &(OpeWidget::getInstance().getBook()->m_iTotal));
            if(strlen(caFileName)>0 && OpeWidget::getInstance().getBook()->m_iTotal > 0)
            {
                OpeWidget::getInstance().getBook()->setDownloadStatus(true);
                m_file.setFileName(OpeWidget::getInstance().getBook()->getSaveFilePath());
                if(!m_file.open(QIODevice::WriteOnly))
                {
                    QMessageBox::warning(this,"警告","获得保存文件的路径失败");
                }
            }
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPONSE:
        {
            QMessageBox::information(this,"共享文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE:
        {
            char *pPath = new char[pdu->uiMsgLen];

            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            qDebug() << "pPath:" << pPath;
            char *pos = strrchr(pPath, '/');
            if(NULL != pos)
            {
                pos++;
                QString strNote = QString("%1 share file->%2 \n Do you accept ?").arg(pdu->caData).arg(pos);
                int ret = QMessageBox::question(this,"共享文件",strNote);
                if(QMessageBox::Yes == ret)
                {
                    PDU *respdu = mkPDU(pdu->uiMsgLen);
                    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPONSE;
                    memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
                    QString strName = TcpCliene::getInstance().loginName();
                    strcpy(respdu->caData,strName.toStdString().c_str());
                    m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
                }
            }
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_RESPONSE:
        {
            QMessageBox::information(this,"移动文件",pdu->caData);
        }
        default:
            break;
        }

        free(pdu);
        pdu = NULL;
    }
    else
    {
        QByteArray buffer = m_tcpSocket.readAll();
        m_file.write(buffer);
        Book *pBook = OpeWidget::getInstance().getBook();
        pBook->m_iRecved += buffer.size();
        if(pBook->m_iTotal == pBook->m_iRecved)
        {
            m_file.close();
            pBook->m_iTotal = 0;
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);
            QMessageBox::information(this,"下载文件","下载文件成功");
        }
        else if(pBook->m_iTotal < pBook->m_iRecved)
        {
            m_file.close();
            pBook->m_iTotal = 0;
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);

            QMessageBox::critical(this,"下载文件","下载文件失败");
        }
    }
}


//void TcpCliene::on_send_pb_clicked()
//{
//    QString strMsg = ui->lineEdit->text();
//    if(!strMsg.isEmpty())
//    {
//        PDU *pdu = mkPDU(strMsg.size());
//        pdu->uiMsgType = 8888;
//        memcpy(pdu->caMsg, strMsg.toStdString().c_str(),strMsg.size());
//        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
//        free(pdu);
//        pdu = NULL;
//    }
//    else
//    {
//        QMessageBox::warning(this,"信息发送","信息发送不能为空");
//    }
//}

void TcpCliene::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        m_strLoginName = strName;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"登录","登录失败，用户名或密码为空");
    }
}

void TcpCliene::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"注册","注册失败，用户名或密码为空");
    }
}

void TcpCliene::on_cancel_pb_clicked()
{

}
