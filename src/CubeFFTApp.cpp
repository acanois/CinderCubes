#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/audio/audio.h"
#include "cinder/CameraUi.h"


/**
 TODO:  Optimize the draw function
        Get a better audio source than the damn microphone
        Figure out how to get the glow effect
 */

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
    
    void initializeAudio();
    void initializeCubes();
    void initializeCamera();
    
    void drawCubes();
    
private:
    // Audio
    audio::InputDeviceNodeRef        mInputDeviceNode;
    audio::MonitorSpectralNodeRef    mMonitorSpectralNode;
    std::vector<float>               mMagSpectrum;
    
    // Visual
    CameraPersp  mCam;
    
    gl::GlslProgRef mShaderProgram;
    gl::BatchRef mCubeBatch;
    
    static constexpr size_t mWindowSize { 128 };
    
    float mTheta { 0.0f };
    size_t mHueMod { 0 };
    
    std::vector<gl::BatchRef> cubeVector;
};

void CubeFFTApp::prepareSettings( Settings* settings )
{
    settings->setHighDensityDisplayEnabled();
    settings->setWindowSize( vec2( 1280, 720 ) );
    settings->setWindowPos( vec2( 200, 0 ) );
}

//================================================================================
void CubeFFTApp::setup()
{
    initializeAudio();
    initializeCamera();
    

    
    initializeCubes();
}

//================================================================================
void CubeFFTApp::initializeAudio()
{
    auto ctx = audio::Context::master();
    
    mInputDeviceNode = ctx->createInputDeviceNode();
    
    auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize( mWindowSize * 2 ).windowSize( mWindowSize );
    
    mMonitorSpectralNode = ctx->makeNode( new audio::MonitorSpectralNode( monitorFormat ) );
    mInputDeviceNode->connect( mMonitorSpectralNode );
    mInputDeviceNode->enable();
    
    ctx->enable();
    
    getWindow()->setTitle( mInputDeviceNode->getDevice()->getName() );
}

void CubeFFTApp::initializeCamera()
{
    auto fov = 60;
    auto nearPlane = 1;
    auto farPlane = 1000;
    
    vec3 eyePoint = vec3( 3.f, 3.f, 3.f );
    vec3 target = vec3( 0.f, 0.f, 0.f );
    
    mCam.setPerspective( fov, getWindowAspectRatio(), nearPlane, farPlane );
    mCam.lookAt( eyePoint, target );
}

/// Not currently being used
void CubeFFTApp::printFFTInfo()
{
    size_t numBins = mMonitorSpectralNode->getNumBins();
    float nyquist  = static_cast<float>( audio::master()->getSampleRate() / 2.f );
    
    console() << "Bin Count: " << numBins << std::endl;
    console() << "Nyquist:   " << nyquist << std::endl;
}

//================================================================================
void CubeFFTApp::initializeCubes()
{
    /// Move to its own function eventually
    mShaderProgram = gl::GlslProg::create( loadAsset( "vert.glsl" ), loadAsset( "frag.glsl" ) );
    
    auto cube = geom::WireCube( vec3( 1.f, 1.f, 1.f ), ivec3( 1 ) );
    mCubeBatch = gl::Batch::create( cube, mShaderProgram );
    
    for ( auto i = 0; i < mWindowSize; ++i )
    {
        cubeVector.push_back( mCubeBatch );
    }
}

//================================================================================
void CubeFFTApp::mouseDown( MouseEvent event )
{
}

void CubeFFTApp::update()
{
    mMagSpectrum = mMonitorSpectralNode->getMagSpectrum();
    mTheta += 0.002;
}

//================================================================================
void CubeFFTApp::drawCubes()
{
    for ( size_t i = 0; i < ( mWindowSize * 0.75f ); ++i )
    {
        // Close enough for now
        float alpha = ( audio::linearToDecibel( mMagSpectrum[i] ) * 0.01f );
        float hue = i / static_cast<float>( mWindowSize );
        float rotationSpeed = 0.05f;
        float sineRotation = std::sin( ( mTheta * rotationSpeed ) );
        float cubeDensity = 0.05f;
        
        gl::pushModelMatrix();
        gl::translate( vec3( std::sin( mTheta ), 0.f, std::cos( mTheta ) ) );
//        gl::rotate( ( i * ( sineRotation * 0.5 ) ),  // x
//                    vec3( 1.f, std::cos( mTheta ),   // y
//                    std::sin( mTheta ) ) );          // z
        gl::rotate( ( i * ( std::sin( ( mTheta * 0.01f ) * ( i * 2.f ) ) * 0.005f ) ),
                            vec3( 0.25f, 0.f, 0.25f ) );
        gl::rotate( mTheta, vec3( 1.f, 0.f, 0.f ) );
        
        
        // Loop the color wheel
        auto hueMod = std::abs( fmod( hue - mTheta, 1.f ) );
        
        gl::color( Color( CM_HSV, hueMod, 1.f, alpha ) );
        gl::scale( vec3( i * cubeDensity ) );
        
        if ( alpha > 0.0f )
            cubeVector[i]->draw();
        
        gl::popModelMatrix();
    }
}

void CubeFFTApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.17f ) );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    gl::enableAdditiveBlending();
    gl::setMatrices( mCam );
    
    drawCubes();
    
    gl::disableDepthWrite();
    gl::disableDepthRead();
}

CINDER_APP( CubeFFTApp, RendererGl( RendererGl::Options().msaa( 16 ) ), CubeFFTApp::prepareSettings )
