#include "pch.h"
#include "d3d9Shader.h"
#include "d3d9Renderer.h"
#include "garnet/GNd3d9.h"

// *****************************************************************************
// Initialize and shutdown
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9PxlShaderHlsl::init( const StrA & code, const StrA & hints )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( D3D9PxlShaderHlsl, () );

    mCode = code;
    setHints( hints );

    if( !deviceCreate() || !deviceRestore() )
    {
        quit(); return selfOK();
    }

    // success
    return selfOK();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9PxlShaderHlsl::quit()
{
    GN_GUARD;

    deviceDispose();
    deviceDestroy();

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// *****************************************************************************
// from D3D9Resource
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::gfx::D3D9PxlShaderHlsl::deviceCreate()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    GN_ASSERT( !mConstTable && !mD3DShader );

    const Registry & hints = getHints();

    mD3DShader = d3d9::compilePS(
        getRenderer().getDevice(),
        mCode.cstr(),
        mCode.size(),
        0, // flags
        hints.getKey( "entry", StrA("main") ).S().cstr(),
        hints.getKey( "target", StrA("") ).S().cstr(),
        &mConstTable );
    if( 0 == mD3DShader ) return false;

    // update userdata of all uniforms
    uint32_t handle = getFirstUniform();
    while( handle )
    {
        Uniform & u = getUniform( handle );
        if( !queryDeviceUniform( u.name.cstr(), u.userData ) ) return false;
        handle = getNextUniform( handle );
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9PxlShaderHlsl::deviceDestroy()
{
    GN_GUARD;

    _GNGFX_DEVICE_TRACE();

    safeRelease( mD3DShader );
    safeRelease( mConstTable );

    GN_UNGUARD;
}


// *****************************************************************************
// from D3D9BasicShader
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9PxlShaderHlsl::apply() const
{
    GN_GUARD_SLOW;

    GN_ASSERT( mD3DShader );

    LPDIRECT3DDEVICE9 dev = getRenderer().getDevice();

    GN_DX9_CHECK( dev->SetPixelShader( mD3DShader ) );

    // apply ALL uniforms to D3D device
    uint32_t handle = getFirstUniform();
    while( handle )
    {
        applyUniform( dev, mConstTable, getUniform( handle ) );
        handle = getNextUniform( handle );
    }
    clearDirtySet();

    GN_UNGUARD_SLOW;
}

//
//
// -----------------------------------------------------------------------------
void GN::gfx::D3D9PxlShaderHlsl::applyDirtyUniforms() const
{
    GN_GUARD_SLOW;

    GN_ASSERT( mD3DShader );

    LPDIRECT3DDEVICE9 dev = getRenderer().getDevice();

    const std::set<uint32_t> dirtySet = getDirtyUniforms();
    std::set<uint32_t>::const_iterator i, e = dirtySet.end();
    for( i = dirtySet.begin(); i != e; ++i )
    {
        applyUniform( dev, mConstTable, getUniform( *i ) );
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
bool GN::gfx::D3D9PxlShaderHlsl::queryDeviceUniform( const char * name, HandleType & userData ) const
{
    GN_GUARD;

    GN_ASSERT( !strEmpty(name) );

    GN_ASSERT( mConstTable );

    D3DXHANDLE h = mConstTable->GetConstantByName( NULL, name );
    if( 0 == h )
    {
        GN_ERROR( "%s is not a valid shader uniform!", name );
        return false;
    }

    // success
    userData = (HandleType)h;
    return true;

    GN_UNGUARD;
}
