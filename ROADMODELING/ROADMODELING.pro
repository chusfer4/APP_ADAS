TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += D:\opencv-mingw\install\include

LIBS += -L"D:/opencv-mingw/install/x86/mingw/bin"
LIBS += -lopencv_core320 -lopencv_highgui320 -lopencv_imgproc320 -lopencv_videoio320 -lopencv_imgcodecs320 -lopencv_objdetect320 -lopencv_features2d320 -lopencv_xfeatures2d320 -lopencv_calib3d320 -lopencv_flann320
