#ifndef CLIENTGOMOKUBOARD_H
#define CLIENTGOMOKUBOARD_H

#include <QObject>
#include "gomokuboard.h"
#include "gomokuconfig.h"
#include <QTcpServer>
#include <QTcpSocket>



class ClientGomokuBoard : public GomokuBoard
{
    Q_OBJECT
public:
    explicit ClientGomokuBoard(m_PieceTYPE myPieceType, QWidget *parent = nullptr);
    ~ClientGomokuBoard();

    m_PieceTYPE m_myPieceType;      // 本方棋子类型
    m_PieceTYPE m_enemyPieceType;   // 对方棋子类型
    bool m_isBeginGame;         // 游戏是否开始

    void click(QPoint realPoint) override;  // 重写点击函数
    void restartClicked() override;     // 重写重新开始
    void gameOverShow(m_PlayerTYPE player) override;// 重写游戏结束
    void gameBeginShow();       // 游戏开始提示
private:
    QTcpSocket *tcpSocket;


public slots:
    void clientReceiveMessages();
    void clientStateChanged(QAbstractSocket::SocketState state);
    void tcpConnectClickHandle();
    void roomButtonClickHandle();
signals:

};

#endif // CLIENTGOMOKUBOARD_H
