# ConversionTool/ConversionTool.pri
INCLUDEPATH += $$PWD
SOURCES += \
    $$PWD/BatchReader.cpp \
    $$PWD/CalibrationLoader.cpp \
    $$PWD/ConversionDialog.cpp \
    $$PWD/ConversionWorker.cpp \
    $$PWD/CubeBuilder.cpp \
    $$PWD/EnviWriter.cpp \
    $$PWD/SigWriter.cpp
HEADERS += \
    $$PWD/BatchReader.h \
    $$PWD/CalibrationLoader.h \
    $$PWD/ConversionDialog.h \
    $$PWD/ConversionWorker.h \
    $$PWD/CubeBuilder.h \
    $$PWD/EnviWriter.h \
    $$PWD/HypercubeData.h \
    $$PWD/SigWriter.h

FORMS += \
    $$PWD/ConversionDialog.ui
