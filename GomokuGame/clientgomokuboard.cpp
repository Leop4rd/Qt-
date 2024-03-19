#include "clientgomokuboard.h"
#include "ui_gomokuboard.h"
#include <QMessageBox>


ClientGomokuBoard::ClientGomokuBoard(m_PieceTYPE myPieceType, QWidget *parent):GomokuBoard(parent)
{
    m_myPieceType = myPieceType;
    // 设置游戏未开始
    m_isBeginGame = false;
    // 关闭ai窗口
    GomokuBoard::ui->aiWidget->hide();
    // 显示网络窗口
    ui->netWidget->show();
    // 关闭悔棋按钮
    ui->backButton->setEnabled(false);
    // 关闭重新开始按钮
    ui->restartPushButton->setEnabled(false);
    // 显示提示
    QString titleStr="";
    if(m_myPieceType == WHITEPIECE){
        m_enemyPieceType = BLACKPIECE;
        titleStr = "白子";
    }else {
        m_enemyPieceType = WHITEPIECE;
        titleStr = "黑子";
    }
    tcpSocket = new QTcpSocket(this);
    ui->netConnectPushButton->setText("连接服务器");
    ui->netTitleLabel->setText("客户端，执"+titleStr);
    // 显示客户端窗口
    ui->clientWidget->show();
    // 网络连接按钮
    connect(ui->netConnectPushButton,SIGNAL(clicked()),this,SLOT(tcpConnectClickHandle()));
    // 加入房间按钮
    connect(ui->roomPushButton,SIGNAL(clicked()),this,SLOT(roomButtonClickHandle()));
    // 接收数据
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(clientReceiveMessages()));
    // 客户端状态改变
    connect(tcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(clientStateChanged(QAbstractSocket::SocketState)));
}

ClientGomokuBoard::~ClientGomokuBoard()
{
    qDebug() << "delete ClientGomokuBoard";
}

void ClientGomokuBoard::click(QPoint realPoint)
{
    if(!m_isBeginGame) {
        qDebug() << "游戏未开始";
        return;}

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
    NetGomokuboard_t gomokuDatas;
    gomokuDatas.cmd = moveChess;
    gomokuDatas.roomId = ui->roomLineEdit->text().toUInt();
    gomokuDatas.col = col;
    gomokuDatas.row = row;
    gomokuDatas.pieceType = m_myPieceType;
    tcpSocket->write((char *)(&gomokuDatas),sizeof(NetGomokuboard_t));

    validClickHandle(row,col,m_myPieceType);

}

void ClientGomokuBoard::restartClicked()
{
    GomokuBoard::init();
    // 关闭悔棋按钮
    ui->backButton->setEnabled(false);
    ui->restartPushButton->setEnabled(false);
    GomokuBoard::update();
}

void ClientGomokuBoard::gameOverShow(m_PlayerTYPE player)
{
    GomokuBoard::gameOverShow(player);
    ClientGomokuBoard::roomButtonClickHandle();
    m_isGameOver = false;
}

void ClientGomokuBoard::gameBeginShow()
{
    restartClicked();
    m_isBeginGame = true;
    QMessageBox message(QMessageBox::Information, "提示", "游戏开始！");
    message.setIconPixmap(QPixmap(":/img/resource/info/about.png"));
    message.setWindowIcon(QIcon(":/img/resource/logo.png")
                          );
    message.setFont(QFont("FangSong", 16, QFont::Bold));
    message.exec();
}




void ClientGomokuBoard::clientReceiveMessages(){
    qDebug() << "客户端接收消息" ;
    // 接收消息
    auto res = tcpSocket->readAll();
    Response_t* sResponse = (Response_t*)res.begin();
    if(sResponse->isAns){
        QString str = QString("%1 %2 %3").arg(sResponse->roomId).arg(sResponse->playerNum).arg(sResponse->err);
        qDebug()<<str;

        switch (sResponse->cmd) {
        case enterRoom:{
            qDebug() << "enter room ok";
            if(sResponse->playerNum == 2){
                gameBeginShow();
            }
            ui->roomPushButton->setText("离开房间");
            return;
        }
        case quitRoom:{
            qDebug() << "quit room ok";
            m_isBeginGame = false;
            ui->roomPushButton->setText("加入房间");
        }
        default:
            break;
        }
    }else{
        // 服务器非应答消息处理
        QString str = QString("%1 %2 %3").arg(sResponse->roomId).arg(sResponse->playerNum).arg(sResponse->err);
        qDebug() << "row:" << sResponse->nextStep.row << " col:" << sResponse->nextStep.col;
        if(sResponse->nextStep.pieceType == WHITEPIECE){
            qDebug() << "white";
        }
        if(sResponse->nextStep.pieceType == BLACKPIECE){
            qDebug() << "black";
        }
        if(sResponse->cmd == moveChess){
            validClickHandle(sResponse->nextStep.row,sResponse->nextStep.col,sResponse->nextStep.pieceType);
        }else if (sResponse->cmd == beginGame) {
            gameBeginShow();

        }
    }

}

void ClientGomokuBoard::clientStateChanged(QAbstractSocket::SocketState state){
    switch (state) {
    // 已断开
    case QAbstractSocket::UnconnectedState:
        qDebug() << "与服务器断开连接";
        ui->netConnectPushButton->setText("连接服务器");
        if(ui->roomPushButton->text() == "离开房间") ui->roomPushButton->setText("加入房间");
        break;
        // 已连接
    case QAbstractSocket::ConnectedState:
        qDebug() << "与服务器建立连接";
        ui->netConnectPushButton->setText("断开连接");
        break;
    default:
        break;
    }
}

void ClientGomokuBoard::tcpConnectClickHandle()
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
}

void ClientGomokuBoard::roomButtonClickHandle()
{
    // 发送消息
    qDebug() << "连接按钮";
    if (tcpSocket->state() == QAbstractSocket::ConnectedState ) {
        if(ui->roomPushButton->text() == "加入房间"){
            NetGomokuboard_t gomokuDatas;
            gomokuDatas.cmd = enterRoom;
            gomokuDatas.roomId = ui->roomLineEdit->text().toUInt();
            gomokuDatas.col = -1;
            gomokuDatas.row = -1;
            gomokuDatas.pieceType = m_myPieceType;
            //tcpSocket->write("GET VIN2\r\n");
            tcpSocket->write((char *)(&gomokuDatas),sizeof(NetGomokuboard_t));
            qDebug() << "发送成功";
        }else if(ui->roomPushButton->text() == "离开房间"){
            NetGomokuboard_t gomokuDatas;
            gomokuDatas.cmd = quitRoom;
            gomokuDatas.roomId = ui->roomLineEdit->text().toUInt();
            gomokuDatas.col = -1;
            gomokuDatas.row = -1;
            gomokuDatas.pieceType = m_myPieceType;
            tcpSocket->write((char *)(&gomokuDatas),sizeof(NetGomokuboard_t));
            qDebug() << "发送成功";
        }

    } else
        qDebug() << "请先与服务端连接！";


}
