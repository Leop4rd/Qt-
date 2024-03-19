#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 注册Response_t& 类型，使该类型可以在信号槽中使用
    qRegisterMetaType<Response_t>("Response_t&");
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
