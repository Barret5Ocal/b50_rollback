#!/bin/bash

code="$PWD"
opts=-g
cd ..\..\build > /dev/null
g++ $opts $code/main.cpp -o client.exe
cd $code > /dev/null
