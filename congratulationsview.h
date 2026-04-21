#ifndef CONGRATULATIONSVIEW_H
#define CONGRATULATIONSVIEW_H

#include <QGraphicsRectItem>
#include <QObject>
#include "basepawnmodel.h"

class CongratulationsView: public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    // winner: 이긴 사람, myColor: 현재 이 화면을 보고 있는 나
    CongratulationsView(PlayerType winner, PlayerType myColor);
};

#endif