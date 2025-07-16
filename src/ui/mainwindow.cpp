#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_videoClient(new VideoClient(this))
{
    setupUI();
    setupConnections();
    initializeFilters();
    
    setWindowTitle("Factory Video Client");
    resize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    // 중앙 위젯 설정
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    
    // 상단 검색 필터 UI 설정
    setupFilterUI();
    
    // 비디오 목록 UI 설정
    setupVideoListUI();
    
    // 상태 표시 UI 설정
    setupStatusUI();
}

void MainWindow::setupFilterUI() {
    m_topLayout = new QHBoxLayout;
    
    // 디바이스 선택
    m_deviceCombo = new QComboBox;
    m_deviceCombo->addItems({"All Devices", "feeder_01", "conveyor_01"});
    m_deviceCombo->setToolTip("비디오를 검색할 디바이스를 선택하세요");
    
    // 에러 ID 입력
    m_errorIdEdit = new QLineEdit;
    m_errorIdEdit->setPlaceholderText("Error Log ID (optional)");
    m_errorIdEdit->setToolTip("특정 에러 로그 ID로 필터링 (선택사항)");
    
    // 시간 범위 선택
    m_startTimeEdit = new QDateTimeEdit;
    m_startTimeEdit->setCalendarPopup(true);
    m_startTimeEdit->setToolTip("검색 시작 시간");
    
    m_endTimeEdit = new QDateTimeEdit;
    m_endTimeEdit->setCalendarPopup(true);
    m_endTimeEdit->setToolTip("검색 종료 시간");
    
    // 새로고침 버튼
    m_refreshBtn = new QPushButton("Refresh");
    m_refreshBtn->setToolTip("설정된 조건으로 비디오 목록을 새로고침합니다");
    
    // 레이아웃 구성
    m_topLayout->addWidget(new QLabel("Device:"));
    m_topLayout->addWidget(m_deviceCombo);
    m_topLayout->addWidget(new QLabel("Error ID:"));
    m_topLayout->addWidget(m_errorIdEdit);
    m_topLayout->addWidget(new QLabel("From:"));
    m_topLayout->addWidget(m_startTimeEdit);
    m_topLayout->addWidget(new QLabel("To:"));
    m_topLayout->addWidget(m_endTimeEdit);
    m_topLayout->addWidget(m_refreshBtn);
    m_topLayout->addStretch();
    
    m_mainLayout->addLayout(m_topLayout);
}

void MainWindow::setupVideoListUI() {
    m_bottomLayout = new QHBoxLayout;
    
    m_videoList = new QListWidget;
    m_videoList->setToolTip("비디오 목록 - 더블클릭하면 새 창에서 재생됩니다");
    m_videoList->setAlternatingRowColors(true);
    
    m_bottomLayout->addWidget(m_videoList);
    m_mainLayout->addLayout(m_bottomLayout);
}

void MainWindow::setupStatusUI() {
    QHBoxLayout* statusLayout = new QHBoxLayout;
    
    m_progressBar = new QProgressBar;
    m_progressBar->setVisible(false); // 초기에는 숨김
    
    m_statusLabel = new QLabel("Ready");
    m_statusLabel->setStyleSheet("QLabel { color: #666; font-size: 12px; }");
    
    statusLayout->addWidget(m_progressBar);
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    
    m_mainLayout->addLayout(statusLayout);
}

void MainWindow::setupConnections() {
    // 버튼 클릭 연결
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    
    // 비디오 목록 연결
    connect(m_videoList, &QListWidget::itemSelectionChanged, this, &MainWindow::onVideoSelected);
    connect(m_videoList, &QListWidget::itemDoubleClicked, this, &MainWindow::onVideoDoubleClicked);
    
    // Enter 키로도 검색 가능
    connect(m_errorIdEdit, &QLineEdit::returnPressed, this, &MainWindow::onRefreshClicked);
}

void MainWindow::initializeFilters() {
    // 기본 시간 범위: 최근 7일
    QDateTime now = QDateTime::currentDateTime();
    m_startTimeEdit->setDateTime(now.addDays(-DEFAULT_SEARCH_DAYS));
    m_endTimeEdit->setDateTime(now);
    
    // 날짜 형식 설정
    m_startTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
    m_endTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
}

void MainWindow::onRefreshClicked() {
    // 입력 값 검증
    if (m_startTimeEdit->dateTime() >= m_endTimeEdit->dateTime()) {
        QMessageBox::warning(this, "Invalid Time Range", 
                           "시작 시간이 종료 시간보다 늦을 수 없습니다.");
        return;
    }
    
    // UI 상태 업데이트
    m_statusLabel->setText("Querying videos...");
    m_videoList->clear();
    m_refreshBtn->setEnabled(false);
    
    // 검색 매개변수 준비
    QString device = m_deviceCombo->currentText();
    if (device == "All Devices") {
        device = ""; // 빈 문자열은 모든 디바이스를 의미
    }
    
    QString errorId = m_errorIdEdit->text().trimmed();
    qint64 startTime = m_startTimeEdit->dateTime().toMSecsSinceEpoch();
    qint64 endTime = m_endTimeEdit->dateTime().toMSecsSinceEpoch();
    
    // 비디오 목록 요청
    m_videoClient->queryVideos(device, errorId, startTime, endTime, MAX_VIDEO_RESULTS,
        [this](const QList<VideoInfo>& videos) {
            // 성공 콜백
            populateVideoList(videos);
            m_statusLabel->setText(QString("Found %1 videos").arg(videos.size()));
            m_refreshBtn->setEnabled(true);
        });
}

void MainWindow::populateVideoList(const QList<VideoInfo>& videos) {
    m_videoList->clear();
    
    for (const auto& video : videos) {
        QString itemText = QString("[%1] %2 - %3 (%4)")
            .arg(video.device_id)
            .arg(video.error_log_id)
            .arg(VideoClient::formatFileSize(video.file_size))
            .arg(VideoClient::formatDuration(video.video_duration));
        
        QListWidgetItem* item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, video.http_url);
        
        // 디버그: HTTP URL 출력
        qDebug() << "[DEBUG] Storing HTTP URL:" << video.http_url;
        qDebug() << "[DEBUG] Video ID:" << video.video_id;
        qDebug() << "[DEBUG] Device ID:" << video.device_id;
        qDebug() << "[DEBUG] File Path:" << video.file_path;
        
        item->setToolTip(QString("비디오 URL: %1\n파일 크기: %2\n재생 시간: %3")
                        .arg(video.http_url)
                        .arg(VideoClient::formatFileSize(video.file_size))
                        .arg(VideoClient::formatDuration(video.video_duration)));
        
        m_videoList->addItem(item);
    }
}

void MainWindow::onVideoSelected() {
    // Nothing to do for now
}

void MainWindow::onVideoDoubleClicked() {
    QListWidgetItem* currentItem = m_videoList->currentItem();
    if (!currentItem) {
        return;
    }
    
    QString httpUrl = currentItem->data(Qt::UserRole).toString();
    
    // 디버그: 더블클릭시 HTTP URL 출력
    qDebug() << "[DEBUG] Double-clicked video HTTP URL:" << httpUrl;
    qDebug() << "[DEBUG] Item text:" << currentItem->text();
    
    if (httpUrl.isEmpty()) {
        QMessageBox::warning(this, "Invalid Video", "비디오 URL이 유효하지 않습니다.");
        return;
    }
    
    // 다운로드 진행률 표시 시작
    m_progressBar->setVisible(true);
    m_statusLabel->setText("Downloading video...");
    
    m_videoClient->downloadVideo(httpUrl,
        [this, httpUrl](bool success, const QString& localPath) {
            // 다운로드 진행률 숨김
            m_progressBar->setVisible(false);
            
            if (success) {
                // 비디오 플레이어 생성 및 표시
                VideoPlayer* player = new VideoPlayer(localPath);
                
                // 창 닫힘 시그널 연결
                connect(player, &VideoPlayer::destroyed, this, &MainWindow::onVideoPlayerClosed);
                
                player->show();
                m_videoPlayers.append(player);
                m_statusLabel->setText(QString("Video opened in new window (%1 players active)")
                                     .arg(m_videoPlayers.size()));
            } else {
                m_statusLabel->setText("Download failed");
                QMessageBox::critical(this, "Download Error", 
                                    QString("비디오 다운로드에 실패했습니다.\nURL: %1")
                                    .arg(httpUrl));
            }
        }, m_progressBar, m_statusLabel);
}

void MainWindow::onVideoPlayerClosed() {
    // 닫힌 VideoPlayer를 리스트에서 제거
    VideoPlayer* closedPlayer = qobject_cast<VideoPlayer*>(sender());
    if (closedPlayer) {
        m_videoPlayers.removeAll(closedPlayer);
        m_statusLabel->setText(QString("Video player closed (%1 players active)")
                             .arg(m_videoPlayers.size()));
    }
}

