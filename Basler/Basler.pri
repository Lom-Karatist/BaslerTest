# Basler.pri – модуль управления камерами Basler
# Поместите этот файл в папку Basler/ вашего проекта
# Подключение в основном .pro: include(Basler/Basler.pri)

BASLER_PATH = $$PWD

INCLUDEPATH += $$BASLER_PATH $$BASLER_PATH/Commands

QT += core gui widgets concurrent

win32 {
    # Путь к установленному Pylon (укажите свой)
    PYLON_ROOT = G:/ProgramData/Pylon/Development
    INCLUDEPATH += $$PYLON_ROOT/include
    LIBS += -L$$PYLON_ROOT/lib/x64 -lPylonBase_v11
}
linux {
    INCLUDEPATH += /opt/pylon/include
    LIBS += -L/opt/pylon/lib -lpylonbase
}

SOURCES += \
    $$BASLER_PATH/BaslerApi.cpp \
    $$BASLER_PATH/BaslerSettings.cpp \
    $$BASLER_PATH/CameraManager.cpp \
    $$BASLER_PATH/SavingModule.cpp \
    $$BASLER_PATH/Commands/SetBinningHorizontalCommand.cpp \
    $$BASLER_PATH/Commands/SetBinningHorizontalModeCommand.cpp \
    $$BASLER_PATH/Commands/SetBinningVerticalCommand.cpp \
    $$BASLER_PATH/Commands/SetBinningVerticalModeCommand.cpp \
    $$BASLER_PATH/Commands/SetExposureCommand.cpp \
    $$BASLER_PATH/Commands/SetFramerateCommand.cpp \
    $$BASLER_PATH/Commands/SetGainCommand.cpp \
    $$BASLER_PATH/Commands/SetHeightCommand.cpp \
    $$BASLER_PATH/Commands/SetOffsetXCommand.cpp \
    $$BASLER_PATH/Commands/SetOffsetYCommand.cpp \
    $$BASLER_PATH/Commands/SetPixelFormatCommand.cpp \
    $$BASLER_PATH/Commands/SetWidthCommand.cpp

HEADERS += \
    $$BASLER_PATH/BaslerApi.h \
    $$BASLER_PATH/BaslerSettings.h \
    $$BASLER_PATH/CameraManager.h \
    $$BASLER_PATH/SavingModule.h \
    $$BASLER_PATH/Types.h \
    $$BASLER_PATH/Commands/ParameterCommand.h \
    $$BASLER_PATH/Commands/SetBinningHorizontalCommand.h \
    $$BASLER_PATH/Commands/SetBinningHorizontalModeCommand.h \
    $$BASLER_PATH/Commands/SetBinningVerticalCommand.h \
    $$BASLER_PATH/Commands/SetBinningVerticalModeCommand.h \
    $$BASLER_PATH/Commands/SetExposureCommand.h \
    $$BASLER_PATH/Commands/SetFramerateCommand.h \
    $$BASLER_PATH/Commands/SetGainCommand.h \
    $$BASLER_PATH/Commands/SetHeightCommand.h \
    $$BASLER_PATH/Commands/SetOffsetXCommand.h \
    $$BASLER_PATH/Commands/SetOffsetYCommand.h \
    $$BASLER_PATH/Commands/SetPixelFormatCommand.h \
    $$BASLER_PATH/Commands/SetWidthCommand.h

RESOURCES += \
    $$PWD/restoring.qrc
