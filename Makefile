TARGET := MAC

IFLAGS := --std=c++17 -Wno-narrowing -Wno-switch
IFLAGS += -Iinclude -I/usr/local/include

LFLAGS := -L/usr/local/lib -L/usr/lib -L/usr/local/lib/lua
LFLAGS += -lpthread -lm -lraylib -llua -lrtaudio -lrtmidi

ifeq ($(TARGET),MAC)
LFLAGS += -Llib/macos
endif
ifeq ($(TARGET),ARM)
LFLAGS += -Llib/arm -L/opt/vc/lib
LFLAGS += -ldl -lGL -lX11 -lrt -lbrcmGLESv2 -lbrcmEGL -lbcm_host
endif
ifeq ($(TARGET),LINUX)
LFLAGS += -Llib/linux -ldl -lGL -lX11 -lrt
endif

all:
	g++ $(IFLAGS) main.cpp $(LFLAGS) -o lua_motif
run:
	./lua_motif
