//#include "gomokuboard.h"
#include "menuwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //GomokuBoard w;
    MenuWindow menuWindow;
    menuWindow.show();
    //w.show();
    return a.exec();
}
