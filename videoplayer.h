#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

class VideoPlayer : public QWidget {
    Q_OBJECT

public:
    explicit VideoPlayer(const QString& videoPath, QWidget *parent = nullptr);
    ~VideoPlayer();

private slots:
    void onPlayPauseClicked();
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onSliderMoved(int position);

private:
    void setupUI();
    void setupConnections();

    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_controlsLayout;
    
    QVideoWidget* m_videoWidget;
    QMediaPlayer* m_mediaPlayer;
    
    QPushButton* m_playPauseBtn;
    QSlider* m_positionSlider;
    QLabel* m_timeLabel;
    
    QString m_videoPath;
};