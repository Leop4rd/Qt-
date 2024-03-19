#include "tiniai.h"
#include "QDebug"
#include <algorithm>
#include <QtCore/qmath.h>



TiniAi::TiniAi(QObject *parent) : QObject(parent)
{

    //15行15列的数组
    m_assValues.resize(15);
    for(int j=0;j<m_assValues.size();j++)
    {
        m_assValues[j].resize(15);
    }
    // 赋初值
    reloadAssValues();
    // 连五添加
    m_lianwu.append("11111");
    gomokuAiList.append(GomokuAi(10000000,m_lianwu));
    // 活四添加
    m_huosi.append("011110");
    gomokuAiList.append(GomokuAi(1000000,m_huosi));
    // 活三添加
    m_huosan.append("001110");
    m_huosan.append("011100");
    m_huosan.append("010110");
    m_huosan.append("011010");
    gomokuAiList.append(GomokuAi(10000,m_huosan));
    // 冲四
    m_chongsi.append("01111");
    m_chongsi.append("10111");
    m_chongsi.append("11011");
    m_chongsi.append("11101");
    m_chongsi.append("11110");
    gomokuAiList.append(GomokuAi(8000,m_chongsi));
    // 眠3
    m_miansan.append("001112");
    m_miansan.append("010112");
    m_miansan.append("011012");
    m_miansan.append("211100");
    m_miansan.append("211010");
    //m_miansan.append("101012");
    gomokuAiList.append(GomokuAi(1000,m_miansan));

    // 活2
    m_huoer.append("001100");
    m_huoer.append("011000");
    m_huoer.append("000110");
    gomokuAiList.append(GomokuAi(800,m_huoer));
    // 眠2
    m_mianer.append("011200");
    m_mianer.append("001120");
    m_mianer.append("002110");
    m_mianer.append("021100");
    m_mianer.append("001010");
    m_mianer.append("010100");
    gomokuAiList.append(GomokuAi(50,m_mianer));
    // 眠1
    m_mianyi.append("001200");
    m_mianyi.append("002100");
    m_mianyi.append("020100");
    m_mianyi.append("000210");
    m_mianyi.append("000120");
    gomokuAiList.append(GomokuAi(10,m_mianyi));
}

int TiniAi::calAssValue(QVector<QVector<m_PieceTYPE> > gomokuPieces, int rowIn, int colIn, m_PieceTYPE pieceColor)
{
   // qDebug() << "calAssValue";

//    for(int i=0;i<gomokuPieces.count();i++){
//        for(int j=0;j<gomokuPieces[i].count();j++){
//            qDebug() << i << " "<<j;
//        }
//    }

    //qDebug()<<"gomocount = " << gomokuPieces.count();
    //qDebug()<<"gomocount[0] = " << gomokuPieces[0].count();


//    QString piecestr;
//    switch (pieceColor) {
//    case WHITEPIECE:
//        piecestr = "white";
//        break;
//    case BLACKPIECE:
//        piecestr = "black";
//        break;
//    case NOPIECE:
//        piecestr = "nopiece";
//        break;
//    }
//    qDebug() << "color: " << piecestr;


    int multiTypesValue = 0;


    // 初始化值
    int assValue = 0;
    int tempValue = 0;
    int col;
    int row;
    QString resStr = "";


    //求期望值
    //上  to  下
    row = rowIn-4;
    col= colIn;
    for(int i=0;i<9;i++){
        if(row<0 || row>=MAXROW){
            row++;
            continue;
        }else {
            if(gomokuPieces[row][col]== pieceColor){
                resStr+="1";
            }else if (gomokuPieces[row][col]== NOPIECE) {
                resStr+="0";
            }else {
                resStr+="2";
            }
            row++;
        }
    }
    // 计算期望值
    tempValue = 0;
    for(int i=0;i<gomokuAiList.count();i++){
        for(int j=0;j<gomokuAiList[i].cmpStrs.count();j++){
            if(resStr.indexOf(gomokuAiList[i].cmpStrs[j])!=-1){
                tempValue = gomokuAiList[i].value;
                multiTypesValue += pow(10,i);
                break;
            }
        }
        if(tempValue!=0) break;
    }
    assValue+=tempValue;

    //左  to  右
    resStr = "";
    row = rowIn;
    col= colIn-4;
    for(int i=0;i<9;i++){
        if(col<0 || col>=MAXCOL){
            col++;
            continue;
        }else {
            if(gomokuPieces[row][col]== pieceColor){
                resStr+="1";
            }else if (gomokuPieces[row][col]== NOPIECE) {
                resStr+="0";
            }else {
                resStr+="2";
            }
            col++;
        }
    }
    // 计算期望值
    tempValue = 0;
    for(int i=0;i<gomokuAiList.count();i++){
        for(int j=0;j<gomokuAiList[i].cmpStrs.count();j++){
            if(resStr.indexOf(gomokuAiList[i].cmpStrs[j])!=-1){
                tempValue = gomokuAiList[i].value;
                multiTypesValue += pow(10,i);
                break;
            }
        }
        if(tempValue!=0) break;
    }
    assValue+=tempValue;

    //左上  to  右下
    resStr = "";
    row = rowIn-4;
    col= colIn-4;
    for(int i=0;i<9;i++){
        if(col<0 || col>=MAXCOL || row<0||row>=MAXROW){
            col++;
            row++;
            continue;
        }else {
            if(gomokuPieces[row][col]== pieceColor){
                resStr+="1";
            }else if (gomokuPieces[row][col]== NOPIECE) {
                resStr+="0";
            }else {
                resStr+="2";
            }
            col++;
            row++;
        }
    }
    // 计算期望值
    tempValue = 0;
    for(int i=0;i<gomokuAiList.count();i++){
        for(int j=0;j<gomokuAiList[i].cmpStrs.count();j++){
            if(resStr.indexOf(gomokuAiList[i].cmpStrs[j])!=-1){
                tempValue = gomokuAiList[i].value;
                multiTypesValue += pow(10,i);
                break;
            }
        }
        if(tempValue!=0) break;
    }
    assValue+=tempValue;

    //左下  to  右上
    resStr = "";
    row = rowIn+4;
    col= colIn-4;
    for(int i=0;i<9;i++){
        if(col<0 || col>=MAXCOL || row<0||row>=MAXROW){
            col++;
            row--;
            continue;
        }else {
            if(gomokuPieces[row][col]== pieceColor){
                resStr+="1";
            }else if (gomokuPieces[row][col]== NOPIECE) {
                resStr+="0";
            }else {
                resStr+="2";
            }
            col++;
            row--;
        }
    }

    // 计算期望值
    tempValue = 0;
    for(int i=0;i<gomokuAiList.count();i++){
        for(int j=0;j<gomokuAiList[i].cmpStrs.count();j++){
            if(resStr.indexOf(gomokuAiList[i].cmpStrs[j])!=-1){
                tempValue = gomokuAiList[i].value;
                multiTypesValue += pow(10,i);
                break;
            }
        }
        if(tempValue!=0) break;
    }
    assValue+=tempValue;

    // 对多种模型同时存在的情况进行加权
    // 连五添加 0位
    // 活四添加 1位
    // 活三添加 2位
    // 冲四
    // 眠3
    // 活2
    // 眠2
    // 眠1   7位
    //int lianwuNum = (multiTypesValue/1)%10;
   // int huosiNum = (multiTypesValue/10)%10;
    int huosanNum = (multiTypesValue/100)%10;
    int chongsiNum = (multiTypesValue/1000)%10;
    //int minasanNum = (multiTypesValue/10000)%10;
    int huoerNum =  (multiTypesValue/10000)%10;
    //int mianerNum = (multiTypesValue/100000)%10;
    //int mianyiNum = (multiTypesValue/1000000)%10;
    //双活三
    if(chongsiNum>=2) assValue *=8;
    if(huosanNum>0&&chongsiNum>0) assValue *= 8;
    if(huosanNum>=2) assValue *= 6;
    if(huoerNum>0&&chongsiNum>0) assValue*=4;
    if(huosanNum>0&&huoerNum>0) assValue *= 3;
    if(huoerNum>=2) assValue*=2;




    //qDebug() << rowIn << " " << colIn<<" assValue:" << assValue << " multiTypesValue:" << multiTypesValue;
    return assValue;
}

int TiniAi::calAssValue(QVector<QVector<m_PieceTYPE>> gomokuPieces, m_PieceTYPE pieceColor,QList<PiesePos> &piecePosList){
    int maxValue=-1;
    int tempValue = -1;
    piecePosList.clear();
    m_PieceTYPE enemyPieceColor = (pieceColor==BLACKPIECE?WHITEPIECE:BLACKPIECE);
    reloadAssValues();
    for(int row=0;row<MAXROW;row++){
        for(int col=0;col<MAXCOL;col++){
            if(gomokuPieces[row][col]!=NOPIECE) continue;

            gomokuPieces[row][col] = pieceColor;
            tempValue = calAssValue(gomokuPieces, row, col, pieceColor);
            gomokuPieces[row][col] = NOPIECE;

            gomokuPieces[row][col] = enemyPieceColor;
            tempValue += calAssValue(gomokuPieces, row, col, enemyPieceColor)*0.8;
            gomokuPieces[row][col] = NOPIECE;

            m_assValues[row][col] = tempValue;
            if(tempValue>maxValue){
                maxValue = tempValue;
            }
        }
    }

    //找最大值 并生成链表
    for(int i=0;i<m_assValues.count();i++){
        for(int j=0;j<m_assValues[i].count();j++){
            if( maxValue == m_assValues[i][j]){
                piecePosList.append(PiesePos(i,j));
            }
        }
    }
    return  maxValue;
}

PiesePos TiniAi::calAllAssValue(QVector<QVector<m_PieceTYPE> > gomokuPieces, m_PieceTYPE pieceColor)
{
    int maxValue=-1;
    QList<PiesePos> maxPosList;
    maxValue = calAssValue(gomokuPieces,pieceColor,maxPosList);
    PiesePos resPos(-1,-1);
    if(maxPosList.count()==0) return resPos;
    qDebug()  << "maxValue:"<< maxValue;
    // return PiesePos(maxRow,maxCol);
    return maxPosList[QRandomGenerator::global()->bounded((int)0,maxPosList.count())];
}


// 增加探索深度
PiesePos TiniAi::calAllAssValueAdvance(QVector<QVector<m_PieceTYPE> > gomokuPieces, m_PieceTYPE pieceColor)
{
    //

    int maxValue=-1;
    QList<PiesePos> maxPosList;
    QList<PiesePos> deepPosList;
    m_PieceTYPE enemyPieceColor = (pieceColor==BLACKPIECE?WHITEPIECE:BLACKPIECE);
    qDebug()<<"first serach";
    maxValue = calAssValue(gomokuPieces,pieceColor,maxPosList);
    qDebug()<<"first serach over";
    qDebug() << maxPosList.count();
    PiesePos resPos(-1,-1);
    if(maxPosList.count()==0) return resPos;
    if(maxPosList.count()==1) return maxPosList[0];

    QVector<int> maxValueList;
    //第二轮搜索
    qDebug()<<"second serach";
    for(int i=0;i<maxPosList.count();i++){
        maxValue = -1;
        gomokuPieces[maxPosList[i].m_row][maxPosList[i].m_col] = pieceColor;
        maxValue = calAssValue(gomokuPieces,enemyPieceColor,deepPosList);
        maxValueList.append(maxValue);
        gomokuPieces[maxPosList[i].m_row][maxPosList[i].m_col] = NOPIECE;
    }
    auto minValue = std::min_element(maxValueList.begin(),maxValueList.end());
    qDebug() << "minValue: " << *minValue;
    QList<int> resPosNumList;

    for(int i=0;i<maxValueList.count();i++){
        if(*minValue == maxValueList[i]) resPosNumList.append(i);
    }
    qDebug() << "minValue Num: " << resPosNumList.count();
    return  maxPosList[resPosNumList[QRandomGenerator::global()->bounded((int)0,resPosNumList.count())]];
}

void  TiniAi::stringSeachTest()
{
    QString str = "hello world";
    QVector<QVector<int>> te = {{1,2,3},{4,5,6}};

    //qDebug() <<
    //qDebug() << str.indexOf("ell");
    //qDebug() << str.indexOf("ldd");


}

void TiniAi::reloadAssValues()
{
    for(int i = 0; i < m_assValues.size(); i++)
    {
        for (int  j= 0; j < m_assValues[0].size();j++)
        {
            m_assValues[i][j] = -1;
        }
    }
}



TiniAi::GomokuAi::GomokuAi(int v, QVector<QString> strs)
{
    value = v;
    cmpStrs = strs;
}
