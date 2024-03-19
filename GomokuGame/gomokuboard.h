#ifndef GOMOKUBOARD_H
#define GOMOKUBOARD_H

#include <QMainWindow>
#include <QPainter>
#include "piesepos.h"
#include "gomokuvoice.h"
#include "tiniai.h"
#include "gomokuconfig.h"
QT_BEGIN_NAMESPACE
namespace Ui { class GomokuBoard; }
QT_END_NAMESPACE



class GomokuBoard : public QMainWindow
{
    Q_OBJECT

public:
    GomokuBoard(QWidget *parent = nullptr);
    ~GomokuBoard();

    const int MaxRow = 15;  // 行数
    const int MaxCol = 15;  // 列数

    //virtual void paintEvent(QPaintEvent *);
    void init();    // 内部成员初始化
    bool eventFilter(QObject *watched, QEvent *event); // 事件过滤器，为了在ui的控件上使用Painter
    void drawEventHandle(void);             // 在widget上绘制棋盘和棋子
    virtual void mouseReleaseEvent(QMouseEvent *ev);  // 鼠标释放事件
    QPoint getRealPoint(QPoint clickPoint); // 从鼠标点击的像素坐标转换为坐标系坐标
    virtual void click(QPoint realPoint);// 执行点击相关事件
    bool getPos(QPoint realPoint, int& row, int& col);
    QPoint center(int row, int col); //通过行列位置算出对应的坐标
    virtual void validClickHandle(int row, int col, m_PieceTYPE pieceColor);//有效点击事件处理
    virtual bool canDrop(int row, int col, m_PieceTYPE pieceColor); //是否可以落子

    void drawGomokuPieces(QPainter &painter);   // 绘制棋子

    m_PlayerTYPE whoWin(PiesePos pos);              // 落子并判断赢家
    virtual void gameOverShow(m_PlayerTYPE player); // 显示游戏结束界面

    QVector<QVector<m_PieceTYPE>> m_gomokuPieces; // 数组记录棋盘上任意位置棋子状态 白色，黑色或者无棋子
    GomokuVoice m_gomokuVoice;               // 下棋音效

    bool m_isGameOver;    // 游戏是否结束
    bool m_isWhiteRound; // 当前是白子的回合？
    bool m_isAIOpen;    // 是否打开AI
    int m_nOffSet;         //距离界面的边距
    int m_nR;              // 棋子半径
    int m_nSpace;       // 相邻棋子中心之间的距离
    QList<PiesePos> m_pieseProcess; // 当前落子进程
    TiniAi m_tiniAi;    // 简单AI

public slots:
    virtual void restartClicked();  // 重新开始

    void on_backButton_clicked();   // 悔棋

    void on_volumePushButton_clicked(bool checked); //bgm音量键

    void volumeSliderValueChangedHandle(int value); //音量滑块

    void backMenuPushButtonClickedHandle();
    void on_aiOpenRadioButton_clicked(bool checked);

    void on_aiDropPushButton_clicked();

    void on_aiAdvancePushButton_clicked(bool checked);

signals:
    void backMenuClicked();

protected:
    Ui::GomokuBoard *ui;
};
#endif // GOMOKUBOARD_H
