@echo off
chcp 65001 >nul
echo ======================================
echo   HotkeyDog C++ 编译工具
echo ======================================
echo.

set OUTPUT=HotkeyDog.exe
set SOURCE=hotkey_dog.cpp
set RCFILE=HotkeyDog.rc
set RESFILE=HotkeyDog.res

if not exist "%SOURCE%" (
    echo [错误] 找不到 %SOURCE%
    pause
    exit /b 1
)

:::: 编译资源文件（如果存在）
set "RES_ARG="
if exist "%RCFILE%" (
    echo [OK] 检测到资源文件 %RCFILE%，正在编译资源...
)

:::: 尝试 MSVC (cl.exe)
where cl >nul 2>&1
if not errorlevel 1 (
    echo [OK] 检测到 MSVC 编译器
    echo.
    echo 正在编译...
    if exist "%RCFILE%" (
        rc %RCFILE%
        cl /EHsc /O2 /utf-8 /DUNICODE /D_UNICODE %SOURCE% %RESFILE% /link comctl32.lib shell32.lib shlwapi.lib user32.lib gdi32.lib /OUT:%OUTPUT%
    ) else (
        cl /EHsc /O2 /utf-8 /DUNICODE /D_UNICODE %SOURCE% /link comctl32.lib shell32.lib shlwapi.lib user32.lib gdi32.lib /OUT:%OUTPUT%
    )
    if errorlevel 1 (
        echo.
        echo [错误] 编译失败
        pause
        exit /b 1
    )
    del /q *.obj 2>nul
    echo.
    echo ======================================
    echo   编译成功！
    echo   输出: %OUTPUT%
    echo ======================================
    pause
    exit /b 0
)

:::: MinGW 编译函数
:set_mingw_path
:::: 优先检查 w64devkit 路径
set "W64DEVKIT=D:\w64devkit\w64devkit\bin"
if exist "%W64DEVKIT%\g++.exe" (
    set "PATH=%W64DEVKIT%;%PATH%"
    goto :do_mingw
)
:::: 检查 PATH 中的 g++
where g++ >nul 2>&1
if errorlevel 1 (
    goto :no_compiler
)

:do_mingw
echo [OK] 检测到 MinGW 编译器
echo.
echo 正在编译 (MinGW, Unicode 模式)...
if exist "%RCFILE%" (
    windres %RCFILE% -O coff -o %RESFILE%
    g++ -O2 -mwindows -municode -static -o %OUTPUT% %SOURCE% %RESFILE% -lcomctl32 -lshlwapi -lgdi32
) else (
    g++ -O2 -mwindows -municode -static -o %OUTPUT% %SOURCE% -lcomctl32 -lshlwapi -lgdi32
)
if errorlevel 1 (
    echo.
    echo [错误] 编译失败
    pause
    exit /b 1
)
echo.
echo ======================================
echo   编译成功！
echo   输出: %OUTPUT%
echo ======================================
pause
exit /b 0

:no_compiler
echo [错误] 未找到 C++ 编译器
echo.
echo 请安装以下任一编译器:
echo.
echo   方案1: 安装 Visual Studio Build Tools
echo     下载: https://visualstudio.microsoft.com/visual-cpp-build-tools/
echo.
echo   方案2: 安装 MinGW-w64
echo     下载: https://www.mingw-w64.org/
echo.
pause
