QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    # Путь к установленному Pylon (укажите свой)
    PYLON_ROOT = G:/ProgramData/Pylon/Development
    INCLUDEPATH += $$PYLON_ROOT/include
    LIBS += -L$$PYLON_ROOT/lib/x64 -lPylonBase_v11

#    PYLON_BIN = $$PYLON_ROOT/Assemblies/Basler.Pylon/x64
#    CONFIG(debug, debug|release) {
#        DLL_FILES = $$files($$PYLON_BIN/*.dll)
#        for(dll, DLL_FILES) {
#            QMAKE_POST_LINK += $$quote(cmd /c copy /y \"$$dll\" \"$$OUT_PWD\\debug\\\" $$escape_expand(\n\t))
#        }
#    } else {
#        DLL_FILES = $$files($$PYLON_BIN/*.dll)
#        for(dll, DLL_FILES) {
#            QMAKE_POST_LINK += $$quote(cmd /c copy /y \"$$dll\" \"$$OUT_PWD\\release\\\" $$escape_expand(\n\t))
#        }
#    }
}
linux {
    INCLUDEPATH += /opt/pylon/include
    LIBS += -L/opt/pylon/lib -lpylonbase
}

SOURCES += \
    BaslerAPI\CameraManager.cpp \
    main.cpp \
    BaslerWindow.cpp

HEADERS += \
    BaslerWindow.h \
    BaslerAPI\CameraManager.h \
    BaslerAPI\Frame.h

FORMS += \
    BaslerWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
