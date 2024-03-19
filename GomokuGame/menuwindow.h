#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QMainWindow>
#include "gomokuboard.h"
#include "tiniai.h"
#include "ptopgomokuboard.h"
#include "clientgomokuboard.h"
#include "aboutgamedialog.h"

namespace Ui {
class MenuWindow;
}

class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

private:
    Ui::MenuWindow *ui;
    GomokuBoard *singleModelGomokuBoard = nullptr;
    PToPGomokuBoard *ptopModelGomkuBoard = nullptr;
    ClientGomokuBoard *clientGomokuBoard = nullptr;
    AboutGameDialog *aboutGameDialog = nullptr;


public slots:
    void singleModelPushButtonCLickedHandle();
private slots:
    void on_closeGamePushButton_clicked();
    void on_aboutGamePushButton_clicked();
    void on_ptopPushButton_clicked();
    void on_netClientPushButton_clicked();
};

#endif // MENUWINDOW_H
