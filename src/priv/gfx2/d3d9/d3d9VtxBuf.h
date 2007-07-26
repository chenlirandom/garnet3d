#ifndef __GN_GFX2_D3D9VTXBUF_H__
#define __GN_GFX2_D3D9VTXBUF_H__
// *****************************************************************************
/// \file
/// \brief   D3D9 vertex buffer class
/// \author  chenli@@FAREAST (2007.6.26)
// *****************************************************************************

namespace GN { namespace gfx
{
    ///
    /// vertex buffer surface
    ///
    class D3D9VtxBuf : public D3D9Surface
    {
    public:

        ///
        /// create new instance
        ///
        static D3D9VtxBuf * sNewInstance(
            D3D9GraphicsSystem          & gs,
            const SurfaceLayout         & layout,
            int                           access,
            const SurfaceCreationHints  & hints );

        ///
        /// dtor
        ///
        ~D3D9VtxBuf() { safeRelease( mSurface ); }

        ///
        /// get D3D surface
        ///
        IDirect3DVertexBuffer9 * getSurface() const { GN_ASSERT(mSurface); return mSurface; }

        // from parent class
        //@{
        virtual const SubSurfaceLayout * getSubSurfaceLayout( size_t subsurface ) const;
        virtual void download(
            size_t                 subsurface,
            const Box<size_t>    * area,
            const void           * source,
            size_t                 srcRowBytes,
            size_t                 srcSliceBytes );
        virtual void upload(
            size_t              subsurface,
            const Box<size_t> * area,
            void              * destination,
            size_t              destRowBytes,
            size_t              destSliceBytes );
        virtual void save( NativeSurfaceData & ) const;
        virtual void load( const NativeSurfaceData & );
        //@}

    private:

        D3D9GraphicsSystem          & mGraphicsSystem;
        IDirect3DVertexBuffer9      * mSurface;
        SurfaceCreationHints          mHints;
        SubSurfaceLayout              mSubsurfaceLayout;

    private:

        D3D9VtxBuf( D3D9GraphicsSystem & gs, const D3D9SurfaceDesc & desc, const SurfaceCreationHints & hints );

        bool init();

    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX2_D3D9VTXBUF_H__
