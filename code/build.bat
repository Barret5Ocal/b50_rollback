@echo off

set opts=-FC -GR- -EHa- -nologo -Zi -DIMGUI -DIOCP=0
set code=%cd%
pushd ..\..\build
cl %opts% %code%\main.cpp -Feclient.exe User32.lib Winmm.lib Gdi32.lib ws2_32.lib Winmm.lib opengl32.lib Xinput.lib
popd
