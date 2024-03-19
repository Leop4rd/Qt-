#include "ptopgomokuboard.h"
#include "ui_gomokuboard.h"
#include <QDebug>

PToPGomokuBoard::PToPGomokuBoard(NetWorkTYPE myNetType,m_PieceTYPE myPieceType,QWidget *parent): GomokuBoard(parent)
{
    m_myNetType=myNetType;
    m_myPieceType=myPieceType;
    // 设置游戏未开始
    m_isBeginGame = false;
    // 隐藏AI窗口
    GomokuBoard::ui->aiWidget->hide();
    // 显示网络窗口
    ui->netWidget->show();
    // 禁止悔棋
    ui->backButton->setEnabled(false);
    // 禁止重新开始
    ui->restartPushButton->setEnabled(false);
    // 隐藏客户端窗口
    ui->clientWidget->hide();
    // 显示ptop模式对应窗口与文本
    QString titleStr="";
    if(m_myPieceType == WHITEPIECE){
        m_enemyPieceType = BLACKPIECE;
        titleStr = "白子";
    }else {
        m_enemyPieceType = WHITEPIECE;
        titleStr = "黑子";
    }
    // 连接按钮
    connect(ui->netConnectPushButton,SIGNAL(clicked()),this,SLOT(tcpConnectClickHandle()));
    // 判断网络类型 并进行相应设置
    if(m_myNetType == SERVER){
        tcpServer = new QTcpServer(this);
        ui->netConnectPushButton->setText("开启监听");
        ui->netTitleLabel->setText("服务器，执"+titleStr);
        connect(tcpServer,SIGNAL(newConnection()), this, SLOT(newConnectHandle()));
    }
    if(m_myNetType == CLIENT){
        tcpSocket = new QTcpSocket(this);
        ui->netConnectPushButton->setText("连接服务器");
        ui->netTitleLabel->setText("客户端，执"+titleStr);
        connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(clientReceiveMessages()));
        connect(tcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                this, SLOT(clientStateChanged(QAbstractSocket::SocketState)));

    }
}

PToPGomokuBoard::~PToPGomokuBoard()
{
    qDebug() << "delete PToPGomkuBoard";
}

void PToPGomokuBoard::click(QPoint realPoint)
{
    if(!m_isBeginGame) return;

    // 获取落点对应行列位置
    int row = -1;
    int col = -1;
    if(!getPos(realPoint,row, col)){
        qDebug()<<"no valid click";
        return;
    }
    qDebug()<< "click row:"<<row<<" col:"<<col;
    // 点击位置有棋子
    if(!canDrop(row,col,m_myPieceType))return;
    // 处理有效点击事件
    switch (m_myNetType) {
    case SERVER:{
        QList <QTcpSocket *> socketList = tcpServer->findChildren<QTcpSocket *>();
        qDebug() << "tcpSocket 数量：" << socketList.count();
        if (socketList.count() == 0) {
            qDebug()<<"当前没有客户端连接，请先与客户端连接！";
            return;
        }
        foreach (QTcpSocket *tmpTcpSocket, socketList) {
            // 服务端向每个客户端发送消息
            PtoP_t gomokuDatas;
            gomokuDatas.col = col;
            gomokuDatas.row = row;
            gomokuDatas.pieceType = m_myPieceType;
            tmpTcpSocket->write((char *)(&gomokuDatas),sizeof(PtoP_t));
        }
        break;
    }
    case CLIENT:{
        PtoP_t gomokuDatas;
        gomokuDatas.col = col;
        gomokuDatas.row = row;
        gomokuDatas.pieceType = m_myPieceType;
        tcpSocket->write((char *)(&gomokuDatas),sizeof(PtoP_t));
        break;
    }
    }
    validClickHandle(row,col,m_myPieceType);
}

// 游戏结束后重新开始
void  PToPGomokuBoard::restartClicked()
{
    GomokuBoard::init();
    // 关闭悔棋按钮
    ui->backButton->setEnabled(false);
    ui->restartPushButton->setEnabled(false);
    GomokuBoard::update();

}

void PToPGomokuBoard::gameOverShow(m_PlayerTYPE player)
{
    GomokuBoard::gameOverShow(player);
    ui->restartPushButton->setEnabled(true);
    m_isBeginGame = false;
    switch (m_myNetType) {
    case CLIENT:{
        tcpSocket->close();
        ui->netConnectPushButton->setText("连接服务器");
        break;
    }
    case SERVER:{
        tcpServer->close();
        ui->netConnectPushButton->setText("开启监听");
        break;
    }
    }

}


void PToPGomokuBoard::newConnectHandle()
{
    // 与客户端连接
    QTcpSocket *tmpTcpSocket = tcpServer->nextPendingConnection();

    // 客户端的ip地址
    QString ipaddr = tmpTcpSocket->peerAddress().toString();
    quint16 port = tmpTcpSocket->peerPort();
    qDebug()<< "客户端连接，ip:" << ipaddr<<" port:"<<port;
    m_isBeginGame = true;

    connect(tmpTcpSocket, SIGNAL(readyRead()), this, SLOT(serverReceiveMessages()));
    connect(tmpTcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(serverStateChanged(QAbstractSocket::SocketState)));
}

void PToPGomokuBoard::serverReceiveMessages()
{
    QTcpSocket *tmpTcpSocket = (QTcpSocket *)sender();
    // 接收消息
    //qDebug() <<  "客户端接收消息：" <<  tmpTcpSocket->readAll();
    auto res = tmpTcpSocket->readAll();
    PtoP_t* resPtoP = (PtoP_t*)res.begin();
    qDebug() << "row:" << resPtoP->row << " col:" << resPtoP->col;
    if(resPtoP->pieceType == WHITEPIECE){
        qDebug() << "white";
    }
    if(resPtoP->pieceType == BLACKPIECE){
        qDebug() << "black";
    }
    //if(m_enemyPieceType==resPtoP->pieceType && (m_enemyPieceType==m_isWhiteRound?WHITEPIECE:BLACKPIECE)){
    validClickHandle(resPtoP->row,resPtoP->col,resPtoP->pieceType);

}

void PToPGomokuBoard::clientReceiveMessages(){
    auto res = tcpSocket->readAll();
    PtoP_t* resPtoP = (PtoP_t*)res.begin();
    qDebug() << "row:" << resPtoP->row << " col:" << resPtoP->col;
    if(resPtoP->pieceType == WHITEPIECE){
        qDebug() << "white";
    }
    if(resPtoP->pieceType == BLACKPIECE){
        qDebug() << "black";
    }
    //if(m_enemyPieceType==resPtoP->pieceType && (m_enemyPieceType==m_isWhiteRound?WHITEPIECE:BLACKPIECE)){
    validClickHandle(resPtoP->row,resPtoP->col,resPtoP->pieceType);
    // }

}

void PToPGomokuBoard::serverStateChanged(QAbstractSocket::SocketState state)
{
    QTcpSocket *tmpTcpSocket = (QTcpSocket *)sender();
    switch (state) {
    // 已断开
    case QAbstractSocket::UnconnectedState:
        qDebug() << "服务端：客户端断开连接";
        tmpTcpSocket->deleteLater();
        m_isBeginGame = false;
        break;
        // 已连接
    case QAbstractSocket::ConnectedState:
        qDebug() << "服务端：客户端已连接";
        m_isBeginGame =true;
        break;
    default:
        break;
    }
}

void PToPGomokuBoard::clientStateChanged(QAbstractSocket::SocketState state){
    switch (state) {
    // 已断开
    case QAbstractSocket::UnconnectedState:
        qDebug() << "与服务器断开连接";
        ui->netConnectPushButton->setText("连接服务器");
        m_isBeginGame = false;
        break;
        // 已连接
    case QAbstractSocket::ConnectedState:
        qDebug() << "与服务器建立连接";
        ui->netConnectPushButton->setText("断开连接");
        m_isBeginGame = true;
        break;
    default:
        break;
    }

}

void PToPGomokuBoard::tcpConnectClickHandle()
{
    if(ui->netConnectPushButton->text()=="连接服务器"){
        qDebug() << "连接服务器";
        // 指定服务端的ip地址与端口
        QStringList ipPortList = ui->ipLineEdit->text().split(":");
        if(ipPortList.count() == 2){
            qDebug() << "ip:" << ipPortList[0]<<" port:" << ipPortList[1].toInt();
        }else {
            qDebug("ip/Port输入错误");
            return;
        }
        tcpSocket->connectToHost(QHostAddress(ipPortList[0]), ipPortList[1].toInt());
    }else if(ui->netConnectPushButton->text() == "断开连接"){
        qDebug() << "断开连接";
        tcpSocket->close();
        ui->netConnectPushButton->setText("连接服务器");
    }
    else if(ui->netConnectPushButton->text() == "开启监听"){
        qDebug()<<"开启中";
        // 指定服务端的ip地址与端口
        QStringList ipPortList = ui->ipLineEdit->text().split(":");
        if(ipPortList.count() == 2){
            qDebug() << "ip:" << ipPortList[0]<<" port:" << ipPortList[1].toInt();
        }else {
            qDebug("ip/Port输入错误");
            return;
        }
        // 绑定ip地址和端口
        if(tcpServer->listen(QHostAddress(ipPortList[0]), ipPortList[1].toInt())){
            ui->netConnectPushButton->setText("关闭监听");
            qDebug()<< "tcp服务器创建成功，等待连接";
        }else {
            qDebug()<<"tcp服务器创建失败";
        }
    }else if(ui->netConnectPushButton->text() == "关闭监听"){
        tcpServer->close();
        qDebug()<<"tcp服务器关闭";
        ui->netConnectPushButton->setText("开启监听");
    }
}
