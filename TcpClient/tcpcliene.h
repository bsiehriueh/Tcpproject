#ifndef TCPCLIENE_H
#define TCPCLIENE_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpCliene; }
QT_END_NAMESPACE

class TcpCliene : public QWidget
{
    Q_OBJECT

public:
    TcpCliene(QWidget *parent = nullptr);
    ~TcpCliene();
    void loadConfig();

    static TcpCliene &getInstance();
    QTcpSocket &getTcpSocket();
    QString loginName();
    QString curPath();
    void setCurPath(QString strCurPath);

public slots:
    void showConnect();
    void recvMsg();

private slots:
//    void on_send_pb_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();


private:
    Ui::TcpCliene *ui;
    QString m_strIP;
    quint16 m_usPort;
    // 连接服务器，和服务器进行交互
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;
    QString m_strCurPath;
    QFile m_file;
};
#endif // TCPCLIENE_H
