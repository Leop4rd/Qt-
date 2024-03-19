#ifndef GOMOKUTHREAD_H
#define GOMOKUTHREAD_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include "gomokuconfig.h"

class GomokuThread : public QThread
{
    Q_OBJECT
public:
    explicit GomokuThread(qintptr sockDesc,QObject *parent = nullptr);
    ~GomokuThread();

private:
    void run() override;
    QTcpSocket *m_socket;   //保存玩家的tcpsocket

public:
    qintptr m_sockDesc;     //server中incomingConnection传来的参数

public slots:
    void readyReadHandle();
    void sendDatasHandle(QTcpSocket*socket, const Response_t& response);
    void disconnectedHandle();


signals:
    void threadReadyRead(QTcpSocket*socket, const QByteArray& datas);   //将接收到的信息传给服务器
    void playerQuit(QTcpSocket*socket); //将玩家退出信号传给服务器

};

#endif // GOMOKUTHREAD_H
