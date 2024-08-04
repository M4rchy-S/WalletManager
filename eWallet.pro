QT       += core gui

QT -= gui

QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    account.cpp \
    eWallet.cpp \
    main.cpp \
    mainwindow.cpp \
    note.cpp

HEADERS += \
    account.h \
    eWallet.h \
    mainwindow.h \
    note.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



#win32: LIBS += -L$$PWD/../../Desktop/eWalletLib/lib/ -leWallet

#INCLUDEPATH += $$PWD/../../Desktop/eWalletLib/include
#DEPENDPATH += $$PWD/../../Desktop/eWalletLib/include

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../Desktop/eWalletLib/lib/eWallet.lib

#win32: LIBS += -LD:\Programs\vcpkg\packages\sqlite3_x64-windows\lib -lsqlite3
#INCLUDEPATH += D:\Programs\vcpkg\packages\sqlite3_x64-windows\include
#DEPENDPATH += D:\Programs\vcpkg\packages\sqlite3_x64-windows\include


#win32: LIBS += -LC:\Users\AdminP\Desktop\eWalletLib\lib -leWallet
#INCLUDEPATH += C:\Users\AdminP\Desktop\eWalletLib\include
#DEPENDPATH += C:\Users\AdminP\Desktop\eWalletLib\include
#win32:!win32-g++: PRE_TARGETDEPS += C:\Users\AdminP\Desktop\eWalletLib\lib\eWallet.lib

LIBS += -LC:\Users\AdminP\Documents\eWallet\dep\sqlite3_x64-windows\lib -lsqlite3
INCLUDEPATH += C:\Users\AdminP\Documents\eWallet\dep\sqlite3_x64-windows\include
DEPENDPATH += C:\Users\AdminP\Documents\eWallet\dep\sqlite3_x64-windows\include

INCLUDEPATH += C:\Users\AdminP\Documents\eWallet\dep\nlohmann-json_x64-windows\include
DEPENDPATH += C:\Users\AdminP\Documents\eWallet\dep\nlohmann-json_x64-windows\include
