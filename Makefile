TARGET   = dust_test 

SRCDIR   = src

ALLWARNINGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Werror -Wno-unused

#CC       = g++
#CC       = gcc
CC       = g++-6
STD		= c++1z
INCLUDE	 = -Iinclude
LIBS		= -lshpp -lcryptopp
CFLAGS   = -std=$(STD) -g -fPIC $(ALLWARNINGS) $(INCLUDE) $(LIBS)

export

all: $(TARGET)

$(TARGET): src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp -o $(TARGET)

clean:
	@rm -f $(TARGET)
