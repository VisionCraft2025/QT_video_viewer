#include "mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_videoClient(new VideoClient(this))
    , m_mediaPlayer(new QMediaPlayer(this))
{
    setupUI();
    setupConnections();
    setWindowTitle("Factory Video Client");
    resize(1200, 800);
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
    m_videoWidget = new QVideoWidget;
    m_videoWidget->setMinimumSize(640, 480);
    
    QHBoxLayout* controlLayout = new QHBoxLayout;
    m_playBtn = new QPushButton("Play Selected");
    m_progressBar = new QProgressBar;
    m_statusLabel = new QLabel("Ready");
    
    controlLayout->addWidget(m_playBtn);
    controlLayout->addWidget(m_progressBar);
    controlLayout->addWidget(m_statusLabel);
    controlLayout->addStretch();
    
    playerLayout->addWidget(m_videoWidget);
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