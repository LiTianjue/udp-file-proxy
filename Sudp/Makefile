#########################################  
# Title : Makefile for UDP FILE server
# Author: TianjueLi
###########################################  
INC=  ./inc
#LIB= -lconfig
#LIBPATH = -L./config

CC=gcc
#CFLAGS=-Wall -lpthread -g
CFLAGS= -g -lpthread
  
PRG= Sudp
OBJ= *.o
SRC= *.c
all:$(PRG)
$(PRG):$(OBJ)
	@$(CC) -o $(PRG) $(OBJ) $(CFLAGS)
$(OBJ): $(SRC)
	@$(CC) -c $(SRC) $(CFLAGS) -I$(INC)
#	@$(CC) -c main.c socks5.c Common.c log.c $(LIB) -I $(INC)
#	@$(CC) -o $(OBJ) -c main.c $(LIB) -I $(INC)
      
  
.PRONY:clean install 
clean:
	rm -rf $(PRG) $(OBJ)
install:
	cp $(PRG) /usr/share/udp-file-proxy/bin
