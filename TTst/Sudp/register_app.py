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
app_name     = sys.argv[1]
server_ip    = sys.argv[2]
server_port  = sys.argv[3]
message_ip   = sys.argv[4]
message_port = sys.argv[5]
dir1         = sys.argv[6]
dir2         = sys.argv[7]
move_file    = sys.argv[8]


tcpCliSock = socket(AF_INET,SOCK_STREAM)
tcpCliSock.connect(ADDR)

cmd_word = b'\x00'
#msg="app_name=testApp,server_ip=172.16.2.16,server_port=9001,delect_file=false,max_file=20"
msg="app_name="+app_name+","
msg=msg + "server_ip="+server_ip + ","
msg=msg + "server_port="+server_port+ ","
msg=msg + "message_ip="+message_ip + ","
msg=msg + "message_port="+message_port + ","
msg=msg + "dir1=" + dir1 +","
msg=msg + "dir2=" + dir2 + ","
msg=msg + "move=" +move_file+ ","


tx_buf = struct.pack('cc',cmd_word,chr(len(msg)));
tx_buf = tx_buf + msg 

s=struct.Struct('cc')

tcpCliSock.send(tx_buf);
rt_buf = tcpCliSock.recv(1024)

rt_msg = s.unpack(rt_buf)

#print "recv:",rt_msg
if(rt_msg[0]=='\x00'):
	print "注册应用",app_name,"成功"
elif(rt_msg[0]=='\x01'):
	print "应用",app_name,"已经存在"
else:
	print "注册应用",app_name,"失败",rt_msg
	
	

tcpCliSock.close();
