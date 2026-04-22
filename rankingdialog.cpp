#include "rankingdialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QHeaderView>

RankingDialog::RankingDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("체스 월드 랭킹");
    setFixedSize(500, 600);
    if (parent) this->setStyleSheet(parent->styleSheet());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- 상단 검색바 영역 ---
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("검색할 아이디 입력...");
    QPushButton *searchBtn = new QPushButton("검색", this);
    connect(searchBtn, &QPushButton::clicked, this, &RankingDialog::handleSearch);

    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchBtn);
    mainLayout->addLayout(searchLayout);

    // --- 중앙 랭킹 테이블 (자동 스크롤 포함) ---
    rankingTable = new QTableWidget(this);
    rankingTable->setColumnCount(4);
    rankingTable->setHorizontalHeaderLabels({"순위", "아이디", "전적", "승률"});
    rankingTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // 수정 불가
    rankingTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 꽉 채우기

    mainLayout->addWidget(rankingTable);

    loadRankingData(); // 창이 열릴 때 데이터 로드
}

void RankingDialog::loadRankingData() {
    rankingTable->setRowCount(0);

    QSqlQuery query;
    // [완벽한 랭킹 정렬 로직]
    // 1순위 (win_rate) : 승률 내림차순. 단, 한 판도 안 한(total=0) 유저는 승률을 -1.0으로 처리해 무조건 꼴찌로 보냅니다.
    // 2순위 (wins)     : 승률이 같다면 승수가 높은 사람이 위로 갑니다. (예: 2승 0패 > 1승 0패)
    // 3순위 (losses)   : 승률과 승수가 같다면 패수가 적은 사람이 위로 갑니다.
    // 4순위 (id)       : 위 조건이 모두 같다면 아이디 알파벳 순으로 정렬합니다.
    QString sql = "SELECT id, wins, losses, (wins + losses) as total, "
                  "CASE WHEN (wins + losses) = 0 THEN -1.0 "
                  "ELSE (CAST(wins AS FLOAT) / (wins + losses)) END as win_rate "
                  "FROM users "
                  "ORDER BY win_rate DESC, wins DESC, losses ASC, id ASC";

    if (query.exec(sql)) {
        int rank = 1;

        while (query.next()) {
            int row = rankingTable->rowCount();
            rankingTable->insertRow(row);

            QString id = query.value("id").toString();
            int wins = query.value("wins").toInt();
            int losses = query.value("losses").toInt();
            int total = query.value("total").toInt();

            QString rankStr;
            QString winRateStr;

            // 순위 및 승률 계산 분기
            if (total == 0) {
                // 한 판도 하지 않은 유저
                rankStr = "-";
                winRateStr = "-";
            } else {
                // 전적이 있는 유저는 순위를 매기고 순위 숫자를 1 증가시킴
                rankStr = QString::number(rank++);
                winRateStr = QString::number((wins * 100) / total) + "%";
            }

            QString recordStr = QString("%1승 %2패").arg(wins).arg(losses);

            rankingTable->setItem(row, 0, new QTableWidgetItem(rankStr));
            rankingTable->setItem(row, 1, new QTableWidgetItem(id));
            rankingTable->setItem(row, 2, new QTableWidgetItem(recordStr));
            rankingTable->setItem(row, 3, new QTableWidgetItem(winRateStr));
        }
    } else {
        qDebug() << "랭킹 데이터를 불러오는데 실패했습니다.";
    }
}

void RankingDialog::handleSearch() {
    QString targetId = searchEdit->text().trimmed();
    if (targetId.isEmpty()) return;

    bool found = false;
    for (int i = 0; i < rankingTable->rowCount(); ++i) {
        if (rankingTable->item(i, 1)->text() == targetId) {
            QString rank = rankingTable->item(i, 0)->text();
            QString record = rankingTable->item(i, 2)->text();
            QString winRate = rankingTable->item(i, 3)->text();

            QString info = QString("순위: %1\n아이디: %2\n전적: %3\n승률: %4")
                               .arg(rank).arg(targetId).arg(record).arg(winRate);

            QMessageBox::information(this, "검색 결과", info);
            found = true;
            break;
        }
    }

    if (!found) {
        QMessageBox::warning(this, "결과 없음", "해당 아이디를 찾을 수 없습니다.");
    }
}