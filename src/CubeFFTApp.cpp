#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CubeFFTApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    static void prepareSettings( Settings* settings );
    
    CameraPersp  mCam;
    gl::BatchRef mCubeBatch;
};

void CubeFFTApp::prepareSettings( Settings* settings )
{
    settings->setHighDensityDisplayEnabled();
    settings->setWindowSize( vec2( 1280, 720 ) );
    settings->setWindowPos( vec2( 200, 0 ) );
}

void CubeFFTApp::setup()
{
    //============CUBE CLASS====================
    auto color = gl::ShaderDef().color();
    gl::GlslProgRef shader = gl::getStockShader( color );
    mCubeBatch = gl::Batch::create( geom::WireCube( vec3( 1.f ), ivec3( 1 ) ), shader );
    //==========================================
    
    //=============CAMERA CLASS=================
    auto fov = 60;
    auto nearPlane = 1;
    auto farPlane = 1000;
    vec3 eyePoint = vec3( 3.f, 3.f, 3.f );
    vec3 target = vec3( 0.f, 0.f, 0.f );
    
    mCam.setPerspective( fov, getWindowAspectRatio(), nearPlane, farPlane );
    mCam.lookAt( eyePoint, target );
    //==========================================
    
}

void CubeFFTApp::mouseDown( MouseEvent event )
{
}

void CubeFFTApp::update()
{
}

void CubeFFTApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    //============CUBE CLASS====================
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::setMatrices( mCam );
    
    size_t numCubes = 10;
    
    for ( size_t i = 0; i < numCubes; ++i )
    {
        gl::pushModelMatrix();
        gl::translate( vec3( 0.f, 0.f, 0.f ) );
        gl::color( ColorA( 1.f, 1.f, 1.f, 1.f ) );
        gl::scale( vec3( i * 0.1f ) );
        mCubeBatch->draw();
        gl::popModelMatrix();
    }
    //==========================================

    gl::disableDepthWrite();
    gl::disableDepthRead();
}

CINDER_APP( CubeFFTApp, RendererGl, CubeFFTApp::prepareSettings )
