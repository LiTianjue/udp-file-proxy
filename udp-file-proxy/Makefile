CC = g++

CFLAGS=-fpermissive
LDFLAGS =-L./lib -Wl,-Bdynamic -lpthread -lrt -Wl,-Bstatic -levent -Wl,-Bdynamic

TARGET = udp-file-proxy

SRC = *.cpp mhp_lib/*.cpp

all:*.o
	@$(CC) -o $(TARGET) *.o $(LDFLAGS) $(CFLAGS)

*.o:$(SRC)
	@$(CC) -c $(SRC)  $(CFLAGS)

install:
	cp $(TARGET) ../release

clean:
	rm -rf $(TARGET)
	rm -f *.o

