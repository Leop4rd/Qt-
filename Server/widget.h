#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include "gomokuthread.h"
#include "gomokuserver.h"

#include "gomokuconfig.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private:
    Ui::Widget *ui;
    GomokuServer *tcpServer;    // 服务器

private slots:
    void on_pushButton_3_clicked(); //发送按钮
    void on_pushButton_clicked();   //监听按钮
    void on_pushButton_2_clicked(); //停止监听按钮
    void windowShowHandle(QString str); //显示服务端传来的信息
    void on_clearPushButton_clicked();  //清屏按钮
};
#endif // WIDGET_H
