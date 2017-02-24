#######################################################
# Copyright:   Shanghai ReCAT Control Tech. Co., Ltd.
# Author:      Vision Team
# Version:     1.0
# Data:        2017.02.03
# url:         
########################################################
cmake_minimum_required (VERSION 2.6) 

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/SeqCapture")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/SeqCapture")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/SeqCapture" SeqCaptureSRC)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
source_group("Include" FILES ${CURRENT_HEADERS})

add_executable(FindSquares squares.cpp )
target_link_libraries(FindSquares ${OpenCV_LIBS})
set_target_properties(FindSquares PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(SearchLineCircle searchLineCircle.cpp )
target_link_libraries(SearchLineCircle ${OpenCV_LIBS})
set_target_properties(SearchLineCircle PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ThinImage imageThin.cpp )
target_link_libraries(ThinImage ${OpenCV_LIBS})
set_target_properties(ThinImage PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(Calib calib.cpp )
target_link_libraries(Calib ${OpenCV_LIBS})
set_target_properties(Calib PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ReadImg readImg.cpp ${CURRENT_HEADERS} ${SeqCaptureSRC})
target_link_libraries(ReadImg ${OpenCV_LIBS})
set_target_properties(ReadImg PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ImageRotation imageRotation.cpp)
target_link_libraries(ImageRotation ${OpenCV_LIBS})
set_target_properties(ImageRotation PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ImageCrop imageCrop.cpp)
target_link_libraries(ImageCrop ${OpenCV_LIBS})
set_target_properties(ImageCrop PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ImageMatch imageMatch.cpp )
target_link_libraries(ImageMatch ${OpenCV_LIBS})
set_target_properties(ImageMatch PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ImageMatting imageMatting.cpp )
target_link_libraries(ImageMatting ${OpenCV_LIBS})
set_target_properties(ImageMatting PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(OCRReg OCR_Crop.cpp )
target_link_libraries(OCRReg ${OpenCV_LIBS})
set_target_properties(OCRReg PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ErodeDilation erodeDilation.cpp )
target_link_libraries(ErodeDilation ${OpenCV_LIBS})
set_target_properties(ErodeDilation PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(BlobDetect blobDetect.cpp )
target_link_libraries(BlobDetect ${OpenCV_LIBS})
set_target_properties(BlobDetect PROPERTIES COMPILE_DEFINITIONS NOMINMAX)