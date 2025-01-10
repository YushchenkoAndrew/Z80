GCC=g++
EM_CC=em++

SOURCE=src
LIB=-I./ -I/usr/local/include/libcamera
INCLUDE=
# INCLUDE=include/*.cpp lib/*.cpp
LUA_SOURCE=$(SOURCE)/lua

DIRS=
TARGET=$(SOURCE)/*.cpp $(foreach dir, $(DIRS), $(addprefix $(SOURCE)/, $(dir))/*.cpp)
COMPILED_TARGET=lua54/emsc/lua54.o

OUTPUT_GCC=build
OUT_NAME_GCC=project.o

CFLAGS_GCC=-lcamera -lcamera-base -lpthread -lpng -lstdc++fs -std=c++17 
LDFALGS_GCC=

all:
	if [ ! -d "./build" ]; then mkdir $(OUTPUT_GCC); fi
	$(GCC) $(TARGET) $(INCLUDE) $(LIB) $(CFLAGS_GCC) -O3 -o $(OUTPUT_GCC)/$(OUT_NAME_GCC)

init:
	if [ ! -d "./assets" ]; then mkdir ./assets; fi
	if [ ! -d "./build" ]; then mkdir $(OUTPUT_GCC); fi
	if [ ! -d "./src" ]; then mkdir ./src; fi
	if [ ! -d "./lib" ]; then mkdir ./lib; fi
	if [ ! -d "./include" ]; then mkdir ./include; fi

debug:
	if [ ! -d "./build" ]; then mkdir $(OUTPUT_GCC); fi
	$(GCC) $(TARGET) $(INCLUDE) $(LIB) $(CFLAGS_GCC) -O0 -ggdb -o $(OUTPUT_GCC)/$(OUT_NAME_GCC)

run:
	LD_LIBRARY_PATH='$(shell pwd)' ./$(OUTPUT_GCC)/$(OUT_NAME_GCC)
