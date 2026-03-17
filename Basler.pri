# Basler.pri

# Путь к инклудам (зависит от платформы)
win32 {
    BASLER_SDK_PATH = $$PWD/BaslerAPI/pylon-6.3.0.23157-x86_64/include
    LIBS += -L$$PWD/BaslerAPI/pylon-6.3.0.23157-x86_64/lib/ -lPylonBase_v6_3
    # ... другие библиотеки
}

linux {
    BASLER_SDK_PATH = /opt/pylon/include
    LIBS += -L/opt/pylon/lib -lpylonbase -lpylonutility -lGenApi_gcc_v3_1_Basler_pylon -lGCBase_gcc_v3_1_Basler_pylon
}

INCLUDEPATH += $$BASLER_SDK_PATH
DEPENDPATH += $$BASLER_SDK_PATH

HEADERS += \
    $$PWD/BaslerApi.h

SOURCES += \
    $$PWD/BaslerApi.cpp
