QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
RC_FILE = recource.rc

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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
    Basler/BaslerApi.cpp \
    Basler/BaslerSettings.cpp \
    Basler/CameraManager.cpp \
    BaslerSettingsForm.cpp \
    main.cpp \
    BaslerWindow.cpp

HEADERS += \
    Basler/BaslerApi.h \
    Basler/BaslerSettings.h \
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
