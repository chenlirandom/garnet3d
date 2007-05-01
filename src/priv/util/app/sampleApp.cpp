#include "pch.h"
#include "garnet/GNapp.h"
#include "garnet/GNscene.h"
#define SO_ASSERT GN_ASSERT
#include <SimpleOpt.h>

float GN::app::SampleApp::UPDATE_INTERVAL = 1.0f/60.0f;

static GN::Logger * sLogger = GN::getLogger("GN.app.SampleApp");

// *****************************************************************************
// public functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
GN::app::SampleApp::SampleApp()
    : mShowHUD(true)
    , mShowHelp(false)
    , mFps( L"FPS: %.2f\n(Press F1 for help)" )
{
    enableCRTMemoryCheck();
    mFps.reset();
}

//
//
// -----------------------------------------------------------------------------
int GN::app::SampleApp::run( int argc, const char * const argv[] )
{
    GN_GUARD_ALWAYS;

    if( !init( argc, argv ) ) { quit(); return -1; }

    mDone = false;

    bool firstframe = true;
    double elapsedUpdateTime;
    double lastUpdateTime = mFps.getCurrentTime();

    while( !mDone )
    {
        // process windows messages
        GN::win::processWindowMessages( gRenderer.getDispDesc().windowHandle, true );

        // update time stuff
        mFps.onFrame();

        // call update in fixed interval
        elapsedUpdateTime = mFps.getCurrentTime() - lastUpdateTime;
        if( elapsedUpdateTime > UPDATE_INTERVAL || firstframe )
        {
            // process user input
            gInput.processInputEvents();

            int count = firstframe ? 1 : (int)( elapsedUpdateTime / UPDATE_INTERVAL );
            static const int MAX_COUNT = (int)( 1.0f / UPDATE_INTERVAL );
            if( count > MAX_COUNT ) count = MAX_COUNT; // make easy of long time debug break.
            for( int i = 0; i < count; ++i )
                onUpdate();
            lastUpdateTime += UPDATE_INTERVAL * count;
            firstframe = false;
        }

        // do render
        if( gRenderer.drawBegin() )
        {
            mLastFrameTime = 1.0 / mFps.getFps();
            mTimeSinceLastUpdate = mFps.getCurrentTime() - lastUpdateTime;
            onRender();
            drawHUD();
            gRenderer.drawEnd();
        }
    }

    // success
    quit();
    return 0;

    // failed
    GN_UNGUARD_ALWAYS_NO_THROW;
    quit();
    return -1;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::onCheckCmdLine( int argc, const char * const argv[] )
{
    if( argc > 1 )
    {
        StrA s = "unknown command line arguments:";
        for( int i = 1; i < argc; ++i )
        {
            s += " ";
            s += argv[i];
        }
        GN_WARN(sLogger)( s.cptr() );
    }
    return true;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::onKeyPress( input::KeyEvent ke )
{
    if( input::KEY_XB360_X == ke.code && ke.status.down ) mDone = true;
    else if( input::KEY_ESCAPE == ke.code && !ke.status.down ) mDone = true;
    else if( input::KEY_R == ke.code && !ke.status.down ) gSceneResMgr.disposeAll();
    else if( input::KEY_F == ke.code && !ke.status.down ) gRenderer.dumpNextFrame();
    else if( input::KEY_RETURN == ke.code && ke.status.down && ke.status.altDown() )
    {
        GN::gfx::RendererOptions ro = gRenderer.getOptions();
        ro.fullscreen = !ro.fullscreen;
        if( !gRenderer.changeOptions(ro) ) postExitEvent();
    }
    else if( input::KEY_F1 == ke.code && !ke.status.down )
    {
        mShowHelp = !mShowHelp;
    }
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::switchRenderer()
{
    GN_GUARD;

    mInitParam.rapi = (gfx::RendererAPI)((mInitParam.rapi+1)%gfx::NUM_RENDERER_API);
    return recreateRenderer();

    GN_UNGUARD;
}

// *****************************************************************************
// private functions
// *****************************************************************************

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::init( int argc, const char * const argv[] )
{
    GN_GUARD_ALWAYS;

    if( !checkCmdLine(argc,argv) ) return false;

    // create global resource manager instance
    new scene::ResourceManager;

    if( !initApp() ) return false;
    onDetermineInitParam( mInitParam );
    if( !initRenderer() ) return false;
    if( !initInput() ) return false;
    if( !initFont() ) return false;

    // success
    return true;

    // failed
    GN_UNGUARD_ALWAYS_NO_THROW;
    return false;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::quit()
{
    GN_GUARD_ALWAYS;

    quitRenderer();
    quitInput();
    quitFont();
    quitApp();

    // delete global resource manager instance
    delete scene::ResourceManager::sGetInstancePtr();

    GN_UNGUARD_ALWAYS_NO_THROW;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::checkCmdLine( int argc, const char * const argv[] )
{
    GN_GUARD;

#if GN_XENON
    GN_UNUSED_PARAM( argc );
    GN_UNUSED_PARAM( argv );
    mInitParam.rapi = gfx::API_D3D9;
    mInitParam.ro = gfx::RendererOptions();
    mInitParam.iapi = input::API_NATIVE;
    mInitParam.ffd.fontname = "font::/simsun.ttc";
    mInitParam.ffd.width = 16;
    mInitParam.ffd.height = 16;
    if( !onCheckCmdLine( argc, argv ) ) return false;
#else
    // setup default parameters
    mInitParam.rapi = gfx::API_AUTO;
    mInitParam.ro = gfx::RendererOptions();
    mInitParam.ro.windowedWidth = 640;
    mInitParam.ro.windowedHeight = 480;
    mInitParam.iapi = input::API_NATIVE;
    mInitParam.ffd.fontname = "font::/simsun.ttc";
    mInitParam.ffd.width = 16;
    mInitParam.ffd.height = 16;

    DynaArray<const char*> unknownArgs;
    unknownArgs.append( argv[0] );

    static const CSimpleOptA::SOption sOptions[] = {
        { 0, "-d3d9",  SO_NONE    },
        { 0, "-d3d10", SO_NONE    },
        { 0, "-ogl",   SO_NONE    },
        { 0, "-fake",  SO_NONE    },
        { 0, "-ww",    SO_REQ_SEP }, // window width
        { 0, "-wh",    SO_REQ_SEP }, // window height
        { 0, "-fs",    SO_NONE    },
        { 0, "-ref",   SO_NONE    },
        { 0, "-msaa",  SO_NONE    },
        { 0, "-sw",    SO_NONE    },
        { 0, "-pure",  SO_NONE    },
        { 0, "-vsync", SO_NONE    },
        { 0, "-m",     SO_REQ_SEP }, // specify monitor index
        { 0, "-di",    SO_NONE    },
        { 0, "-le",    SO_REQ_SEP }, // enable logger
        { 0, "-ld",    SO_REQ_SEP }, // diable logger
        { 0, "-ll",    SO_REQ_SEP }, // set log level
        { 1, "-?",     SO_NONE    },
        { 1, "-h",     SO_NONE    },
        { 1, "--help", SO_NONE    },
        SO_END_OF_OPTIONS            // END
    };

    CSimpleOptA so( argc, (char**)argv, sOptions );

    while( so.Next() )
    {
        const char * a = so.OptionText();
        if( SO_SUCCESS == so.LastError() )
        {
            if( 1 == so.OptionId() )
            {
                GN_INFO(sLogger)( "\n"
                    "Usage : %s [options]\n\n"
                    "Options : (options are case-insensitive)\n"
                    "    -h, -?, --help         : Show help screen.\n"
                    "    -d3d9/d3d10/ogl/fake   : Select rendering API.\n"
                    "    -ww [num]              : Windows width. Default is 640.\n"
                    "    -wh [num]              : Windows width. Default is 480.\n"
                    "    -fs                    : Use fullsreen mode.\n"
                    "    -ref                   : Use reference device.\n"
                    "    -msaa                  : Enable MSAA/FSAA.\n"
                    "    -sw                    : Use software vertex processing. (D3D only)\n"
                    "    -pure                  : Use pure device (D3D only).\n"
                    "    -vsync                 : Enale vsync.\n"
                    "    -m [num]               : Specify monitor index. Default is 0.\n"
                    "    -di                    : Use direct input.\n"
                    "    -le [name]             : Enable specific logger.\n"
                    "    -ld [name]             : Disable specific logger.\n"
                    "    -ll [name:level]       : Set log level for specific logger. Level is integer:\n"
                    "                               positive : enable all log levels smaller then it.\n"
                    "                               zero     : disable all levels.\n"
                    "                               negative : enable specific log level only (-level).\n"
                    "                             Normally used levels are:\n"
                    "                               FATAL  = 10\n"
                    "                               ERROR  = 20\n"
                    "                               WARN   = 30\n"
                    "                               INFO   = 40\n"
                    "                               DETAIL = 50\n"
                    "                               TRACE  = 200\n"
                    , baseName(argv[0]).cptr() );
                return false;
            }
            else if( 0 == strCmpI( a, "-d3d9" ) ) mInitParam.rapi = gfx::API_D3D9;
            else if( 0 == strCmpI( a, "-d3d10" ) ) mInitParam.rapi = gfx::API_D3D10;
            else if( 0 == strCmpI( a, "-ogl" ) ) mInitParam.rapi = gfx::API_OGL;
            else if( 0 == strCmpI( a, "-ww" ) )
            {
                UInt32 i;
                if( !str2UInt32( i, so.OptionArg() ) )
                {
                    GN_ERROR(sLogger)( "invalid windows width." );
                }
                else
                {
                    mInitParam.ro.windowedWidth = i;
                }
            }
            else if( 0 == strCmpI( a, "-wh" ) )
            {
                UInt32 i;
                if( !str2UInt32( i, so.OptionArg() ) )
                {
                    GN_ERROR(sLogger)( "invalid windows height." );
                }
                else
                {
                    mInitParam.ro.windowedHeight = i;
                }
            }
            else if( 0 == strCmpI( a, "-fs" ) ) mInitParam.ro.fullscreen = true;
            else if( 0 == strCmpI( a, "-ref" ) ) mInitParam.ro.reference = true;
            else if( 0 == strCmpI( a, "-fake" ) ) mInitParam.rapi = gfx::API_FAKE;
            else if( 0 == strCmpI( a, "-msaa" ) ) mInitParam.ro.msaa = GN::gfx::MSAA_ULTRA;
            else if( 0 == strCmpI( a, "-sw" ) ) mInitParam.ro.software = true;
            else if( 0 == strCmpI( a, "-pure" ) ) mInitParam.ro.pure = true;
            else if( 0 == strCmpI( a, "-vsync" ) ) mInitParam.ro.vsync = true;
            else if( 0 == strCmpI( a, "-m" ) )
            {
                UInt32 idx;
                if( !str2UInt32( idx, so.OptionArg() ) )
                {
                    GN_ERROR(sLogger)( "monitor index must be integer." );
                    return false;
                }
                mInitParam.ro.monitorHandle = win::getMonitorByIndex( idx );
            }
            else if( 0 == strCmpI( a, "-di") ) mInitParam.iapi = input::API_DINPUT;
            else if( 0 == strCmpI( a, "-le") ) getLogger( so.OptionArg() )->setEnabled( true );
            else if( 0 == strCmpI( a, "-ld") ) getLogger( so.OptionArg() )->setEnabled( false );
            else if( 0 == strCmpI( a, "-ll") )
            {
                StrA   o( so.OptionArg() );
                size_t k = o.findFirstOf( ":" );
                if( StrA::NOT_FOUND == k )
                {
                    GN_ERROR(sLogger)( "Log level must be in format of 'name:level'" );
                }
                else
                {
                    StrA name( o.subString( 0, k ) );
                    StrA leveltok( o.subString( k+1, 0 ) );
                    int level;
                    if( !name.empty() && str2SInt32( level, leveltok.cptr() ) )
                    {
                        getLogger( name.cptr() )->setLevel( level );
                    }
                    else
                    {
                        GN_ERROR(sLogger)( "Log level must be in format of 'name:level'" );
                    }
                }
            }
        }
        else
        {
            unknownArgs.append( a );
        }
    }

    // handle unknown arguments
    for( int i = 0; i < so.FileCount(); ++i )
    {
        unknownArgs.append( so.File( i ) );
    }
    if( !onCheckCmdLine( (int)unknownArgs.size(), unknownArgs.cptr() ) ) return false;
#endif

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::initApp()
{
    GN_GUARD;

    if( !onAppInit() ) return false;

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::quitApp()
{
    GN_GUARD;

    onAppQuit();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::initRenderer()
{
    GN_GUARD;

    // connect to renderer signals
    gSigRendererCreate.connect( this, &SampleApp::onRendererCreate );
    gSigRendererRestore.connect( this, &SampleApp::onRendererRestore );
    gSigRendererDispose.connect( this, &SampleApp::onRendererDispose );
    gSigRendererDestroy.connect( this, &SampleApp::onRendererDestroy );
    gSigRendererWindowClosing.connect( this, &SampleApp::postExitEvent );

    // create renderer
    return recreateRenderer();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::quitRenderer()
{
    GN_GUARD;

    // delete Renderer
    GN::gfx::deleteRenderer();

    // disconnect to renderer signals
    gSigRendererDestroy.disconnect( this );
    gSigRendererDispose.disconnect( this );
    gSigRendererRestore.disconnect( this );
    gSigRendererCreate.disconnect( this );
    gSigRendererWindowClosing.disconnect( this );

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::recreateRenderer()
{
    GN_GUARD;

    // (re)create renderer
    GN::gfx::Renderer * r = gfx::createRenderer( mInitParam.rapi );
    if( NULL == r ) return false;
    if( !r->changeOptions( mInitParam.ro ) ) return false;

    // reattach input window
    const GN::gfx::DispDesc & dd = r->getDispDesc();
    if( gInputPtr && !gInput.attachToWindow( dd.displayHandle,dd.windowHandle ) )
    {
        return false;
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::initInput()
{
    GN_GUARD;

    // connect to input signals
    gSigKeyPress.connect( this, &SampleApp::onKeyPress );
    gSigCharPress.connect( this, &SampleApp::onCharPress );
    gSigAxisMove.connect( this, &SampleApp::onAxisMove );

    // release old input system
    quitInput();

    // create INPUT system
    GN::input::Input * input = GN::input::createInputSystem( mInitParam.iapi );
    if( 0 == input ) return false;

    if( gRendererPtr )
    {
        const GN::gfx::DispDesc & dd = gRenderer.getDispDesc();
        if( !input->attachToWindow(dd.displayHandle,dd.windowHandle) ) return false;
    }

    // success
    return true;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::quitInput()
{
    GN_GUARD;

    if( gInputPtr ) delete gInputPtr;

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
bool GN::app::SampleApp::initFont()
{
    GN_GUARD;

    return mFontRenderer.init( mInitParam.ffd );

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::quitFont()
{
    GN_GUARD;

    mFontRenderer.quit();

    GN_UNGUARD;
}

//
//
// -----------------------------------------------------------------------------
void GN::app::SampleApp::drawHUD()
{
    GN_GUARD_SLOW;

    if( mShowHUD )
    {
        if( mShowHelp )
        {
            static const wchar_t * help =
                L"ESC            : �˳�\n"
                L"XB360 button X : �˳�\n"
                L"R              : reload all resources\n"
                L"F              : dump next frame\n"
                L"ALT+ENTER      : �л�ȫ��ģʽ\n"
                L"F1             : �л�������Ļ";
            mFontRenderer.drawText( help, 0, 0 );
        }
        else
        {
            mFontRenderer.drawText( mFps.getFpsString().cptr(), 0, 0 );
        }
    }

    GN_UNGUARD_SLOW;
}