#include "pch.h"
#include "mtrndr.h"

// *************************************************************************
// local functions
// *************************************************************************

static GN::Logger * sLogger = GN::getLogger("GN.gfx.util.rndr");

///
/// Function type to create renderer.
///
typedef GN::gfx::Renderer * (*CreateRendererFunc)( const GN::gfx::RendererOptions & );

#if GN_XENON

///
/// fake OGL renderer creator for Xenon platform
///
inline GN::gfx::Renderer *
GN::gfx::createOGLRenderer( const GN::gfx::RendererOptions & )
{
    GN_ERROR(sLogger)( "No OGL support on Xenon." );
    return 0;
}

#endif

//
// determine renderer API
// -----------------------------------------------------------------------------
static GN::gfx::RendererAPI sDetermineRendererAPI()
{
#if defined(HAS_D3D9)
    return GN::gfx::API_D3D9;
#elif defined(HAS_OGL)
    return GN::gfx::API_OGL;
#else
    return GN::gfx::API_FAKE;
#endif
}

static const GN::Guid RENDERER_DLL_GUID = { 0xa397a51a, 0xa008, 0x4ffd, { 0xbd, 0x8a, 0x7f, 0xd4, 0xa3, 0xaf, 0x59, 0xd7 } };

namespace GN { namespace gfx
{
    //
    //
    // -------------------------------------------------------------------------
    Renderer * createSingleThreadRenderer( const RendererOptions & ro )
    {
        GN_GUARD;

        RendererAPI api = ro.api;

        if( API_AUTO == api ) api = sDetermineRendererAPI();

        // then create new one.
        if( API_FAKE == api )
        {
            GN_UNIMPL();
            return NULL;
        }

#if GN_BUILD_STATIC
        switch( api )
        {
            case API_D3D9  : return createD3D9Renderer( ro );
            case API_D3D10 : return createD3D10Renderer( ro );
            case API_OGL   : return createOGLRenderer( ro );
            default        : GN_ERROR(sLogger)( "Invalid API(%d)", api ); return 0;
        }
#else
        const char * dllName;
        CreateRendererFunc creator;
        switch( api )
        {
            case API_D3D9  : dllName = "GNrndrD3D9"; break;
            case API_D3D10 : dllName = "GNrndrD3D10"; break;
            case API_OGL   : dllName = "GNrndrOGL"; break;
            default        : GN_ERROR(sLogger)( "Invalid API(%d)", api ); return 0;
        }
        std::auto_ptr<SharedLib> dll( new SharedLib );
        if( !dll->load( dllName ) ) return 0;
        creator = (CreateRendererFunc)dll->getSymbol( "GNgfxCreateRenderer" );
        if( !creator ) return 0;
        Renderer * r = creator( ro );
        if( 0 == r ) return 0;
        SharedLib * dllptr = dll.get();
        r->setUserData( RENDERER_DLL_GUID, &dllptr, sizeof(dllptr) );
        dll.release();
        return r;
#endif
        GN_UNGUARD;
    }

    //
    //
    // -------------------------------------------------------------------------
    Renderer * createMultiThreadRenderer( const RendererOptions & ro )
    {
        GN_GUARD;

        MultiThreadRenderer * r = new MultiThreadRenderer;
        if( !r->init( ro ) ) delete r, r = NULL;
        return r;

        GN_UNGUARD;
    }

    //
    //
    // -------------------------------------------------------------------------
    void deleteRenderer( Renderer * r )
    {
        GN_GUARD;

        if( r )
        {
            bool hasdll = r->hasUserData( RENDERER_DLL_GUID );
            SharedLib * dll = NULL;
            if( hasdll ) r->getUserData( RENDERER_DLL_GUID, &dll, sizeof(dll) );
            delete r;
            if( hasdll ) delete dll;
        }

        GN_UNGUARD;
    }
}}