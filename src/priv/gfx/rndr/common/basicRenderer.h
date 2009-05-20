#ifndef __GN_GFXCOMMON_BASICRENDERER_H__
#define __GN_GFXCOMMON_BASICRENDERER_H__
// *****************************************************************************
/// \file
/// \brief   Basic renderer class
/// \author  chenlee (2005.10.1)
// *****************************************************************************

///
/// Rest-in-peace macro
///
#define GN_RNDR_RIP GN::gfx::rip

namespace GN { namespace gfx
{
    ///
    /// reset in peace...
    ///
    void rip( const char * msg, ... );

    ///
    /// basic renderer class
    ///
    class BasicRenderer : public Renderer, public StdClass
    {
        GN_DECLARE_STDCLASS( BasicRenderer, StdClass );

        // ********************************
        // constructor/destructor
        // ********************************

        //@{
    public:
        BasicRenderer() { clear(); }
        virtual ~BasicRenderer() {}
        //@}

        // ********************************
        // standard init / quit
        // ********************************

        //@{
    public:
        bool init( const RendererOptions & );
        void quit();
    private :
        void clear()
        {
            contextClear();
            miscClear();
        }
        //@}

        // *****************************************************************************
        //
        /// \name                   Display Manager
        //
        // *****************************************************************************

        //@{

     protected:

        ///
        /// Called by sub class to respond to render window resizing/moving
        ///
        virtual void handleRenderWindowSizeMove() = 0;

        //@}

        // *****************************************************************************
        //
        /// \name                   Caps Manager
        //
        // *****************************************************************************

        //@{

        //@}

        // *****************************************************************************
        //
        /// \name                   Resource Manager
        //
        // *****************************************************************************

        //@{

        //@}

        // *****************************************************************************
        //
        /// \name                   Context Manager
        //
        // *****************************************************************************

        //@{

    public:

        virtual void bindContext( const RendererContext & c );
        virtual void rebindContext();
        virtual inline const RendererContext & getContext() const { return mContext; }

    protected:

        virtual bool bindContextImpl( const RendererContext & context, bool skipDirtyCheck ) = 0;

    private:

        void contextClear() { mContextOk = false; }

    protected:

        RendererContext mContext;
        bool            mContextOk;

        //@}

        // *****************************************************************************
        //
        /// \name                   Draw Manager
        //
        // *****************************************************************************

        //@{

        //@}

        // *****************************************************************************
        //
        /// \name                   Misc Manager
        //
        // *****************************************************************************

        //@{

    public:

        virtual RendererSignals & getSignals() { return mSignals; }
        virtual void              enableParameterCheck( bool enable ) { mParamCheckEnabled = enable; }
        virtual void              setUserData( const Guid & id, const void * data, size_t length );
        virtual const void      * getUserData( const Guid & id, size_t * length ) const;
        virtual bool              hasUserData( const Guid & id ) const;

    public:

        bool                      paramCheckEnabled() const { return mParamCheckEnabled; }

    private:

        void miscClear()
        {
            mParamCheckEnabled = GN_BUILD_DEBUG;
            mUserData.clear();
        }

    private:

        typedef std::map<Guid,DynaArray<UInt8> > UserDataMap;

        RendererSignals mSignals;
        bool            mParamCheckEnabled;
        UserDataMap     mUserData;

        //@}
    };

    ///
    /// render targets description
    ///
    struct RenderTargetDesc
    {
        /// color render targets
        StackArray<RenderTargetTexture, RendererContext::MAX_COLOR_RENDER_TARGETS> colortargets;

        /// depth stencil render target
        RenderTargetTexture                                                        depthstencil;

        /// return true, if the description represents the render target setup for rendering to back buffer.
        bool isRenderingToBackBuffer() const
        {
            return 0 == colortargets.size() && 0 == depthstencil.texture;
        }

        /// return true, if color buffers are empty and depth render target is not.
        bool isRenderingToDepthTextureOnly() const
        {
            return 0 == colortargets.size() && 0 != depthstencil.texture;
        }

        /// check for invalid description.
        bool valid() const
        {
            for( size_t i = 0; i < colortargets.size(); ++i )
            {
                if( !colortargets[i].texture )
                {
                    GN_ERROR(GN::getLogger("GN.gfx"))(
                        "NULL color render targets in render target array is not allowed." );
                    return false;
                }
            }

            return true;
        }

        /// equality check
        bool operator==( const RenderTargetDesc & rhs ) const
        {
            if( colortargets.size() != rhs.colortargets.size() ) return false;
            for( size_t i = 0; i < colortargets.size(); ++i )
            {
                if( colortargets[i] != rhs.colortargets[i] ) return false;
            }
            return depthstencil == rhs.depthstencil;
        }

        /// equality check
        bool operator!=( const RenderTargetDesc & rhs ) const
        {
            return !operator==( rhs );
        }
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFXCOMMON_BASICRENDERER_H__
