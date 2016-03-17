#!/bin/bash
# argv1 app_name
# argv2 server_ip
# argv3 server_port
# argv4 message_ip
# argv5 message_port
# argv6 dir1
# argv7 dir2
# argv8 move_file


# argv1 app_name



echo "Adding..."
./register_app.py appTest1 0.0.0.0 8001 172.16.2.16 10001 /run/shm /root/file false
./register_app.py appTest2 0.0.0.0 8002 172.16.2.16 10002 /run/shm /root/file true
#./register_app.py appTest2
#./register_app.py appTest3
