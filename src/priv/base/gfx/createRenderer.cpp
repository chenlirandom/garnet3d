#include "pch.h"

// *************************************************************************
// local functions
// *************************************************************************

//!
//! Function prototype to create instance of renderer.
//!
typedef GN::gfx::Renderer * (*CreateRendererFunc)();

//
// create fake renderer
//
extern GN::gfx::Renderer * createFakeRenderer();

#if GN_STATIC

//
// create D3D9 renderer
//
#if GN_MSWIN
extern GN::gfx::Renderer * createD3D9Renderer();
#else
inline GN::gfx::Renderer * createD3D9Renderer()
{ GN_ERROR( "No D3D9 support on platform other than MS Windows." ); return 0; }
#endif

//
// create D3D10 renderer
//
#if GN_MSWIN && !GN_XENON
extern GN::gfx::Renderer * createD3D10Renderer();
#else
inline GN::gfx::Renderer * createD3D10Renderer()
{ GN_ERROR( "No D3D10 support on platform other than MS Vista." ); return 0; }
#endif

//
// create OGL renderer
//
#if GN_XENON
inline GN::gfx::Renderer * createOGLRenderer()
{ GN_ERROR( "No OGL support on Xenon." ); return 0; }
#else
extern GN::gfx::Renderer * createOGLRenderer();
#endif

#endif

//
// determine renderer API
// -----------------------------------------------------------------------------
static GN::gfx::RendererAPI sDetermineRendererAPI()
{
#if GN_XENON
    return GN::gfx::API_D3D9;
#elif GN_MSWIN
    return GN::gfx::API_D3D9;
#else
    return GN::gfx::API_OGL;
#endif
}

// *************************************************************************
// public functions
// *************************************************************************

//
//
// -------------------------------------------------------------------------
GN::gfx::Renderer * GN::gfx::createRenderer( RendererAPI api )
{
    GN_GUARD;

    // release old renderer
    deleteRenderer();

    if( API_AUTO == api ) api = sDetermineRendererAPI();

    // then create new one.
    if( API_FAKE == api ) return createFakeRenderer();
#if GN_STATIC
    switch( api )
    {
        case API_D3D9  : return createD3D9Renderer();
        case API_D3D10 : return createD3D10Renderer();
        case API_OGL   : return createOGLRenderer();
        default        : GN_ERROR( "Invalid API(%d)", api ); return 0;
    }
#else
    const char * dllName;
    CreateRendererFunc creator;
    switch( api )
    {
        case API_D3D9  : dllName = "GNgfxD3D9"; break;
        case API_D3D10 : dllName = "GNgfxD3D10"; break;
        case API_OGL   : dllName = "GNgfxOGL"; break;
        default        : GN_ERROR( "Invalid API(%d)", api ); return 0;
    }
    if( !Renderer::msSharedLib.load( dllName ) ) return 0;
    creator = (CreateRendererFunc)Renderer::msSharedLib.getSymbol( "GNgfxCreateRenderer" );
    if( !creator ) return 0;
    return creator();
#endif
    GN_UNGUARD;
}

//
//
// -------------------------------------------------------------------------
void GN::gfx::deleteRenderer()
{
    GN_GUARD;

    delete gRendererPtr;
    Renderer::msSharedLib.free();

    GN_UNGUARD;
}
