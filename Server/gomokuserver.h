#ifndef GOMOKUSERVER_H
#define GOMOKUSERVER_H

#include <QObject>
#include <QWidget>
#include <QTcpServer>
#include "gomokuconfig.h"
#include "gomokuthread.h"

class GomokuServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit GomokuServer(QObject *parent = nullptr);
    ~GomokuServer();

    bool validRowCol(int rc);   //有效的行 列值
    int addPlayer(GomokuRoom_t& room, QTcpSocket*socket, m_PieceTYPE pieceType);    //增加玩家
    GomokuRoom_t *getRoom(int roomId, QTcpSocket*socket);   //根据房间号和玩家socket获取房间信息
    void playerQuitRoom(QTcpSocket *socket);    // 玩家退出房间

private:
    void incomingConnection(qintptr handle) override;   //
    QString m_showStr;  // 用于保存界面显示文本

    QList<GomokuRoom_t> m_rooms; //当前房间列表

signals:
    void sendDatas(QTcpSocket *socket, const Response_t &response); //通知socket给客户端发送信息
    void windowShow(QString str);   //通知窗体追加显示文本


public slots:
    void threadReadyReadHandle(QTcpSocket*socket, const QByteArray& datas); //读取socket传来的信息
    void playerQuitHandle(QTcpSocket*socket);   // 玩家退出信号的处理

};

#endif // GOMOKUSERVER_H
