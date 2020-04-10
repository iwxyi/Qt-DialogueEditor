QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    dialogue_editor/

SOURCES += \
    dialogue_editor/dialogueavatar.cpp \
    dialogue_editor/dialoguebubble.cpp \
    dialogue_editor/dialoguebucket.cpp \
    dialogue_editor/dialogueeditor.cpp \
    dialogue_editor/dialoguenarrator.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    dialogue_editor/dialogueavatar.h \
    dialogue_editor/dialoguebubble.h \
    dialogue_editor/dialoguebucket.h \
    dialogue_editor/dialogueeditor.h \
    dialogue_editor/dialoguenarrator.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
