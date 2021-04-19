QT       += core gui sql axcontainer printsupport qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DatabaseManager/DatabaseContainer.cpp \
    DatabaseManager/ProtocolQueryModel.cpp \
    DatabaseManager/StructureTreeModel.cpp \
    DatabaseManager/TreeQueryItem.cpp \
    GUI/uiConnectionDialog/uiConnectionDialog.cpp \
    GUI/uiPhotoWidget/uiPhotoWidget.cpp \
    GUI/uiPhotolabel/uiPhotolabel.cpp \
    GUI/uiReportSelector/uiReportSelector.cpp \
    GUI/uiStructureDialog/uiStructDetectorDialog.cpp \
    GUI/uiStructureDialog/uiStructObjectDialog.cpp \
    Report/clExcelExport.cpp \
    Report/clReportGenerator.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    DatabaseManager/DatabaseContainer.h \
    DatabaseManager/ProtocolQueryModel.h \
    DatabaseManager/StructureTreeModel.h \
    DatabaseManager/TreeQueryItem.h \
    GUI/uiConnectionDialog/uiConnectionDialog.h \
    GUI/uiPhotoWidget/uiPhotoWidget.h \
    GUI/uiPhotolabel/uiPhotolabel.h \
    GUI/uiReportSelector/uiReportSelector.h \
    GUI/uiStructureDialog/uiStructDetectorDialog.h \
    GUI/uiStructureDialog/uiStructObjectDialog.h \
    Report/clExcelExport.h \
    Report/clReportGenerator.h \
    mainwindow.h

FORMS += \
    GUI/uiConnectionDialog/uiConnectionDialog.ui \
    GUI/uiReportSelector/uiReportSelector.ui \
    GUI/uiStructureDialog/uiStructDetectorDialog.ui \
    GUI/uiStructureDialog/uiStructObjectDialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:RC_ICONS = $$PWD/icons/app.ico

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../StoreSettings/shared/release/ -lStoreSettings
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../StoreSettings/shared/debug/ -lStoreSettings
else:unix:!macx: LIBS += -L$$PWD/../StoreSettings/shared/ -lStoreSettings

INCLUDEPATH += $$PWD/../StoreSettings \
                GUI/uiPhotoLabel

DEPENDPATH += $$PWD/../StoreSettings

RESOURCES += \
    res.qrc

win32:CONFIG(release, debug|release): {
    DESTDIR = $$OUT_PWD/../XeomaDetectorsRelease

    LIBS += -L$$PWD/Report/LimeReport/lib64/release/ -llimereport
    INCLUDEPATH += $$PWD/Report/LimeReport/lib64/release/include
    DEPENDPATH += $$PWD/Report/LimeReport/lib64/release/include

    unix:OBJECTS_DIR = common/build/o/unix
    win32:OBJECTS_DIR = common/build/o/win32
    macx:OBJECTS_DIR = common/build/o/mac

    QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$OUT_PWD/../XeomaDetectorsRelease
    #QMAKE_POST_LINK = $$PWD/release_deploy

}
else:win32:CONFIG(debug, debug|release): {
    LIBS += -L$$PWD/Report/LimeReport/lib64/debug/ -llimereportd
    INCLUDEPATH += $$PWD/Report/LimeReport/lib64/debug/include
    DEPENDPATH += $$PWD/Report/LimeReport/lib64/debug/include
    }
