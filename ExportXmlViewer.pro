#-------------------------------------------------
#
# Project created by QtCreator 2015-01-30T22:30:42
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ExportXmlViewer
TEMPLATE = app


SOURCES += main.cpp\
        QExportXmlViewer.cpp \
    CFGMgr.cpp \
    QDisplaySettingDlg.cpp \
    QExportXmlParser.cpp \
    qparameterlistwidget.cpp \
    qpatientinfowidget.cpp \
    qwaveinfodlg.cpp \
    qwaveswidget.cpp \
    RenderArea.cpp \
    scalecalculator.cpp

HEADERS  += QExportXmlViewer.h \
    cfgid.h \
    CFGMgr.h \
    QDisplaySettingDlg.h \
    QExportXmlParser.h \
    qexportxmlstruct.h \
    qexportxmltagname.h \
    qparameterlistwidget.h \
    qpatientinfowidget.h \
    qwaveinfodlg.h \
    qwaveswidget.h \
    RenderArea.h \
    scalecalculator.h

FORMS    += ui/qexportxmlviewer.ui \
    ui/qdisplaysettingdlg.ui \
    ui/qwaveinfodlg.ui

DISTFILES += \
    ui/app.png \
    ui/help.png \
    ui/information.png \
    ui/medianbeattemplates.png \
    ui/open.png \
    ui/parameters.png \
    ui/patient.png \
    ui/settings.png \
    ui/waveforms.png \
    ui/exportxmlviewer.ico

RESOURCES += \
    ui/qexportxmlviewer.qrc
