#-------------------------------------------------
#
# Project created by QtCreator 2016-03-15T10:40:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ludo
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    game.cpp \
    ludo_player.cpp \
    ludo_player_random.cpp \
    ludo_player_fast.cpp \
    ludo_player_aggressive.cpp \
    ludo_player_defensive.cpp \
    ludo_player_expert.cpp \
    ludo_player_qlearning.cpp \
    ludo_player_genetic.cpp

HEADERS  += dialog.h \
    game.h \
    ludo_player.h \
    positions_and_dice.h \
    ludo_player_random.h \
    ludo_player_fast.h \
    ludo_player_aggressive.h \
    ludo_player_defensive.h \
    ludo_player_expert.h \
    ludo_player_qlearning.h \
    ludo_player_genetic.h

FORMS    += dialog.ui

DISTFILES += star.png \
             globe.png

LIBS += -lfann

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wunused

# Copy image to the build directory
QMAKE_POST_LINK += cp $$PWD/images/* $$OUT_PWD
