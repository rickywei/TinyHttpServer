#!/bin/bash

make clean

make
rm -f *.log
sudo ./HttpServer.exe

