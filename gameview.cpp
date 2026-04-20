#include "gameview.h"
#include <QGraphicsTextItem>
#include <QColor>
#include <QBrush>
#include "actionbutton.h"
#include "congratulationsview.h"
#include "constants.h"
#include "utils.h"

int viewWidth = 1200;
int viewHeight= 768;

GameView::GameView() {
    gameStarted = false;
    networkManager = nullptr; // 추가

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(viewWidth, viewHeight);

    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, viewWidth, viewHeight);
    setScene(scene);

    QBrush brush;
    brush.setStyle((Qt::SolidPattern));
    QColor color = QColor(44, 41, 51);
    brush.setColor(color);
    scene->setBackgroundBrush(brush);

    gameStarted = false;
}

void GameView::displayMainMenu() {
    scene->clear();

    // create title label
    double titleYPosition = 150;
    drawTitle(titleYPosition, 50);

    // create start button
    ActionButton *startButton = new ActionButton("Play");
    double buttonXPosition = this->width()/2 - startButton->boundingRect().width()/2;
    double buttonYPosition = 275;
    startButton->setPos(buttonXPosition, buttonYPosition);

    connect(startButton, SIGNAL(buttonPressed()), this, SLOT(displayRoomList()));
    scene->addItem(startButton);

    // create quit button
    ActionButton *quitButton = new ActionButton("Quit");
    double quitXPosition = this->width()/2 - quitButton->boundingRect().width()/2;
    double quitYPosition = 350;
    quitButton->setPos(quitXPosition, quitYPosition);

    connect(quitButton, SIGNAL(buttonPressed()), this, SLOT(quitGame()));
    scene->addItem(quitButton);
}

void GameView::displayRoomList() {
    scene->clear(); // 현재 화면 지우기

    // 1. 제목 그리기
    drawTitle(50, 40);

    // 2. 테이블 헤더 설정 (간격 조절)
    int startY = 150;
    int col1 = 200, col3 = 700, col4 = 900; // X 좌표 간격

    QGraphicsTextItem *h1 = Utils::createTextItem("Game Name", 20, Qt::white);
    h1->setPos(col1, startY);
    scene->addItem(h1);

    QGraphicsTextItem *h2 = Utils::createTextItem("Players", 20, Qt::white);
    h2->setPos(col3, startY);
    scene->addItem(h2);

    QGraphicsTextItem *h3 = Utils::createTextItem("Action", 20, Qt::white);
    h3->setPos(col4, startY);
    scene->addItem(h3);

    // 3. 더미 방 목록 데이터 루프 (예시 3개)
    struct Room { QString name; QString players; };
    QList<Room> rooms = { {"Software Chess", "1/2"}, {"PVP Challenge", "1/2"}, {"Wait for King", "0/2"} };

    for(int i = 0; i < rooms.size(); ++i) {
        int rowY = startY + 60 + (i * 70); // 행 간격

        // 방 이름
        QGraphicsTextItem *rName = Utils::createTextItem(rooms[i].name, 18, Qt::lightGray);
        rName->setPos(col1, rowY + 10);
        scene->addItem(rName);

        // 인원 수
        QGraphicsTextItem *rPlayers = Utils::createTextItem(rooms[i].players, 18, Qt::lightGray);
        rPlayers->setPos(col3, rowY + 10);
        scene->addItem(rPlayers);

        // Join 버튼 (기존 ActionButton 재활용)
        ActionButton *joinBtn = new ActionButton("Join");
        joinBtn->setPos(col4, rowY);
        joinBtn->setScale(0.7); // 리스트용으로 조금 작게 조절
        // 수정된 코드
        connect(joinBtn, &ActionButton::buttonPressed, this, [this](){
            myColor = PlayerType::black;

            // 1. 인스턴스 생성 (누락되었던 부분)
            if (!networkManager) {
                networkManager = new NetworkManager(this);
            }

            // 2. 시그널 연결을 먼저 수행하는 것이 안전합니다.
            connect(networkManager, &NetworkManager::dataReceived, this, &GameView::onDataReceived);
            connect(networkManager, &NetworkManager::connected, this, &GameView::startGame);

            // 3. 서버 접속 시도
            networkManager->connectToHost("127.0.0.1", 12345);
        });
        scene->addItem(joinBtn);
    }

    // 4. 뒤로가기 버튼
    ActionButton *hostButton = new ActionButton("Host Game");
    // 위치를 "Back to Menu" 버튼 옆으로 잡습니다.
    hostButton->setPos(this->width()/2 - hostButton->boundingRect().width() - 20, 650);
    connect(hostButton, SIGNAL(buttonPressed()), this, SLOT(hostGame()));
    scene->addItem(hostButton);

    // --- 기존: Back to Menu 버튼 (위치 살짝 조정) ---
    ActionButton *backButton = new ActionButton("Back to Menu");
    backButton->setPos(this->width()/2 + 20, 650);
    connect(backButton, SIGNAL(buttonPressed()), this, SLOT(displayMainMenu()));
    scene->addItem(backButton);
}

// 방 만들기 버튼 클릭 시 실행될 함수
void GameView::hostGame() {
    myColor = PlayerType::white; // 방장은 흰색
    networkManager = new NetworkManager(this);
    if (networkManager->startHosting()) {
        qDebug() << "Server started! Waiting for player...";
        // 연결될 때까지 대기 메시지 표시 (UI 작업)
        connect(networkManager, &NetworkManager::connected, this, [this](){
            startGame(); // 상대방 접속 시 게임 시작
        });
    }
    connect(networkManager, &NetworkManager::dataReceived, this, &GameView::onDataReceived);
}

void GameView::startGame() {

    scene->clear();

    boardViewModel = BoardViewModel();

    drawBoard();
    drawSettingsPanel();
    drawUserPanel();
    int titleYPosition = Constants::defaultMargin;
    drawTitle(titleYPosition, 40);
    gameStarted = true;
}

void GameView::quitGame() {
    close();
}

void GameView::resetGame() {
    gameStarted = false;
    scene->clear();
    startGame();
}

void GameView::drawBoard() {
    board = new BoardView();
    board->draw();
    board->initializePawnFields(boardViewModel.getBlackPawns());
    board->initializePawnFields(boardViewModel.getWhitePawns());
}

void GameView::drawSettingsPanel() {
    // create quit button
    ActionButton *resetButton = new ActionButton("Reset game");
    double resetXPosition = 690 + resetButton->boundingRect().width()/2;
    double resetYPosition = 420;
    resetButton->setPos(resetXPosition, resetYPosition);

    connect(resetButton, SIGNAL(buttonPressed()), this, SLOT(resetGame()));
    scene->addItem(resetButton);

    // create quit button
    ActionButton *quitButton = new ActionButton("Quit game");
    double quitXPosition = 690 + quitButton->boundingRect().width()/2;
    double quitYPosition = 490;
    quitButton->setPos(quitXPosition, quitYPosition);

    connect(quitButton, SIGNAL(buttonPressed()), this, SLOT(quitGame()));
    scene->addItem(quitButton);
}

void GameView::drawUserPanel() {
    blackPlayerView = drawViewForUser(PlayerType::black);
    whitePlayerView = drawViewForUser(PlayerType::white);

    blackPlayerView->setActive(true);
}

PlayerView* GameView::drawViewForUser(PlayerType player) {
    PlayerView *playerView = new PlayerView();

    int xPosition = 80;
    int yPosition = BoardView::startYPosition;

    switch (player) {
    case PlayerType::black:
        xPosition = 680;
        break;
    case PlayerType::white:
        xPosition = 680 + PlayerView::defaultWidthHeight + 20;
        break;
    }

    scene->addItem(playerView);
    playerView->setRect(xPosition, yPosition, PlayerView::defaultWidthHeight, PlayerView::defaultWidthHeight);
    playerView->setPlayer(player);

    return playerView;
}

void GameView::drawTitle(double yPosition, int fontSize) {
    QGraphicsTextItem *title = Utils::createTextItem("Chess Game", fontSize, Qt::white);
    double xPosition = this->width()/2 - title->boundingRect().width()/2;
    title->setPos(xPosition, yPosition);
    scene->addItem(title);
}

void GameView::mousePressEvent(QMouseEvent *event) {
    if (!gameStarted) {
        QGraphicsView::mousePressEvent(event);
        return;
    } else if (event->button() == Qt::RightButton) {
        releaseActivePawn();
    } else if (boardViewModel.getActivePawn()) {
        handleSelectingPointForActivePawnByMouse(event->pos());
    } else {
        PawnField *pawn = board->getPawnAtMousePosition(event->pos());

        if (pawn != nullptr) {
            // 1. 클릭한 위치의 좌표를 가져옵니다.
            BoardPosition pos = boardViewModel.getBoardPositionForMousePosition(event->pos());

            // 2. 해당 좌표에 있는 말의 주인이 내 색깔(myColor)과 같은지 확인합니다.
            // boardViewModel의 기능을 활용하여 직접 비교합니다.
            if (boardViewModel.getPawnColorAtPosition(pos) != myColor) {
                return; // 내 말이 아니면 무시
            }
        }
        selectPawn(pawn);
    }

    QGraphicsView::mousePressEvent(event);
}

void GameView::mouseMoveEvent(QMouseEvent *event) {
    // if there is a pawn selected, then make it follow the mouse
    if (gameStarted && boardViewModel.getActivePawn()) {
        board->moveActivePawnToMousePosition(event->pos(), boardViewModel.getActivePawn());
    }

    QGraphicsView::mouseMoveEvent(event);
}


void GameView::selectPawn(PawnField *pawn) {
    if (pawn == nullptr) {
        return;
    }

    boardViewModel.setActivePawnForField(pawn);
}

void GameView::handleSelectingPointForActivePawnByMouse(QPoint point) {
    if (!gameStarted || boardViewModel.getWhosTurn() != myColor) return; // 내 턴이 아니면 무시

    if (boardViewModel.getActivePawn() == nullptr) {
        return;
    }

    // check if mouse selected place on board
    if (!boardViewModel.validatePawnPalcementForMousePosition(point)) {
        return;
    }

    BoardPosition boardPosition = boardViewModel.getBoardPositionForMousePosition(point);

    // first validate Move
    if (!boardViewModel.validatePawnMove(boardPosition)) {
        return;
    }

    // Players cannot make any move that places their own king in check
    bool isKingInCheck = boardViewModel.isKingInCheck(boardViewModel.getActivePawn()->owner, true, boardPosition);
    board->setPawnMoveCheckWarning(isKingInCheck);
    if (isKingInCheck) {
        return;
    }
    // check if field was taken by opposite player and remove it from the board
    if (boardViewModel.didRemoveEnemyOnBoardPosition(boardPosition)) {
        board->removePawnAtBoardPosition(boardPosition);
    }

    // move active pawn to new position
    moveActivePawnToSelectedPoint(point);

    // check if pawn can be promoted
    if (boardViewModel.didPromoteActivePawn()) {
        board->promotePawnAtBoardPosition(boardPosition);
    }

    // check for opposite player king's check
    switch (boardViewModel.getActivePawn()->owner) {
    case PlayerType::black:
        setCheckStateOnPlayerView(PlayerType::white, boardViewModel.isKingInCheck(PlayerType::white, false, boardPosition));
        break;
    case PlayerType::white:
        setCheckStateOnPlayerView(PlayerType::black, boardViewModel.isKingInCheck(PlayerType::black, false, boardPosition));
        break;
    }

    // update active player check state
    setCheckStateOnPlayerView(boardViewModel.getActivePawn()->owner, isKingInCheck);

    // check if game is over
    if (boardViewModel.getWinner()) {
        showCongratulationsScreen(*boardViewModel.getWinner());
        return;
    }

    // 1. 이동 전의 원래 위치 저장 (상대에게 보낼 데이터)
    BoardPosition from = boardViewModel.getActivePawn()->position;

    // 2. 로컬에서 말 이동 실행
    moveActivePawnToSelectedPoint(point);

    // 3. 상대방에게 전송 (이동 직후 바로 전송)
    if (networkManager) {
        QString movePacket = QString("MOVE|%1|%2|%3|%4")
        .arg(from.x).arg(from.y).arg(boardPosition.x).arg(boardPosition.y);
        networkManager->sendMove(movePacket);
    }

    // 4. 턴 교체 및 마무리
    boardViewModel.discardActivePawn();
    boardViewModel.switchRound();

    blackPlayerView->setActive(boardViewModel.getWhosTurn() == PlayerType::black);
    whitePlayerView->setActive(boardViewModel.getWhosTurn() == PlayerType::white);
}

void GameView::onDataReceived(QString data) {
    QStringList parts = data.split("|");
    if (parts[0] == "MOVE") {
        int fx = parts[1].toInt();
        int fy = parts[2].toInt();
        int tx = parts[3].toInt();
        int ty = parts[4].toInt();

        BoardPosition from(fx, fy);
        BoardPosition to(tx, ty);

        // 내 화면에 있는 전체 말(PawnField) 중에서 해당 좌표에 있는 말을 찾음
        PawnField* remotePawn = board->getPawnAtBoardPosition(from);

        if(remotePawn) {
            // 1. 모델에도 이 말을 선택한 것으로 설정
            boardViewModel.setActivePawnForField(remotePawn);

            // 2. 화면 이동 (강제 좌표 이동)
            board->placeActivePawnAtBoardPosition(boardViewModel.getActivePawn(), to);

            // 3. 모델 데이터 갱신 (Pawn의 x, y 좌표값 변경)
            boardViewModel.setNewPositionForActivePawn(to);

            // 4. 턴 교체
            boardViewModel.discardActivePawn();
            boardViewModel.switchRound();

            // 5. UI 업데이트
            blackPlayerView->setActive(boardViewModel.getWhosTurn() == PlayerType::black);
            whitePlayerView->setActive(boardViewModel.getWhosTurn() == PlayerType::white);
        } else {
            qDebug() << "Error: Could not find piece at" << fx << fy;
        }
    }
}

void GameView::setCheckStateOnPlayerView(PlayerType player, bool isInCheck) {
    switch (player) {
    case PlayerType::black:
        blackPlayerView->setIsInCheck(isInCheck);
        break;
    case PlayerType::white:
        whitePlayerView->setIsInCheck(isInCheck);
        break;
    }
}

// update pawn field position and pawn model position
void GameView::moveActivePawnToSelectedPoint(QPoint point) {
    BoardPosition boardPosition = boardViewModel.getBoardPositionForMousePosition(point);
    board->placeActivePawnAtBoardPosition(boardViewModel.getActivePawn(), boardPosition);
    boardViewModel.setNewPositionForActivePawn(boardPosition);
}

void GameView::releaseActivePawn() {
    if (boardViewModel.getActivePawn() == nullptr) {
        return;
    }

    BasePawnModel *activePawn = boardViewModel.getActivePawn();
    board->placeActivePawnAtBoardPosition(activePawn, activePawn->position);
    board->setPawnMoveCheckWarning(false);
    boardViewModel.discardActivePawn();
}

void GameView::showCongratulationsScreen(PlayerType winner) {
    gameStarted = false;

    scene->clear();

    CongratulationsView *congratulationsView = new CongratulationsView(winner);
    congratulationsView->setRect(0, 0, viewWidth, viewHeight);
}
