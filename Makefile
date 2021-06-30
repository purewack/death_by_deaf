TARGET := MAC

ifeq ($(TARGET),MAC)
IFLAGS := --std=c++17 -Wno-narrowing -Wno-switch
IFLAGS += -Iinclude -I/usr/local/include -I/usr/local/include/lua
LFLAGS := -L/usr/local/lib -L/usr/lib
LFLAGS += -lpthread -lm -lraylib -llua -lrtaudio -lrtmidi
endif

ifeq ($(TARGET),ARM)
IFLAGS := --std=c++17 -Wno-narrowing -Wno-switch
IFLAGS += -Iinclude -I/usr/local/include -I/usr/local/include/lua
LFLAGS := -L/usr/local/lib -L/usr/lib
LFLAGS += -lpthread -lm -lraylib -llua -lrtaudio -lrtmidi
LFLAGS += -L/opt/vc/lib
LFLAGS += -ldl -lGL -lX11 -lrt -lbrcmGLESv2 -lbrcmEGL -lbcm_host
endif

all:
	g++ $(IFLAGS) main.cpp $(LFLAGS) -o lua_motif
run:
	./lua_motif
