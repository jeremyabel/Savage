#include "ofMain.h"
#include "svgToObj.h"

int main()
{
	ofSetupOpenGL( 800, 500, OF_WINDOW );
	ofRunApp( new svgToObj() );
}
