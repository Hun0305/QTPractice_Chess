#include "actionbutton.h"
#include <QBrush>
#include <QPen>
#include <QFont>
#include "constants.h"
#include "utils.h"

// [핵심 해결 부분!] 괄호 안에 parent 인자를 넣어 .h 파일과 완벽하게 짝을 맞춥니다.
ActionButton::ActionButton(QString title, QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
{
    setRect(0, 0, 200, 50);

    // 1. 미래지향적 투명 배경 설정
    QBrush brush;
    brush.setStyle(Qt::NoBrush);
    setBrush(brush);

    // 2. 형광 시안(Neon Cyan) 테두리 선 설정
    QPen pen(QColor("#00F0FF"));
    pen.setWidth(2); // 선 두께
    setPen(pen);

    // 3. 해커 스타일 폰트 적용 (Consolas, 굵게, 자간 넓게)
    text = new QGraphicsTextItem(title, this);
    QFont font("Consolas", 15, QFont::Bold);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
    text->setFont(font);
    text->setDefaultTextColor(QColor("#00F0FF"));

    // 4. 텍스트 중앙 정렬 계산 및 배치
    double xPosition = rect().width()/2 - text->boundingRect().width()/2;
    double yPosition = rect().height()/2 - text->boundingRect().height()/2;
    text->setPos(xPosition, yPosition);

    // 마우스 호버(Hover) 이벤트 활성화
    setAcceptHoverEvents(true);
}

void ActionButton::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit buttonPressed();
}

void ActionButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    // [마우스 올렸을 때] 형광색으로 배경을 채우고, 글자는 어두운 배경색으로 반전
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor("#00F0FF"));
    setBrush(brush);

    if (text) {
        text->setDefaultTextColor(QColor("#0A0F16")); // 딥 블랙
    }
}

void ActionButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    // [마우스 뗐을 때] 다시 투명 배경과 형광색 글자로 복구
    QBrush brush;
    brush.setStyle(Qt::NoBrush);
    setBrush(brush);

    if (text) {
        text->setDefaultTextColor(QColor("#00F0FF")); // 네온 시안
    }
}