# OS-Specific Configurations
ifeq ($(OS),Windows_NT)
    SHELL := cmd.exe
    RM := rmdir /s /q
    MKDIR := mkdir
    CCFLAGS += -D WIN32
    RAYLIB_FLAGS += -lgdi32 -lwinmm
    RAYLIB_LIB_DIR += raylib/lib/win64
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CCFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CCFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CCFLAGS += -D IA32
        endif
    endif
else
    SHELL := bash
    RM := rm -rf
    MKDIR := mkdir -p
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -D LINUX
        RAYLIB_FLAGS += -lGL -lm -lpthread -ldl -lrt -lX11
        RAYLIB_LIB_DIR += raylib/lib/linux64
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -D OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif
