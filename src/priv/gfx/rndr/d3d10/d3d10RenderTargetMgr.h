#ifndef __GN_GFX_D3D10RENDERTARGETMGR_H__
#define __GN_GFX_D3D10RENDERTARGETMGR_H__
// *****************************************************************************
//! \file    d3d10/d3d10RenderTargetMgr.h
//! \brief   D3D10 render target manager
//! \author  chen@@CHENLI-HOMEPC (2007.1.11)
// *****************************************************************************

namespace GN { namespace gfx
{
    class D3D10Renderer;

    ///
    /// D3D10 render target manager
    ///
    class D3D10RTMgr : public StdClass
    {
        GN_DECLARE_STDCLASS( D3D10RTMgr, StdClass );

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        D3D10RTMgr( D3D10Renderer & r ) : mRenderer(r) { clear(); }
        virtual ~D3D10RTMgr() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{
    public:
        bool init();
        void quit();
    private:
        void clear() { memset(mColors, 0, sizeof(mColors) ); mDepth = 0; mNumColors = 0; }
        //@}

        // ********************************
        // public functions
        // ********************************
    public:

        ///
        /// bind render target to device
        ///
        void bind(
            const RenderTargetDesc & oldDesc,
            const RenderTargetDesc & newDesc,
            bool forceRebind,
            bool & needRebindViewport );

        ///
        /// get render target count
        ///
        UInt32 getRenderTargetCount() const { return mNumColors; }

        ///
        /// get render target view
        ///
        ID3D10RenderTargetView * getRenderTargetView( UInt32 index ) const
        {
            GN_ASSERT( index < mNumColors );
            return mColors[index];
        }

        ///
        /// get depth stencil view
        ///
        ID3D10DepthStencilView * getDepthStencilView() const { return mDepth; }

        // ********************************
        // private variables
        // ********************************
    private:

        D3D10Renderer & mRenderer;

        AutoComPtr<ID3D10RenderTargetView> mAutoColor0;
        AutoComPtr<ID3D10DepthStencilView> mAutoDepth;

        ID3D10RenderTargetView * mColors[16];
        ID3D10DepthStencilView * mDepth;
        UInt32                   mNumColors;

        // ********************************
        // private functions
        // ********************************
    private:

    };
}}

// *****************************************************************************
//                           End of d3d10RenderTargetMgr.h
// *****************************************************************************
#endif // __GN_GFX_D3D10RENDERTARGETMGR_H__
