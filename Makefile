TARGET := MAC

IFLAGS := -ggdb --std=c++17 -Wno-narrowing -Wno-switch
IFLAGS += -Iinclude -I/usr/local/include -I/usr/local/include/lua
LFLAGS := -L/usr/local/lib -L/usr/lib
LFLAGS += -lpthread -lm -lraylib -llua -lrtaudio -lrtmidi -ljack

ifeq ($(TARGET),MAC)

endif

ifeq ($(TARGET),LNX)
LFLAGS += -ldl -lX11 -lrt -lGL
endif

ifeq ($(TARGET),ARM)
LFLAGS += -L/opt/vc/lib
LFLAGS += -ldl -lGL -lX11 -lrt -lbrcmGLESv2 -lbrcmEGL -lbcm_host
endif

all:
	g++ $(IFLAGS) src/main.cpp src/audio.cpp $(LFLAGS) -o lua_motif
run:
	./lua_motif
