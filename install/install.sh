#!/bin/bash

# 创建应用目录
mkdir /usr/share/udp-file-proxy  2>>./install.log
mkdir /usr/share/udp-file-proxy/bin 2>>./install.log
mkdir /usr/share/udp-file-proxy/etc 2>>./install.log
mkdir /usr/share/udp-file-proxy/log	2>>./install.log
cp -r  ./udp-file-proxy  /usr/share/ 2>>./install.log

if [[ $1 == "server" ]]
then
	echo "Install for Server( File receive)"
	#设置的时候要区分服务端还是客户端
	# 增大接收缓冲区的大小
	sysctl -w net.core.rmem_max=16777216
	sysctl -w net.core.rmem_default=16777216
	# 增加接收队列的容量
	sysctl -w kernel.msgmnb=16777216
	sysctl -p

elif [[ $1 == "client" ]]
then
	echo "Install for Client( File Send)"
	cp -f ./udp-file-proxy/etc/udp-file-proxy-client.cfg /usr/share/udp-file-proxy/etc/udp-file-proxy.cfg
else
	echo "Usage :"
	echo "$0 client/server"
	exit
fi
chmod +x /usr/share/udp-file-proxy/bin/*



