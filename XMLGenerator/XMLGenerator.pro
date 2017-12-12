#-------------------------------------------------
#
# Project created by QtCreator 2017-09-11T10:38:13
#
#-------------------------------------------------

QT       += core gui
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XMLGenerator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    MainWindow.cpp \
    AddFieldWindow.cpp \
    TableRow.cpp \
    TableCell.cpp \
    Table.cpp \
    FileController.cpp \
    TableController.cpp \
    AddAttributeWindow.cpp\
    CacheConfig.cpp\
    FieldInfo.cpp \
    UpdateMetaDataWindow.cpp \
    Change.cpp \
    AddEnum.cpp \
    Enum.cpp \
    FindDiff.cpp \
    Version.cpp \
    CommitWindow.cpp

HEADERS += \
    MainWindow.h \
    AddFieldWindow.h \
    TableRow.h \
    TableCell.h \
    Table.h \
    FileController.h \
    TableController.h \
    AddAttributeWindow.h\
    CacheConfig.h\
    FieldInfo.h \
    UpdateMetaDataWindow.h \
    Change.h \
    AddEnum.h \
    Enum.h \
    FindDiff.h \
    Version.h \
    CommitWindow.h

FORMS += \
    AddFieldWindow.ui \
    MainWindow.ui \
    AddAttributeWindow.ui \
    UpdateMetaDataWindow.ui \
    AddEnum.ui \
    FindDiff.ui \
    CommitWindow.ui

RESOURCES += \
    res.qrc
