CC = g++

CFLAGS = -I./src
LIBS =  -ltorrent-rasterbar -lpthread
TARGET = a.out

$(TARGET): ./src/ip_country_detector.o ./src/main.o ./src/target_list_reader.o
	LD_LIBRARY_PATH=/usr/local/lib/libtorrent-rasterbar.so $(CC) -o $@ $^ $(LIBDIR) $(LIBS)

./src/ip_country_detector.o: ./src/ip_country_detector.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	 
./src/main.o: ./src/main.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

./src/target_list_reader.o: ./src/target_list_reader.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

all: clean $(OBJS) $(TARGET)

clean:
	rm -f ./src/ip_country_detector.o ./src/main.o ./src/target_list_reader.o $(TARGET) 