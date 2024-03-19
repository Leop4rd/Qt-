#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QHostInfo>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    tcpServer = new GomokuServer(this);
    this->setWindowTitle("GomokuGameServer");
    // 新的连接
    //connect(tcpServer, SIGNAL(newConnection()), this, SLOT(mNewConnetion()));
    ui->pushButton_2->setEnabled(false);
    // 获取端口号
    QHostAddress address;
    QString localHostName = QHostInfo::localHostName();  //获取主机名
    ui->textBrowser->append(localHostName);
    QHostInfo info = QHostInfo::fromName(localHostName); //获取主机IP
    foreach(address, info.addresses()){
        if(address.protocol() == QAbstractSocket::IPv4Protocol) ui->ipComboBox->addItem(address.toString());
    }
    connect(tcpServer, SIGNAL(windowShow(QString)), this,SLOT(windowShowHandle(QString)));

    qDebug() << "Widget::Widget thread:" << QThread::currentThreadId();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_3_clicked()
{
    ui->textBrowser->append("此功能暂时未开通");
}

void Widget::on_pushButton_clicked()
{
    // 绑定ip地址和端口
    QString serIP = ui->ipComboBox->currentText();//获取IP
    //QString serPort = 9999;//获取端口
    ui->textBrowser->append(serIP+":9999");
    tcpServer->listen((QHostAddress)serIP,9999);//启动监听
    ui->pushButton_2->setEnabled(true);
    ui->pushButton->setEnabled(false);
}

void Widget::on_pushButton_2_clicked()
{
    // 停止监听
    tcpServer->close();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
}

void Widget::windowShowHandle(QString str)
{
    ui->textBrowser->append(str);
}

void Widget::on_clearPushButton_clicked()
{
    ui->textBrowser->clear();
}
