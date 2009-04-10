#include "pch.h"
#include "d3d10Renderer.h"
#include "d3d10RenderTargetMgr.h"
#include "d3d10StateObject.h"
#include "d3d10Shader.h"
#include "d3d10Texture.h"
#include "d3d10VtxLayout.h"
#include "d3d10Buffer.h"

// *****************************************************************************
// local functions
// *****************************************************************************

// *****************************************************************************
// init/shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10Renderer::contextInit()
{
    GN_GUARD;

    mSOMgr = new D3D10StateObjectManager( *this );
    if( 0 == mSOMgr ) return false;

    // create render target manager
    mRTMgr = new D3D10RTMgr( *this );
    if( !mRTMgr->init() ) return false;

    // bind default context
    rebindContext();

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D10Renderer::contextQuit()
{
    GN_GUARD;

    mContext.clear();

    safeDelete( mRTMgr );

    safeDelete( mSOMgr );

    GN_UNGUARD;
}

// *****************************************************************************
// from BasicRenderer
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D10Renderer::bindContextImpl( const RendererContext & newContext, bool skipDirtyCheck )
{
    PIXPERF_FUNCTION_EVENT();

    //
    // Parameter check
    //
    if( paramCheckEnabled() )
    {
        // TODO: verify data in new context
        // TODO: make sure all fields in current context are valid.
    }

    if( !bindContextStates( newContext, skipDirtyCheck ) ) return false;

    if( !bindContextResources( newContext, skipDirtyCheck ) ) return false;

    return true;
}

// *****************************************************************************
// private functions
// *****************************************************************************


//
//
// -----------------------------------------------------------------------------
inline bool GN::gfx::D3D10Renderer::bindContextStates(
    const RendererContext & newContext,
    bool                    skipDirtyCheck )
{
    //
    // bind shaders
    //
    if( newContext.gpuProgram )
    {
        D3D10GpuProgram * p = (D3D10GpuProgram*)newContext.gpuProgram.get();
        p->apply();
    }
    else
    {
        mDevice->VSSetShader( NULL );
        mDevice->GSSetShader( NULL );
        mDevice->PSSetShader( NULL );
    }

    /*
    // bind render states
    //
    if( newFlags.rsb )
    {
        const RenderStateBlockDesc & newrsb = newContext.rsb;
        GN_ASSERT( newrsb.valid() );

        if( skipDirtyCheck )
        {
            #define GNGFX_DEFINE_RS( tag, type, defval, minVal, maxVal ) \
                if( newrsb.isSet(RS_##tag) ) mSOMgr->Set_##tag( newrsb.get(RS_##tag) );
            #include "garnet/gfx/renderStateMeta.h"
            #undef GNGFX_DEFINE_RS
        }
        else
        {
            const RenderStateBlockDesc & oldrsb = mContext.rsb;

            #define GNGFX_DEFINE_RS( tag, type, defval, minVal, maxVal ) \
                GN_ASSERT( oldrsb.isSet( RS_##tag ) ); \
                if( newrsb.isSet(RS_##tag) && newrsb.get(RS_##tag) != oldrsb.get(RS_##tag) ) \
                    mSOMgr->Set_##tag( newrsb.get(RS_##tag) );
            #include "garnet/gfx/renderStateMeta.h"
            #undef GNGFX_DEFINE_RS
        }

        mSOMgr->apply();
    }*/

    //
    // bind render targets
    //
    bool needRebindViewport = false;
    if( !mRTMgr->bind(
            newContext.crts,
            newContext.dsrt,
            mContext.crts,
            newContext.dsrt,
            skipDirtyCheck,
            needRebindViewport ) )
    {
        return false;
    }

    // bind viewport
    const Vector2<UInt32> & rtsize = mRTMgr->getRenderTargetSize();
    const Rect<UInt32> & newvp = newContext.viewport;
    D3D10_VIEWPORT d3dvp;
    d3dvp.MinDepth = 0.0f;
    d3dvp.MaxDepth = 1.0f;
    if( 0 == newvp.x && 0 == newvp.y && 0 == newvp.w && 0 == newvp.h )
    {
        d3dvp.TopLeftX = 0;
        d3dvp.TopLeftY = 0;
        d3dvp.Width    = rtsize.x;
        d3dvp.Height   = rtsize.y;
    }
    else
    {
        UInt32 l = newvp.x;
        UInt32 t = newvp.y;
        UInt32 r = newvp.x + newvp.w;
        UInt32 b = newvp.y + newvp.h;

        math::clamp<UInt32>( l, 0, rtsize.width );
        math::clamp<UInt32>( t, 0, rtsize.height );
        math::clamp<UInt32>( r, 0, rtsize.width );
        math::clamp<UInt32>( b, 0, rtsize.height );

        d3dvp.TopLeftX = l;
        d3dvp.TopLeftY = t;
        d3dvp.Width    = r - l;
        d3dvp.Height   = b - t;
    }
    mDevice->RSSetViewports( 1, &d3dvp );

    // update scissor (same as viewport)
    D3D10_RECT scissor = {
        (long)( d3dvp.TopLeftX ),
        (long)( d3dvp.TopLeftY ),
        (long)( d3dvp.TopLeftX+d3dvp.Width ),
        (long)( d3dvp.TopLeftY+d3dvp.Height ),
    };
    mDevice->RSSetScissorRects( 1, &scissor );

    return true;
}

//
//
// -----------------------------------------------------------------------------
inline bool GN::gfx::D3D10Renderer::bindContextResources(
    const RendererContext & newContext,
    bool                    skipDirtyCheck )
{
    GN_UNUSED_PARAM( skipDirtyCheck );

    /*
    // bind vertex format
    //
    if( newContext.vtxfmt )
    {
        const D3D10VtxLayoutDesc * layout;
        layout = &mVtxFmts[newContext.vtxfmt];
        GN_ASSERT( layout->layout );
        if( newContext.vtxfmt != mContext.vtxfmt || skipDirtyCheck )
        {
            mDevice->IASetInputLayout( layout->layout );
        }
    }*/

    ///
    /// bind vertex buffers
    ///
    ID3D10Buffer * buf[RendererContext::MAX_VERTEX_BUFFERS];
    UINT           strides[RendererContext::MAX_VERTEX_BUFFERS];
    UINT           offsets[RendererContext::MAX_VERTEX_BUFFERS];
    for( UINT i = 0; i < RendererContext::MAX_VERTEX_BUFFERS; ++i )
    {
        const AutoRef<VtxBuf> & vb = newContext.vtxbufs[i];
        buf[i]     = vb ? safeCastPtr<const D3D10VtxBuf>(vb.get())->getD3DBuffer() : NULL;
        strides[i] = newContext.strides[i];
        offsets[i] = 0;
    }
    mDevice->IASetVertexBuffers( 0, RendererContext::MAX_VERTEX_BUFFERS, buf, strides, offsets );

    //
    // bind index buffer
    //
    if( newContext.idxbuf != mContext.idxbuf || skipDirtyCheck )
    {
        if( newContext.idxbuf )
        {
            const D3D10IdxBuf * ib = (const D3D10IdxBuf*)newContext.idxbuf.get();

            const IdxBufDesc & ibdesc = ib->getDesc();

            mDevice->IASetIndexBuffer(
                ib->getD3DBuffer(),
                ibdesc.bits32 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT,
                0 );
        }
        else
        {
            mDevice->IASetIndexBuffer( NULL, DXGI_FORMAT_R16_UINT, 0 );
        }
    }

    //
    // bind textures
    //
    /*
    UINT maxStages = getCaps(CAPS_MAX_TEXTURE_STAGES);
    UINT numTex = min<UINT>( (UINT)newContext.numTextures, maxStages );
    UINT stage;
    for( stage = 0; stage < numTex; ++stage )
    {
        const Texture * tex = newContext.textures[stage];
        if( tex != mContext.textures[stage] ||
            stage > mContext.numTextures ||
            !skipDirtyCheck )
        {
            if( tex )
            {
                safeCastPtr<const D3D10Texture>(tex)->bind( stage );
            }
            else
            {
                mDevice->SetTexture( stage, NULL );
            }
        }
    }
    // clear unused stages
    numTex = min<UINT>( (UINT)mContext.numTextures, maxStages );
    for( ; stage < numTex; ++stage )
    {
        mDevice->SetTexture( stage, 0 );
    }
    */

    return true;
}
