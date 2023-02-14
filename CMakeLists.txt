cmake_minimum_required(VERSION 3.23)
project(coin_detector)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD 20)


find_package(OpenCV REQUIRED)


set(OpenCV_LIBS opencv_core opencv_highgui opencv_imgcodecs opencv_videoio opencv_ximgproc)

include_directories(${OpenCV_INCLUDE_DIRS} include)
set(SOURCE_FILES src/main.cpp src/CoinDetector.cpp)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})


target_link_libraries(coin_detector ${OpenCV_LIBS})
