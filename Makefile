TARGET   = dust_test 

SRCDIR   = src

CC       = g++
INCLUDE	 = -Iinclude
CFLAGS   = -std=c++11 -fPIC -Wall $(INCLUDE) -lcryptopp

export

all: $(TARGET)

$(TARGET): src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp -o $(TARGET)

clean:
	@rm -f $(TARGET)
