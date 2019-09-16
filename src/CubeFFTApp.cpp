#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/audio/audio.h"

using namespace ci;
using namespace ci::app;

class CubeFFTApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    static void prepareSettings( Settings* settings );
    
    void printFFTInfo();
    
private:
    // Audio
    audio::InputDeviceNodeRef        mInputDeviceNode;
    audio::MonitorSpectralNodeRef    mMonitorSpectralNode;
    std::vector<float>               mMagSpectrum;
    
    // Visual
    CameraPersp  mCam;
    gl::BatchRef mCubeBatch;
    
    static constexpr size_t mWindowSize = 32;
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
    auto ctx = audio::Context::master();
    
    mInputDeviceNode = ctx->createInputDeviceNode();
    
    // Use double FFT size vs windowSize because it causes the FFT to be zero-padded
    auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize( 64 ).windowSize( mWindowSize );
    mMonitorSpectralNode = ctx->makeNode( new audio::MonitorSpectralNode( monitorFormat ) );

    mInputDeviceNode->connect( mMonitorSpectralNode );
    
    mInputDeviceNode->enable();
    ctx->enable();
    
    getWindow()->setTitle( mInputDeviceNode->getDevice()->getName() );
    //==========================================
    
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
    
    printFFTInfo();
}

void CubeFFTApp::printFFTInfo()
{
    size_t numBins = mMonitorSpectralNode->getNumBins();
    float nyquist = static_cast<float>( audio::master()->getSampleRate() / 2.f );
    
    console() << "Bin Count: " << numBins << std::endl;
    console() << "Nyquist:   " << nyquist << std::endl;
}

void CubeFFTApp::mouseDown( MouseEvent event )
{
}

void CubeFFTApp::update()
{
    mMagSpectrum = mMonitorSpectralNode->getMagSpectrum();
}

void CubeFFTApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    //============CUBE CLASS====================
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::setMatrices( mCam );
    
    for ( size_t i = 0; i < mWindowSize; ++i )
    {
        float alpha = mMagSpectrum[i];
        gl::pushModelMatrix();
        gl::translate( vec3( 0.f, 0.f, 0.f ) );
        gl::color( ColorA( 1.f, 1.f, 1.f, alpha ) );
        gl::scale( vec3( i * 0.2f ) );
        mCubeBatch->draw();
        gl::drawLine( vec2( 10, 0 ), vec2( 10, mMagSpectrum[i] ) );
        gl::popModelMatrix();
    }
    //==========================================

    gl::disableDepthWrite();
    gl::disableDepthRead();
}

CINDER_APP( CubeFFTApp, RendererGl, CubeFFTApp::prepareSettings )
