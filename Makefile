CC = g++

CFLAGS = -I./src
SRCS = ./src/ip_country_detector.cpp ./src/main.cpp
OBJS = $(SRCS:.cpp=.o)
LIBS =  -ltorrent-rasterbar -lpthread
TARGET = a.out

$(TARGET): ./src/ip_country_detector.o ./src/main.o
	LD_LIBRARY_PATH=/usr/local/lib/libtorrent-rasterbar.so $(CC) -o $@ $^ $(LIBDIR) $(LIBS)

./src/ip_country_detector.o: ./src/ip_country_detector.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	 
./src/main.o: ./src/main.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

all: clean $(OBJS) $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *.d