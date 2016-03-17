#!/usr/bin/python

from socket import *
import binascii
import ctypes
import struct

host=''
port= 54321

s= socket(AF_INET,SOCK_DGRAM)
s.setsockopt(SOL_SOCKET,SO_REUSEADDR,1)
s.bind((host,port))

s1 = struct.Struct('ccqqi')
s2 = struct.Struct('i')

def handle_msg(msg):
	upack_data = s1.unpack(msg[:28])
	print upack_data


while 1:
	try:
		message,address = s.recvfrom(8192)
		#print "Got data from ",address,":",message
		print "Got data from ",address
		handle_msg(message)

	except (KeyboardInterrupt,SystemExit):
		raise
