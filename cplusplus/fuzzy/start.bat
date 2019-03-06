@echo off


call environment.bat
mkdir examples
clang++ -O1 -g -fsanitize=address -fno-omit-frame-pointer example_UseAfterFree.cc -o %CD%/examples/UseAfterFree.exe
clang++ -O1 -g -fsanitize=signed-integer-overflow,null,alignment -fno-sanitize-recover=null -fsanitize-trap=alignment example_Undefined.cc -o %CD%/examples/Undefined.exe
rem clang++ -O2 -g -fsanitize=memory -fno-omit-frame-pointer example_MemorySanitizer.cc -o %CD%/examples/MemorySanitizer.exe
del %CD%\examples\*.exp %CD%\examples\*.lib %CD%\examples\*.pdb %CD%\examples\*.ilk
pause


