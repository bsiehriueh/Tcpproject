#include "tcpserver.h"
#include <QApplication>
#include "opedb.h"


int main(int argc, char *argv[])//这是所有C++程序的入口点，也就是程序开始运行的地方。
{
    QApplication a(argc, argv);//创建一个QApplication对象，对一些全局性的初始设置进行初始化，并处理所有Qt应用程序的事件。

    OpeDB::getInstance().init();// 调用OpeDB类的静态方法getInstance()获取OpeDB类的实例，并调用其init()方法进行某些初始化工作。这可能是进行数据库连接的设置。exact意思取决于类OpeDB的实现。

    TcpServer w;//创建一个TcpServer对象w。
    w.show();//启动TCP服务器，显示其窗口（如果有的话）。
    return a.exec();//开始运行应用程序，并等待用户的操作

}
