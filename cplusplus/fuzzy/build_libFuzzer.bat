@echo off

call environment.bat

svn co https://llvm.org/svn/llvm-project/compiler-rt/trunk/lib/fuzzer libFuzzer
cd libFuzzer && clang -c -g -O2 -std=c++11 *.cpp 
ar cr libFuzzer.a libFuzzer/*.o
