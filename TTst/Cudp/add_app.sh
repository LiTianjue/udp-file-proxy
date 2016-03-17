#!/bin/bash
# arg1 : app_name
# arg2 : server_ip
# arg3 : server_port
# arg4 : del_file
# arg5 : max_queue

echo "Sending..."
#./register_app.py appTest1 192.168.1.110 8002 true 20
#./register_app.py appTest2 192.168.1.110 9001 true 20
#./register_app.py appTest2
#./register_app.py appTest3

./register_app.py appTest1 172.16.2.16 9001 true 20
./register_app.py appTest2 172.16.2.16 9002 true 20
