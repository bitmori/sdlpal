# Adapted from Makefile for Dingux by Rikku2000

TARGET = sdlpal

HOST =

ADPLUG_FILES = src/adplug/rix.cpp src/adplug/player.cpp src/adplug/binio.cpp \
	src/adplug/fprovide.cpp src/adplug/binfile.cpp src/adplug/dosbox_opl.cpp \
	src/adplug/fmopl.c src/adplug/surroundopl.cpp src/adplug/emuopl.cpp

LIBMAD_FILES = src/libmad/bit.c src/libmad/decoder.c src/libmad/fixed.c src/libmad/frame.c \
	src/libmad/huffman.c src/libmad/layer12.c src/libmad/layer3.c src/libmad/music_mad.c \
	src/libmad/stream.c src/libmad/synth.c src/libmad/timer.c

TOML_FILES = src/toml/toml.c

FILES = src/rixplay.cpp src/text.c src/font.c src/itemmenu.c src/scene.c src/palcommon.c src/script.c \
	src/util.c src/play.c src/getopt.c src/input.c src/uibattle.c src/game.c src/magicmenu.c src/map.c \
	src/ending.c src/uigame.c src/rngplay.c src/ui.c src/global.c src/main.c src/fight.c \
	src/video.c src/palette.c src/sound.c src/res.c src/battle.c src/yj1.c src/mbridge.c

FILES += $(ADPLUG_FILES)
FILES += $(LIBMAD_FILES)
FILES += $(TOML_FILES)

CFLAGS = `sdl2-config --cflags` -g -Wall -O2 -fno-strict-aliasing -Isrc/mruby/include
LDFLAGS = `sdl2-config --libs` -lstdc++ -lm -Lsrc/mruby/build/host/lib -lmruby -lmruby_core

.PHONY: all clean prepare

all: $(TARGET) prepare

$(TARGET):
	$(HOST)gcc $(CFLAGS) -o $(TARGET) $(FILES) $(LDFLAGS)

mruby:
	cd src/mruby; make

prepare: $(TARGET)
	mkdir -p palgame
	mv -f $(TARGET) palgame

clean:
	rm -f $(TARGET)
