#!/bin/bash

#设置的时候要区分服务端还是客户端
# 增大接收缓冲区的大小
#sysctl -w net.core.rmem_max=16777216
#sysctl -w net.core.rmem_default=16777216

# 增加接收队列的容量
#sysctl -w kernel.msgmnb=16777216

#sysctl -p

# 创建应用目录
mkdir /usr/share/udp-file-proxy  2>>./install.log
mkdir /usr/share/udp-file-proxy/bin 2>>./install.log
mkdir /usr/share/udp-file-proxy/etc 2>>./install.log
mkdir /usr/share/udp-file-proxy/log	2>>./install.log

cp -r  ./udp-file-proxy  /usr/share/ 2>>./install.log
