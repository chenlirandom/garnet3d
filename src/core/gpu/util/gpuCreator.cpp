#include "pch.h"
#include "mtgpu.h"

using namespace GN;
using namespace GN::gfx;

// *************************************************************************
// local functions
// *************************************************************************

static GN::Logger * sLogger = GN::getLogger("GN.gfx.util.gpu");

// ***********************************************************************
// Public functions
// ***********************************************************************

//
//
// -------------------------------------------------------------------------
#if !GN_BUILD_HAS_OGL
GN_API Gpu * GN::gfx::createOGLGpu( const GpuOptions &, uint32 )
{
    GN_ERROR(sLogger)( "OpenGL renderer is not available." );
    return 0;
}
#endif

//
//
// -------------------------------------------------------------------------
#if !GN_BUILD_HAS_D3D11
GN_API Gpu * GN::gfx::createD3DGpu( const GpuOptions &, uint32 )
{
    GN_ERROR(sLogger)( "D3D11 renderer is not available." );
    return 0;
}
#endif

//
//
// -------------------------------------------------------------------------
//#if !GN_BUILD_HAS_D3D12
//GN_API Gpu2 * GN::gfx::Gpu2::createD3D12Gpu2(const CreationParameters &)
//{
//    GN_ERROR(sLogger)( " D3D12 renderer is not available." );
//    return 0;
//}
//#endif

//
//
// -------------------------------------------------------------------------
GN_API Gpu * GN::gfx::createGpu( const GpuOptions & inputOptions, uint32 creationFlags )
{
    GpuOptions ro = inputOptions;

    if( GpuAPI::AUTO == ro.api ) ro.api = GpuAPI::OGL;

    switch( ro.api )
    {
        case GpuAPI::OGL   : return createOGLGpu( ro, creationFlags );
        case GpuAPI::D3D11 : return createD3DGpu( ro, creationFlags );
        case GpuAPI::FAKE  : GN_UNIMPL(); return 0;
        default : GN_ERROR(sLogger)( "Invalid API(%d)", ro.api.toRawEnum() ); return 0;
    }
}

/*
//
// -------------------------------------------------------------------------
GN_API Gpu2 * GN::gfx::Gpu2::createGpu2(const CreationParameters & cp)
{
    switch( cp.api )
    {
        case GraphicsAPI::AUTO :
        case GraphicsAPI::D3D12 : return createD3D12Gpu2(cp);
        case GraphicsAPI::VULKAN :
        default : GN_UNIMPL(); return nullptr;
    }
}*/
