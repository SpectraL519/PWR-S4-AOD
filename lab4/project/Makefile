# TODO: os detection
# SHELL ALLIASSES
RM := del *.exe

# PROJECT STRUCTURE
APP_DIR := ./app
SRC_DIR := ./src
INCLUDE_DIR := ./include

# FILES
SRC := $(wildcard $(SRC_DIR)/*.cpp)
APP_SRC := $(wildcard $(APP_DIR)/*.cpp) $(SRC)

LIB := 

H_INC := $(foreach d, $(INCLUDE_DIR), -I $d)
L_INC := $(foreach l, $(LIB), -l$l)

# EXEC
EXEC := flow

# CXX
CXX := g++
CXXSTD := -std=c++2a

# FLAGS
# ifeq ($(CXX), clang++)
# 	CXXFLAGS += -Weverything -pedantic -Wno-c++98-compat -g
# else
# 	CXXFLAGS += -Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g
# endif

CXXFLAGS += $(CXXSTD)
# $(GGDB) $(OPT)

# MAKE
all: 
	$(CXX) -o $(EXEC) $(APP_SRC) $(CXXFLAGS) $(H_INC) $(L_INC)

clean:
	$(RM)
