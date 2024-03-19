#include "aboutgamedialog.h"
#include "ui_aboutgamedialog.h"
#include <QDebug>

AboutGameDialog::AboutGameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutGameDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("游戏说明");
}

AboutGameDialog::~AboutGameDialog()
{
    qDebug() << "delete AboutGameDialog";
    delete ui;
}

void AboutGameDialog::on_backPushButton_clicked()
{
    this->hide();
}
