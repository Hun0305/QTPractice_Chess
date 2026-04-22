#include "congratulationsview.h"
#include <QLabel>
#include <QGraphicsProxyWidget>
#include <QBrush>
#include "actionbutton.h"
#include "constants.h"
#include "gameview.h" // GameView의 전체 정의 포함
#include "utils.h"

CongratulationsView::CongratulationsView(PlayerType winner, PlayerType myColor, GameView *gameView) {

    // 1. 반투명한 어두운 배경을 설정하여 보드 위를 멋지게 덮습니다.
    setBrush(QBrush(QColor(0, 0, 0, 200)));

    QString mainTitle;
    QString subMessage;
    QColor mainColor;

    if (winner == myColor) {
        mainTitle = "VICTORY!";
        subMessage = "Congratulations! You won the match.";
        mainColor = QColor(255, 215, 0);
    } else {
        mainTitle = "DEFEAT";
        subMessage = "Better luck next time!";
        mainColor = QColor(233, 69, 96);
    }

    QGraphicsTextItem *titleItem = Utils::createTextItem(mainTitle, 60, mainColor, this);
    double titleXPosition = 600 - titleItem->boundingRect().width()/2;
    double titleYPosition = 120;
    titleItem->setPos(titleXPosition, titleYPosition);

    if (winner == myColor) {
        QString imagePath = ":Images/confetti.svg";
        QPixmap image(imagePath);
        QLabel *imageLabel = new QLabel();
        QGraphicsProxyWidget *pMyProxy = new QGraphicsProxyWidget(this);
        imageLabel->setPixmap(image.scaled(200, 200, Qt::KeepAspectRatio));
        imageLabel->setAttribute(Qt::WA_TranslucentBackground);
        pMyProxy->setWidget(imageLabel);
        pMyProxy->setPos(500, 200);
    }

    QGraphicsTextItem *descriptionItem = Utils::createTextItem(subMessage, 25, Qt::white, this);
    double descriptionXPosition = 600 - descriptionItem->boundingRect().width()/2;
    double descriptionYPosition = 420;
    descriptionItem->setPos(descriptionXPosition, descriptionYPosition);

    // [핵심] 버튼을 생성하고 부모를 this로 설정해야 화면에 나타납니다!
    ActionButton *actionButton = new ActionButton("Back to Lobby"); // 1. 버튼 이름 변경
    actionButton->setParentItem(this);

    double buttonXPosition = 600 - actionButton->boundingRect().width()/2;
    double buttonYPosition = 520;
    actionButton->setPos(buttonXPosition, buttonYPosition);

    if (gameView) {
        // 2. 프로그램 종료(quitGame) 대신 로비 화면(displayRoomList)으로 이동하게 연결!
        connect(actionButton, SIGNAL(buttonPressed()), gameView, SLOT(displayRoomList()));
    }
}