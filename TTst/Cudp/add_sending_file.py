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
# get commandline opt
app_name = sys.argv[1]
file_name = sys.argv[2]
delect = sys.argv[3]


tcpCliSock = socket(AF_INET,SOCK_STREAM)
tcpCliSock.connect(ADDR)

cmd_word = b'\x10'
# testApp
msg="app_name=" + app_name +",file_name="+file_name +",delect="+delect

tx_buf = struct.pack('cc',cmd_word,chr(len(msg)));
tx_buf = tx_buf + msg 

tcpCliSock.send(tx_buf);
#tcpCliSock.send(msg);
s=struct.Struct('cc')
rt_buf=tcpCliSock.recv(1024)

rt_msg = s.unpack(rt_buf)

if(rt_msg[0]=='\x00'):
	print "添加文件",file_name,"成功"
else:
	print "添加文件",file_name,"失败",rt_msg

tcpCliSock.close();
