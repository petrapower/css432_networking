#!/bin/bash
echo "building socket server"
g++ --std=c++11 -o socket_server server_main.cpp
