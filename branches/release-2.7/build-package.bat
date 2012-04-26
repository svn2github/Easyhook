echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86_amd64
msbuild build.proj /t:BeforeBuild
msbuild build.proj /t:Build
msbuild build.proj /t:PreparePackage;Package
pause