#!/usr/bin/python
#-*- coding:utf-8 -*-

from socket import *
import struct
import sys

HOST = 'localhost'
#HOST='172.16.2.16'
PORT = 9001
BUFSIZE = 1024
ADDR = (HOST,PORT)
app_name = sys.argv[1]
server_ip = sys.argv[2]
server_port = sys.argv[3]
del_file = sys.argv[4]
max_queue = sys.argv[5]


tcpCliSock = socket(AF_INET,SOCK_STREAM)
tcpCliSock.connect(ADDR)

cmd_word = b'\x00'
msg = "app_name="+app_name+","
msg = msg + "server_ip="+server_ip+","
msg = msg + "server_port="+server_port+","
#msg = msg + "delect_file="+del_file+","
msg = msg + "max_file="+max_queue+","

tx_buf = struct.pack('cc',cmd_word,chr(len(msg)));
tx_buf = tx_buf + msg 

s=struct.Struct('cc')

tcpCliSock.send(tx_buf);

rt_buf=tcpCliSock.recv(1024)

rt_msg = s.unpack(rt_buf)

if(rt_msg[0]=='\x00'):
	print "注册应用",app_name,"成功"
else:
	print "注册应用",app_name,"失败",rt_msg

tcpCliSock.close();
