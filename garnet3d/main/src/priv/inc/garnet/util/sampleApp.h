#ifndef __GN_UTIL_SAMPLEAPP_H__
#define __GN_UTIL_SAMPLEAPP_H__
// *****************************************************************************
/// \file
/// \brief   Sample application framework
/// \author  chenlee (2006.1.7)
// *****************************************************************************

#include <garnet/GNengine.h>

namespace GN { namespace util
{
    ///
    /// Sample application framework
    ///
    class SampleApp : public SlotBase
    {
        // ********************************
        // ctor/dtor
        // ********************************

        //@{
    public:
        SampleApp();
        virtual ~SampleApp() {}
        //@}

        // ********************************
        // sample framework
        // ********************************
    public:

        //@{

        ///
        /// application initialization structure
        ///
        struct InitParam
        {
            gfx::GpuOptions           ro;                ///< renderer options
            bool                      useMultithreadGpu; ///< use multithread renderer or not.
            input::InputAPI           iapi;              ///< input API
            gfx::FontFaceCreationDesc defaultFont;       ///< default non-ascii font face creation descriptor
            gfx::FontFaceCreationDesc asciiFont;         ///< default ascii font face creation descriptor.
        };

        static const uint32 UPDATE_FREQUENCY; //< Frequency of onUpdate() call.
        static const float  UPDATE_INTERVAL;  //< 1.0 / UPDATE_FREQUENCY.

        virtual int  run( int argc, const char * const argv[] );
        virtual bool onCheckExtraCmdlineArguments( const char * exename, int argc, const char * const argv[] ); // only non-standard/unknown arguments are passed to this function.
        virtual void onPrintHelpScreen( const char * executableName );
        virtual bool onPreInit( InitParam & ) { return true; }
        virtual bool onInit() = 0;
        virtual void onQuit() = 0;
        virtual void onUpdate() = 0;
        virtual void onRender() = 0;
        virtual void onRenderWindowResize( intptr_t /*window*/, uint32 /*width*/, uint32 /*height*/ ) {}
        virtual void onKeyPress( input::KeyEvent );
        virtual void onCharPress( wchar_t ) {}
        virtual void onAxisMove( input::Axis, int ) {}

        ///
        /// Draw X/Y/Z coordinate axes onto screen
        ///
        void drawXYZCoordinateAxes( const Matrix44f & projViewWorld );

        ///
        /// print standard command line options to console screen
        ///
        void printStandardCommandLineOptions();

        ///
        /// post exit event. Application will exit at the beginning of next frame.
        ///
        void postExitEvent() { mDone = true; }

		///
		/// show HUD or not?
		///
		void showHUD( bool show ) { mShowHUD = show; }

        ///
        /// get the FPS
        ///
        float fps() const { return mFps.fps(); }

        ///
        /// Get the frame idle percentage
        ///
        int frameIdlePercentage() const { return mFrameIdlePercentage; }

        //@}

        // ********************************
        // private variables
        // ********************************
    private:

        InitParam                  mInitParam;

        util::FpsCalculator        mFps; ///< FPS counter
        int                        mFrameIdlePercentage;

        StrW                       mHelpText;
        bool                       mShowHUD;  ///< show simple HUD over game scene
        bool                       mShowHelp; ///< show detailed help text in HUD. No effect if mShowHUD is false.
        bool                       mDone;     ///< Set to true to quit the application.

        // ********************************
        // private functions
        // ********************************
    private:

        bool init( int argc, const char *  const argv[] );
        void quit();
        bool checkCmdLine( int argc, const char * const argv[] );
        bool initEngine();
        void quitEngine();
        void drawHUD();
    };
}}

// *****************************************************************************
//                                     EOF
// *****************************************************************************
#endif // __GN_UTIL_SAMPLEAPP_H__
