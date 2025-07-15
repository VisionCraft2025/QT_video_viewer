#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QSlider>
#include <QStackedLayout>
#include <QTimer>
#include <QEvent>
#include "video_client_functions.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRefreshClicked();
    void onPlayClicked();
    void onVideoSelected();
    void onVideoDoubleClicked();
    
    // Video control slots
    void onPlayPauseClicked();
    void onBackwardClicked();
    void onForwardClicked();
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onSliderMoved(int position);
    void showVideoControls();
    void hideVideoControls();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void setupConnections();

    // UI Components
    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_topLayout;
    QHBoxLayout* m_bottomLayout;
    
    // Controls
    QComboBox* m_deviceCombo;
    QLineEdit* m_errorIdEdit;
    QDateTimeEdit* m_startTimeEdit;
    QDateTimeEdit* m_endTimeEdit;
    QPushButton* m_refreshBtn;
    
    // Video List
    QListWidget* m_videoList;
    
    // Playback
    QVideoWidget* m_videoWidget;
    QMediaPlayer* m_mediaPlayer;
    QPushButton* m_playBtn;
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    
    // Video Controls
    QWidget* m_videoControlsWidget;
    QPushButton* m_playPauseBtn;
    QPushButton* m_backwardBtn;
    QPushButton* m_forwardBtn;
    QSlider* m_positionSlider;
    QLabel* m_timeLabel;
    QTimer* m_hideControlsTimer;
    QWidget* m_videoContainer;
    
    // Client
    VideoClient* m_videoClient;
};