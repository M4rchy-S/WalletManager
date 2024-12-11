QT += core gui
QT += network
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++17
CONFIG -= console
CONFIG -= app_bundle

TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    account.cpp \
    accountcardwidget.cpp \
    eWallet.cpp \
    main.cpp \
    mainwindow.cpp \
    note.cpp \
    notecardwidget.cpp

HEADERS += \
    account.h \
    accountcardwidget.h \
    app_info.rc \
    eWallet.h \
    mainwindow.h \
    note.h \
    notecardwidget.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += language_ru.ts
TRANSLATIONS += language_ua.ts



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



LIBS += -LC:\Users\dexag\OneDrive\Documents\eWallet\dep\sqlite3_x64-windows\lib -lsqlite3
INCLUDEPATH += C:\Users\dexag\OneDrive\Documents\eWallet\dep\sqlite3_x64-windows\include
DEPENDPATH += C:\Users\dexag\OneDrive\Documents\eWallet\dep\sqlite3_x64-windows\include

INCLUDEPATH += C:\Users\dexag\OneDrive\Documents\eWallet\dep\nlohmann-json_x64-windows\include
DEPENDPATH +=C:\Users\dexag\OneDrive\Documents\eWallet\dep\nlohmann-json_x64-windows\include

RESOURCES += \
    icons.qrc \
    languages.qrc

#DISTFILES +=

VERSION = 1.0.0

RC_FILE = app_info.rc



