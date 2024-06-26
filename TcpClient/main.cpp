#include "tcpcliene.h"
#include <QApplication>
#include "sharefile.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font("Times", 24, QFont::Bold);
    TcpCliene::getInstance().show();
//    ShareFile w;
//    w.show();
//    TcpCliene w;
//    w.show();
//    TcpCliene book;
//    book.show();
    return a.exec();
}
