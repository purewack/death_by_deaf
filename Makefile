TARGET := MAC

IFLAGS := --std=c++1z -Wno-narrowing -Wno-switch
IFLAGS += -Iinclude -I/usr/local/include -I/usr/local/include/lua

LFLAGS := -L/usr/local/lib  
LFLAGS += -lm -lraylib -llua

ifeq ($(TARGET),RPI)
LDFLAGS += -L/opt/vc/lib 
LDLIBS += -lbrcmGLESv2 -lbrcmEGL -lbcm_host 
endif

all:
	g++ --std=c++17 main.cpp $(IFLAGS) $(LFLAGS) -o lua_motif 
