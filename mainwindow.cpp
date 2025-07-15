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
    setWindowTitle("Factory Video Client");
    resize(800, 600);
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
    
    // Status bar
    QHBoxLayout* statusLayout = new QHBoxLayout;
    m_progressBar = new QProgressBar;
    m_statusLabel = new QLabel("Ready");
    
    statusLayout->addWidget(m_progressBar);
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    
    m_bottomLayout->addWidget(m_videoList);
    
    m_mainLayout->addLayout(m_bottomLayout);
    m_mainLayout->addLayout(statusLayout);
}

void MainWindow::setupConnections() {
    connect(m_refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
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

void MainWindow::onVideoSelected() {
    // Nothing to do for now
}

void MainWindow::onVideoDoubleClicked() {
    QListWidgetItem* currentItem = m_videoList->currentItem();
    if (!currentItem) {
        return;
    }
    
    QString httpUrl = currentItem->data(Qt::UserRole).toString();
    
    m_videoClient->downloadVideo(httpUrl,
        [this](bool success, const QString& localPath) {
            if (success) {
                VideoPlayer* player = new VideoPlayer(localPath);
                player->show();
                m_videoPlayers.append(player);
                m_statusLabel->setText("Video opened in new window");
            } else {
                m_statusLabel->setText("Download failed");
                QMessageBox::critical(this, "Error", "Failed to download video");
            }
        }, m_progressBar, m_statusLabel);
}

