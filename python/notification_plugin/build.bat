@echo off

set PATH=E:\Python27\;D:\msys64\mingw64\bin;D:\msys64\usr\bin;%PATH%
set CFLAGS="-I D:/msys64/usr/include"

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
rem call D:\VisualStudio\2017\Community\VC\Auxiliary\Build\vcvars32.bat

rem curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
rem python get-pip.py

rem cd yowsup
rem python -m pip install --upgrade pip
rem pip install setuptools
rem python setup.py install
python E:\Python27\Scripts\yowsup-cli registration --config-phone 9032243895 
pause