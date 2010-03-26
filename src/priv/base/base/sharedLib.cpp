#include "pch.h"
#include "garnet/GNbase.h"

#if GN_MSWIN
#define SHLIB_LOAD( libName )          ((void*)LoadLibraryA(libName))
#define SHLIB_FREE( lib )              (!!FreeLibrary(HMODULE(lib)))
#define SHLIB_LOAD_SYMBOL( lib, symb ) ((void*)GetProcAddress(HMODULE(lib), symb))
#define SHLIB_ERROR()                  ::GN::GetWin32LastErrorInfo()
#define SHLIB_EXT                      ".dll"
#elif GN_POSIX
#include <dlfcn.h>
#include <stdio.h>
#define SHLIB_LOAD( libName )          ((void*)dlopen(libName, RTLD_NOW))
#define SHLIB_FREE( lib )              (!dlclose( lib ))
#define SHLIB_LOAD_SYMBOL( lib, symb ) ((void*)dlsym(lib, symb))
#define SHLIB_ERROR()                  dlerror()
#if GN_CYGWIN
#define SHLIB_EXT                      ".dll"
#else
#define SHLIB_EXT                      ".so"
#endif
#else
#error "Unknown platform!"
#endif

static GN::Logger * sLogger = GN::GetLogger("GN.base.SharedLib");

//
//
// -----------------------------------------------------------------------------
bool GN::SharedLib::Load( const char * libName )
{
    GN_GUARD;

    // Free old library
    Free();

    // check parameter
    if( IsStringEmpty(libName) )
    {
        GN_ERROR(sLogger)( "Library name can't be empty!" );
        return false;
    }

    // Handle libName
    StrA filename;
    if( fs::ExtName(libName).Empty() )
        filename = StrA(libName) + SHLIB_EXT;
    else
        filename = libName;

    // Load library
    mHandle = SHLIB_LOAD( filename.ToRawPtr() );
    if( 0 == mHandle )
    {
        GN_ERROR(sLogger)( "Fail to Load library %s: %s!", filename.ToRawPtr(), SHLIB_ERROR() );
        return false;
    }

    GN_TRACE(sLogger)( "Load library '%s'.", filename.ToRawPtr() );

    // success
    mFileName = filename;
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::SharedLib::Free()
{
    GN_GUARD;

    if( mHandle )
    {
        SHLIB_FREE( mHandle );
        mHandle = 0;
        GN_TRACE(sLogger)( "Unload library '%s'.", mFileName.ToRawPtr() );
    }

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void * GN::SharedLib::GetSymbol( const char * symbol )
{
    GN_GUARD;

    if( 0 == mHandle )
    {
        GN_ERROR(sLogger)( "Shared library is uninitialized!" );
        return 0;
    }

    if( IsStringEmpty(symbol) )
    {
        GN_ERROR(sLogger)( "Symbol name can't be empty!" );
        return 0;
    }

    void * result = SHLIB_LOAD_SYMBOL( mHandle, symbol );
    if( 0 == result )
    {
        GN_ERROR(sLogger)( "Failed to get symbol named '%s': %s.", symbol, SHLIB_ERROR() );
    }

    return result;

    GN_UNGUARD;
}
