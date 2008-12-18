#ifndef __GN_GFX_OGLRENDERER_H__
#define __GN_GFX_OGLRENDERER_H__
// *****************************************************************************
/// \file
/// \brief   OGL renderer class
/// \author  chenlee (2005.10.2)
// *****************************************************************************

#include "../common/basicRenderer.h"
#include "../common/cgShader.h"

namespace GN { namespace gfx
{
    struct OGLResource;
    class OGLVtxFmt;
    class OGLSamplerObject;
    class OGLLine;
    class OGLBasicRTMgr;

    ///
    /// OGL renderer class
    ///
    class OGLRenderer : public BasicRenderer
    {
        GN_DECLARE_STDCLASS(OGLRenderer, BasicRenderer);

        static Logger * sLogger;

        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public :
        OGLRenderer() : BasicRenderer(API_OGL) { clear(); }
        virtual ~OGLRenderer() { quit(); }
        //@}

        // ********************************
        // from StdClass
        // ********************************

        //@{

    public:

        bool init();
        void quit();

    private:

        void clear()
        {
            deviceClear();
            dispClear();
            capsClear();
            resourceClear();
            contextClear();
            drawClear();
        }

        //@}

    // ************************************************************************
    //
    /// \name                     Device Manager
    //
    // ************************************************************************

        //@{

    public:

        virtual bool reset( const RendererOptions & ro );

    private:

        ///
        /// if true, then we are in the middle of renderer reset.
        ///
        bool mOnGoingReset;

    private:


        void deviceClear() { mOnGoingReset = false; }
        bool deviceCreate();
        void deviceDestroy();

        //@}

    // ************************************************************************
    //
    /// \name                     Display Manager
    //
    // ************************************************************************

        //@{

    public:

        virtual void * getD3DDevice() const { return 0; }
        virtual void * getOGLRC() const { return mRenderContext; }


#if GN_MSWIN
    private :
        bool dispInit() { return true; }
        void dispQuit() {}
        void dispClear()
        {
            mDeviceContext = 0;
            mRenderContext = 0;
            mDisplayModeActivated = false;
            mIgnoreMsgHook = false;
        }

        bool dispDeviceCreate();
        void dispDeviceDestroy();

        bool activateDisplayMode();
        void restoreDisplayMode();
        void msgHook( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp );

    private :

        HDC     mDeviceContext;
        HGLRC   mRenderContext;
        bool    mDisplayModeActivated;
        bool    mIgnoreMsgHook;

#elif GN_POSIX
    private :
        bool dispInit() { return true; }
        void dispQuit() {}
        bool dispOK() const { return true; }
        void dispClear()
        {
            mRenderContext = 0;
        }

        bool dispDeviceCreate();
        void dispDeviceDestroy();

    private :
        GLXContext mRenderContext;
#endif

        //@}

    // ************************************************************************
    //
    /// \name                 Capability Manager
    //
    // ************************************************************************

        //@{

    public :

        virtual const RendererCaps & getCaps() const { return mCaps; }
        virtual bool                 checkTextureFormatSupport( ColorFormat format, TextureUsages usages ) const;
        virtual ColorFormat          getDefaultTextureFormat( TextureUsages usages ) const;

    private :
        bool capsInit() { return true; }
        void capsQuit() {}
        void capsClear() {}

        bool capsDeviceCreate();
        void capsDeviceDestroy() {}

    private:

        RendererCaps mCaps;

        //@}

    // ************************************************************************
    //
    /// \name                     Resource Manager
    //
    // ************************************************************************

        //@{

    public :

        virtual CompiledShaderBlob * compileShader( const ShaderDesc & desc );
        virtual Shader             * createShader( const CompiledShaderBlob * );
        virtual Texture            * createTexture( const TextureDesc & desc );
        virtual VtxBuf             * createVtxBuf( const VtxBufDesc & desc );
        virtual IdxBuf             * createIdxBuf( const IdxBufDesc & desc );

    public:

#if HAS_CG
        ///
        /// get global Cg context
        ///
        CGcontext getCgContext() const { return mCgContext; }
#endif

        ///
        /// Insert resource into resource list. Can be only called by
        /// constructor of OGLResource.
        ///
        void insertResource( OGLResource * p ) { mResourceList.push_back(p); }

        ///
        /// Remove resource from resource list. Can be only called by
        /// destructor of OGLResource.
        ///
        void removeResource( OGLResource * p ) { mResourceList.remove(p); }

    private :

        bool resourceInit() { return true; }
        void resourceQuit() {}
        void resourceClear() {}
        bool resourceDeviceCreate();
        void resourceDeviceDestroy();

    private:

#if HAS_CG
        CgContextWrapper mCgContext;
#endif
        std::list<OGLResource*> mResourceList;

        //@}

    // ************************************************************************
    //
    /// \name                   Context Manager
    //
    // ************************************************************************

        //@{

    public:

        virtual void bindContext( const RendererContext & context, bool forceRebinding );

    public:

        void chooseClientTextureStage( size_t ) const; ///< Choose one stage as client active texture stage.
        void chooseTextureStage( size_t ) const; ///< Choose one texture stage as active stage
        void disableTextureStage( size_t ) const; ///< Disable one texture stage

    private:

        bool contextInit() { return true; }
        void contextQuit() {}
        void contextClear();
        bool contextDeviceCreate();
        void contextDeviceDestroy();

        inline void bindContextShaders( const RendererContext & newContext, bool forceRebind );
        inline void bindContextRenderStates( const RendererContext & newContext, bool forceRebind );
        inline void bindContextFfp( const RendererContext & newContext, bool forceRebind );
        inline void bindContextData( const RendererContext & newContext, bool forceRebind );

    private:

        OGLBasicRTMgr * mRTMgr;
        UInt32          mNeedRebindVtxBufs; // each bit represent a vertex stream.

        //@}

    // ************************************************************************
    //
    /// \name                     Drawing Manager
    //
    // ************************************************************************

        //@{

    public:

        virtual void present();
        virtual void clearScreen( const Vector4f & c, float z, UInt8 s, BitFields flags );
        virtual void drawIndexed( PrimitiveType prim,
                                  size_t        numprim,
                                  size_t        startvtx,
                                  size_t        minvtxidx,
                                  size_t        numvtx,
                                  size_t        startidx );
        virtual void draw( PrimitiveType prim,
                           size_t        numprim,
                           size_t        startvtx );
        virtual void drawIndexedUp(
                             PrimitiveType  prim,
                             size_t         numprim,
                             size_t         numvtx,
                             const void *   vertexData,
                             size_t         strideInBytes,
                             const UInt16 * indexData );
        virtual void drawUp( PrimitiveType prim,
                             size_t        numprim,
                             const void *  vertexData,
                             size_t        strideInBytes );
        virtual void drawLines( BitFields         options,
                                const void *      positions,
                                size_t            stride,
                                size_t            count,
                                UInt32            rgba,
                                const Matrix44f & model,
                                const Matrix44f & view,
                                const Matrix44f & proj );

    private:

        bool drawInit() { return true; }
        void drawQuit() {}
        void drawClear()
        {
            mLine = 0;
            mCurrentStartVtx = (size_t)-1;
            mFrameCounter = 0;
            mDrawCounter = 0;
        }

        bool drawDeviceCreate();
        bool drawDeviceRestore() { return true; }
        void drawDeviceDispose() {}
        void drawDeviceDestroy();

    private:

        OGLLine * mLine;
        size_t mCurrentStartVtx;
        size_t mFrameCounter;
        size_t mDrawCounter;

        //@}

        // ********************************************************************
        //
        /// \name Misc. utilities
        //
        // ********************************************************************

        //@{

    public:

        virtual void dumpNextFrame( size_t startBatchIndex, size_t numBatches )
        {
            GN_UNUSED_PARAM( startBatchIndex );
            GN_UNUSED_PARAM( numBatches );
            GN_TODO( "OpenGL frame dump is not implemented." );
        }

        //@}
    };
}}

#include "oglContextMgr.inl"

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_OGLRENDERER_H__
