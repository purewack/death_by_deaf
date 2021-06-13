TARGET := MAC

IFLAGS := --std=c++17 -Wno-narrowing -Wno-switch
IFLAGS += -Iinclude -I/usr/local/include -I/usr/local/include/lua

LFLAGS := -L/usr/local/lib
LFLAGS += -lpthread -ldl -lGL -lX11 -lrt -lm -lraylib -llua

ifeq ($(TARGET),RPI)
LFLAGS += -L/opt/vc/lib 
LLIBS += -lbrcmGLESv2 -lbrcmEGL -lbcm_host
endif

all:
	g++ $(IFLAGS) main.cpp $(LFLAGS) -o lua_motif 
