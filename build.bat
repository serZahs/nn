@echo off
set Arg=%1
set CompilerFlags=-nologo -Zi -W4 -WX -D_CRT_SECURE_NO_WARNINGS

IF NOT EXIST build mkdir build
pushd build
if "%Arg%"=="test" (
	cl %CompilerFlags% ..\test.c
) else (
	cl %CompilerFlags% ..\main.c
)
popd