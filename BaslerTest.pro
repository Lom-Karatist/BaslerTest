QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
RC_FILE = recource.rc

include(Basler/Basler.pri)

SOURCES += \
    BaseTools/IniFileLoader.cpp \
    BaseTools/QrcFilesRestorer.cpp \    
    BaslerSettingsForm.cpp \
    main.cpp \
    BaslerWindow.cpp

HEADERS += \
    BaseTools/IniFileLoader.h \
    BaseTools/QrcFilesRestorer.h \    
    BaslerSettingsForm.h \
    BaslerWindow.h \
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
