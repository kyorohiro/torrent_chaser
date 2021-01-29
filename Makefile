CC = g++

SRCS=./src/my_ip_country_detector.cpp ./src/my_target_list_reader.cpp ./src/my_server.cpp
OBJS= $(SRCS:.cpp=.o)
TARGET = main.out
TEST_TARGET= test_my_ip_country_detector.out
CFLAGS = -I./src
LIBS =  -ltorrent-rasterbar -lpthread

main.out: ./src/my_ip_country_detector.o ./src/main.o ./src/my_target_list_reader.o ./src/my_server.o
	LD_LIBRARY_PATH=/usr/local/lib/libtorrent-rasterbar.so $(CC) -o $@ $^ $(LIBDIR) $(LIBS)

test_my_ip_country_detector.out: ./src/test_my_ip_country_detector.cpp ./src/my_ip_country_detector.o
	$(CC) -o $@ $^ $(LIBDIR) $(LIBS) $(CFLAGS)

./src/my_ip_country_detector.o: ./src/my_ip_country_detector.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	 
./src/main.o: ./src/main.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

./src/my_target_list_reader.o: ./src/my_target_list_reader.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

./src/my_server.o: ./src/my_server.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@



all: clean $(OBJS) $(TARGET) $(TEST_TARGET)

clean:
	rm -f ./src/*.o ./*.out
