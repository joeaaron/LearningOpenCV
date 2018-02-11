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

add_executable(BrightnessContrast imageBrightness.cpp)
target_link_libraries(BrightnessContrast ${OpenCV_LIBS})
set_target_properties(BrightnessContrast PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(Mosaic mosaic.cpp)
target_link_libraries(Mosaic ${OpenCV_LIBS})
set_target_properties(Mosaic PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(frostedGlass frostedGlass.cpp)
target_link_libraries(frostedGlass ${OpenCV_LIBS})
set_target_properties(frostedGlass PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ImageStitch imageStitching.cpp)
target_link_libraries(ImageStitch ${OpenCV_LIBS})
set_target_properties(ImageStitch PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ImageQuality imageQuality.cpp)
target_link_libraries(ImageQuality ${OpenCV_LIBS})
set_target_properties(ImageQuality PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(JigSaw jigsaw.cpp)
target_link_libraries(JigSaw ${OpenCV_LIBS})
set_target_properties(JigSaw PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(Video videoOpen.cpp)
target_link_libraries(Video ${OpenCV_LIBS})
set_target_properties(Video PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(Incircle incircle.cpp)
target_link_libraries(Incircle ${OpenCV_LIBS})
set_target_properties(Incircle PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ChessBoardCenter chessBoardCenter.cpp)
target_link_libraries(ChessBoardCenter ${OpenCV_LIBS})
set_target_properties(ChessBoardCenter PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(FitLine fitLine.cpp)
target_link_libraries(FitLine ${OpenCV_LIBS})
set_target_properties(FitLine PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(SVMSimpleDemo svmSimpleDemo.cpp)
target_link_libraries(SVMSimpleDemo ${OpenCV_LIBS})
set_target_properties(SVMSimpleDemo PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ModifyPixel modifyPixel.cpp)
target_link_libraries(ModifyPixel ${OpenCV_LIBS})
set_target_properties(ModifyPixel PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(HahaMirror hahaMirror.cpp)
target_link_libraries(HahaMirror ${OpenCV_LIBS})
set_target_properties(HahaMirror PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(FillHole fillHole.cpp)
target_link_libraries(FillHole ${OpenCV_LIBS})
set_target_properties(FillHole PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ChessBoard ChessBoard.cpp)
target_link_libraries(ChessBoard ${OpenCV_LIBS})
set_target_properties(ChessBoard PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(LBPFeature LBPFeature.cpp)
target_link_libraries(LBPFeature ${OpenCV_LIBS})
set_target_properties(LBPFeature PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(TerminalLine terminalLine.cpp)
target_link_libraries(TerminalLine ${OpenCV_LIBS})
set_target_properties(TerminalLine PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ImshowManyImg imshowManyImg.cpp ${CURRENT_HEADERS} ${SeqCaptureSRC})
target_link_libraries(ImshowManyImg ${OpenCV_LIBS})
set_target_properties(ImshowManyImg PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(InPaint inPaint.cpp)
target_link_libraries(InPaint ${OpenCV_LIBS})
set_target_properties(InPaint PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(CortAlgorithm cortAlgorithm.cpp)
target_link_libraries(CortAlgorithm ${OpenCV_LIBS})
set_target_properties(CortAlgorithm PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(CamScanner camScanner.cpp)
target_link_libraries(CamScanner ${OpenCV_LIBS})
set_target_properties(CamScanner PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ImgMosaicking imgMosaicking.cpp)
target_link_libraries(ImgMosaicking ${OpenCV_LIBS})
set_target_properties(ImgMosaicking PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(LowResolution lowResolution.cpp)
target_link_libraries(LowResolution ${OpenCV_LIBS})
set_target_properties(LowResolution PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(SuperResolution superResolution.cpp ${CURRENT_HEADERS} ${SeqCaptureSRC})
target_link_libraries(SuperResolution ${OpenCV_LIBS})
set_target_properties(SuperResolution PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(3DPointCloud 3DPointCloud.cpp)
target_link_libraries(3DPointCloud ${OpenCV_LIBS})
set_target_properties(3DPointCloud PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ImagePartition imagePartition.cpp)
target_link_libraries(ImagePartition ${OpenCV_LIBS})
set_target_properties(ImagePartition PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(BrightestPoint brightestPoint.cpp)
target_link_libraries(BrightestPoint ${OpenCV_LIBS})
set_target_properties(BrightestPoint PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(SingleCamera single_camera_calibration.cpp)
target_link_libraries(SingleCamera ${OpenCV_LIBS})
set_target_properties(SingleCamera PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(Bwareaopen bwareaopen.cpp)
target_link_libraries(Bwareaopen ${OpenCV_LIBS})
set_target_properties(Bwareaopen PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(ObjectMainDirection objectMainDirection.cpp)
target_link_libraries(ObjectMainDirection ${OpenCV_LIBS})
set_target_properties(ObjectMainDirection PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(BadPoint badPoint.cpp)
target_link_libraries(BadPoint ${OpenCV_LIBS})
set_target_properties(BadPoint PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(Hsv hsv.cpp)
target_link_libraries(Hsv ${OpenCV_LIBS})
set_target_properties(Hsv PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(FindContourCenterPoint findContourCenterPoint.cpp)
target_link_libraries(FindContourCenterPoint ${OpenCV_LIBS})
set_target_properties(FindContourCenterPoint PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(Test test.cpp ${CURRENT_HEADERS} ${SeqCaptureSRC})
target_link_libraries(Test ${OpenCV_LIBS})
set_target_properties(Test PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(CountCellsNum countCellsNum.cpp)
target_link_libraries(CountCellsNum ${OpenCV_LIBS})
set_target_properties(CountCellsNum PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(RemoveSeal removeSeal.cpp)
target_link_libraries(RemoveSeal ${OpenCV_LIBS})
set_target_properties(RemoveSeal PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(HVProjection hvProjection.cpp)
target_link_libraries(HVProjection ${OpenCV_LIBS})
set_target_properties(HVProjection PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(FindObject findObject.cpp)
target_link_libraries(FindObject ${OpenCV_LIBS})
set_target_properties(FindObject PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(Realtime_online_cb_det Realtime_online_cb_det.cpp ${CURRENT_HEADERS} ${SeqCaptureSRC})
target_link_libraries(Realtime_online_cb_det ${OpenCV_LIBS})
set_target_properties(Realtime_online_cb_det PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(TwoLineAngle twoLineAngle.cpp)
target_link_libraries(TwoLineAngle ${OpenCV_LIBS})
set_target_properties(TwoLineAngle PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

<<<<<<< HEAD
add_executable(Histograph histograph.cpp)
target_link_libraries(Histograph ${OpenCV_LIBS})
set_target_properties(Histograph PROPERTIES COMPILE_DEFINITIONS NOMINMAX)
=======
add_executable(RMatrixtoAngle RMatrixtoAngle.cpp)
target_link_libraries(RMatrixtoAngle ${OpenCV_LIBS})
set_target_properties(RMatrixtoAngle PROPERTIES COMPILE_DEFINITIONS NOMINMAX)

add_executable(OpenCam openCam.cpp)
target_link_libraries(OpenCam ${OpenCV_LIBS})
set_target_properties(OpenCam PROPERTIES COMPILE_DEFINITIONS NOMINMAX)
>>>>>>> f013da8e3dff6accd583c6f39e7763209b83375e
