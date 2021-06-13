all:
	g++ --std=c++17 main.cpp -Iinclude -I/usr/local/include/lua -L/usr/local/lib/ -llua -lraylib -o engine 
