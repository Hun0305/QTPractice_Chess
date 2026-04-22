#ifndef CONGRATULATIONSVIEW_H
#define CONGRATULATIONSVIEW_H

#include <QGraphicsRectItem>
#include <QObject>
#include "basepawnmodel.h"

class GameView; // 전방 선언 추가

class CongratulationsView: public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    // QWidget 대신 GameView 포인터를 받도록 수정
    CongratulationsView(PlayerType winner, PlayerType myColor, GameView *gameView = nullptr);
};

#endif