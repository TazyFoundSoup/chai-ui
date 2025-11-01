@echo off
echo Compiling sources...
cl /c /EHsc src\Chai\Inter\Display.cpp /Iinclude /Iinclude\Chai\Inter >nul 2>&1
echo Turning objects into library...
lib Display.obj /OUT:ChaiUI.lib >nul 2>&1
echo Success!
echo ChaiUI.lib is now located in the current directory
