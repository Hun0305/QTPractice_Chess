#include "gameview.h"
#include "loginwindow.h"
#include <QApplication>

GameView *game; // 전역 변수 유지

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    LoginWindow login;
    if (login.exec() == QDialog::Accepted) {
        QString userId = login.getLoggedInId(); // 로그인창에서 ID 가져오기

        GameView *gameView = new GameView();

        // [수정된 부분] public 함수인 setLoggedInUser를 통해 ID를 전달합니다.
        gameView->setLoggedInUser(userId);

        gameView->displayMainMenu();
        gameView->show();

        return a.exec();
    }

    return 0;
}