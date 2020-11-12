#!/bin/bash

if [ ! -d "./build" ]; then
  mkdir ./build
else
  rm -rf ./build/*
fi

cp -r ./pages ./build 
cd build

cmake ..
make

echo "start..."
sudo ./HttpServer.exe

