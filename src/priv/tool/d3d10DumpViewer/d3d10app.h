#ifndef __GN_GFX_D3D10APP_H__
#define __GN_GFX_D3D10APP_H__
// *****************************************************************************
/// \file
/// \brief   interface of d3d10 application framework
/// \author  chen@@CHENLI-HOMEPC (2007.4.16)
// *****************************************************************************

#include "garnet/GNgfx.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <d3d9.h>
#include <d3d10.h>
#include <d3dx10.h>

namespace GN { namespace gfx { namespace d3d10
{
    ///
    /// scoped PIX event
    ///
    struct PixPerfScopeEvent
    {
        //@{
        PixPerfScopeEvent( D3DCOLOR color, const wchar_t * name )
        {
            D3DPERF_BeginEvent( color, name );
        }
        ~PixPerfScopeEvent()
        {
            D3DPERF_EndEvent();
        }
        //@}
    };

    ///
    /// D3D10 application framework
    ///
    struct D3D10AppOption
    {
        ///
        /// ctor
        ///
        D3D10AppOption()
            : parent(0)
            , monitor(0)
            , vsync(false)
            , fullscreen(false)
            , width(640)
            , height(480)
            , depth(0)
            , refrate(0)
        {
        }

        ///
        /// Handle of parent window. Default is zero, means a top-level window.
        ///
        HWND parent;

        ///
        /// Monitor handle.
        ///
        /// - 0 means using the monitor where parent window stays in.
        ///   If monitor and parent are both zero, primary monitor will be used.
        /// - Default is 0.
        ///
        HMONITOR monitor;

        ///
        /// V-SYNC enable or not. Default is false.
        ///
        bool vsync;

        ///
        /// fullscreen or windowed mode.
        /// Default is false.
        ///
        bool fullscreen;

        UInt32 width;   ///< Screen width.
        UInt32 height;  ///< Screen height.
        UInt32 depth;   ///< Color depth. Ignored for windowed mode.
        UInt32 refrate; ///< Referesh rate. Ignored for windowed mode.
    };

    ///
    /// D3D10 application framework
    ///
    class D3D10Application
    {
    public:

        //@{

        D3D10Application();
        ~D3D10Application();

        ID3D10Device & device() const { GN_ASSERT( mDevice ); return *mDevice; }
        IDXGISwapChain & swapChain() const { GN_ASSERT( mSwapChain ); return *mSwapChain; }

        int run( const D3D10AppOption * = 0 );

        bool changeOption( const D3D10AppOption & );

        //@}

    protected:

        //@{

        virtual bool onInit( D3D10AppOption & ) { return true; }
        virtual bool onCreate() { return true; }
        virtual void onDestroy() {}
        virtual void onQuit() {}

        virtual void onDraw() {}
        virtual void onUpdate() {}

        //@}

    private:

        bool init();
        void quit();

        bool createDevice();
        void destroyDevice();

    private:

        D3D10AppOption        mOption;
        HWND                  mWindow;
        IDXGIAdapter        * mAdapter;
        ID3D10Device        * mDevice;
        IDXGISwapChain      * mSwapChain;
		ID3D10Debug         * mDebug;
		ID3D10InfoQueue     * mInfoQueue;
    };
}}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_GFX_D3D10APP_H__