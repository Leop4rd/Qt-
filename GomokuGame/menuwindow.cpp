#include "menuwindow.h"
#include "ui_menuwindow.h"
#include <QDebug>
#include <QtNetwork>
#include <QMessageBox>


MenuWindow::MenuWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MenuWindow)
{
    ui->setupUi(this);
    //connect(ui.sin )
    this->setWindowIcon(QIcon(":/img/resource/logo.png"));
    aboutGameDialog = new AboutGameDialog(); //游戏说明窗口创建
    // 信号：单机模式按钮点击 槽：打开单机模式窗口
    connect(ui->singleModelPushButton,SIGNAL(clicked()),this, SLOT(singleModelPushButtonCLickedHandle()));
}

MenuWindow::~MenuWindow()
{
    delete ui;
    qDebug()<<"delete MenuWindow";
    if(singleModelGomokuBoard!=nullptr) {
        delete singleModelGomokuBoard;
    };
    if(ptopModelGomkuBoard!=nullptr) {

        delete ptopModelGomkuBoard;
    };
    if(clientGomokuBoard!=nullptr){
        delete clientGomokuBoard;
    }
    if(aboutGameDialog!=nullptr){
        delete aboutGameDialog;
    }
}

// 单机模式
void MenuWindow::singleModelPushButtonCLickedHandle()
{
    singleModelGomokuBoard = new GomokuBoard();
    //设置单机模式窗口内退出游戏按钮的槽函数并连接
    connect(singleModelGomokuBoard,&GomokuBoard::backMenuClicked,[=](){
        // 设置关闭时释放窗口
        singleModelGomokuBoard->setAttribute(Qt::WA_DeleteOnClose);
        // 关闭单机模式界面
        singleModelGomokuBoard->close();
        // 重新显示菜单
        this->show();
        singleModelGomokuBoard = nullptr;
    });
    // 隐藏菜单
    this->hide();
    // 显示单机模式界面
    singleModelGomokuBoard->show();
}


// 退出游戏
void MenuWindow::on_closeGamePushButton_clicked()
{
    this->close();
}

// 游戏说明
void MenuWindow::on_aboutGamePushButton_clicked()
{
    aboutGameDialog->show();
}

// 两台电脑双人作战 客户端服务器ptop模式
void MenuWindow::on_ptopPushButton_clicked()
{
    QMessageBox::StandardButtons ret = QMessageBox::question(
                this,
                "提示",
                "是否作为[服务器]启动?<br>"
                "- Yes: 服务器, 属白色<br>"
                "- No: 客户端, 属黑方<br><br>",
                QMessageBox::Yes | QMessageBox::No
                );

    NetWorkTYPE netType = (ret == QMessageBox::Yes ? SERVER : CLIENT);
    m_PieceTYPE pieceType = (ret == QMessageBox::Yes ? WHITEPIECE : BLACKPIECE);

    ptopModelGomkuBoard = new PToPGomokuBoard(netType,pieceType);
    connect(ptopModelGomkuBoard,&GomokuBoard::backMenuClicked,[=](){
        ptopModelGomkuBoard->setAttribute(Qt::WA_DeleteOnClose);
        ptopModelGomkuBoard->close();
        this->show();
        ptopModelGomkuBoard = nullptr;
    });
    this->hide();
    ptopModelGomkuBoard->show();
}

// 联网模式 作为客户端
void MenuWindow::on_netClientPushButton_clicked()
{
    QMessageBox::StandardButtons ret = QMessageBox::question(
                this,
                "提示",
                "是否作为[先手方]启动?<br>"
                "- Yes: 属黑方<br>"
                "- No: 属白方<br><br>",
                QMessageBox::Yes | QMessageBox::No
                );
    m_PieceTYPE pieceType = (ret == QMessageBox::Yes ? BLACKPIECE:WHITEPIECE);
    clientGomokuBoard = new ClientGomokuBoard(pieceType);
    connect(clientGomokuBoard,&ClientGomokuBoard::backMenuClicked,[=](){
        clientGomokuBoard->setAttribute(Qt::WA_DeleteOnClose);
        clientGomokuBoard->close();
        this->show();
        clientGomokuBoard = nullptr;
    });
    this->hide();
    clientGomokuBoard->show();
}
