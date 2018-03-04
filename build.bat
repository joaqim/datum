@echo off
setlocal
cd /D %0\..
set MSBUILD_EXE="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\amd64\MSBuild.exe"

@echo off
if not defined DEV_ENV_DIR (
	REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
	call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
	p:
	REM cd /D %0\..
)
set DEV_ENV_DIR=???
IF NOT EXIST build mkdir build
pushd build

set CMAKE_FLAGS=-DCMAKE_BUILD_TYPE=Debug
if not "%~1"=="" (
	@echo on
	cmake -G "Visual Studio 15 2017 Win64" -T v141 .. %CMAKE_FLAGS%	
	@echo off
)
@echo on
%MSBUILD_EXE% DATUM.sln /nologo /v:q /property:GenerateFullPaths=true /property:Configuration=Debug /p:PlatformToolset=v141
@echo off
set BUILD_STATUS=%ERRORLEVEL%
REM devenv vorp.vcxproj /build Debug
REM MSBuild.exe vorp.vcxproj /property:Configuration=Debug

REM GOTO :POPD

@echo off
if %BUILD_STATUS% == 0 (
	pushd bin\Debug
    echo Starting datum.exe
    datum.exe
	popd
)

:POPD
popd

endlocal
