@echo off

set opts=-FC -GR- -EHa- -nologo -Zi -DIMGUI
set code=%cd%
set links=User32.lib Winmm.lib Gdi32.lib wsock32.lib Ws2_32.lib opengl32.lib 
pushd ..\..\build
cl %opts% %code%\main.cpp -Fecustom_rollback.exe -link %links%
popd
