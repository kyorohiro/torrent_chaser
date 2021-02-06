CC = g++

SRCS=./src/my_ip_country_detector.cpp ./src/my_server.cpp ./src/my_db.o ./src/my_torrent.o
OBJS= $(SRCS:.cpp=.o)
TARGET = main.out
TEST_TARGET= test_my_ip_country_detector.out
CFLAGS = -I./src -DTHREADSAFE=1. -std=c++14
LIBS = -ltorrent-rasterbar -lpthread -lsqlite3 -lboost_filesystem
LD_LIBRARY_PATH:=/usr/local/lib/libtorrent-rasterbar.so

main.out: ./src/my_ip_country_detector.o ./src/main.o ./src/my_server.o ./src/my_db.o ./src/my_torrent.o ./src/my_base_encode.o
	$(CC) -o $@ $^ $(LIBDIR) $(LIBS)

test_my_ip_country_detector.out: ./src/test_my_ip_country_detector.cpp ./src/my_ip_country_detector.o
	$(CC) -o $@ $^ $(LIBDIR) $(LIBS) $(CFLAGS)

./src/my_ip_country_detector.o: ./src/my_ip_country_detector.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	 
./src/main.o: ./src/main.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

./src/my_server.o: ./src/my_server.cpp 
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

./src/my_db.o: ./src/my_db.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

./src/my_torrent.o: ./src/my_torrent.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

./src/my_base_encode.o: ./src/my_base_encode.cpp
	 $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

all: $(OBJS) $(TARGET) $(TEST_TARGET)
test: $(OBJS) $(TEST_TARGET)
clean:
	rm -f ./src/*.o ./*.out
