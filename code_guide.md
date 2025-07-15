# Factory Video Client - 코드 가이드

## 아키텍처 개요

이 애플리케이션은 Qt6 기반의 공장 비디오 클라이언트로, 서버에서 에러 로그와 연관된 비디오를 조회하고 재생하는 기능을 제공합니다.

## 주요 클래스 구조

### 1. MainWindow (include/ui/mainwindow.h, src/ui/mainwindow.cpp)
- **역할**: 메인 애플리케이션 창, 비디오 목록 관리
- **주요 기능**:
  - 비디오 검색 필터 UI (디바이스, 에러 ID, 시간 범위)
  - 비디오 목록 표시
  - 비디오 다운로드 진행률 표시
- **핵심 멤버**:
  - `VideoClient* m_videoClient`: 서버 통신 클라이언트
  - `QListWidget* m_videoList`: 비디오 목록 위젯
  - `QList<VideoPlayer*> m_videoPlayers`: 열린 비디오 플레이어 창들

### 2. VideoPlayer (include/video/videoplayer.h, src/video/videoplayer.cpp)
- **역할**: 독립적인 비디오 재생 창
- **주요 기능**:
  - 비디오 파일 재생
  - 재생/일시정지 컨트롤
  - 시간 슬라이더 및 시간 표시
- **핵심 멤버**:
  - `QMediaPlayer* m_mediaPlayer`: Qt6 미디어 플레이어
  - `QVideoWidget* m_videoWidget`: 비디오 출력 위젯
  - `QSlider* m_positionSlider`: 재생 위치 슬라이더

### 3. VideoClient (include/core/video_client_functions.hpp)
- **역할**: 서버와의 HTTP 통신 및 비디오 관리
- **주요 기능**:
  - 비디오 목록 조회 API 호출
  - 비디오 파일 다운로드
  - 로컬 비디오 파일 관리

## 데이터 흐름

```
1. 사용자가 검색 조건 입력 → MainWindow::onRefreshClicked()
2. VideoClient::queryVideos() → 서버 API 호출
3. 비디오 목록 수신 → QListWidget에 표시
4. 사용자가 비디오 더블클릭 → MainWindow::onVideoDoubleClicked()
5. VideoClient::downloadVideo() → 비디오 파일 다운로드
6. 다운로드 완료 → VideoPlayer 창 생성 및 재생
```

## 주요 설계 결정사항

### 1. 독립적인 비디오 플레이어 창
- **이유**: 여러 비디오를 동시에 재생할 수 있도록 함
- **구현**: 각 비디오마다 별도의 VideoPlayer 인스턴스 생성
- **메모리 관리**: MainWindow에서 VideoPlayer 포인터 리스트로 관리

### 2. 단순화된 메인 UI
- **이유**: 복잡한 오버레이 컨트롤 대신 직관적인 리스트 기반 UI
- **구현**: 비디오 목록과 상태 표시만 포함
- **사용성**: 더블클릭으로 간단한 비디오 재생

### 3. Qt6 Multimedia 사용
- **이유**: 크로스 플랫폼 비디오 재생 지원
- **요구사항**: 로컬 빌드된 Qt6 Multimedia 라이브러리 필요
- **제약사항**: 시스템 패키지로는 빌드 불가

## 빌드 의존성

### 필수 라이브러리
- Qt6 Base (시스템 패키지)
- Qt6 ShaderTools (로컬 빌드)
- Qt6 Multimedia (로컬 빌드)
- Qt6 MultimediaWidgets (로컬 빌드)
- Qt6 Mqtt (로컬 빌드)

### 빌드 순서
1. Qt6 ShaderTools 빌드 (qtmultimedia 의존성)
2. Qt6 Multimedia 빌드
3. Qt6 Mqtt 빌드
4. 애플리케이션 빌드

## 파일 구조 설명

```
client/
├── src/                       # 소스 파일들
│   ├── core/
│   │   └── main.cpp          # 애플리케이션 진입점
│   ├── ui/
│   │   └── mainwindow.cpp    # 메인 창 구현
│   ├── video/
│   │   └── videoplayer.cpp   # 비디오 재생 창 구현
│   └── network/
│       └── mqtt.cpp          # MQTT 클라이언트 구현
├── include/                   # 헤더 파일들
│   ├── core/
│   │   └── video_client_functions.hpp # HTTP 클라이언트 및 유틸리티
│   ├── ui/
│   │   └── mainwindow.h      # 메인 창 헤더
│   ├── video/
│   │   └── videoplayer.h     # 비디오 재생 창 헤더
│   └── network/
│       └── mqtt.h            # MQTT 클라이언트 헤더
├── CMakeLists.txt             # 빌드 설정
└── build*.sh                  # 빌드 스크립트들
```

## 확장 가능성

### 1. MQTT 통신
- 현재: HTTP 기반 비디오 조회
- 향후: MQTT를 통한 실시간 알림 및 스트리밍

### 2. 비디오 스트리밍
- 현재: 다운로드 후 로컬 재생
- 향후: 실시간 스트리밍 지원

### 3. 다중 서버 지원
- 현재: 단일 서버 연결
- 향후: 여러 공장 서버 동시 연결

## 개발 가이드라인

### 1. 새로운 기능 추가시
- **핵심 로직**: `include/core/`에 헤더, `src/core/`에 구현
- **UI 관련**: `include/ui/`에 헤더, `src/ui/`에 구현
- **비디오 처리**: `include/video/`에 헤더, `src/video/`에 구현
- **네트워크 통신**: `include/network/`에 헤더, `src/network/`에 구현

### 2. 파일 명명 규칙
- 헤더 파일: `.h` (Qt 클래스), `.hpp` (템플릿/인라인 함수)
- 소스 파일: `.cpp`
- 클래스명과 파일명 일치 (예: `MainWindow` → `mainwindow.h/cpp`)

### 3. Include 경로 규칙
- 소스 파일에서 헤더 포함: `#include "../../include/category/header.h"`
- 헤더 파일 간 포함: `#include "../category/header.h"`
- CMakeLists.txt에서 include 디렉토리 설정으로 간소화 가능

### 4. 메모리 관리
- Qt의 부모-자식 관계 활용
- 동적 생성된 VideoPlayer는 MainWindow에서 추적

### 5. 에러 처리
- 네트워크 에러는 사용자에게 명확히 표시
- 다운로드 실패시 적절한 복구 메커니즘 제공

### 6. 새 모듈 추가 절차
1. 해당 카테고리 폴더에 헤더/소스 파일 생성
2. CMakeLists.txt에 파일 경로 추가
3. 필요한 include 경로 설정
4. 의존성 있는 다른 파일들의 include 문 업데이트