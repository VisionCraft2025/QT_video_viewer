#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_videoClient(new VideoClient(this))
    , m_mediaPlayer(new QMediaPlayer(this))
    , m_hideControlsTimer(new QTimer(this))
{
    setupUI();
    setupConnections();
    setWindowTitle("Factory Video Client");
    resize(1200, 800);
    
    m_hideControlsTimer->setSingleShot(true);
    m_hideControlsTimer->setInterval(3000); // 3초 후 숨김
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    
    // Top controls
    m_topLayout = new QHBoxLayout;
    
    m_deviceCombo = new QComboBox;
    m_deviceCombo->addItems({"All Devices", "feeder_01", "conveyor_01"});
    
    m_errorIdEdit = new QLineEdit;
    m_errorIdEdit->setPlaceholderText("Error Log ID (optional)");
    
    m_startTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime().addDays(-7));
    m_endTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    
    m_refreshBtn = new QPushButton("Refresh");
    
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
    
    // Bottom layout
    m_bottomLayout = new QHBoxLayout;
    
    // Video list
    m_videoList = new QListWidget;
    m_videoList->setMaximumWidth(400);
    
    // Video player
    QVBoxLayout* playerLayout = new QVBoxLayout;
    
    // Video widget with overlay controls
    m_videoContainer = new QWidget;
    m_videoContainer->setMinimumSize(640, 480);
    m_videoContainer->installEventFilter(this);
    
    QStackedLayout* stackedLayout = new QStackedLayout(m_videoContainer);
    stackedLayout->setStackingMode(QStackedLayout::StackAll);
    
    m_videoWidget = new QVideoWidget;
    stackedLayout->addWidget(m_videoWidget);
    
    // Overlay widget for controls
    QWidget* overlayWidget = new QWidget;
    overlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    stackedLayout->addWidget(overlayWidget);
    
    // Semi-transparent video controls (initially hidden)
    m_videoControlsWidget = new QWidget(m_videoContainer);
    m_videoControlsWidget->setStyleSheet(
        "QWidget { background-color: rgba(0, 0, 0, 150); border-radius: 8px; }"
        "QPushButton { background-color: rgba(255, 255, 255, 200); border: none; border-radius: 4px; padding: 8px; font-size: 14px; }"
        "QPushButton:hover { background-color: rgba(255, 255, 255, 255); }"
        "QSlider::groove:horizontal { border: 1px solid #999; height: 6px; background: rgba(255, 255, 255, 100); border-radius: 3px; }"
        "QSlider::handle:horizontal { background: white; border: 2px solid #5c5c5c; width: 16px; margin: -5px 0; border-radius: 8px; }"
        "QSlider::handle:horizontal:hover { background: #f0f0f0; }"
    );
    
    QHBoxLayout* controlsLayout = new QHBoxLayout(m_videoControlsWidget);
    controlsLayout->setContentsMargins(15, 8, 15, 8);
    controlsLayout->setSpacing(10);
    
    m_playPauseBtn = new QPushButton("▶");
    m_playPauseBtn->setFixedSize(45, 35);
    
    m_backwardBtn = new QPushButton("⏪");
    m_backwardBtn->setFixedSize(45, 35);
    
    m_forwardBtn = new QPushButton("⏩");
    m_forwardBtn->setFixedSize(45, 35);
    
    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setMinimum(0);
    m_positionSlider->setMinimumWidth(200);
    
    m_timeLabel = new QLabel("00:00 / 00:00");
    m_timeLabel->setStyleSheet("color: white; font-weight: bold; font-size: 12px;");
    m_timeLabel->setMinimumWidth(80);
    
    controlsLayout->addWidget(m_playPauseBtn);
    controlsLayout->addWidget(m_backwardBtn);
    controlsLayout->addWidget(m_forwardBtn);
    controlsLayout->addWidget(m_positionSlider);
    controlsLayout->addWidget(m_timeLabel);
    
    m_videoControlsWidget->setFixedHeight(55);
    m_videoControlsWidget->hide(); // Initially hidden
    
    // Set initial position when container is ready
    QTimer::singleShot(100, this, [this]() {
        if (m_videoContainer && m_videoControlsWidget) {
            int x = (m_videoContainer->width() - m_videoControlsWidget->width()) / 2;
            int y = m_videoContainer->height() - m_videoControlsWidget->height() - 20;
            m_videoControlsWidget->move(x, y);
        }
    });
    
    QHBoxLayout* controlLayout = new QHBoxLayout;
    m_playBtn = new QPushButton("Play Selected");
    m_progressBar = new QProgressBar;
    m_statusLabel = new QLabel("Ready");
    
    controlLayout->addWidget(m_playBtn);
    controlLayout->addWidget(m_progressBar);
    controlLayout->addWidget(m_statusLabel);
    controlLayout->addStretch();
    
    playerLayout->addWidget(m_videoContainer);
    playerLayout->addLayout(controlLayout);
    
    m_bottomLayout->addWidget(m_videoList);
    m_bottomLayout->addLayout(playerLayout);
    
    m_mainLayout->addLayout(m_bottomLayout);
}

void MainWindow::setupConnections() {
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(m_playBtn, &QPushButton::clicked, this, &MainWindow::onPlayClicked);
    connect(m_videoList, &QListWidget::itemSelectionChanged, this, &MainWindow::onVideoSelected);
    connect(m_videoList, &QListWidget::itemDoubleClicked, this, &MainWindow::onVideoDoubleClicked);
    
    // Video control connections
    connect(m_playPauseBtn, &QPushButton::clicked, this, &MainWindow::onPlayPauseClicked);
    connect(m_backwardBtn, &QPushButton::clicked, this, &MainWindow::onBackwardClicked);
    connect(m_forwardBtn, &QPushButton::clicked, this, &MainWindow::onForwardClicked);
    connect(m_positionSlider, &QSlider::sliderMoved, this, &MainWindow::onSliderMoved);
    
    // Media player connections
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    
    // Timer connection
    connect(m_hideControlsTimer, &QTimer::timeout, this, &MainWindow::hideVideoControls);
}

void MainWindow::onRefreshClicked() {
    m_statusLabel->setText("Querying videos...");
    m_videoList->clear();
    
    QString device = m_deviceCombo->currentText();
    if (device == "All Devices") device = "";
    
    QString errorId = m_errorIdEdit->text().trimmed();
    qint64 startTime = m_startTimeEdit->dateTime().toMSecsSinceEpoch();
    qint64 endTime = m_endTimeEdit->dateTime().toMSecsSinceEpoch();
    
    m_videoClient->queryVideos(device, errorId, startTime, endTime, 100,
        [this](const QList<VideoInfo>& videos) {
            m_videoList->clear();
            for (const auto& video : videos) {
                QString itemText = QString("[%1] %2 - %3 (%4)")
                    .arg(video.device_id)
                    .arg(video.error_log_id)
                    .arg(VideoClient::formatFileSize(video.file_size))
                    .arg(VideoClient::formatDuration(video.video_duration));
                
                QListWidgetItem* item = new QListWidgetItem(itemText);
                item->setData(Qt::UserRole, video.http_url);
                m_videoList->addItem(item);
            }
            m_statusLabel->setText(QString("Found %1 videos").arg(videos.size()));
        });
}

void MainWindow::onPlayClicked() {
    QListWidgetItem* currentItem = m_videoList->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Warning", "Please select a video first.");
        return;
    }
    
    QString httpUrl = currentItem->data(Qt::UserRole).toString();
    
    m_videoClient->downloadVideo(httpUrl,
        [this](bool success, const QString& localPath) {
            if (success) {
                m_videoClient->playVideo(localPath, m_mediaPlayer, m_videoWidget);
                m_statusLabel->setText("Playing video");
                // Show controls briefly when video starts
                QTimer::singleShot(500, this, &MainWindow::showVideoControls);
            } else {
                m_statusLabel->setText("Download failed");
                QMessageBox::critical(this, "Error", "Failed to download video");
            }
        }, m_progressBar, m_statusLabel);
}

void MainWindow::onVideoSelected() {
    m_playBtn->setEnabled(m_videoList->currentItem() != nullptr);
}

void MainWindow::onVideoDoubleClicked() {
    onPlayClicked();
}

void MainWindow::onPlayPauseClicked() {
    if (m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_mediaPlayer->pause();
        m_playPauseBtn->setText("▶");
    } else {
        m_mediaPlayer->play();
        m_playPauseBtn->setText("⏸");
    }
}

void MainWindow::onBackwardClicked() {
    qint64 position = m_mediaPlayer->position();
    m_mediaPlayer->setPosition(qMax(0LL, position - 3000)); // 3초 뒤로
}

void MainWindow::onForwardClicked() {
    qint64 position = m_mediaPlayer->position();
    qint64 duration = m_mediaPlayer->duration();
    m_mediaPlayer->setPosition(qMin(duration, position + 3000)); // 3초 앞으로
}

void MainWindow::onPositionChanged(qint64 position) {
    if (!m_positionSlider->isSliderDown()) {
        m_positionSlider->setValue(position);
    }
    
    // 시간 표시 업데이트
    qint64 duration = m_mediaPlayer->duration();
    QString currentTime = QString("%1:%2")
        .arg(position / 60000, 2, 10, QChar('0'))
        .arg((position / 1000) % 60, 2, 10, QChar('0'));
    QString totalTime = QString("%1:%2")
        .arg(duration / 60000, 2, 10, QChar('0'))
        .arg((duration / 1000) % 60, 2, 10, QChar('0'));
    
    m_timeLabel->setText(QString("%1 / %2").arg(currentTime, totalTime));
}

void MainWindow::onDurationChanged(qint64 duration) {
    m_positionSlider->setMaximum(duration);
}

void MainWindow::onSliderMoved(int position) {
    m_mediaPlayer->setPosition(position);
}

void MainWindow::showVideoControls() {
    if (!m_videoControlsWidget) return;
    
    // Position controls at bottom center
    int parentWidth = m_videoContainer->width();
    int parentHeight = m_videoContainer->height();
    int controlsWidth = m_videoControlsWidget->width();
    int controlsHeight = m_videoControlsWidget->height();
    
    int x = (parentWidth - controlsWidth) / 2;
    int y = parentHeight - controlsHeight - 20;
    
    m_videoControlsWidget->move(x, y);
    m_videoControlsWidget->show();
    m_videoControlsWidget->raise();
    
    m_hideControlsTimer->start();
}

void MainWindow::hideVideoControls() {
    m_videoControlsWidget->hide();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_videoContainer) {
        if (event->type() == QEvent::Enter) {
            showVideoControls();
        } else if (event->type() == QEvent::Leave) {
            m_hideControlsTimer->start();
        } else if (event->type() == QEvent::MouseMove) {
            showVideoControls();
        } else if (event->type() == QEvent::Resize) {
            // Reposition controls on resize
            if (m_videoControlsWidget->isVisible()) {
                QTimer::singleShot(0, this, &MainWindow::showVideoControls);
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}