#include "pch.h"
#include "xenonRenderer.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.xenon");

///
/// 根据显示模式和option设定present parameters
// ------------------------------------------------------------------------
static bool
sSetupD3dpp( D3DPRESENT_PARAMETERS   & d3dpp,
             const GN::gfx::DispDesc & dd,
             GN::gfx::MsaaType         msaa,
             bool                      vsync )
{
    GN_GUARD;

    using namespace GN::gfx;

    // clear all field
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    // back buffer parameters
    GN_ASSERT( dd.height > 0 && dd.width > 0 );
    d3dpp.BackBufferWidth  = dd.width;
    d3dpp.BackBufferHeight = dd.height;
    d3dpp.BackBufferFormat = D3DFMT_A8B8G8R8;
    d3dpp.BackBufferCount  = 0;
    d3dpp.DisableAutoBackBuffer = false;

    // msaa parameters
    switch( msaa )
    {
        case MSAA_NONE   : d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE; break;
        case MSAA_LOW    :
        case MSAA_MEDIUM : d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES; break;
        case MSAA_HIGH   :
        case MSAA_ULTRA  : d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES; break;
        default          : GN_ERROR(sLogger)( "invalid MSAA type: %d", msaa ); return false;
    }

    // depth parameters
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

    // front buffer parameters
    d3dpp.DisableAutoFrontBuffer = false;
    d3dpp.FrontBufferFormat      = D3DFMT_LE_X2R10G10B10;
    d3dpp.FrontBufferColorSpace  = D3DCOLORSPACE_YCbCr709; // the color space corresponding to HDTV.

    // misc. parameters
    d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow              = 0;
    d3dpp.Windowed                   = false;
    d3dpp.Flags                      = 0;
    d3dpp.FullScreen_RefreshRateInHz = 0;
    d3dpp.PresentationInterval       = vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

    // success
    return true;

    GN_UNGUARD;
}

// *****************************************************************************
// init/quit
// *****************************************************************************


//
//
// -----------------------------------------------------------------------------
bool GN::gfx::XenonRenderer::dispInit()
{
    GN_GUARD;

    // create d3d object
    mD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if( 0 == mD3D )
    {
        GN_ERROR(sLogger)( "Fail to create D3D object!" );
        return false;
    }

    const RendererOptions & ro = getOptions();
    const DispDesc        & dd = getDispDesc();

    // init d3d present parameters
    if( !sSetupD3dpp( mD3DPresentParameters, dd, ro.msaa, ro.vsync ) ) return false;

    // default behavior flags.
    mD3DBehavior  = 0;

    // create device
    GN_DX9_CHECK_RV(
        mD3D->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            0,
            mD3DBehavior,
            &mD3DPresentParameters,
            &mDevice ),
        false );

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonRenderer::dispQuit()
{
    GN_GUARD;

    PIXPERF_FUNCTION_EVENT();

    safeRelease( mDevice );
    safeRelease( mD3D );

    GN_UNGUARD;
}
