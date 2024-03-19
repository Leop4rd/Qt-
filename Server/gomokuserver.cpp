#include "gomokuserver.h"
#include <QDebug>

GomokuServer::GomokuServer(QObject *parent) : QTcpServer(parent)
{
    m_rooms.clear();
}

GomokuServer::~GomokuServer()
{
    qDebug() << "delete GomokuServer";
}


bool GomokuServer::validRowCol(int rc)
{
    if(rc>=0&&rc<15){
        return true;
    }
    return  false;
}

int GomokuServer::addPlayer(GomokuRoom_t& room, QTcpSocket *socket, m_PieceTYPE pieceType)
{
    if(room.playerOne == nullptr){
        if(room.playerTwoPieceType != pieceType){
            room.playerOne = socket;
            room.playerOnePieceType = pieceType;
            return room.roomId;
        }
    }else if(room.playerTwo == nullptr){
        if(room.playerOnePieceType != pieceType){
            room.playerTwo = socket;
            room.playerTwoPieceType = pieceType;
            return  room.roomId;
        }
    }
    return -1;
}

GomokuRoom_t *GomokuServer::getRoom(int roomId, QTcpSocket *socket)
{
    for(int i=0;i<m_rooms.count();i++){
        if(m_rooms[i].roomId == roomId){
            if(m_rooms[i].playerOne == socket || m_rooms[i].playerTwo == socket) return &m_rooms[i];
            break;
        }
    }
    return  nullptr;
}

// 玩家退出房间处理
void GomokuServer::playerQuitRoom(QTcpSocket *socket)
{
    for(int i=0;i<m_rooms.count();i++){
        if(m_rooms[i].playerOne == socket){
            m_rooms[i].playerOne = nullptr;
            m_rooms[i].playerOnePieceType = NOPIECE;
            QString showStr = QString("玩家离开room：%1").arg(m_rooms[i].roomId);
            emit windowShow(showStr);
        }else if (m_rooms[i].playerTwo == socket) {
            m_rooms[i].playerTwo = nullptr;
            m_rooms[i].playerTwoPieceType = NOPIECE;
            m_showStr = QString("玩家离开room：%1").arg(m_rooms[i].roomId);
            emit windowShow(m_showStr);
        }
        if(m_rooms[i].playerOne == nullptr && m_rooms[i].playerTwo == nullptr){
            qDebug()<< "房间:" << m_rooms[i].roomId << "无人，删除该房间";
            m_showStr = QString("room：%1 无人，删除该room").arg(m_rooms[i].roomId);
            emit windowShow(m_showStr);
            m_rooms.removeAt(i);
            i--;
        }
    }
}

void GomokuServer::incomingConnection(qintptr handle)
{
    qDebug() << "new connect, handle = " << handle ;
    GomokuThread* thread = new GomokuThread(handle);
    // 线程发送接收到的数据给服务器
    connect(thread, SIGNAL(threadReadyRead(QTcpSocket*, const QByteArray& )),this, SLOT(threadReadyReadHandle(QTcpSocket *, const QByteArray &)) ,Qt::DirectConnection);
    // 服务器通知线程发送数据给客户端
    connect(this, SIGNAL(sendDatas(QTcpSocket *, const Response_t &)), thread,SLOT(sendDatasHandle(QTcpSocket *, const Response_t &)));
    // 线程发送玩家退出信号给服务器
    connect(thread,SIGNAL(playerQuit(QTcpSocket*)), this,SLOT(playerQuitHandle(QTcpSocket*)));
    // 开始执行线程
    thread->start();
}

void GomokuServer::threadReadyReadHandle(QTcpSocket *socket, const QByteArray &datas)
{
    NetGomokuboard_t* res = (NetGomokuboard_t*)datas.begin();
    qDebug() << "GomokuServer::threadReadyReadHandle thread:" << QThread::currentThread();
    qDebug() << "roomId:" << res->roomId << " row:" << res->row << " col:" << res->col;
    Response_t responce;
    responce.roomId = res->roomId;
    responce.isAns = true;

    //处理接收数据
    switch (res->cmd) {
    case enterRoom:{    //加入房间
        //判断房间名是否满足要求 301~310
        if(res->roomId > 310 || res->roomId<301){
            qDebug() << "企图创建违规房间，创建房间失败";
            responce.err=101;
            responce.playerNum = -1;
            responce.playerPieceType = NOPIECE;
            socket->write((char *)&responce, sizeof(Response_t));
            return;
        }
        int roomCount = -1;
        //寻找是否存在想要进入的房间
        for(int i=0;i<m_rooms.count();i++){
            if(m_rooms[i].roomId == res->roomId){
                roomCount = i;
                break;
            }
        }
        //没有房间，创建房间
        if(roomCount == -1){
            qDebug() << "玩家1创建room："<< res->roomId;
            m_showStr = QString("玩家1创建room：%1").arg(res->roomId);
            emit windowShow(m_showStr);
            responce.err = -1;
            responce.playerPieceType = res->pieceType;
            responce.playerNum = 1;
            responce.cmd = enterRoom;
            socket->write((char *)&responce, sizeof(Response_t));
            m_rooms.append(GomokuRoom_t(res->roomId,socket,res->pieceType));
        }else {
            // 房间存在，加入房间
            if(addPlayer(m_rooms[roomCount], socket,res->pieceType) == -1){
                responce.err = 102;
                responce.playerNum = -1;
                responce.playerPieceType = NOPIECE;
                socket->write((char *)&responce, sizeof(Response_t));
                qDebug() << "新玩家加入room："<< res->roomId << " 失败";
            }else{
                responce.err = -1;
                responce.playerPieceType = res->pieceType;
                responce.playerNum = 2;
                responce.cmd = enterRoom;
                socket->write((char *)&responce, sizeof(Response_t));

                m_rooms[roomCount].isGaming = true;
                // 通知对方游戏开始
                responce.isAns = false;
                responce.cmd = beginGame;
                if(socket == m_rooms[roomCount].playerOne){
                    emit sendDatas(m_rooms[roomCount].playerTwo,responce);
                }else if (socket == m_rooms[roomCount].playerTwo) {
                    emit sendDatas(m_rooms[roomCount].playerOne,responce);
                }
                qDebug() << "新玩家加入room："<< res->roomId << " 成功";
                QString showStr = QString("新玩家加入room：%1，游戏开始").arg(res->roomId);
                emit windowShow(showStr);
            }
        }
        break;
    }
    // 离开房间
    case quitRoom:{
        qDebug() << "player quit room";
        //QList<int> backRoomId;
        playerQuitRoom(socket);
        responce.cmd = quitRoom;
        responce.err=-1;
        responce.playerNum = -1;
        responce.playerPieceType = NOPIECE;
        socket->write((char *)&responce, sizeof(Response_t));
        break;
    }
    // 落子
    case moveChess:{
        //是否为有效的房间号
        auto room = getRoom(res->roomId,socket);
        if(room == nullptr || room->isGaming == false) return;
        Response_t stepInfo;
        stepInfo.cmd = moveChess;
        stepInfo.isAns = false;
        stepInfo.roomId = res->roomId;
        stepInfo.nextStep.row = res->row;
        stepInfo.nextStep.col = res->col;
        stepInfo.nextStep.pieceType = res->pieceType;
        stepInfo.err = -1;
        if(socket == room->playerOne){
            emit sendDatas(room->playerTwo,stepInfo);
        }else if (socket == room->playerTwo) {
            emit sendDatas(room->playerOne,stepInfo);
        }
        break;
    }
    default:
        responce.err=401;
        responce.playerNum = -1;
        responce.playerPieceType = NOPIECE;
        socket->write((char *)&responce, sizeof(Response_t));
        break;

    }
}

void GomokuServer::playerQuitHandle(QTcpSocket *socket)
{
    qDebug() << "player quit game";
    playerQuitRoom(socket);
    // 删除该线程
    delete sender();
}
