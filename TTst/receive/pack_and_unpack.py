#!/usr/bin/python

from socket import *
import struct 
import binascii
import ctypes


#values = (b'\x17',b'\x14',10,20,30,"test_file.txt",40,"abcedfg")
values = (b'\x17',b'\x14',10,20,30)
# char ,char ,longlong ,longlong ,int ,char *,int,char *

s = struct.Struct('ccqqi')
packed_data = s.pack(*values)

upack_data = s.unpack(packed_data)

print "Packed Value:",binascii.hexlify(packed_data)
print "Unpacked Type:",type(upack_data),'Value:',upack_data
