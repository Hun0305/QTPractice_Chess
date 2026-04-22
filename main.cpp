#include "gameview.h"
#include "loginwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 🎨 [미래지향적 네온 사이버 테마]
    QString styleSheet =
        "QWidget { background-color: #0A0F16; color: #00F0FF; font-family: 'Consolas', 'Segoe UI', sans-serif; font-size: 15px; }"
        "QPushButton { "
        "   background-color: transparent; border: 2px solid #00F0FF; border-radius: 4px; "
        "   font-weight: bold; padding: 10px 15px; color: #00F0FF; text-transform: uppercase; letter-spacing: 2px; "
        "}"
        "QPushButton:hover { background-color: #00F0FF; color: #0A0F16; }"
        "QLineEdit, QComboBox { "
        "   background-color: rgba(0, 240, 255, 0.05); border: 1px solid #1A2639; "
        "   border-bottom: 2px solid #00F0FF; border-radius: 2px; "
        "   padding: 8px; color: #FFFFFF; font-weight: bold; min-height: 30px; " // 높이 보강
        "}"
        "QLineEdit:focus, QComboBox:focus { border: 1px solid #00F0FF; background-color: rgba(0, 240, 255, 0.1); }"
        "QTableWidget { background-color: #0B121A; border: 1px solid #00F0FF; color: #FFFFFF; gridline-color: #00F0FF; }"
        "QHeaderView::section { background-color: #004D55; color: #00F0FF; font-weight: bold; padding: 8px; border: 1px solid #00F0FF; }"
        "QLabel { background: transparent; }";
    a.setStyleSheet(styleSheet);

    LoginWindow login;
    if (login.exec() == QDialog::Accepted) {
        QString userId = login.getLoggedInId();
        GameView *gameView = new GameView();
        gameView->setLoggedInUser(userId);
        gameView->displayMainMenu();
        gameView->show();
        return a.exec();
    }
    return 0;
}