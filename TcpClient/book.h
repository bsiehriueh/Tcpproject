#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include <QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    void clearEnterDir();
    QString enterDir();
    bool getDownloadStatus();
    void setDownloadStatus(bool status);
    QString getSaveFilePath();
    QString getShareFileName();

    qint64 m_iTotal;
    qint64 m_iRecved;

private:
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPb;
    QPushButton *m_pCreateDirPb;

    QPushButton *m_pDelDirPb;
    QPushButton *m_pRenamePb;
    QPushButton *m_pFlushFilePb;
    QPushButton *m_pUploadPb;
    QPushButton *m_pDownLoadPb;
    QPushButton *m_pDelFilePb;
    QPushButton *m_pShareFilePb;
    QPushButton *m_pMoveFilePB;
    QPushButton *m_pSelectDirPB;

    QString m_strEnterDir;
    QString m_strUploadFilePath;

    QTimer *m_pTimer;
    QString m_strSaveFilePath;
    bool m_bDownload;

    QString m_strShareFileName;
    QString m_strMoveFileName;
    QString m_strMoveFilePath;
    QString m_strDestDir;

signals:

public slots:
    void createDir();
    void flushFile();
    void delDir();
    void renameFile();
    void enterDir(const QModelIndex &index);
    void returnPre();
    void uploadFile();
    void delRegFile();

    void uploadFileData();
    void downLoadFile();
    void shareFile();
    void moveFile();
    void selectDestDir();
};

#endif // BOOK_H
