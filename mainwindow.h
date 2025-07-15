#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include "video_client_functions.hpp"
#include "videoplayer.h"

/**
 * @brief 메인 애플리케이션 창 - 비디오 목록 조회 및 관리
 * 
 * 서버에서 비디오 목록을 조회하고, 사용자가 선택한 비디오를
 * 별도의 VideoPlayer 창에서 재생할 수 있도록 하는 메인 인터페이스
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /// 비디오 목록 새로고침 버튼 클릭 처리
    void onRefreshClicked();
    /// 비디오 목록에서 항목 선택 처리
    void onVideoSelected();
    /// 비디오 목록에서 항목 더블클릭 처리 (새 창에서 재생)
    void onVideoDoubleClicked();
    /// VideoPlayer 창이 닫힐 때 리스트에서 제거
    void onVideoPlayerClosed();

private:
    /// UI 컴포넌트 초기화
    void setupUI();
    /// 검색 필터 UI 설정
    void setupFilterUI();
    /// 비디오 목록 UI 설정
    void setupVideoListUI();
    /// 상태 표시 UI 설정
    void setupStatusUI();
    /// 시그널-슬롯 연결 설정
    void setupConnections();
    /// 검색 필터 초기값 설정
    void initializeFilters();
    /// 비디오 목록에 데이터 채우기
    void populateVideoList(const QList<VideoInfo>& videos);

    // === UI 컴포넌트 ===
    QWidget* m_centralWidget;           ///< 중앙 위젯
    QVBoxLayout* m_mainLayout;          ///< 메인 레이아웃
    QHBoxLayout* m_topLayout;           ///< 상단 필터 레이아웃
    QHBoxLayout* m_bottomLayout;        ///< 하단 컨텐츠 레이아웃
    
    // === 검색 필터 컨트롤 ===
    QComboBox* m_deviceCombo;           ///< 디바이스 선택 콤보박스
    QLineEdit* m_errorIdEdit;           ///< 에러 ID 입력 필드
    QDateTimeEdit* m_startTimeEdit;     ///< 시작 시간 선택
    QDateTimeEdit* m_endTimeEdit;       ///< 종료 시간 선택
    QPushButton* m_refreshBtn;          ///< 새로고침 버튼
    
    // === 비디오 목록 ===
    QListWidget* m_videoList;           ///< 비디오 목록 위젯
    
    // === 상태 표시 ===
    QProgressBar* m_progressBar;        ///< 다운로드 진행률 표시
    QLabel* m_statusLabel;              ///< 상태 메시지 표시
    
    // === 비즈니스 로직 ===
    VideoClient* m_videoClient;         ///< 서버 통신 클라이언트
    QList<VideoPlayer*> m_videoPlayers; ///< 열린 비디오 플레이어 창들
    
    // === 상수 ===
    static constexpr int DEFAULT_WINDOW_WIDTH = 800;
    static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
    static constexpr int DEFAULT_SEARCH_DAYS = 7;
    static constexpr int MAX_VIDEO_RESULTS = 100;
};