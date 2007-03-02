#include "pch.h"

#if GN_MSVC
#if GN_DEBUG_BUILD
#pragma comment( lib, "d3dx10d.lib" )
#else
#pragma comment( lib, "d3dx10.lib" )
#endif
#endif

using namespace GN;
using namespace GN::gfx;
using namespace GN::app;
using namespace GN::scene;

static const char * vs_code =
"uniform float4x4 gPvw : register(c0);      \n"
"struct vsi                                 \n"
"{                                          \n"
"    float4 pos : POSITION;                 \n"
"    //float3 nml : NORMAL;                 \n"
"    //float2 uv  : TEXCOORD0;              \n"
"};                                         \n"
"struct vso                                 \n"
"{                                          \n"
"    float4 pos : POSITION;                 \n"
"    float4 clr : COLOR0;                   \n"
"};                                         \n"
"vso main( vsi i )                          \n"
"{                                          \n"
"    vso o;                                 \n"
"    o.pos = mul( gPvw, i.pos );            \n"
"    //float3 n = 2*abs(i.nml) + i.nml;     \n"
"    o.clr = 1;//float4( n/3.0, 1.0 );      \n"
"    //o.uv = i.uv;                         \n"
"    return o;                              \n"
"};";

static const char * ps_code =
"struct vso                     \n"
"{                              \n"
"    float4 pos : POSITION;     \n"
"    float4 clr : COLOR0;       \n"
"};                             \n"
"float4 main( vso i ) : COLOR0  \n"
"{                              \n"
"	return i.clr;               \n"
"}";

class MyApp : public SampleApp
{
    AutoRef<Shader> vs, ps;

    UInt32 mesh;

    Matrix44f world, view, proj;
    util::ArcBall arcball;

public:

    void onDetermineInitParam( InitParam & ip )
    {
        ip.rapi = API_D3D10;
        //ip.ro.reference = true;
    }

    bool onAppInit()
    {
        world.identity();
        view.translate( 0, 0, -3 );
        proj.perspectiveD3DRh( 1.0f, 4.0f/3.0f, 1.0f, 100.0f );
        arcball.setHandness( util::ArcBall::RIGHT_HAND );
        arcball.setViewMatrix( view );

        return true;
    }

    bool onRendererCreate()
    {
        Renderer & r = gRenderer;
        ResourceManager & rm = gSceneResMgr;

        // load shaders
        vs.attach( r.createVS( LANG_D3D_HLSL, vs_code ) );
        ps.attach( r.createPS( LANG_D3D_HLSL, ps_code ) );
        if( !vs || !ps ) return false;

        // load cube mesh
        mesh = rm.getResourceId( "media::mesh/cube.xml" );
        if( 0 == mesh ) return false;

        // initial arcball window
        const DispDesc & dd = gRenderer.getDispDesc();
        arcball.setMouseMoveWindow( 0, 0, (int)dd.width, (int)dd.height );

        // success
        return true;
    }

    void onRendererDestroy()
    {
        vs.clear();
        ps.clear();
    }

    void onKeyPress( input::KeyEvent key )
    {
        GN::app::SampleApp::onKeyPress( key );
        if( input::KEY_MOUSEBTN_0 == key.code )
        {
            if( key.status.down )
            {
                int x, y;
                gInput.getMousePosition( x, y );
                arcball.onMouseButtonDown( x, y );
            }
            else
            {
                arcball.onMouseButtonUp();
            }
        }
    }

    void onAxisMove( input::Axis, int )
    {
        int x, y;
        gInput.getMousePosition( x, y );
        arcball.onMouseMove( x, y );
    }

    void onUpdate()
    {
        world = arcball.getRotationMatrix();
        Matrix44f pvw = proj * view * world;
        vs->setUniformByName( "gPvw", pvw );
    }

    void onRender()
    {
        Renderer & r = gRenderer;
        ResourceManager & rm = gSceneResMgr;

        r.clearScreen( Vector4f(1,0,0,1) );

        r.setShaders( vs, ps, 0 );
        Mesh * m = rm.getResourceT<Mesh>( mesh );
        m->updateContext( r );
        m->draw( r );
    }
};

int main( int argc, const char * argv[] )
{
    MyApp app;
    return app.run( argc, argv );
}