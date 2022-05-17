#-------------------------------------------------
#
# Project created by QtCreator 2022-04-26T11:36:33
#
#-------------------------------------------------

QT       += core gui
QT    +=charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OSwindow
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    Memory_manager.cpp \
    file_manager.cpp \
        main.cpp \
        mainwindow.cpp \
    dir.cpp \
    disk.cpp \
    editor.cpp \
    file.cpp \
    filetools.cpp \
    mkdir.cpp \
    mkfile.cpp \
    pagetable.cpp \
    procpagedialog.cpp \
    quicktable.cpp \
    cpu.cpp \
    process_manager.cpp \
    device_driver.cpp

HEADERS += \
    editor.h \
    file_dir.h \
    cpu.h \
    device_driver.h \
    memory.h \
    mkdir.h \
    mkfile.h \
    pcb.h \
    process_manager.h \
    constant.h \
    procpagedialog.h \
    mainwindow.h

FORMS += \
        mainwindow.ui \
    editor.ui \
    mainwindow.ui \
    mkdir.ui \
    mkfile.ui \
    procpagedialog.ui

DISTFILES += \
    OSwindow.pro.user \
    OSwindow.pro.user.bfe5b37.4.8-pre1
