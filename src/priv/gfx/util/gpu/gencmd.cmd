@echo off

if not exist mtgpuCommandMeta.txt (
    echo ERROR: mtgpuCommandMeta.txt not found!
    goto :EOF
)

set TARGET=mtgpuCmd.h

REM generate file header
echo //>%TARGET%
echo // DO NOT EDIT. Generated by "gencmd.cmd">>%TARGET%
echo //>>%TARGET%
echo.>>%TARGET%
echo namespace GN { namespace gfx>>%TARGET%
echo {>>%TARGET%
echo.>>%TARGET%

REM generate enum table
echo ///>>%TARGET%
echo /// Gpu command enumerations.>>%TARGET%
echo ///>>%TARGET%
echo enum GpuCommand {>>%TARGET%
for /F "tokens=1*" %%a in ( mtgpuCommandMeta.txt ) do echo.    CMD_%%a,%%b>>%TARGET%
echo };>>%TARGET%
echo.>>%TARGET%

REM generate handler table
echo ///>>%TARGET%
echo /// Gpu command handler type.>>%TARGET%
echo ///>>%TARGET%
echo typedef void (*GpuCommandHandler)( Gpu ^& r, void * param, uint32 size );>>%TARGET%
echo.>>%TARGET%
echo ///>>%TARGET%
echo /// Gpu command handler table.>>%TARGET%
echo ///>>%TARGET%
echo extern const GpuCommandHandler g_gpuCommandHandlers[];>>%TARGET%
echo.>>%TARGET%

REM generate end of file
echo }} // end of file>>%TARGET%


set TARGET=mtgpuCmd.cpp

REM file header
echo //>%TARGET%
echo // DO NOT EDIT. Generated by "gencmd.cmd">>%TARGET%
echo //>>%TARGET%
echo.>>%TARGET%
echo #include "pch.h">>%TARGET%
echo #include "mtgpuCmd.h">>%TARGET%
echo.>>%TARGET%
echo namespace GN { namespace gfx>>%TARGET%
echo {>>%TARGET%
echo.>>%TARGET%

REM function prototypes
for /F "tokens=1" %%a in ( mtgpuCommandMeta.txt ) do echo void func_%%a( Gpu ^&, void *, uint32 );>>%TARGET%
echo.>>%TARGET%

REM handler table
echo const GpuCommandHandler g_gpuCommandHandlers[] = {>>%TARGET%
for /F "tokens=1" %%a in ( mtgpuCommandMeta.txt ) do echo ^&func_%%a,>>%TARGET%
echo };>>%TARGET%
echo.>>%TARGET%

echo }} // end of file>>%TARGET%

REM done
echo DONE!