#!/usr/bin/python
#-*- coding: utf-8 -*-


from socket import *
import struct
import sys

HOST = 'localhost'
#HOST='172.16.2.16'
PORT = 9001
BUFSIZE = 1024
ADDR = (HOST,PORT)
app_name = sys.argv[1]


tcpCliSock = socket(AF_INET,SOCK_STREAM)
tcpCliSock.connect(ADDR)

cmd_word = b'\x01'
#msg="app_name=testApp,server_ip=172.16.2.16,server_port=9001,delect_file=false,max_file=20"
msg="app_name="+app_name+","

tx_buf = struct.pack('cc',cmd_word,chr(len(msg)));
tx_buf = tx_buf + msg 

s=struct.Struct('cc')

tcpCliSock.send(tx_buf);
rt_buf = tcpCliSock.recv(1024)

rt_msg = s.unpack(rt_buf)

#print "recv:",rt_msg
if(rt_msg[0]=='\x00'):
	print "删除应用",app_name,"成功"
elif(rt_msg[0]=='\x02'):
	print "应用",app_name,"不存在"
else:
	print "删除应用",app_name,"失败",rt_msg
	
	

tcpCliSock.close();
