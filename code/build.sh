#!/bin/bash

code="$PWD"
opts=-g
cd ..\..\build > /dev/null
g++ $opts $code/main.cpp -o custom_rollback.exe
cd $code > /dev/null
