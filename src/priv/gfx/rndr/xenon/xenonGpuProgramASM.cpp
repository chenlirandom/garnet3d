#include "pch.h"
#include "xenonShader.h"
#include "xenonRenderer.h"
#include "garnet/GNd3d9.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.xenon");

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::XenonGpuProgramASM::init( const GpuProgramDesc & desc )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( XenonGpuProgramASM, () );

    GN_ASSERT( GPL_ASM == desc.lang );

    IDirect3DDevice9 & dev = getRenderer().getDeviceInlined();

    if( desc.vs.code )
    {
        mVs = d3d9::ShaderCompiler<IDirect3DVertexShader9>::assemble( dev, desc.vs.code );
        if( NULL == mVs ) return failure();
    }

    if( desc.ps.code )
    {
        mPs = d3d9::ShaderCompiler<IDirect3DPixelShader9>::assemble( dev, desc.ps.code );
        if( NULL == mPs ) return failure();
    }

    // success
    return success();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpuProgramASM::quit()
{
    GN_GUARD;

    safeRelease( mVs );
    safeRelease( mPs );

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// from XenonBasicGpuProgram
// *****************************************************************************

/*
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpuProgramASM::apply() const
{
    GN_GUARD_SLOW;

    GN_ASSERT( mD3DShader );

    LPDIRECT3DDEVICE9 dev = getRenderer().getDevice();

    GN_DX9_CHECK( dev->SetVertexShader( mD3DShader ) );

    // apply ALL uniforms to D3D device
    UInt32 handle = getFirstUniform();
    while( handle )
    {
        applyUniform( dev, getUniform( handle ) );
        handle = getNextUniform( handle );
    }
    clearDirtySet();

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::XenonGpuProgramASM::applyDirtyUniforms() const
{
    GN_GUARD_SLOW;

    GN_ASSERT( mD3DShader );

    LPDIRECT3DDEVICE9 dev = getRenderer().getDevice();

    const std::set<UInt32> dirtySet = getDirtyUniforms();
    std::set<UInt32>::const_iterator i, e = dirtySet.end();
    for( i = dirtySet.begin(); i != e; ++i )
    {
        applyUniform( dev, getUniform( *i ) );
    }
    clearDirtySet();

    GN_UNGUARD_SLOW;
}

// *****************************************************************************
// from Shader
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::XenonGpuProgramASM::queryDeviceUniform( const char * name, HandleType & userData ) const
{
    GN_GUARD;

    GN_ASSERT( !strEmpty(name) );

    unsigned int index;

    if( !str2Int<unsigned int>( index, name+1 ) )
    {
        GN_ERROR(sLogger)( "invalid register name: %s", name );
        return false;
    }

    D3DAsmConstDesc desc;

    switch( name[0] )
    {
        case 'c':
        case 'C':
            if( index >= mMaxConstF )
            {
                GN_ERROR(sLogger)( "register index(%d) is too large. (max: %d)", index, mMaxConstF );
                return false;
            }
            desc.type = CONST_F;
            break;

        case 'i':
        case 'I':
            if( index >= mMaxConstI )
            {
                GN_ERROR(sLogger)( "register index(%d) is too large. (max: %d)", index, mMaxConstI );
                return false;
            }
            desc.type = CONST_I;
            break;

        case 'b':
        case 'B':
            if( index >= mMaxConstB )
            {
                GN_ERROR(sLogger)( "register index(%d) is too large. (max: %d)", index, mMaxConstB );
                return false;
            }
            desc.type = CONST_B;
            break;

        default:
            GN_ERROR(sLogger)( "invalid register name: %s", name );
            return false;
    }

    // set user data
    desc.index = (UInt16)index;
    userData = (HandleType)desc.u32;

    // success
    return true;

    GN_UNGUARD;
}


// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::XenonGpuProgramASM::analyzeUniforms( const DWORD * shaderFunction )
{
    GN_GUARD;

    GN_ASSERT( shaderFunction );
    DWORD version = D3DXGetShaderVersion( shaderFunction );

    if( 0 == version )
    {
        GN_ERROR(sLogger)( "Fail to get shader version!" );
        return false;
    }

    D3DCAPS9 caps;
    GN_DX9_CHECK_RV( getRenderer().getDevice()->GetDeviceCaps( &caps ), false );

    if( HIWORD(version) >= 3 ) // vs_3_x
    {
        mMaxConstF = caps.MaxVertexShaderConst;
        mMaxConstI = 16;
        mMaxConstB = 16;
    }
    else if( HIWORD(version) >= 2 ) // vs_2_x
    {
        mMaxConstF = caps.MaxVertexShaderConst;
        mMaxConstI = 16;
        mMaxConstB = 16;
    }
    else // vs_1_x
    {
        mMaxConstF = caps.MaxVertexShaderConst;
        mMaxConstI = 0;
        mMaxConstB = 0;
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// --------------------------------------------------------------------------------------
inline void
GN::gfx::XenonGpuProgramASM::applyUniform( LPDIRECT3DDEVICE9 dev, const Uniform & u ) const
{
    GN_GUARD_SLOW;

    D3DAsmConstDesc desc;

    desc.u32 = (UInt32)u.userData;

    switch( desc.type )
    {
        case CONST_F :
            switch( u.value.type )
            {
                case UVT_FLOAT:
                    if( u.value.floats.empty() ) return;
                    // FIXME : may read memory beyond the end of array, if (array_size % 4 != 0)
                    GN_DX9_CHECK( dev->SetVertexShaderConstantF( desc.index, &u.value.floats[0], (UINT)( u.value.floats.size() + 3 ) / 4 ) );
                    break;

                case UVT_VECTOR4:
                    if( u.value.vector4s.empty() ) return;
                    GN_DX9_CHECK( dev->SetVertexShaderConstantF( desc.index, u.value.vector4s[0], (UINT)u.value.vector4s.size() ) );
                    break;

                case UVT_MATRIX44:
                    if( u.value.matrix44s.empty() ) return;
                    GN_DX9_CHECK( dev->SetVertexShaderConstantF( desc.index, u.value.matrix44s[0][0], (UINT)(u.value.matrix44s.size()*4) ) );
                    break;

                case UVT_BOOL:
                case UVT_INT:
                    GN_ERROR(sLogger)( "Setting integer or boolean value to float shader register does not work." );
                    break;

                default :
                    GN_ERROR(sLogger)( "invalid uniform type." );
            }
            break;

        case CONST_I :
            switch( u.value.type )
            {
                case UVT_INT:
                    if( u.value.floats.empty() ) return;
                    // FIXME : may read memory beyond the end of array, if (array_size % 4 != 0)
                    GN_DX9_CHECK( dev->SetVertexShaderConstantI( desc.index, (const int*)&u.value.ints[0], (UINT)( u.value.ints.size() + 3 ) / 4 ) );
                    break;

                case UVT_FLOAT:
                case UVT_VECTOR4:
                case UVT_MATRIX44:
                case UVT_BOOL:
                    GN_ERROR(sLogger)( "integer register accepts only integer value." );
                    break;

                default :
                    GN_ERROR(sLogger)( "invalid uniform type." );
            }
            break;

        case CONST_B :
            switch( u.value.type )
            {
                case UVT_BOOL:
                    if( u.value.floats.empty() ) return;
                    // FIXME : may read memory beyond the end of array, if (array_size % 4 != 0)
                    GN_DX9_CHECK( dev->SetVertexShaderConstantB( desc.index, (const BOOL*)&u.value.bools[0], (UINT)u.value.bools.size() ) );
                    break;

                case UVT_FLOAT:
                case UVT_VECTOR4:
                case UVT_MATRIX44:
                case UVT_INT:
                    GN_ERROR(sLogger)( "Bool register accepts only boolean value." );
                    break;

                default :
                    GN_ERROR(sLogger)( "invalid uniform type." );
            }
            break;

        default:
            // Program should not reach here.
            GN_ERROR(sLogger)( "invalid register type!" );
            GN_UNEXPECTED();
    }

    GN_UNGUARD_SLOW;
}*/
