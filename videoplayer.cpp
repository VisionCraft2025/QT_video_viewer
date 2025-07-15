#include "videoplayer.h"
#include <QUrl>

VideoPlayer::VideoPlayer(const QString& videoPath, QWidget *parent)
    : QWidget(parent)
    , m_videoPath(videoPath)
    , m_mediaPlayer(new QMediaPlayer(this))
{
    setupUI();
    setupConnections();
    
    setWindowTitle("Video Player");
    resize(800, 600);
    
    // Load and play video
    m_mediaPlayer->setSource(QUrl::fromLocalFile(videoPath));
    m_mediaPlayer->setVideoOutput(m_videoWidget);
    m_mediaPlayer->play();
}

VideoPlayer::~VideoPlayer() = default;

void VideoPlayer::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    
    // Video widget
    m_videoWidget = new QVideoWidget;
    m_videoWidget->setMinimumSize(640, 480);
    m_mainLayout->addWidget(m_videoWidget);
    
    // Controls
    m_controlsLayout = new QHBoxLayout;
    
    m_playPauseBtn = new QPushButton("⏸");
    m_playPauseBtn->setFixedSize(40, 30);
    
    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setMinimum(0);
    
    m_timeLabel = new QLabel("00:00 / 00:00");
    m_timeLabel->setMinimumWidth(80);
    
    m_controlsLayout->addWidget(m_playPauseBtn);
    m_controlsLayout->addWidget(m_positionSlider);
    m_controlsLayout->addWidget(m_timeLabel);
    
    m_mainLayout->addLayout(m_controlsLayout);
}

void VideoPlayer::setupConnections() {
    connect(m_playPauseBtn, &QPushButton::clicked, this, &VideoPlayer::onPlayPauseClicked);
    connect(m_positionSlider, &QSlider::sliderMoved, this, &VideoPlayer::onSliderMoved);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::onPositionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayer::onDurationChanged);
}

void VideoPlayer::onPlayPauseClicked() {
    if (m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_mediaPlayer->pause();
        m_playPauseBtn->setText("▶");
    } else {
        m_mediaPlayer->play();
        m_playPauseBtn->setText("⏸");
    }
}

void VideoPlayer::onPositionChanged(qint64 position) {
    if (!m_positionSlider->isSliderDown()) {
        m_positionSlider->setValue(position);
    }
    
    qint64 duration = m_mediaPlayer->duration();
    QString currentTime = QString("%1:%2")
        .arg(position / 60000, 2, 10, QChar('0'))
        .arg((position / 1000) % 60, 2, 10, QChar('0'));
    QString totalTime = QString("%1:%2")
        .arg(duration / 60000, 2, 10, QChar('0'))
        .arg((duration / 1000) % 60, 2, 10, QChar('0'));
    
    m_timeLabel->setText(QString("%1 / %2").arg(currentTime, totalTime));
}

void VideoPlayer::onDurationChanged(qint64 duration) {
    m_positionSlider->setMaximum(duration);
}

void VideoPlayer::onSliderMoved(int position) {
    m_mediaPlayer->setPosition(position);
}