#!/usr/bin/python
#-*- coding:utf-8 -*-

from socket import *
import struct
import sys

HOST = 'localhost'
PORT = 9001
BUFSIZE = 1024
ADDR = (HOST,PORT)
app_name = sys.argv[1]


tcpCliSock = socket(AF_INET,SOCK_STREAM)
tcpCliSock.connect(ADDR)

#delect
cmd_word = b'\x01'

#msg="app_name=testApp,server_ip=172.16.2.16,server_port=9001,delect_file=false,max_file=20"
msg="app_name="+app_name

tx_buf = struct.pack('cc',cmd_word,chr(len(msg)));
tx_buf = tx_buf + msg 

tcpCliSock.send(tx_buf);

s=struct.Struct('cc')
rt_buf=tcpCliSock.recv(1024)

rt_msg = s.unpack(rt_buf)

if(rt_msg[0]=='\x00'):
	print "删除应用",app_name,"成功"
else:
	print "删除应用",app_name,"失败",rt_msg



tcpCliSock.close();
