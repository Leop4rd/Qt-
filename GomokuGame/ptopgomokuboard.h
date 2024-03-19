#ifndef PTOPGOMOKUBOARD_H
#define PTOPGOMOKUBOARD_H

#include <QObject>
#include "gomokuboard.h"
#include "gomokuconfig.h"
#include <QTcpServer>
#include <QTcpSocket>


class PToPGomokuBoard : public GomokuBoard
{
    Q_OBJECT
public:
    explicit PToPGomokuBoard(NetWorkTYPE myNetType,m_PieceTYPE myPieceType,QWidget *parent = nullptr);
    ~PToPGomokuBoard();

    //bool canDrop(int row, int col, m_PieceTYPE pieceColor) override;
    //void validClickHandle(int row, int col, m_PieceTYPE pieceColor) override;
    void click(QPoint realPoint) override; // 重写鼠标点击函数
    void restartClicked() override; // 游戏结束后重新开始
    void gameOverShow(m_PlayerTYPE player) override;    // 重写游戏结束


private:
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;


public:
    bool m_isBeginGame;
    m_PieceTYPE m_myPieceType;
    m_PieceTYPE m_enemyPieceType;
    NetWorkTYPE m_myNetType;


public slots:
    void newConnectHandle();
    void serverReceiveMessages();
    void clientReceiveMessages();
    void serverStateChanged(QAbstractSocket::SocketState state);
    void clientStateChanged(QAbstractSocket::SocketState state);
    void tcpConnectClickHandle();




signals:
};
#endif // PTOPGOMOKUBOARD_H
