cmake_minimum_required(VERSION 3.16)
project(VideoClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 로컬 Qt 라이브러리 경로 설정
set(CPP_LIBS_DIR "$ENV{HOME}/dev/cpp_libs")
list(PREPEND CMAKE_PREFIX_PATH 
    "${CPP_LIBS_DIR}/qtmultimedia/install/lib/x86_64-linux-gnu/cmake"
    "${CPP_LIBS_DIR}/qtshadertools/install/lib/x86_64-linux-gnu/cmake"
    "${CPP_LIBS_DIR}/qtmqtt/install/lib/x86_64-linux-gnu/cmake"
)

# 개별 모듈 경로 설정
set(Qt6Multimedia_DIR "${CPP_LIBS_DIR}/qtmultimedia/install/lib/x86_64-linux-gnu/cmake/Qt6Multimedia")
set(Qt6MultimediaWidgets_DIR "${CPP_LIBS_DIR}/qtmultimedia/install/lib/x86_64-linux-gnu/cmake/Qt6MultimediaWidgets")
set(Qt6Mqtt_DIR "${CPP_LIBS_DIR}/qtmqtt/install/lib/x86_64-linux-gnu/cmake/Qt6Mqtt")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt6Multimedia REQUIRED)
find_package(Qt6MultimediaWidgets REQUIRED)
find_package(Qt6Mqtt REQUIRED)

qt6_standard_project_setup()

qt6_add_executable(video_client
    main.cpp
    mainwindow.cpp
    mainwindow.h
    videoplayer.cpp
    videoplayer.h
    mqtt.cpp
    mqtt.h
    video_client_functions.hpp
)

target_link_libraries(video_client PRIVATE
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Network 
    Qt6::Multimedia 
    Qt6::MultimediaWidgets
    Qt6::Mqtt
)