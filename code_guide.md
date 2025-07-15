# Factory Video System Code Guide

## 시스템 개요

공장 모니터링 시스템으로 MQTT 에러 메시지 수신 시 자동 영상 녹화 및 Qt6 기반 클라이언트를 통한 영상 조회/재생 시스템입니다.

## 아키텍처

```
┌─────────────────┐    MQTT     ┌─────────────────┐    HTTP     ┌─────────────────┐
│   Qt6 Client    │ ◄────────► │  Video Server   │ ◄────────► │    MongoDB      │
│  (GUI 재생기)    │             │ (MQTT+HTTP서버) │             │  (메타데이터)    │
└─────────────────┘             └─────────────────┘             └─────────────────┘
                                         │
                                         ▼
                                ┌─────────────────┐
                                │  File System    │
                                │ (실제 영상파일)  │
                                └─────────────────┘
```

## 디렉토리 구조

```
db_video_mqtt/
├── video_server.cpp          # 메인 서버 (MQTT + HTTP + MongoDB)
├── CMakeLists.txt            # 서버 빌드 설정
├── client/                   # Qt6 클라이언트
│   ├── main.cpp             # 클라이언트 진입점
│   ├── mainwindow.h/cpp     # 메인 GUI 윈도우
│   ├── mqtt.h/cpp           # MQTT 통신 클래스
│   ├── video_client_functions.hpp  # 비디오 다운로드/재생 로직
│   └── CMakeLists.txt       # 클라이언트 빌드 설정
└── build/                   # 빌드 출력
```

## 서버 (video_server.cpp)

### 주요 기능
- **MQTT 구독**: `factory/query/videos/request` 토픽 모니터링
- **MongoDB 쿼리**: 영상 메타데이터 검색
- **HTTP 파일 서버**: 포트 8081에서 영상 파일 제공
- **멀티스레드**: HTTP 요청별 별도 스레드 처리

### 핵심 클래스/함수

```cpp
// MQTT 콜백 핸들러
class VideoServerCallback : public virtual mqtt::callback {
    void message_arrived(mqtt::const_message_ptr msg) override;
    void connected(const std::string& cause) override;
};

// 비디오 쿼리 처리 (MongoDB 검색)
void process_video_query(mongocxx::client& mongo_client, 
                        mqtt::async_client* mqtt_client, 
                        const json& query);

// HTTP 파일 서버
void handle_http_request(int client_socket);
void start_http_server();
```

### 설정값
```cpp
const std::string MQTT_SERVER_ADDRESS = "tcp://mqtt.kwon.pics:1883";
const std::string MONGO_URI = "mongodb://localhost:27017";
const std::string MONGO_DB_NAME = "factory_monitoring";
const std::string VIDEOS_COLLECTION = "videos";
const int HTTP_PORT = 8081;
```

## 클라이언트 (Qt6)

### 주요 구성 요소

#### 1. MainWindow (mainwindow.h/cpp)
메인 GUI 윈도우 - 영상 검색, 목록 표시, 재생 기능

**주요 UI 컴포넌트:**
```cpp
// 검색 필터
QComboBox* m_deviceCombo;        // 디바이스 선택
QLineEdit* m_errorIdEdit;        // 에러 ID 입력
QDateTimeEdit* m_startTimeEdit;  // 시작 시간
QDateTimeEdit* m_endTimeEdit;    // 종료 시간

// 영상 목록 및 재생
QListWidget* m_videoList;        // 영상 목록
QVideoWidget* m_videoWidget;     // 영상 재생 위젯
QMediaPlayer* m_mediaPlayer;     // 미디어 플레이어

// 오버레이 컨트롤 바 (마우스 호버시 표시)
QWidget* m_videoControlsWidget;  // 반투명 컨트롤 바
QPushButton* m_playPauseBtn;     // 재생/일시정지
QPushButton* m_backwardBtn;      // 3초 뒤로
QPushButton* m_forwardBtn;       // 3초 앞으로
QSlider* m_positionSlider;       // 재생 위치 슬라이더
QLabel* m_timeLabel;             // 시간 표시
```

**주요 슬롯 함수:**
```cpp
void onRefreshClicked();         // 영상 목록 새로고침
void onVideoDoubleClicked();     // 영상 더블클릭 → 자동 재생
void onPlayPauseClicked();       // 재생/일시정지 토글
void onBackwardClicked();        // 3초 뒤로 이동
void onForwardClicked();         // 3초 앞으로 이동
void showVideoControls();        // 컨트롤 바 표시
void hideVideoControls();        // 컨트롤 바 숨김
bool eventFilter();              // 마우스 이벤트 처리
```

#### 2. MqttClient (mqtt.h/cpp)
MQTT 통신 전담 클래스

```cpp
class MqttClient : public QObject {
    QMqttClient* m_client;
    QMap<QString, VideoQueryCallback> m_pendingQueries;
    
public:
    void connectToHost();        // MQTT 브로커 연결
    void queryVideos(...);       // 영상 쿼리 요청
    
private slots:
    void onConnected();          // 연결 완료 시 구독 설정
    void onMessageReceived();    // 응답 메시지 처리
};
```

#### 3. VideoClient (video_client_functions.hpp)
영상 다운로드 및 재생 로직

```cpp
class VideoClient : public QObject {
    QNetworkAccessManager* m_networkManager;
    MqttClient* m_mqttClient;
    
public:
    void queryVideos(..., VideoQueryCallback callback);
    void downloadVideo(..., VideoDownloadCallback callback);
    void playVideo(QString localPath, QMediaPlayer*, QVideoWidget*);
};
```

## 데이터 플로우

### 1. 영상 검색 플로우
```
1. 사용자가 검색 조건 입력 후 "Refresh" 클릭
2. MainWindow::onRefreshClicked()
3. VideoClient::queryVideos()
4. MqttClient::queryVideos() → MQTT 발송
5. video_server가 MongoDB 쿼리 실행
6. MQTT 응답 → MqttClient::onMessageReceived()
7. 콜백으로 영상 목록 UI 업데이트
```

### 2. 영상 재생 플로우
```
1. 사용자가 영상 더블클릭
2. MainWindow::onVideoDoubleClicked()
3. VideoClient::downloadVideo() → HTTP GET 요청
4. video_server가 파일 전송
5. 다운로드 완료 후 VideoClient::playVideo()
6. QMediaPlayer로 로컬 파일 재생
```

### 3. 컨트롤 바 동작
```
1. 마우스가 영상 영역 진입 → eventFilter() 감지
2. showVideoControls() → 컨트롤 바 표시
3. 3초 타이머 시작
4. 마우스 이동 시 타이머 리셋
5. 타이머 만료 또는 마우스 이탈 → hideVideoControls()
```

## 빌드 방법

### 서버 빌드
```bash
cd db_video_mqtt
mkdir build && cd build
cmake ..
make
./video_server
```

### 클라이언트 빌드
```bash
cd client
mkdir build && cd build
cmake ..
make

# 실행 (라이브러리 경로 설정 필요)
LD_LIBRARY_PATH="$HOME/dev/cpp_libs/qtmultimedia/install/lib/x86_64-linux-gnu:$HOME/dev/cpp_libs/qtshadertools/install/lib/x86_64-linux-gnu:$HOME/dev/cpp_libs/qtmqtt/install/lib/x86_64-linux-gnu" ./video_client
```

## 의존성

### 서버
- MongoDB C++ Driver
- Paho MQTT C++
- nlohmann/json

### 클라이언트
- Qt6 Core, Widgets, Network
- Qt6 Multimedia, MultimediaWidgets (로컬 빌드)
- Qt6 MQTT (로컬 빌드)

## 주요 특징

### UI/UX
- **더블클릭 재생**: 영상 목록에서 더블클릭으로 즉시 재생
- **반투명 컨트롤 바**: 마우스 호버 시에만 표시되는 오버레이 컨트롤
- **진행률 표시**: 다운로드 진행률 실시간 표시
- **드래그 탐색**: 재생바 드래그로 원하는 위치 이동

### 네트워크
- **비동기 MQTT**: 논블로킹 쿼리/응답
- **HTTP 스트리밍**: 대용량 영상 파일 효율적 전송
- **멀티스레드**: 동시 다운로드 지원

### 데이터 관리
- **MongoDB 인덱싱**: 빠른 검색을 위한 최적화된 쿼리
- **로컬 캐싱**: 다운로드된 영상 임시 저장
- **메타데이터 분리**: 파일과 메타데이터 독립 관리

## 확장 포인트

1. **인증 시스템**: MQTT/HTTP 보안 인증 추가
2. **실시간 스트리밍**: RTSP 직접 재생 기능
3. **영상 분석**: OpenCV 연동 자동 분석
4. **알림 시스템**: 중요 에러 발생 시 푸시 알림
5. **다중 서버**: 로드 밸런싱 및 고가용성 구성