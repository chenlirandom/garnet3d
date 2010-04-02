#include "pch.h"

using namespace GN;
using namespace GN::gfx;
using namespace GN::input;
using namespace GN::util;
using namespace GN::util;

static GN::Logger * sLogger = GN::GetLogger("GN.tool.meshViewer");

class MyApp : public SampleApp
{
    struct Ripple
    {
        float x, y;
        float time;
        float _;
    };

    SpriteRenderer * mSprite;
    GpuContext       mContext;
    Ripple           mRipples[20];
    size_t           mNextRipple;

public:

    MyApp() : mSprite(NULL) {}

    bool OnInit()
    {
        Gpu & gpu = GetGpu();

        mSprite = new SpriteRenderer(gpu);
        if( !mSprite->Init() ) return false;

        const char * rippleCode =
            "struct VSIO                                                                  \n"
            "{                                                                            \n"
            "    float4 pos   : POSITION0;                                                \n"
            "    float4 color : COLOR0;                                                   \n"
            "    float2 tex   : TEXCOORD0;                                                \n"
            "};                                                                           \n"
            "                                                                             \n"
            "VSIO vsmain( VSIO i )                                                        \n"
            "{                                                                            \n"
            "   VSIO o = i;                                                               \n"
            "   o.pos.x  = i.pos.x * 2.0 - 1.0;                                           \n"
            "   o.pos.y  = i.pos.y * -2.0 + 1.0;                                          \n"
            "   return o;                                                                 \n"
            "}                                                                            \n"
            "                                                                             \n"
            "float4 calcRippleColor( float2 origin, float2 position, float time )         \n"
            "{                                                                            \n"
            "   const float SPEED = 30.0f;                                                \n"
            "   const float WAVE_LENGTH = 20.0f;                                          \n"
            "   const float PI = 3.1415926535897932384626;                                \n"
            "   float2 v = position - origin;                                             \n"
            "   float  d1 = length( v );                                                  \n"
            "   float  d2 = SPEED * time;                                                 \n"
            "   float  n  = (d2 - d1) / WAVE_LENGTH;                                      \n"
            "   float  c = n < 0 ? -1.0 : sin( 2.0 * PI * frac(n) );                      \n"
            "   c = ( c + 1.0f ) / 2.0f;                                                  \n"
            "   float da = 1.0 / ( d1 / 30.0 ) / ( d1 / 30.0f );                          \n"
            "   da = clamp( 0, 1, da );                                                   \n"
            "   float ta = 1.0 / (n * n + 1.0);                                           \n"
            "   c = c * ta * da;                                                          \n"
            "   return float4( c, c, c, 1.0 );                                            \n"
            "}                                                                            \n"
            "                                                                             \n"
            "uniform float4 ripples[20];                                                  \n"
            "                                                                             \n"
            "float4 psmain( VSIO input ) : COLOR0                                         \n"
            "{                                                                            \n"
            "    float4 c = 0;                                                            \n"
            "                                                                             \n"
            "    for( int i = 0; i < 20; ++i )                                            \n"
            "    {                                                                        \n"
            "        c += calcRippleColor( ripples[i].xy, input.pos.xy, ripples[i].z );   \n"
            "    }                                                                        \n"
            "                                                                             \n"
            "    return c;                                                                \n"
            "}                                                                            \n"
            ;

        DynaArray<char> rippleCodeFromFile;
        const char * filename = "media/ripple.ps";
        if( fs::PathExist( filename ) )
        {
            DiskFile f;
            if( f.Open( filename, "rt" ) )
            {
                rippleCodeFromFile.Resize( f.Size() + 1 );
                rippleCodeFromFile[f.Size()] = '\0';
                if( f.Read( &rippleCodeFromFile[0], f.Size(), NULL ) )
                {
                    rippleCode = &rippleCodeFromFile[0];
                }
            }
        }

        mContext.Clear();

        GpuProgramDesc gpd;
        gpd.lang = GpuProgramLanguage::HLSL9;
        gpd.vs.source = rippleCode;
        gpd.vs.entry = "vsmain";
        gpd.ps.source = rippleCode;
        gpd.ps.entry = "psmain";
        mContext.gpuProgram.Attach( gpu.CreateGpuProgram( gpd ) );
        if( !mContext.gpuProgram ) return false;

        mContext.uniforms.Resize( 1 );
        mContext.uniforms[0].Attach( gpu.CreateUniform( sizeof(mRipples) ) );
        if( !mContext.uniforms[0] ) return false;

        // initialize ripples
        for( size_t i = 0; i < GN_ARRAY_COUNT(mRipples); ++i )
        {
            mRipples[i].time = 0;
        }
        mNextRipple = 0;

        // success
        return true;
    }

    void OnQuit()
    {
        SafeDelete( mSprite );
        mContext.Clear();
    }

    void OnKeyPress( input::KeyEvent key )
    {
        if( key.code == KeyCode::MOUSEBTN_0 && key.status.down )
        {
            int mx, my;
            gInput.GetMousePosition( mx, my );

            mRipples[mNextRipple].x = (float)mx;
            mRipples[mNextRipple].y = (float)my;
            mRipples[mNextRipple].time = 0.01f;

            mNextRipple = ( mNextRipple + 1 ) % GN_ARRAY_COUNT(mRipples);
        }
    }

    void OnUpdate()
    {
        for( size_t i = 0; i < GN_ARRAY_COUNT(mRipples); ++i )
        {
            if( mRipples[i].time > 0.0f )
            {
                mRipples[i].time += UPDATE_INTERVAL;
            }
        }

        mContext.uniforms[0]->Update( 0, sizeof(mRipples), mRipples );
    }

    void OnRender()
    {
        Gpu & gpu = GetGpu();

        const DispDesc & dd = gpu.GetDispDesc();

        gpu.BindContext( mContext );
        mSprite->DrawSingleSolidSprite(
            0xFFFF0000,
            SpriteRenderer::USE_EXTERNAL_GPU_PROGRAM,
            0.0f, 0.0f, (float)dd.width, (float)dd.height, 0.0f );
    }
};

int main( int argc, const char * argv[] )
{
   MyApp app;
   return app.Run( argc, argv );
}
