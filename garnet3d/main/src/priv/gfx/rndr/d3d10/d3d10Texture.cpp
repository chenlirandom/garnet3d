#include "pch.h"
#include "d3d10Renderer.h"
#include "d3d10Texture.h"
#include "garnet/GNd3d10.h"

static GN::Logger * sLogger = GN::getLogger("GN.gfx.rndr.D3D10");

// *****************************************************************************
// local functions
// *****************************************************************************

using namespace GN;
using namespace GN::gfx;


//
// less operator for SRV descriptor
// ----------------------------------------------------------------------------
static inline bool operator<(
    const D3D10_SHADER_RESOURCE_VIEW_DESC & a,
    const D3D10_SHADER_RESOURCE_VIEW_DESC & b )
{
    return ::memcmp( &a, &b, sizeof(a) ) < 0;
}

//
//
// ----------------------------------------------------------------------------
static void
sDetermineTextureDimension(
    const GN::gfx::TextureDesc     & desc,
    GN::gfx::D3D10TextureDimension & texdim,
    D3D10_SRV_DIMENSION            & srvdim )
{
    if( 1 == desc.height && 1 == desc.depth )
    {
        texdim = TEXDIM_1D;
        srvdim = desc.faces > 1 ? D3D10_SRV_DIMENSION_TEXTURE1DARRAY : D3D10_SRV_DIMENSION_TEXTURE1D;
    }
    else if( 1 == desc.depth )
    {
        texdim = TEXDIM_2D;
        if( 6 == desc.faces && desc.width == desc.height )
        {
            srvdim = D3D10_SRV_DIMENSION_TEXTURECUBE;
        }
        else if( desc.faces > 1 )
        {
            srvdim = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
        }
        else
        {
            srvdim = D3D10_SRV_DIMENSION_TEXTURE2D;
        }
    }
    else
    {
        texdim = TEXDIM_3D;
        srvdim = D3D10_SRV_DIMENSION_TEXTURE3D;
    }
}

// ****************************************************************************
//  init / quit functions
// ****************************************************************************

//
//
// ----------------------------------------------------------------------------
bool GN::gfx::D3D10Texture::init( const TextureDesc & desc )
{
    GN_GUARD;

    // standard init procedure
    GN_STDCLASS_INIT( GN::gfx::D3D10Texture, () );

    // create device data
    if( !setDesc( desc ) || !createTexture() ) return failure();

    // success
    return success();

    GN_UNGUARD;
}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::D3D10Texture::quit()
{
    GN_GUARD;

    mSRViews.clear();
    mRTViews.clear();
    safeRelease( mTexture );

    // standard quit procedure
    GN_STDCLASS_QUIT();

    GN_UNGUARD;
}

// ****************************************************************************
// from Texture
// ****************************************************************************

//
//
// ----------------------------------------------------------------------------
void GN::gfx::D3D10Texture::updateMipmap(
    size_t              face,
    size_t              level,
    const Box<UInt32> * area,
    size_t              rowPitch,
    size_t              slicePitch,
    const void        * data,
    SurfaceUpdateFlag   flag )
{
    Box<UInt32> clippedArea;
    if( !validateUpdateParameters( face, level, area, flag, clippedArea ) ) return;

    const TextureDesc & desc = getDesc();

    if( desc.usages.fastCpuWrite )
    {
        GN_TODO( "Updating dynamic texture is not implemented yet." );
    }
    else
    {
        ID3D10Device & dev = getDeviceRef();

        D3D10_BOX box =
        {
            clippedArea.x,
            clippedArea.y,
            clippedArea.z,
            clippedArea.x + clippedArea.w,
            clippedArea.y + clippedArea.h,
            clippedArea.z + clippedArea.d,
        };

        const d3d10::DXGI_FORMAT_DESCRIPTION & fmtdesc = d3d10::getDXGIFormatDesc( mTextureFormat );

        dev.UpdateSubresource(
            mTexture,
            D3D10CalcSubresource( level, face, desc.levels ),
            &box,
            data,
            rowPitch * fmtdesc.m_blockHeight,
            slicePitch );
    }
}

//
//
// ----------------------------------------------------------------------------
void GN::gfx::D3D10Texture::readMipmap(
    size_t       /*face*/,
    size_t       /*level*/,
    MipmapData & /*data*/ )
{
    GN_UNIMPL();
}

// ****************************************************************************
// public methods
// ****************************************************************************

//
//
// ----------------------------------------------------------------------------
ID3D10ShaderResourceView *
GN::gfx::D3D10Texture::getSRView(
    DXGI_FORMAT format,
    UInt32      firstFace,
    UInt32      numFaces,
    UInt32      firstMipLevel,
    UInt32      numLevels,
    UInt32      firstSlice,
    UInt32      numSlices )
{
    D3D10_SHADER_RESOURCE_VIEW_DESC srvdesc;
    memset( &srvdesc, 0, sizeof(srvdesc) );
    srvdesc.Format        = format;
    srvdesc.ViewDimension = mDefaultSrvDimension;

    switch( mDefaultSrvDimension )
    {
        case D3D10_SRV_DIMENSION_BUFFER :
            break;

        case D3D10_SRV_DIMENSION_TEXTURE1D :
            srvdesc.Texture1D.MostDetailedMip = firstMipLevel;
            srvdesc.Texture1D.MipLevels       = numLevels;
            break;

        case D3D10_SRV_DIMENSION_TEXTURE1DARRAY :
            srvdesc.Texture1DArray.FirstArraySlice = firstFace;
            srvdesc.Texture1DArray.ArraySize       = numFaces;
            srvdesc.Texture1DArray.MostDetailedMip = firstMipLevel;
            srvdesc.Texture1DArray.MipLevels       = numLevels;
            break;

        case D3D10_SRV_DIMENSION_TEXTURE2D :
            srvdesc.Texture2D.MostDetailedMip = firstMipLevel;
            srvdesc.Texture2D.MipLevels       = numLevels;
            break;

        case D3D10_SRV_DIMENSION_TEXTURE2DARRAY :
            srvdesc.Texture2DArray.FirstArraySlice = firstFace;
            srvdesc.Texture2DArray.ArraySize       = numFaces;
            srvdesc.Texture2DArray.MostDetailedMip = firstMipLevel;
            srvdesc.Texture2DArray.MipLevels       = numLevels;
            break;

        case D3D10_SRV_DIMENSION_TEXTURE2DMS :
            // do nothing
            break;

        case D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY :
            // do nothing
            srvdesc.Texture2DMSArray.FirstArraySlice = firstFace;
            srvdesc.Texture2DMSArray.ArraySize       = numFaces;
            break;

        case D3D10_SRV_DIMENSION_TEXTURE3D :
            srvdesc.Texture3D.MostDetailedMip = firstMipLevel;
            srvdesc.Texture3D.MipLevels       = numLevels;
            break;

        case D3D10_SRV_DIMENSION_TEXTURECUBE :
            srvdesc.TextureCube.MostDetailedMip = firstMipLevel;
            srvdesc.TextureCube.MipLevels       = numLevels;
            break;

        default:
            GN_UNEXPECTED();
            return NULL;
    }

    AutoComPtr<ID3D10ShaderResourceView> & srv = mSRViews[srvdesc];

    if( srv.empty() )
    {
        ID3D10Device & dev = getDeviceRef();

        GN_DX10_CHECK_RV( dev.CreateShaderResourceView( mTexture, &srvdesc, &srv ), NULL );
    }

    return srv;
}

//
//
// ----------------------------------------------------------------------------
ID3D10RenderTargetView *
GN::gfx::D3D10Texture::getRTView( UInt32 face, UInt32 level, UInt32 slice )
{
    GN_UNUSED_PARAM( face );
    GN_UNUSED_PARAM( level );
    GN_UNUSED_PARAM( slice );
    GN_UNIMPL();
    return NULL;
}

//
//
// ----------------------------------------------------------------------------
ID3D10DepthStencilView *
GN::gfx::D3D10Texture::getDSView( UInt32 face, UInt32 level, UInt32 slice )
{
    GN_UNUSED_PARAM( face );
    GN_UNUSED_PARAM( level );
    GN_UNUSED_PARAM( slice );
    GN_UNIMPL();
    return NULL;
}


// ****************************************************************************
//      private functions
// ****************************************************************************

//
//
// ----------------------------------------------------------------------------
bool GN::gfx::D3D10Texture::createTexture()
{
    GN_GUARD;

    GN_ASSERT( !mTexture );

    const TextureDesc & desc = getDesc();

    // determine shader resource view format
    mDefaultSRVFormat = (DXGI_FORMAT)colorFormat2DxgiFormat( desc.format );
    if( DXGI_FORMAT_UNKNOWN == mDefaultSRVFormat )
    {
        GN_ERROR(sLogger)( "Fail to convert color format '%s' to DXGI_FORMAT.", desc.format.toString().cptr() );
        return false;
    }

    // always try using typeless format to create the texture
    mTextureFormat = mDefaultSRVFormat;//d3d10::getDXGIFormatDesc( mDefaultSRVFormat ).m_typelessFormat;

    // determine usage and CPU access flag
    D3D10_USAGE usage;
    UINT        caf;
    if( desc.usages.fastCpuWrite )
    {
        usage = D3D10_USAGE_DYNAMIC;
        caf   = D3D10_CPU_ACCESS_WRITE;
    }
    else
    {
        usage = D3D10_USAGE_DEFAULT;
        caf   = 0;
    }

    // determine bind flags
    UINT bf = D3D10_BIND_SHADER_RESOURCE;
    if( desc.usages.depth )
    {
        bf |= D3D10_BIND_DEPTH_STENCIL;
    }
    else if( desc.usages.rendertarget )
    {
        bf |= D3D10_BIND_RENDER_TARGET;
    }

    // determine texture dimension
    sDetermineTextureDimension( desc, mTextureDimension, mDefaultSrvDimension );

    // determine cube flag
    UINT mf = 0;
    if( TEXDIM_2D == mTextureDimension && desc.width == desc.height && 6 == desc.faces )
        mf |= D3D10_RESOURCE_MISC_TEXTURECUBE;

    // create texture instance
    ID3D10Device & dev = getDeviceRef();
    if( TEXDIM_1D == mTextureDimension )
    {
        ID3D10Texture1D * tex1d;
        D3D10_TEXTURE1D_DESC desc1d;
        desc1d.Width = desc.width;
        desc1d.MipLevels = desc.levels;
        desc1d.ArraySize = desc.faces;
        desc1d.Format = mTextureFormat;
        desc1d.Usage = usage;
        desc1d.BindFlags = bf;
        desc1d.CPUAccessFlags = caf;
        desc1d.MiscFlags = mf;
        GN_DX10_CHECK_RV( dev.CreateTexture1D( &desc1d, 0, &tex1d ), false );
        mTexture = tex1d;
    }
    else if( TEXDIM_2D == mTextureDimension )
    {
        ID3D10Texture2D * tex2d;
        D3D10_TEXTURE2D_DESC desc2d;
        desc2d.Width = desc.width;
        desc2d.Height = desc.height;
        desc2d.MipLevels = desc.levels;
        desc2d.ArraySize = desc.faces;
        desc2d.Format = mTextureFormat;
        desc2d.SampleDesc.Count = 1;
        desc2d.SampleDesc.Quality = 0;
        desc2d.Usage = usage;
        desc2d.BindFlags = bf;
        desc2d.CPUAccessFlags = caf;
        desc2d.MiscFlags = mf;
        GN_DX10_CHECK_RV( dev.CreateTexture2D( &desc2d, 0, &tex2d ), false );
        mTexture = tex2d;
    }
    else if( TEXDIM_3D == mTextureDimension )
    {
        ID3D10Texture3D * tex3d;
        D3D10_TEXTURE3D_DESC desc3d;
        desc3d.Width = desc.width;
        desc3d.Height = desc.height;
        desc3d.Depth = desc.depth;
        desc3d.MipLevels = desc.levels;
        desc3d.Format = mTextureFormat;
        desc3d.Usage = usage;
        desc3d.BindFlags = bf;
        desc3d.CPUAccessFlags = caf;
        desc3d.MiscFlags = mf;
        GN_DX10_CHECK_RV( dev.CreateTexture3D( &desc3d, 0, &tex3d ), false );
        mTexture = tex3d;
    }
    else
    {
        GN_ERROR(sLogger)( "Invalid texture dimension: %d", mTextureDimension );
        GN_UNEXPECTED();
        return false;
    }

    // calculate mipmap sizes
    Vector3<UInt32> mipSize( desc.width, desc.height, desc.depth );
    for( size_t i = 0; i < desc.levels; ++i )
    {
        setMipSize( i, mipSize );
        if( mipSize.x > 1 ) mipSize.x >>= 1;
        if( mipSize.y > 1 ) mipSize.y >>= 1;
        if( mipSize.z > 1 ) mipSize.z >>= 1;
    }

    // success
    return true;

    GN_UNGUARD;
}
