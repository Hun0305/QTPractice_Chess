#include "congratulationsview.h"
#include <QLabel>
#include <QGraphicsProxyWidget>
#include "actionbutton.h"
#include "constants.h"
#include "gameview.h"
#include "utils.h"

extern GameView *game;

CongratulationsView::CongratulationsView(PlayerType winner, PlayerType myColor) {
    // 1. 승리/패배 판정 및 문구/색상 설정
    QString mainTitle;
    QString subMessage;
    QColor mainColor;

    if (winner == myColor) {
        mainTitle = "VICTORY!";
        subMessage = "Congratulations! You won the match.";
        mainColor = QColor(255, 215, 0); // 승리는 화려한 금색 (Gold)
    } else {
        mainTitle = "DEFEAT";
        subMessage = "Better luck next time!";
        mainColor = QColor(233, 69, 96); // 패배는 강렬한 붉은색
    }

    // 2. 메인 타이틀 그리기 (VICTORY 또는 DEFEAT)
    QGraphicsTextItem *titleItem = Utils::createTextItem(mainTitle, 60, mainColor, this);
    double titleXPosition = 600 - titleItem->boundingRect().width()/2;
    double titleYPosition = 120;
    titleItem->setPos(titleXPosition, titleYPosition);

    // 3. 축하 이미지 (이미지는 승리했을 때만 보여주거나, 패배 시 다른 이미지를 써도 좋습니다)
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

    // 4. 서브 메시지 그리기
    QGraphicsTextItem *descriptionItem = Utils::createTextItem(subMessage, 25, Qt::white, this);
    double descriptionXPosition = 600 - descriptionItem->boundingRect().width()/2;
    double descriptionYPosition = 420;
    descriptionItem->setPos(descriptionXPosition, descriptionYPosition);

    // 5. 나가기 버튼 (기존 동일)
    ActionButton *actionButton = new ActionButton("Quit game");
    double buttonXPosition = 600 - actionButton->boundingRect().width()/2;
    double buttonYPosition = 520;
    actionButton->setPos(buttonXPosition, buttonYPosition);

    connect(actionButton, SIGNAL(buttonPressed()), game, SLOT(quitGame()));

    // 중요: addItem 순서 주의
    game->scene->addItem(this);
    game->scene->addItem(actionButton);
}