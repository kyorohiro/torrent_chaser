CC = g++

CFLAGS = -I./src
LIBS =  -ltorrent-rasterbar -lpthread
TARGET = a.out

$(TARGET): ./src/my_ip_country_detector.o ./src/main.o ./src/my_target_list_reader.o ./src/my_server.o
	LD_LIBRARY_PATH=/usr/local/lib/libtorrent-rasterbar.so $(CC) -o $@ $^ $(LIBDIR) $(LIBS)

./src/my_ip_country_detector.o: ./src/my_ip_country_detector.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	 
./src/main.o: ./src/main.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

./src/my_target_list_reader.o: ./src/my_target_list_reader.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

./src/my_server.o: ./src/my_server.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@


all: clean $(OBJS) $(TARGET)

clean:
	rm -f ./src/my_ip_country_detector.o ./src/main.o ./src/my_target_list_reader.o ./src/my_server.o $(TARGET) 