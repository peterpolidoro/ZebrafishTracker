cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules )

project( ZebrafishTracker )

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

find_package( FlyCapture REQUIRED )
include_directories( ${FLYCAPTURE_INCLUDE_DIRS} )

find_package( Boost COMPONENTS timer thread filesystem REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )

find_package( OpenCV 2 REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package( LibSerial REQUIRED )
include_directories( ${LIBSERIAL_INCLUDE_DIRS} )

add_executable(ZebrafishTracker
  ${PROJECT_SOURCE_DIR}/src/Main.cpp
  ${PROJECT_SOURCE_DIR}/src/ZebrafishTracker.cpp
  ${PROJECT_SOURCE_DIR}/src/Camera.cpp
  ${PROJECT_SOURCE_DIR}/src/BlobTracker.cpp
  ${PROJECT_SOURCE_DIR}/src/StageController.cpp
  ${PROJECT_SOURCE_DIR}/src/FrameRateCounter.cpp)

target_link_libraries( ZebrafishTracker ${FLYCAPTURE_LIBRARIES})
target_link_libraries( ZebrafishTracker ${Boost_LIBRARIES} )
target_link_libraries( ZebrafishTracker ${OpenCV_LIBS} )
target_link_libraries( ZebrafishTracker ${LIBSERIAL_LIBRARIES} )
