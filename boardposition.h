#ifndef BOARDPOSITION_H
#define BOARDPOSITION_H

struct BoardPosition {
    int x;
    int y;

    // 1. 기본 생성자 (기존 코드 호환성을 위해 필요)
    BoardPosition() : x(0), y(0) {}

    // 2. 오버로딩된 생성자 (int& 대신 int 사용)
    // 이렇게 해야 계산된 값(R-value)도 받아낼 수 있습니다.
    BoardPosition(int _x, int _y) : x(_x), y(_y) {}

    // 3. 비교 연산자 (필요하다면 추가해두면 알고리즘 짜기 편합니다)
    bool operator==(const BoardPosition& other) const {
        return x == other.x && y == other.y;
    }
};

#endif // BOARDPOSITION_H
