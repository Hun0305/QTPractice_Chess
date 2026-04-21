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

// 2. 패킷 전송 로직 수정 (문제 2 해결)
void GameView::handleSelectingPointForActivePawnByMouse(QPoint point) {
    if (!gameStarted || boardViewModel.getWhosTurn() != myColor) return;
    if (boardViewModel.getActivePawn() == nullptr) return;

    if (!boardViewModel.validatePawnPalcementForMousePosition(point)) return;
    BoardPosition toPosition = boardViewModel.getBoardPositionForMousePosition(point);
    if (!boardViewModel.validatePawnMove(toPosition)) return;

    // [중요] 이동 전의 원래 위치를 '가장 먼저' 저장해야 합니다!
    BoardPosition fromPosition = boardViewModel.getActivePawn()->position;

    // 킹 체크 로직
    bool isKingInCheck = boardViewModel.isKingInCheck(boardViewModel.getActivePawn()->owner, true, toPosition);
    board->setPawnMoveCheckWarning(isKingInCheck);
    if (isKingInCheck) return;

    // 상대 기물 제거 로직
    if (boardViewModel.didRemoveEnemyOnBoardPosition(toPosition)) {
        board->removePawnAtBoardPosition(toPosition);
    }

    // [수정] 여기서 한 번만 이동을 실행합니다.
    moveActivePawnToSelectedPoint(point);

    // 승급 체크
    if (boardViewModel.didPromoteActivePawn()) {
        board->promotePawnAtBoardPosition(toPosition);
    }

    if (networkManager) {
        QString movePacket = QString("MOVE|%1|%2|%3|%4")
        .arg(fromPosition.x).arg(fromPosition.y)
            .arg(toPosition.x).arg(toPosition.y);
        networkManager->sendMove(movePacket);
    } else {
        qDebug() << "Critical Error: networkManager is null on Host!";
    }

    // 마무리 로직
    boardViewModel.switchRound();
    boardViewModel.discardActivePawn();

    blackPlayerView->setActive(boardViewModel.getWhosTurn() == PlayerType::black);
    whitePlayerView->setActive(boardViewModel.getWhosTurn() == PlayerType::white);
}

void GameView::onDataReceived(QString data) {
    QStringList messages = data.split(";", Qt::SkipEmptyParts);

    for (const QString& singleMove : messages) {
        QStringList parts = singleMove.split("|");
        if (parts.size() < 5 || parts[0] != "MOVE") continue;

        int fx = parts[1].toInt();
        int fy = parts[2].toInt();
        int tx = parts[3].toInt();
        int ty = parts[4].toInt();

        BoardPosition from(fx, fy);
        BoardPosition to(tx, ty);

        PawnField* remotePawn = board->getPawnAtBoardPosition(from);
        if (remotePawn) {
            // 1. 모델에서 현재 움직이는 말을 활성화
            boardViewModel.setActivePawnForField(remotePawn);

            // 2. [추가] 목적지에 적이 있다면 제거 로직 실행 (승리 조건 체크 포함)
            if (boardViewModel.didRemoveEnemyOnBoardPosition(to)) {
                board->removePawnAtBoardPosition(to);
            }

            // 3. 화면 이동 및 모델 좌표 갱신
            board->placeActivePawnAtBoardPosition(boardViewModel.getActivePawn(), to);
            boardViewModel.setNewPositionForActivePawn(to);

            // 4. [추가] 승리자가 있는지 확인 (킹이 잡혔는지 체크)
            if (boardViewModel.getWinner()) {
                showCongratulationsScreen(*boardViewModel.getWinner());
                return; // 게임 종료 시 함수 탈출
            }

            // 5. 턴 교체 및 마무리
            boardViewModel.discardActivePawn();
            boardViewModel.switchRound();

            blackPlayerView->setActive(boardViewModel.getWhosTurn() == PlayerType::black);
            whitePlayerView->setActive(boardViewModel.getWhosTurn() == PlayerType::white);
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
