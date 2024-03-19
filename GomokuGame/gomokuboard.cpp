#include "gomokuboard.h"
#include "ui_gomokuboard.h"

#include "QtDebug"
#include <QMouseEvent>
#include <QMessageBox>
GomokuBoard::GomokuBoard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GomokuBoard)
{
    ui->setupUi(this);

    //15行15列的数组
    m_gomokuPieces.resize(15);
    for(int j=0;j<m_gomokuPieces.size();j++)
    {
        m_gomokuPieces[j].resize(15);
    }
    // 初始化
    init();
    // 默认设置bgm音量10，最大音量100
    ui->volumeSlider->setValue(10);
    // 默认关闭AI
    m_isAIOpen = false;

    // 给widget安装事件过滤器 在eventFilter中捕获widget上发生的Paint事件
    ui->widget->installEventFilter(this);
    ui->volumePushButton->setCheckable(true);
    ui->clientWidget->hide();
    //ui->netConn
    ui->netWidget->hide();
    this->setWindowIcon(QIcon(":/img/resource/logo.png"));
    // 重新开始按钮
    connect(ui->restartPushButton, SIGNAL(clicked()),this,SLOT(restartClicked()));
    // bgm音量调整
    connect(ui->volumeSlider, SIGNAL(valueChanged(int)),this,SLOT(volumeSliderValueChangedHandle(int)));
    // 返回主菜单
    connect(ui->backMenuPushButton, &QPushButton::clicked, [=](){ emit backMenuClicked();});
    // 开启bgm
    m_gomokuVoice.setBgmVolume(ui->volumeSlider->value());
    m_gomokuVoice.startBgm();


}

GomokuBoard::~GomokuBoard()
{
    //if(m_gomokuVoice!=nullptr) delete m_gomokuVoice;
    qDebug() << "delete GomokuBoard";
    delete ui;
}


bool GomokuBoard::eventFilter(QObject *watched, QEvent *event){

    if(watched==ui->widget && event->type() == QEvent::Paint)//发生绘图事件，且是在widget上发生的
    {
        drawEventHandle();
        return true;
    }
    else
        return false;

}

// 在widget上绘制棋盘和棋子
void GomokuBoard::drawEventHandle()
{
    QPainter painter(ui->widget);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    int side = qMin(ui->widget->width(), ui->widget->height());
    painter.scale(side / 1520.0, side / 1520.0);

    m_nOffSet = 60;  //距离界面的边距
    m_nR = 40;
    m_nSpace = 100;
    QPen pen;
    pen.setColor(QColor(0,0,0,255));
    pen.setWidth(2);
    painter.setPen(pen);
    // 绘制15条横线
    for(int i = 0; i <= 14; i++){
        painter.drawLine(QPoint(m_nOffSet, m_nOffSet+i*m_nSpace), QPoint(m_nOffSet+14*m_nSpace, m_nOffSet+i*m_nSpace));
    }
    // 绘制15条竖线
    for(int i = 0; i <= 14; i++){
        painter.drawLine(QPoint(m_nOffSet+i*m_nSpace, m_nOffSet), QPoint(m_nOffSet+i*m_nSpace, m_nOffSet+14*m_nSpace));
    }

    // 绘制五个圆点
    double markScale = 0.3;
    painter.setBrush(QBrush(Qt::black));
    painter.drawEllipse(m_nOffSet+3*m_nSpace-m_nR*markScale, m_nOffSet+3*m_nSpace-m_nR*markScale,2*m_nR*markScale,2*m_nR*markScale);
    painter.drawEllipse(m_nOffSet+3*m_nSpace-m_nR*markScale, m_nOffSet+11*m_nSpace-m_nR*markScale,2*m_nR*markScale,2*m_nR*markScale);
    painter.drawEllipse(m_nOffSet+11*m_nSpace-m_nR*markScale, m_nOffSet+3*m_nSpace-m_nR*markScale,2*m_nR*markScale,2*m_nR*markScale);
    painter.drawEllipse(m_nOffSet+11*m_nSpace-m_nR*markScale, m_nOffSet+11*m_nSpace-m_nR*markScale,2*m_nR*markScale,2*m_nR*markScale);
    painter.drawEllipse(m_nOffSet+7*m_nSpace-m_nR*markScale, m_nOffSet+7*m_nSpace-m_nR*markScale,2*m_nR*markScale,2*m_nR*markScale);
    // 绘制棋子
    drawGomokuPieces(painter);

}

// 绘制棋子
void GomokuBoard::drawGomokuPieces(QPainter &painter)
{
    for(int rowNum=0;rowNum<15;rowNum++){
        for(int colNum=0;colNum<15;colNum++){
            switch (m_gomokuPieces[rowNum][colNum]) {
            case WHITEPIECE:
            {// 显示一枚棋子
                QRect rect1(m_nOffSet+colNum*m_nSpace-m_nR,m_nOffSet+rowNum*m_nSpace-m_nR, 2*m_nR, 2*m_nR);
                painter.drawPixmap(rect1,QPixmap(":/img/resource/chess/white1.png"));
                break;
            }
            case BLACKPIECE:
            {// 显示一枚棋子
                QRect rect1(m_nOffSet+colNum*m_nSpace-m_nR,m_nOffSet+rowNum*m_nSpace-m_nR, 2*m_nR, 2*m_nR);
                painter.drawPixmap(rect1,QPixmap(":/img/resource/chess/black1.png"));
                break;
            }
            case NOPIECE:
                break;
            }
        }
    }

    //绘制最后一枚棋子光标
    if(m_pieseProcess.count()!=0){
        int colNum = m_pieseProcess.last().m_col;
        int rowNum = m_pieseProcess.last().m_row;
        double courseScale = 0.6;
        QRect rect(m_nOffSet+colNum*m_nSpace-m_nR*courseScale,m_nOffSet+rowNum*m_nSpace-m_nR*courseScale, 2*m_nR*courseScale, 2*m_nR*courseScale);
        painter.drawPixmap(rect,QPixmap(":/img/resource/chess/cursor.png"));

    }
}

// 落子并判断赢家
m_PlayerTYPE GomokuBoard::whoWin(PiesePos pos)
{
    // 判断这一手是谁下的
    m_PieceTYPE pieseColor;
    m_PlayerTYPE player;
    if(m_isWhiteRound){
        pieseColor = BLACKPIECE;
        player = BLACKPLAYER;
    }else {
        pieseColor = WHITEPIECE;
        player = WHITEPLAYER;
    }

    // 判断是否胜利
    int sameNum = -1;
    int col = pos.m_col;
    int row = pos.m_row;
    //判断上下有多少一样的棋子
    //上
    while ( col<MaxCol&&row<MaxRow&&col>=0&&row>=0) {
        if(m_gomokuPieces[row][col]==pieseColor){
            sameNum++;
            row++;
        }else {
            break;
        }
    }
    // 下
    col = pos.m_col;
    row = pos.m_row;
    while ( col<MaxCol&&row<MaxRow&&col>=0&&row>=0) {
        if(m_gomokuPieces[row][col]==pieseColor){
            sameNum++;
            row--;
        }else {
            break;
        }
    }
    if(sameNum>=5)  m_isGameOver= true;

    // 判断左右
    sameNum = -1;
    col = pos.m_col;
    row = pos.m_row;
    //左
    while(col<MaxCol&&row<MaxRow&&col>=0&&row>=0){
        if(m_gomokuPieces[row][col]==pieseColor){
            sameNum++;
            col--;
        }else {
            break;
        }
    }
    // 右
    col = pos.m_col;
    row = pos.m_row;
    while(col<MaxCol&&row<MaxRow&&col>=0&&row>=0){
        if(m_gomokuPieces[row][col]==pieseColor){
            sameNum++;
            col++;
        }else {
            break;
        }
    }
    if(sameNum>=5)  m_isGameOver= true;

    // 判断左上右下
    sameNum = -1;
    col = pos.m_col;
    row = pos.m_row;
    //左上
    while(col<MaxCol&&row<MaxRow&&col>=0&&row>=0){
        if(m_gomokuPieces[row][col]==pieseColor){
            sameNum++;
            row--;
            col--;
        }else {
            break;
        }
    }
    //右下
    col = pos.m_col;
    row = pos.m_row;
    while(col<MaxCol&&row<MaxRow&&col>=0&&row>=0){
        if(m_gomokuPieces[row][col]==pieseColor){
            sameNum++;
            row++;
            col++;
        }else {
            break;
        }
    }
    if(sameNum>=5)  m_isGameOver= true;

    // 判断左下右上
    sameNum = -1;
    col = pos.m_col;
    row = pos.m_row;
    //左下
    while(col<MaxCol&&row<MaxRow&&col>=0&&row>=0){
        if(m_gomokuPieces[row][col]==pieseColor){
            sameNum++;
            row++;
            col--;
        }else {
            break;
        }
    }
    //右下
    col = pos.m_col;
    row = pos.m_row;
    while(col<MaxCol&&row<MaxRow&&col>=0&&row>=0){
        if(m_gomokuPieces[row][col]==pieseColor){
            sameNum++;
            row--;
            col++;
        }else {
            break;
        }
    }
    if(sameNum>=5){
        m_isGameOver= true;
    }else {
        // 判断是否平局
        bool isDogfall = true;
        for(int i=0;i<m_gomokuPieces.count();i++){
            for(int j=0;j<m_gomokuPieces[i].count();j++){
                if(m_gomokuPieces[i][j]==NOPIECE){
                    isDogfall = false;
                    break;
                }
            }
            if(!isDogfall) break;
        }
        if(isDogfall){
            m_isGameOver = true;
            player = NOPLAYER;
        }
    }

    if(m_isGameOver){
        //m_gomokuVoice.playWinVoice();
        ui->backButton->setDisabled(true);
        gameOverShow(player);
        return player;
    }
    return NOPLAYER;
}

void GomokuBoard::gameOverShow(m_PlayerTYPE player)
{
    switch (player) {
    case BLACKPLAYER:{
        QMessageBox message(QMessageBox::Information, "提示", "黑色方取得胜利！");
        message.setIconPixmap(QPixmap(":/img/resource/info/happy.png"));
        message.setWindowIcon(QIcon(":/img/resource/logo.png"));
        message.setFont(QFont("FangSong", 16, QFont::Bold));
        message.exec();
        break;
    }
    case WHITEPLAYER:{
        QMessageBox message(QMessageBox::Information, "提示", "白色方取得胜利！");
        message.setIconPixmap(QPixmap(":/img/resource/info/happy.png"));
        message.setWindowIcon(QIcon(":/img/resource/logo.png"));
        message.setFont(QFont("FangSong", 16, QFont::Bold));
        message.exec();
        break;
    }
    case NOPLAYER:
        QMessageBox message(QMessageBox::Information, "提示", "平局！");
        message.setIconPixmap(QPixmap(":/img/resource/info/happy.png"));
        message.setWindowIcon(QIcon(":/img/resource/logo.png"));
        message.setFont(QFont("FangSong", 16, QFont::Bold));
        message.exec();
        break;
    }


}

// 重新开始
void GomokuBoard::restartClicked()
{
    init();
    update();
}

// 鼠标释放事件
void GomokuBoard::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() != Qt::LeftButton || m_isGameOver==true) { // 排除鼠标右键点击 游戏已结束则直接返回
        return;
    }
    QPoint pt= ev->pos();  //获取当前鼠标位置坐标
    pt=getRealPoint(pt);    //转换至实际像素坐标
    click(pt);
}


QPoint GomokuBoard::getRealPoint(QPoint clickPoint)
{
    int side = qMin(ui->widget->width(), ui->widget->height());
    QPoint realPoint;

    realPoint.setX(clickPoint.x() / double(side) * 1520.0);
    realPoint.setY(clickPoint.y() / double(side) * 1520.0);

    return realPoint;
}

void GomokuBoard::click(QPoint realPoint)
{
    // 获取落点对应行列位置
    int row = -1;
    int col = -1;
    if(!getPos(realPoint,row, col)){
        qDebug()<<"no valid click";
        return;
    }
    qDebug()<< "click row:"<<row<<" col:"<<col;
    // 点击位置有棋子

    // 处理有效点击事件
    validClickHandle(row,col,m_isWhiteRound?WHITEPIECE:BLACKPIECE);
}

bool GomokuBoard::getPos(QPoint realPoint, int &row, int &col)
{
    //qDebug()<<"realPoint.x"<<realPoint.x()<<" realPoint.y"<<realPoint.y()<<" m_nR:"<< m_nR;
    for(row = 0; row < 15; row++)
    {
        for(col = 0; col < 15; col++)
        {
            QPoint temp = center(row, col);
            //qDebug()<< "click row:"<<row<<" col:"<<col<<"temp.x:"<< temp.x()<<"temp.y:"<< temp.y();
            int x = temp.x()-realPoint.x();
            int y = temp.y()-realPoint.y();
            if(x*x+y*y < m_nR*m_nR) return true;
        }
    }
    return false;
}

QPoint GomokuBoard::center(int row, int col)
{
    QPoint rePoint;
    //这里注意坐标的转换
    rePoint.ry() = row*m_nSpace+m_nOffSet;
    rePoint.rx() = col*m_nSpace+m_nOffSet;

    return rePoint;
}


bool GomokuBoard::canDrop(int row, int col, m_PieceTYPE pieceColor){
    if(pieceColor == WHITEPIECE){
        return (m_gomokuPieces[row][col]==NOPIECE) && m_isWhiteRound;
    }else {
        return (m_gomokuPieces[row][col]==NOPIECE) && (!m_isWhiteRound);
    }
}

void GomokuBoard::validClickHandle(int row, int col, m_PieceTYPE pieceColor)
{
    if(!canDrop(row,col,pieceColor)) return;

    if(m_isWhiteRound){
        m_gomokuPieces[row][col]=WHITEPIECE;
        m_isWhiteRound = false;
    }else {
        m_gomokuPieces[row][col]=BLACKPIECE;
        m_isWhiteRound = true;
    }
    m_pieseProcess.append(PiesePos(row,col));
    m_gomokuVoice.playDropPieseVoice();
    m_tiniAi.calAssValue(m_gomokuPieces,row, col,m_isWhiteRound?BLACKPIECE:WHITEPIECE);
    update();
    // 判断是否有人胜利
    whoWin(PiesePos(row,col));

    // 开启人机对战 AI
    if(m_isAIOpen && !m_isGameOver){
        // AI辅助落子
        //auto pos =  m_tiniAi.calAllAssValue(m_gomokuPieces, m_isWhiteRound?WHITEPIECE:BLACKPIECE);
        auto pos =  m_tiniAi.calAllAssValueAdvance(m_gomokuPieces, m_isWhiteRound?WHITEPIECE:BLACKPIECE);
        if(pos.m_col != -1){
            if(m_isWhiteRound){
                m_gomokuPieces[pos.m_row][pos.m_col]=WHITEPIECE;
                m_isWhiteRound = false;
            }else {
                m_gomokuPieces[pos.m_row][pos.m_col]=BLACKPIECE;
                m_isWhiteRound = true;
            }
            m_pieseProcess.append(PiesePos(pos.m_row,pos.m_col));
            m_gomokuVoice.playDropPieseVoice();
            //m_tiniAi.calAssValue(m_gomokuPieces,row, col,m_isWhiteRound?BLACKPIECE:WHITEPIECE);
            update();
            // 判断是否有人胜利
            whoWin(PiesePos(pos.m_row,pos.m_col));
        }else {
            qDebug()<<"AI error!";
        }
    }

}

// 单机模式内部成员初始化
void GomokuBoard::init()
{
    m_isWhiteRound = false;
    m_isGameOver = false;
    m_pieseProcess.clear();
    ui->backButton->setEnabled(true);

    // 赋初值
    for(int i = 0; i < m_gomokuPieces.size(); i++)
    {
        for (int  j= 0; j < m_gomokuPieces[0].size();j++)
        {
            m_gomokuPieces[i][j] = NOPIECE;
        }
    }
}



void GomokuBoard::on_backButton_clicked()
{
    if(m_pieseProcess.count()!=0){
        m_gomokuPieces[m_pieseProcess[m_pieseProcess.count()-1].m_row][m_pieseProcess[m_pieseProcess.count()-1].m_col] = NOPIECE;
        m_pieseProcess.removeLast();
        if(m_isWhiteRound)
            m_isWhiteRound = false;
        else m_isWhiteRound = true;
        update();
    }
}


void GomokuBoard::on_volumePushButton_clicked(bool checked)
{
    if(!checked){
        //        ui->volumePushButton->setStyleSheet( "#volumePushButton{border-image: "
        //                                             "url(:/img/resource/bgm/volume-notice.png);}");
        m_gomokuVoice.setBgmVolume(ui->volumeSlider->value());
        m_gomokuVoice.startBgm();
    }else{
        // ui->volumePushButton->setStyleSheet( "#volumePushButton{border-image: url(:/img/resource/bgm/volume-mute.png);}");
        m_gomokuVoice.stopBgm();
    }
}

void GomokuBoard::volumeSliderValueChangedHandle(int value)
{
    m_gomokuVoice.setBgmVolume(value);

}

void GomokuBoard::backMenuPushButtonClickedHandle()
{
    emit backMenuClicked();
}


void GomokuBoard::on_aiOpenRadioButton_clicked(bool checked)
{
    if(checked){
        m_isAIOpen = true;
        qDebug() << "ai open";
    }else {
        m_isAIOpen = false;
        qDebug() << "ai close";
    }
}

void GomokuBoard::on_aiDropPushButton_clicked()
{
    if(!m_isGameOver){
        // AI辅助落子
       //auto pos =  m_tiniAi.calAllAssValue(m_gomokuPieces, m_isWhiteRound?WHITEPIECE:BLACKPIECE);

        auto pos =  m_tiniAi.calAllAssValueAdvance(m_gomokuPieces, m_isWhiteRound?WHITEPIECE:BLACKPIECE);

        if(pos.m_col != -1){
            if(m_isWhiteRound){
                m_gomokuPieces[pos.m_row][pos.m_col]=WHITEPIECE;
                m_isWhiteRound = false;
            }else {
                m_gomokuPieces[pos.m_row][pos.m_col]=BLACKPIECE;
                m_isWhiteRound = true;
            }
            m_pieseProcess.append(PiesePos(pos.m_row,pos.m_col));
            m_gomokuVoice.playDropPieseVoice();
            //m_tiniAi.calAssValue(m_gomokuPieces,row, col,m_isWhiteRound?BLACKPIECE:WHITEPIECE);
            update();
            // 判断是否有人胜利
            whoWin(PiesePos(pos.m_row,pos.m_col));
        }else {
            qDebug()<<"AI error!";
        }
    }
}

void GomokuBoard::on_aiAdvancePushButton_clicked(bool checked)
{
    if(!m_isGameOver){
        // AI辅助落子
       auto pos =  m_tiniAi.calAllAssValue(m_gomokuPieces, m_isWhiteRound?WHITEPIECE:BLACKPIECE);

        //auto pos =  m_tiniAi.calAllAssValueAdvance(m_gomokuPieces, m_isWhiteRound?WHITEPIECE:BLACKPIECE);

        if(pos.m_col != -1){
            if(m_isWhiteRound){
                m_gomokuPieces[pos.m_row][pos.m_col]=WHITEPIECE;
                m_isWhiteRound = false;
            }else {
                m_gomokuPieces[pos.m_row][pos.m_col]=BLACKPIECE;
                m_isWhiteRound = true;
            }
            m_pieseProcess.append(PiesePos(pos.m_row,pos.m_col));
            m_gomokuVoice.playDropPieseVoice();
            //m_tiniAi.calAssValue(m_gomokuPieces,row, col,m_isWhiteRound?BLACKPIECE:WHITEPIECE);
            update();
            // 判断是否有人胜利
            whoWin(PiesePos(pos.m_row,pos.m_col));
        }else {
            qDebug()<<"AI error!";
        }
    }

}
