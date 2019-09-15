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
};

void CubeFFTApp::setup()
{
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
}

CINDER_APP( CubeFFTApp, RendererGl )
