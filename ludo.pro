#-------------------------------------------------
#
# Project created by QtCreator 2016-03-15T10:40:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ludo
TEMPLATE = app


SOURCES += src/main.cpp\
           src/dialog.cpp \
           src/game.cpp \
           src/ludo_player.cpp \
           src/ludo_player_random.cpp \
           src/ludo_player_fast.cpp \
           src/ludo_player_aggressive.cpp \
           src/ludo_player_defensive.cpp \
           src/ludo_player_expert.cpp \
           src/ludo_player_qlearning.cpp \
           src/ludo_player_genetic.cpp

HEADERS  += src/dialog.h \
            src/game.h \
            src/ludo_player.h \
            src/positions_and_dice.h \
            src/ludo_player_random.h \
            src/ludo_player_fast.h \
            src/ludo_player_aggressive.h \
            src/ludo_player_defensive.h \
            src/ludo_player_expert.h \
            src/ludo_player_qlearning.h \
            src/ludo_player_genetic.h

FORMS    += dialog.ui

DISTFILES += images/star.png \
             images/globe.png

LIBS += -lfann

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wunused

# Copy image to the build directory
QMAKE_POST_LINK += cp $$PWD/images/* $$OUT_PWD
