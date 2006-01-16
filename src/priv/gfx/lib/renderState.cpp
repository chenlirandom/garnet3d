#include "pch.h"
#if !GN_ENABLE_INLINE
#include "garnet/gfx/renderState.inl"
#endif

// *****************************************************************************
// RenderStateBlockDesc
// *****************************************************************************

const GN::gfx::RenderStateBlockDesc GN::gfx::RenderStateBlockDesc::DEFAULT( RenderStateBlockDesc::RESET_TO_DEFAULT );
const GN::gfx::RenderStateBlockDesc GN::gfx::RenderStateBlockDesc::INVALID( RenderStateBlockDesc::RESET_TO_INVALID );

//
//
// -----------------------------------------------------------------------------
void GN::gfx::RenderStateBlockDesc::reset( ResetFlag flag )
{
    if( RESET_TO_DEFAULT == flag )
    {
        #define GNGFX_DEFINE_RS( tag, defval ) rs[RS_##tag] = RSV_##defval;
        #include "garnet/gfx/renderStateMeta.h"
        #undef GNGFX_DEFINE_RS
    }
    else if( RESET_TO_INVALID == flag )
    {
        #define GNGFX_DEFINE_RS( tag, defval ) rs[RS_##tag] = RSV_INVALID;
        #include "garnet/gfx/renderStateMeta.h"
        #undef GNGFX_DEFINE_RS
    }
}

// *****************************************************************************
// TextureStateBlockDesc
// *****************************************************************************

const GN::gfx::TextureStateBlockDesc GN::gfx::TextureStateBlockDesc::DEFAULT( TextureStateBlockDesc::RESET_TO_DEFAULT );
const GN::gfx::TextureStateBlockDesc GN::gfx::TextureStateBlockDesc::INVALID( TextureStateBlockDesc::RESET_TO_INVALID );

//
//
// -----------------------------------------------------------------------------
void GN::gfx::TextureStateBlockDesc::reset( ResetFlag flag )
{
    if( RESET_TO_DEFAULT == flag )
    {
        #define GNGFX_DEFINE_TS( tag, defval0, defval, d3dname, glname1, glname2 )  \
            ts[0][TS_##tag] = TSV_##defval0;
        #include "garnet/gfx/textureStateMeta.h"
        #undef GNGFX_DEFINE_TS

        for ( int i = 1; i < MAX_TEXTURE_STAGES; ++i )
        {
            #define GNGFX_DEFINE_TS( tag, defval0, defval, d3dname, glname1, glname2 ) \
                ts[i][TS_##tag] = TSV_##defval;
            #include "garnet/gfx/textureStateMeta.h"
            #undef GNGFX_DEFINE_TS
        }
    }
    else if( RESET_TO_INVALID == flag )
    {
        for ( int i = 0; i < MAX_TEXTURE_STAGES; ++i )
        {
            #define GNGFX_DEFINE_TS( tag, defval0, defval, d3dname, glname1, glname2 ) \
                ts[i][TS_##tag] = TSV_INVALID;
            #include "garnet/gfx/textureStateMeta.h"
            #undef GNGFX_DEFINE_TS
        }
    }
}
