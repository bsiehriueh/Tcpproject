#include "book.h"
#include "tcpcliene.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "opewidget.h"
#include "sharefile.h"

Book::Book(QWidget *parent) : QWidget(parent)
{
    m_strEnterDir.clear();
    m_bDownload = false;
    m_pTimer = new QTimer;

    m_pBookListW = new QListWidget;

    m_pReturnPb = new QPushButton("返回");
    m_pCreateDirPb = new QPushButton("创建文件夹");
    m_pDelDirPb = new QPushButton("删除文件夹");
    m_pRenamePb = new QPushButton("重命名文件");
    m_pFlushFilePb = new QPushButton("刷新文件");

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPb);
    pDirVBL->addWidget(m_pCreateDirPb);
    pDirVBL->addWidget(m_pDelDirPb);
    pDirVBL->addWidget(m_pRenamePb);
    pDirVBL->addWidget(m_pFlushFilePb);

    m_pUploadPb = new QPushButton("上传文件");
    m_pDownLoadPb = new QPushButton("下载文件");
    m_pDelFilePb = new QPushButton("删除文件");
    m_pShareFilePb= new QPushButton("共享文件");
    m_pMoveFilePB = new QPushButton("移动文件");
    m_pSelectDirPB = new QPushButton("目标目录");
    m_pSelectDirPB->setEnabled(false);

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPb);
    pFileVBL->addWidget(m_pDownLoadPb);
    pFileVBL->addWidget(m_pDelFilePb);
    pFileVBL->addWidget(m_pShareFilePb);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);

    connect(m_pCreateDirPb,SIGNAL(clicked(bool)),
            this,SLOT(createDir()));
    connect(m_pFlushFilePb,SIGNAL(clicked(bool)),
            this,SLOT(flushFile()));
    connect(m_pDelDirPb,SIGNAL(clicked(bool)),
            this,SLOT(delDir()));
    connect(m_pRenamePb,SIGNAL(clicked(bool)),
            this,SLOT(renameFile()));
    connect(m_pBookListW, SIGNAL(doubleClicked(QModelIndex)),
            this,SLOT(enterDir(QModelIndex)));
    connect(m_pReturnPb,SIGNAL(clicked(bool)),
            this,SLOT(returnPre()));
    connect(m_pUploadPb,SIGNAL(clicked(bool))
            ,this,SLOT(uploadFile()));
    connect(m_pTimer,SIGNAL(timeout()),
            this,SLOT(uploadFileData()));
    connect(m_pDelFilePb,SIGNAL(clicked(bool)),
            this,SLOT(delRegFile()));
    connect(m_pDownLoadPb,SIGNAL(clicked(bool)),
            this,SLOT(downLoadFile()));
    connect(m_pShareFilePb,SIGNAL(clicked(bool)),
            this,SLOT(shareFile()));
    connect(m_pMoveFilePB,SIGNAL(clicked(bool)),
            this,SLOT(moveFile()));
    connect(m_pSelectDirPB,SIGNAL(clicked(bool)),
            this,SLOT(selectDestDir));
}

void Book::updateFileList(const PDU *pdu)
{
    if(NULL == pdu)
    {
        return;
    }
//    m_pBookListW->clear();
    QListWidgetItem *pItemTmp = NULL;
    int row = m_pBookListW->count();
    while(m_pBookListW->count()>0)
    {
        m_pBookListW->item(row-1);
        m_pBookListW->removeItemWidget(pItemTmp);
        delete pItemTmp;
        row = row-1;
    }
    FileInfo *pFileInfo = NULL;
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    for(int i=0;i<iCount;i++)
    {
        pFileInfo = (FileInfo*)(pdu->caMsg)+i;
        qDebug() <<pFileInfo->caName << pFileInfo->iFileType;
        QListWidgetItem *pItem = new QListWidgetItem;
        if(0 == pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/dir.jpg")));
        }
        else if(1 == pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/reg.jpg")));
        }
        pItem->setText(pFileInfo->caName);
        m_pBookListW->addItem(pItem);
    }
}

void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString Book::enterDir()
{
    return m_strEnterDir;
}

bool Book::getDownloadStatus()
{
    return m_bDownload;
}

void Book::setDownloadStatus(bool status)
{
    m_bDownload = status;
}

QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

QString Book::getShareFileName()
{
    return m_strShareFileName;
}

void Book::createDir()
{
    QString strNewDir = QInputDialog::getText(this, "新建文件夹", "请输入新的文件夹名");
    if(!strNewDir.isEmpty())
    {
        if(strNewDir.size()>32)
        {
            QMessageBox::warning(this,"新建文件夹","新文件夹名字不能超过32个字符");
        }
        else
        {
            QString strName = TcpCliene::getInstance().loginName();
            QString strCurPath = TcpCliene::getInstance().curPath();
            PDU *pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData,strName.toStdString().c_str(), strName.size());
            strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.size());
            memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());

            TcpCliene::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }

    }
    else
    {
        QMessageBox::warning(this,"新建文件夹","新文件夹名字不能为空");
    }
}

void Book::flushFile()
{
    QString strCurPath = TcpCliene::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg),strCurPath.toStdString().c_str(),strCurPath.size());

    TcpCliene::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::delDir()
{
    QString strCurPath = TcpCliene::getInstance().curPath();
    QListWidgetItem *pItem =  m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this,"删除","请选择要删除的文件");
    }
    else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpCliene::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::renameFile()
{
    QString strCurPath = TcpCliene::getInstance().curPath();
    QListWidgetItem *pItem =  m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this,"重命名","请选择要重命名的文件");
    }
    else
    {
        QString strOldName = pItem->text();
        QString strNewName = QInputDialog::getText(this,"重命名文件","请输入新的重命名文件名");
        if(!strNewName.isEmpty())
        {
            PDU *pdu = mkPDU(strCurPath.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.size());
            strncpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

            TcpCliene::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
        else
        {
            QMessageBox::warning(this,"重命名","新文件名不能为空");
        }
    }
}

void Book::enterDir(const QModelIndex &index)
{
    QString strDirName = index.data().toString();
    qDebug() << strDirName;
    m_strEnterDir = strDirName;
    QString strCurPath = TcpCliene::getInstance().curPath();
    PDU *pdu = mkPDU(strCurPath.size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy(pdu->caData,strDirName.toStdString().c_str(),strDirName.size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());

    TcpCliene::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::returnPre()
{
    QString strCurPath = TcpCliene::getInstance().curPath();
    QString strRootPath = "./"+TcpCliene::getInstance().loginName();
    if(strCurPath == strRootPath)
    {
        QMessageBox::warning(this, "返回", "返回失败：已经在最开始的文件夹目录中");
    }
    else
    {
        int index = strCurPath.lastIndexOf('/');
        strCurPath.remove(index,strCurPath.size()-index);
        qDebug() << "return -->" <<strCurPath;
        TcpCliene::getInstance().setCurPath(strCurPath);
        clearEnterDir();
        flushFile();
    }
}

void Book::uploadFile()
{

    m_strUploadFilePath = QFileDialog::getOpenFileName();
    qDebug() << m_strUploadFilePath;
    if(!m_strUploadFilePath.isEmpty())
    {
        int index = m_strUploadFilePath.lastIndexOf('/');
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
        qDebug() << strFileName;

        QFile file(m_strUploadFilePath);
        qint64 fileSize = file.size();
        QString strCurPath = TcpCliene::getInstance().curPath();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        sprintf(pdu->caData,"%s %lld", strFileName.toStdString().c_str(),fileSize);
//        TcpCliene::getInstance().setCurPath(strCurPath);
//        clearEnterDir();
//        flushFile();
        TcpCliene::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
        m_pTimer->start(1000);
    }
    else
    {
        QMessageBox::warning(this,"上传文件","上传文件名字不能为空");
    }
}

void Book::delRegFile()
{
    QString strCurPath = TcpCliene::getInstance().curPath();
    QListWidgetItem *pItem =  m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this,"删除","请选择要删除的文件");
    }
    else
    {
        QString strDelName = pItem->text();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpCliene::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::uploadFileData()
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,"上传文件","打开文件失败");
        return;
    }
    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while(true)
    {
        ret = file.read(pBuffer,4096);
        if(ret > 0 && ret <= 4096)
        {
            TcpCliene::getInstance().getTcpSocket().write(pBuffer,ret);
        }
        else if(ret == 0)
        {
            break;
        }
        else
        {
            QMessageBox::warning(this,"警告","上传文件失败：读取文件失败");
            break;
        }
    }
    file.close();
    delete []pBuffer;
    pBuffer = NULL;
}

void Book::downLoadFile()
{
    QString strCurPath = TcpCliene::getInstance().curPath();
    QListWidgetItem *pItem =  m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this,"下载","请选择要下载的文件");
    }
    else
    {
        QString strSaveFilePath = QFileDialog::getSaveFileName();
        if(strSaveFilePath == NULL)
        {
            QMessageBox::warning(this,"警告","请指定文件保存的位置");
            m_strSaveFilePath.clear();
        }
        else
        {
            m_strSaveFilePath = strSaveFilePath;
        }
        QString strCurPath = TcpCliene::getInstance().curPath();
        PDU *pdu = mkPDU(strCurPath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        QString strFileName = pItem->text();
        strcpy(pdu->caData, strFileName.toStdString().c_str());
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(),strCurPath.size());
        TcpCliene::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);


    }
}

void Book::shareFile()
{
    QString strCurPath = TcpCliene::getInstance().curPath();
    QListWidgetItem *pItem =  m_pBookListW->currentItem();
    if(NULL == pItem)
    {
        QMessageBox::warning(this,"分享文件","请选择要分享的文件");
        return;
    }
    else
    {
        m_strShareFileName = pItem->text();

    }
    Friend *pFriend = OpeWidget::getInstance().getFriend();
    QListWidget *pFriendList = pFriend->getFriendList();
    ShareFile::getInstance().updateFriend(pFriendList);
    if(ShareFile::getInstance().isHidden())
    {
        ShareFile::getInstance().show();
    }
}

void Book::moveFile()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if(NULL != pCurItem)
    {
        m_strMoveFileName =pCurItem->text();
        QString strCurPath = TcpCliene::getInstance().curPath();
        m_strMoveFilePath = strCurPath +'/'+m_strMoveFileName;
        m_pSelectDirPB->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
}

void Book::selectDestDir()
{
    QListWidgetItem *pCurItem = m_pBookListW->currentItem();
    if(NULL != pCurItem)
    {
        QString strDestDir =pCurItem->text();
        QString strCurPath = TcpCliene::getInstance().curPath();
        m_strDestDir = strCurPath +'/'+strDestDir;
        int srcLen = m_strMoveFilePath.size();
        int destLen = m_strDestDir.size();
        PDU *pdu = mkPDU(srcLen + destLen + 2);
        pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
        sprintf(pdu->caData, "%d %d %s", srcLen, destLen, m_strMoveFileName.toStdString().c_str());
        memcpy(pdu->caMsg,m_strMoveFilePath.toStdString().c_str(),srcLen);
        memcpy((char*)(pdu->caMsg)+(srcLen+1),m_strDestDir.toStdString().c_str(),destLen);

        TcpCliene::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
    m_pSelectDirPB->setEnabled(false);
}
