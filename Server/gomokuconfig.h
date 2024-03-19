#ifndef GOMOKUCONFIG_H
#define GOMOKUCONFIG_H

#include <QTcpSocket>
#include <QMetaType>

#define  MAXROW  15 // 列数 15
#define  MAXCOL 15  // 行数 15

enum m_PieceTYPE{WHITEPIECE, BLACKPIECE, NOPIECE};  // 棋子类型
enum m_PlayerTYPE{WHITEPLAYER, BLACKPLAYER, NOPLAYER}; // 玩家类型
enum NetWorkTYPE{CLIENT, SERVER};   //网络类型

struct PtoP_t{  //PtoP模式下的结构体
    PtoP_t(){
        pieceType = NOPIECE;
        row = -1;
        col = -1;
    }
    m_PieceTYPE pieceType;
    int row;
    int col;
};

enum NetGomokuBoardCmd_e{   //
    enterRoom,
    quitRoom,
    moveChess,
    beginGame,
    err
};

struct NetGomokuboard_t{    // 网络传输结构体 客户端传输给服务器
    NetGomokuBoardCmd_e cmd;
    int roomId;
    m_PieceTYPE pieceType;
    int row;
    int col;

};

struct Response_t{      // 服务器传输给客户端结构体
    NetGomokuBoardCmd_e cmd;
    bool isAns;
    int roomId;
    //int stepNum;
    int err;
    m_PieceTYPE playerPieceType;
    int playerNum;
    PtoP_t nextStep;
    //bool unDo;
};
Q_DECLARE_METATYPE(Response_t);

struct GomokuRoom_t{    //网络房间结构体
    GomokuRoom_t(int id, QTcpSocket *player, m_PieceTYPE piece){
        roomId = id;
        playerOne = player;
        playerTwo = nullptr;
        playerOnePieceType = piece;
        playerTwoPieceType = NOPIECE;
        isGaming = false;
    }
    GomokuRoom_t(){
        roomId = -1;
        playerOne = nullptr;
        playerTwo = nullptr;
        playerOnePieceType = NOPIECE;
        playerTwoPieceType = NOPIECE;
        isGaming = false;
    }
    int roomId;
    QTcpSocket *playerOne;
    m_PieceTYPE playerOnePieceType;
    QTcpSocket *playerTwo;
    m_PieceTYPE playerTwoPieceType;
    bool isGaming;
    //int playerNum;
};



#endif // CONFIG_H
