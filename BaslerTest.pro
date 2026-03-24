QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
RC_FILE = recource.rc

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
    BaseTools/IniFileLoader.cpp \
    BaseTools/QrcFilesRestorer.cpp \
    Basler/BaslerApi.cpp \
    Basler/BaslerSettings.cpp \
    Basler/CameraManager.cpp \
    Basler/Commands/SetBinningHorizontalCommand.cpp \
    Basler/Commands/SetBinningHorizontalModeCommand.cpp \
    Basler/Commands/SetBinningVerticalCommand.cpp \
    Basler/Commands/SetBinningVerticalModeCommand.cpp \
    Basler/Commands/SetExposureCommand.cpp \
    Basler/Commands/SetFramerateCommand.cpp \
    Basler/Commands/SetGainCommand.cpp \
    Basler/Commands/SetHeightCommand.cpp \
    Basler/Commands/SetOffsetXCommand.cpp \
    Basler/Commands/SetOffsetYCommand.cpp \
    Basler/Commands/SetPixelFormatCommand.cpp \
    Basler/Commands/SetWidthCommand.cpp \
    Basler/SavingModule.cpp \
    BaslerSettingsForm.cpp \
    main.cpp \
    BaslerWindow.cpp

HEADERS += \
    BaseTools/IniFileLoader.h \
    BaseTools/QrcFilesRestorer.h \
    Basler/BaslerApi.h \
    Basler/BaslerSettings.h \
    Basler/Commands/ParameterCommand.h \
    Basler/Commands/SetBinningHorizontalCommand.h \
    Basler/Commands/SetBinningHorizontalModeCommand.h \
    Basler/Commands/SetBinningVerticalCommand.h \
    Basler/Commands/SetBinningVerticalModeCommand.h \
    Basler/Commands/SetExposureCommand.h \
    Basler/Commands/SetFramerateCommand.h \
    Basler/Commands/SetGainCommand.h \
    Basler/Commands/SetHeightCommand.h \
    Basler/Commands/SetOffsetXCommand.h \
    Basler/Commands/SetOffsetYCommand.h \
    Basler/Commands/SetPixelFormatCommand.h \
    Basler/Commands/SetWidthCommand.h \
    Basler/SavingModule.h \
    Basler/Types.h \
    BaslerSettingsForm.h \
    BaslerWindow.h \
    Basler/CameraManager.h \
    version.h

FORMS += \
    BaslerSettingsForm.ui \
    BaslerWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
