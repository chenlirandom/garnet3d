@echo off

pushd %~dp0..
set GARNET_ROOT=%cd%
popd

REM ============================
REM setup garnet build parameers
REM ============================

if "" == "%GN_BUILD_COMPILER%" set GN_BUILD_COMPILER=vc80
if "" == "%GN_BUILD_VARIANT%" set GN_BUILD_VARIANT=debug

REM ==========
REM setup path
REM ==========
if "AMD64" == "%PROCESSOR_ARCHITECTURE%" (
    set mypath=%GARNET_ROOT%\env\bin\mswin\x64;%GARNET_ROOT%\env\bin\mswin\x86
) else (
    set mypath=%GARNET_ROOT%\env\bin\mswin\x86
)
set PATH=%mypath%;%PATH%
set mypath=

REM =======================
REM setup xenon environment
REM =======================
if "xenon" == "%GN_BUILD_COMPILER%" (
    if "" == "%XEDK%" (
        echo Environment variable XEDK not found.
    ) else (
        pushd .
        call %XEDK%\bin\win32\xdkvars.bat
        popd
    )
)


REM ===========
REM setup scons
REM ===========
echo SCons Directory : %GARNET_ROOT%\env\scons
set PATH=%GARNET_ROOT%\env\scons\Scripts;%PATH%
set SCONS_LIB_DIR=%GARNET_ROOT%\env\scons\Lib

REM ===========
REM setup alias
REM ===========

alias root      "cd /d %GARNET_ROOT%\$*"
alias bin       "cd /d %GARNET_ROOT%\bin\$*"
alias bld       "cd /d %GARNET_ROOT%\bin\build.tmp\scons\win32\%GN_BUILD_COMPILER%\%GN_BUILD_VARIANT%\bin\$*"
alias env       "cd /d %GARNET_ROOT%\env\$*"
alias msvc      "cd /d %GARNET_ROOT%\msvc"
alias src       "cd /d %GARNET_ROOT%\src\$*"
alias extern    "cd /d %GARNET_ROOT%\src\extern\$*"
alias media     "cd /d %GARNET_ROOT%\src\media\$*"
alias priv      "cd /d %GARNET_ROOT%\src\priv\$*"
alias base      "cd /d %GARNET_ROOT%\src\priv\base\$*"
alias core      "cd /d %GARNET_ROOT%\src\priv\core\$*"
alias doc       "cd /d %GARNET_ROOT%\src\priv\doc$*"
alias inc       "cd /d %GARNET_ROOT%\src\priv\inc\garnet\$*"
alias gfx       "cd /d %GARNET_ROOT%\src\priv\gfx\$*"
alias misc      "cd /d %GARNET_ROOT%\src\priv\misc$*"
alias sample    "cd /d %GARNET_ROOT%\src\priv\sample\$*"
alias test      "cd /d %GARNET_ROOT%\src\priv\test\$*"
alias tool      "cd /d %GARNET_ROOT%\src\priv\tool$*"

REM =====
REM Misc.
REM =====

set INCLUDE=
set LIBS=

REM ============
REM End of setup
REM ============

echo Garnet root : %GARNET_ROOT%
cd /d %GARNET_ROOT%

REM =========================
REM Call user specific script
REM =========================

echo User name: %USERNAME%
if exist env\user\%USERNAME%.cmd call env\user\%USERNAME%.cmd
