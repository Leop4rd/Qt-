#ifndef TINIAI_H
#define TINIAI_H

#include <QObject>
#include <QVector>
#include <QString>
#include "gomokuconfig.h"
#include "piesepos.h"
#include <QRandomGenerator>

class TiniAi : public QObject
{
    Q_OBJECT
public:
    class GomokuAi{
    public:
        int value;
        QVector<QString> cmpStrs;
        GomokuAi(int v, QVector<QString> strs);
    };

public:

    explicit TiniAi(QObject *parent = nullptr);
    int calAssValue(QVector<QVector<m_PieceTYPE>> gomokuPieces, int rowIn, int colIn, m_PieceTYPE pieceColor);//计算单个位置的期望值，返回期望值大小
    int calAssValue(QVector<QVector<m_PieceTYPE>> gomokuPieces, m_PieceTYPE pieceColor,QList<PiesePos> &piecePosList);//计算所有位置的期望值，返回所有位置中的最大值及包含所有最大值位置的链表
    PiesePos calAllAssValue(QVector<QVector<m_PieceTYPE>> gomokuPieces, m_PieceTYPE pieceColor);
    PiesePos calAllAssValueAdvance(QVector<QVector<m_PieceTYPE>> gomokuPieces, m_PieceTYPE pieceColor);
    void static stringSeachTest();
    void reloadAssValues();
    QVector<QVector<int>> m_assValues;

    QVector<QString> m_lianwu;
    QVector<QString> m_huosi;
    QVector<QString> m_huosan;
    QVector<QString> m_chongsi;
    QVector<QString> m_miansan;
    QVector<QString> m_huoer;
    QVector<QString> m_mianer;
    QVector<QString> m_mianyi;
    QList<GomokuAi> gomokuAiList;

signals:

};

#endif // TINIAI_H
