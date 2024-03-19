#include "gomokuthread.h"
#include <QDebug>

GomokuThread::GomokuThread(qintptr sockDesc,QObject *parent) : QThread(parent)
{
    moveToThread(this);
    m_sockDesc = sockDesc;
    m_socket = nullptr;
}

GomokuThread::~GomokuThread()
{
    qDebug() << "delete GomokuThread";
    if(m_socket!=nullptr){
        delete m_socket;
        m_socket = nullptr;
    }
}

void GomokuThread::run()
{
    qDebug() << "new thread run";
    m_socket = new QTcpSocket();
    if(!m_socket->setSocketDescriptor(m_sockDesc)){
        return;
    }
    qDebug() << "socket ok";
    // socket接受数据
    connect(m_socket, SIGNAL(readyRead()),this, SLOT(readyReadHandle()));
    // socket断开连接
    connect(m_socket,SIGNAL(disconnected()),this,SLOT(disconnectedHandle()));
    qDebug() << "GomokuThread::run :" << QThread::currentThreadId();
    // 进入事件循环
    this->exec();
    qDebug() << "GomokuThread::run :" << QThread::currentThreadId()<<" delete";
}

void GomokuThread::readyReadHandle()
{
    qDebug() << "GomokuThread::readyReadHandle :" << QThread::currentThread();
    emit threadReadyRead(m_socket,m_socket->readAll());
}


void GomokuThread::sendDatasHandle(QTcpSocket *socket, const Response_t &response)
{
    if(m_socket != socket) return;
    qDebug() << "GomokuThread::sendDatasHandle :" << QThread::currentThreadId();
    m_socket->write((char *)&response, sizeof(Response_t));
}

void GomokuThread::disconnectedHandle()
{
    // 通知服务器该玩家退出游戏
    emit playerQuit(m_socket);
    this->quit();
}


