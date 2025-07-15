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
    
    // Client
    VideoClient* m_videoClient;
};