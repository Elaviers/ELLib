msbuild %~dp0ELLib.sln /p:configuration="release" /p:platform="x64"
msbuild %~dp0ELLib.sln /p:configuration="debug" /p:platform="x64"
msbuild %~dp0ELLib.sln /p:configuration="release" /p:platform="x86"
msbuild %~dp0ELLib.sln /p:configuration="debug" /p:platform="x86"