QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DatabaseManager/ConnectionManager.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    DatabaseManager/ConnectionManager.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:RC_ICONS = $$PWD/icons/app.ico

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../StoreSettings/shared/release/ -lStoreSettings
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../StoreSettings/shared/debug/ -lStoreSettings
else:unix:!macx: LIBS += -L$$PWD/../StoreSettings/shared/ -lStoreSettings

INCLUDEPATH += $$PWD/../StoreSettings
DEPENDPATH += $$PWD/../StoreSettings

RESOURCES += \
    res.qrc
